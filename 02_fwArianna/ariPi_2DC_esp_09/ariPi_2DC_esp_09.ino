#define V_FW_ATMEGA  "3.05.02"
/**@file ariPi_2DC_esp_08.ino */
/* stuffcube.wordpress.com
    27dic18 3.05.02
		valore default KpTeta da 0.8 a 8
		baco: caricava KpTeta con valore di default in firstRun
		rimossa risposta "mis:"
		rimosso cmd 'B', il sensore ostacolo si autodetecta
		aggiornata stringa di risposta a comando 'p'
		campo ID msg 'p' in posizione 2

	24dic18 3.05.00
		aggiunto vl53 per la gestione ostacoli, ritorna ost come segnale  per non creare confusione con lidar, aggiunto parametro lidar

	09dic18  3.04.01
		gestione lidar con ritorno in mm divido il valore per 10
		aggiunto divisore divLidar come parametro in E2prom per vecchi lidar
		modificati valori di default per meccanica corrente
		corretto errore in load default
		caricando questa versione la prima volta vanno ripristinati i valori meccanici

	01dic18 3.04.00
		sensore ostacolo anteriore off alla partenza. va abilitato per usarlo
		aggiunta V_FW_ATMEGA
		formato M.mm.sb
		M: il cambio richiede modifiche hw. Aggiungere un sensore (es sensore frontale) non è una modifica hw, è una opzione
		mm: aggiunta di una funzione, le versioni precedenti non la supportano. es la gestione del sensore frontale.
		sb: subversion. si migliorano funzioni o corregono bachi. nulla viene aggiunto.

		richiesta "h" versione V_FW_ATMEGA
		printDatiCalibrazione dava errore per risposta troppo lunga

	25nov18 definiti LED1..3
		attivato led1 insieme a LedPuntatore

	11Nov18
		gestione sensore ostacolo frontale
		comando B per abilitarlo B0 off, B1 On
		nota che il Lidar si sente sull'IR. Se tolgo lidar dall'asse
		non interferisce.


	01Nov18 
		rivisto lidar. Introdotta guida a distanza dalla parete. Modo R1 e R2. Parametro 'O' distRef.
		nellla stringa "pos:..." aggiunto in coda "statoRun". Questo permette di coordinare il sw su pc.
		introdotti parametri per guadagni regolatori vari

  08ott18
    lidardist da float a int, Piccola modifica in testOstacoli()

  04set18
  //    delay(25); rimosso in lidar() insieme a 3 assegnamenti

  07ago18
  revisione 3.0
  rimosso sonar
  rimosso BT
  rimossa gestione comandi BT
  sensori di distanza IR: tolto interrupt per misura lineare, inserito stato ON/Off
  FILE_PATTERNS = [...] *.ino
  EXTENSION_MAPPING = ino=c doxygen config filebuf

  da ora i commenti più recenti sono in alto
  intestazioni routine per doxygen
/ * *

     @brief  Gets the most recent sensor values and transform in angle.
            This function reads only x and y magnetometer values
            The coefficients for correction are applied.

      the output is tetaCompass which is a global variable.

    @param  a .....
    @return b ....


 * /

/// Brief description.
https://learn.adafruit.com/the-well-automated-arduino-library/doxygen-tips
http://www.stack.nl/~dimitri/doxygen/manual/markdown.html






----------------------------------------------------------------------------

note : 30lug16
fare un doppio movimento per la posizione
il primo fa in grosso e nel caso eccede.
il secondo si posiziona fine.
la macchina a stati è già pronta

giocare con baseline per la giusta larghezza

R5 si ferma troppo avanti, l'angolo che trova è sbagliato
devo dare due R5 per farlo andare


testare con modifica VA_zero attiva, nei test precedenti non lo era
testare modo R5 che gira su una ruota.


sistemata cablatura:
motore DX morsetti A - per il piu e meno fare foto
motore SX morsetti B
IR DX a ingDX
IR SX a ingSX
Encoder SX a ing SX
Encoder DX a ing DX

parametri taratura salvati e recuperati da E2prom
talvolta perde dei dati (giroRuota). Confusione con aggancio BT ??
messo comando f per leggere distanza degli IR
R5 e R6 girano ma non danno risultati ripetibili
m sonar rivedere





aggiustare le dirVx = 0 gestite in odometro


*/
/*
 * arianna
 * giorgio323@gmail.com
 * infostuffcube@gmail.com
 *
 * 16nov16  inizio

  test su sensore ir
  l'idea è trovare il tempo a 1 rispetto al tempo totale
  questo è un indicatore della distanza

  10dic16 sensore rotazione motore (odometro)
      spostato su Raspberry la compilazione
      ID_000  digitalPinToInterrupt(SIDE_IR_PIN) non dichiarato

  27dic16 comandi da seriale
      gestione arrivo in posizione
      aggiunto sonar montato su servo

      per trovare la direzione da puntare cerco il punto a distanza minima
      con un algoritmo di bisezione
      faccio con uno script esterno

  02gen17 penso a gestire sensore dx e sx
  numbers 0 (on digital pin 2) and 1 (on digital pin 3)

  19gen17 differenziale elettronico
      tolgo ovfTimer perchè usato dai PWM
      gestire misura sensore senza interrupt timer

  28gen17 add blueTooth on serial port1
      vanno ignorati i vari messaggi da BT
      quando non è connesso
      code 0000
      http://wiki.seeedstudio.com/wiki/Bluetooth_Bee

  23mar17 ID_001 rimosso limite angolo teta

  14mag17 si aggiunge comunicazione con esp ID_002
      si usa la serial2 verso esp
      modifica protocollo Alessandro Airaghi

  21mag17 ripristinato BT in parallelo ID_003
      Ok

  07giu17 cambiati encoder
      # come indicatore inizio msg monitor
        aggiunto comado di home 'H', prende la posizione attuale come 0, 0, 0
      cambiato terminatore seriale, da println a print + \n ID_004
      nel controllo di teta porto -2 <= S <= 2  ID_005

  29lug17 macchina a stati e rampa su tetaRef Rr


  11ago17 comando H modificato (distanza, odometro, lastPosition = 0.0)
      introdotto statoRun 99. Questo indica una rampa in corso.
      precedentemente lo statoRun 0 era presente anche durante la rampa di fermata
      il 99 serve per eviatre di inserire un ritardo coi comandi seriali per attendere la fine rampa

      nella lettura encoder se tensione al motore zero non conto impulsi.
      serve per evitare che quando curva su una ruota ferma possano esserci movimenti

  22ago17 ariPi_2DC_esp_02.ino
      messo ordine su nomi e posizioni encoder motori etc
      sistemati segni delle reazioni
      questo per capire su cosa si agisce
      Lettura velocità angolare e lineare

  10set17 diventa _03
      salvo dati di taratura in eeprom  ID_006
      rivisti sensori IR left e dx
  20set17
      inserita richiesta p per avere posx posy e teta in una sola volta
  060tt17
    ale
    aggiunta richiesta q per scansione area circostante con sonar per mappa

  12dic2017
    rimossa parte di bt libero porta SerialBT per rilevatore laser
    pulizia generale codice esp su serial 3


  08nov17
      inizializzato 'i' in eeprom
      rimosso #include <TimerOne.h>


  03dic17
      ID_007
      porta4 per comunicazione con la scheda sensori IR per la ricerca della
      docking station
      un micro esegue il sw nel foleder IRrecvDemo_x.ino
      viene fatto un bridge tra comandi da seriale verso la porta 4
      al momento è bloccante

  sistemare i parametri in funzione di ed anche per ari02



  10feb18
    merge

  21feb18
    // ID_008 dimenticato un pezzo

  05mag18
    merge per ari06.
    Lidar,
    Esp con IR
    tolgo BlueTooth
    tolgo ponte verso scheda IR

  12giu18
    // ID_009
    aggiunto getione sensore compass, gyro etc
    liberate porte 20 e 21, I2C (SCL, SDAT)
    eliminati sensori dx e sx per avere liberi interrupt per encoder (erano sul pin 3)
    modificati pin interrupt encoder
    pin 21 ora su pin 2
    pin 20 ora su pin 3
  aggiunta richiesta bussola 'f'
  agginto comando selezione guida 'G' con odometro, bussola, fusione ..

    ID_010
  riportati comandi seriale mancanti su "comandi"
  aggiungta possibilita di indice ai parametri
  la prima cifra dopo la lettera seleziona sino a 10
  indici (Es dati taratura F0, F1, F2 ..
  Rimango compatibile con comandi precedenti
  Salvati coefficienti taratura bussola in EEprom


  liberia tfmini.h   https://github.com/opensensinglab/tfmini/tree/master/src


 * Simple test for the DRV8833 library.
 * The DRV8833 is a dual motor driver carrier made by Pololu.
 * You can find it here: https://www.pololu.com/product/2130


 */



#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include <TFMini.h>
#include <EEPROM.h>     // ID_006

// ID_009
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <math.h>
#include "Adafruit_VL53L0X.h"
// \ID_009


//#define ARI2  1
#define ARI3  1

#define SCRIVI  0
#define LEGGI   1
#define DEFAULT 2

#define DUE_PI  6.28318

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345); //ID_009

Servo servoPan;       	// create servo object to control a servo
Servo servoTilt;        // create servo object to control a servo


//********** ID_002
#define ESPserial   Serial2
#define TFserial  Serial1   // LaserRadar

//creo tfmini
TFMini  tfmini;

String  inputString     	= "";
String  inputStringTmp    	= "";
int   	okcomm        		= 0;
static  String risposta;
char  	port        		= 0;

//**********fine ID_002

// dc motor
#include <DRV8833.h>

// Create an instance of the DRV8833:
DRV8833 driver = DRV8833();

//creo istanza vl530l0x
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// Pin numbers. Replace with your own!
// Attach the Arduino's pin numbers below to the
// Ain1 and Ain2 DRV8833 pins.
// N.B. la libreria sembra non gestire i pin 44-46 (timer diverso)
const int MTR_A_DX_P1   = 5;  /*!< output pin 1 for right motor */
const int MTR_A_DX_P2   = 6;  /*!< output pin 2 for right motor */
const int MTR_B_SX_P1   = 9;  /*!< output pin 1 for left motor */
const int MTR_B_SX_P2   = 10; /*!< output pin 2 for left motor */


int motorSpeedRef   = 0;
int direzione     	= 1;
int dirVA     		= 1;    // ID_005
int dirVB     		= 1;

int VB_zero     	= 0;
int VA_zero     	= 0;


#define SERVO_PAN_PIN     	45	///< Digital IO pin connected to the servo pin.
#define SERVO_TILT_PIN    	44  ///< Digital IO pin connected to the servo pin.

#define GIRO_DX_PIN     	3   ///< encoder  rotazione albero motore ID_009
#define GIRO_SX_PIN     	2   ///< encoder  rotazione albero motore ID_009

#define R_SIDE_FRONT    	29  ///< sensore frontale Dx      Low = ostacolo
#define L_SIDE_FRONT    	31  ///< sensore frontale Sx
#define ENB_R_SIDE_FRONT  	28  ///< abilita sensore frontale Dx. High = Enable
#define ENB_L_SIDE_FRONT  	30  ///< abilita frontale Sx
#define LED1        		34  ///< LED1
#define LED2        		33  ///< LED2
#define LED3        		32  ///< LED3




#define ledPin        		13  ///< led segnalazioni varie su Arduino
#define laserPin      		8   ///< porta puntatore laser

#define  TEST_MOTORE      	0  	///< modalità di test per fasi di debug
#define  TEST_STERZO      	1   ///< modalità di test per fasi di debug
#define  TEST_SENSORS     	2   ///< modalità di test per fasi di debug
#define  TEST_CONTROLLO   	3   ///< modalità di test per fasi di debug
#define  TEST_GIRO_SENSOR   4   ///< modalità di test per fasi di debug

long odometroCnt, odometroDxCnt, odometroSxCnt;     ///< contatori encoder

char firstRun;


#define TEMPO_CONTROLLO 	25  ///< tempo del controllo sterzo, posizione etc in ms


float ED      		= 1.0;    	///< definiscono la meccanica del robot. descrizione nella parte cinematica 
float ED_BASE     	= 1.0;    	///< definiscono la meccanica del robot. descrizione nella parte cinematica  
float BASELINE    	= 130.0;  	///< larghezza carreggiata in [mm]. descrizione nella parte cinematica  
float LAGHEZZA_A_MEZZI = 0.065; ///< larghezza meta' carreggiata in [m]. descrizione nella parte cinematica  

float GIRO_RUOTA_SX = 1.0;    	///< [mm per impulso*0.5]  sviluppo ruota[mm]/(2*ppr) (pulses per revolution)]. descrizione nella parte cinematica  
float GIRO_RUOTA_DX = 1.0;    	///< [mm per impulso*0.5]  sviluppo ruota[mm]/(2*ppr) (pulses per revolution)]. descrizione nella parte cinematica  
float GIRO_RUOTA  = 1.0;    	///< (GIRO_RUOTA_SX + GIRO_RUOTA_DX)/2. descrizione nella parte cinematica  

unsigned long MIN_TIME_TRA_PULSE = 9;     ///< tempo minimo tra impulsi encoder per evitare errate letture [ms]


#define E_POSIZIONAMENTO    10    ///< [mm], distanza dall'obbiettivo dove il robot si arresta
#define E_APPROCCIO     	300   ///< [mm], distanza dall'obbiettivo dove il robot inizia a rallentare

/*
  MODERATA    è il pwm di movimento normale
  APPROCCIO     durante l'avvicinamento alla poszione
  ACCELERAZIONE   nella fase del partenza
*/
#define FERMO     		0 		///< valore pwm di uscita durante stato di FERMO
#define MODERATA    	190     ///< valore pwm di uscita durante marcia MODERATA
#define APPROCCIO   	110     ///< valore pwm di uscita durante marcia APPROCCIO
#define ACCELERAZIONE 	160     ///< valore pwm di uscita durante fase accelerazione (non usato)
#define AVANTI      0
#define INDIETRO    1

#define MAX_STERZO  1.0       	///< massimo valore di sterzo. con 1 si ha una ruota ferma e l'altra al doppio della velocita'

// parametri lettura e/o scrittura
int   statoRun    	= 0;      	///< macchina a stati del movimento. Definisce anche il tipo controllo guida.
float   odometro    = 0;      	///< distanza percorsa in mm
float   distanza    = 0;      	///< distanza da percorrere col prossio Run
float   raggiorSterzo = 0.0;    ///< raggiorSterzo indica lo scorrimento che applichiamo alle ruote. vedi function differenziale
int     lidardist     = 0;      ///< [cm] distanza misurata dal Lidar
float   errore;

int   motorSpeed    = 0;
int   motorSpeedValue = MODERATA;
int   panAngle    	= 90;     	///< angolo destinazione del servo Pan
int   tiltAngle     = 90;   	///< angolo destinazione del servo Tilt
char  laser       	= 0;      	///< stato uscita puntatore laser
float kp_guida;  				///< guadagno proporzionale per controllo su distanza muro
float kd_guida;   				///< guadagno derivativo    per controllo su distanza muro

float kpTeta;      				///< guadagno proporzionale per controllo su teta
float kiTeta;     				///< guadagno integrale per controllo su teta

float   MAX_S     	= 0.2;      ///< max_s = LARGHEZZA_A_MEZZI/Raggio massimo
float   teta      	= 0.0;      ///< teta attuale misurato da odometria
float   xpos, ypos  = 0.0;      ///< x    attuale misurato da odometria
float   tetaRef     = 0.0;      ///< y    attuale misurato da odometria

int   distRef     	= 60;     	///< distanza riferimento per modo R1/R2           [cm]
int   distOstacolo  = 51;       ///< distanza dell'Ostacolo per arresto automatico [cm]. N.B. sotto i 50 cm a volte non legge.
int   divLidar      = 1;      	///< alcune versioni del lidar ritornano in mm altre in cm, uniformiamo a cm
int   vl53dist      = 999;    	///distanza letta da vl53
float tetaMisura    = 0.0;    	///< teta da bussola
float xc, yc, tetaCompass;      ///< vettori magnetici dalla bussola
float deltaCompass,tetaCompass_prev, tetaCompassRead;   ///< variabili bussola
char  modoGuida       = 0;      ///< modalita' di guida: odometro, bussola, misto, fusione ...
char  bussola;            		///< presenza bussola
char  sensore_ost;          	///< presenza laser ostacoli VL53L0X, 1 attivo

/** coefficienti calibrazione bussola 
sono salvati in E2prom */
float ox = 0.0;    				///< offset lungo l'asse x. viene ricavato a seguito della calibrazione
float oy = 0.0;    				///< offset lungo l'asse y. viene ricavato a seguito della calibrazione
float ky = 0.0;  				///< guadagno lungo asse y. viene ricavato a seguito della calibrazione

// variabili appoggio

float VA, VB;
int   cntStop = 0;
int   mode = TEST_STERZO;
char  monitorDati = 0;
float i_part = 0;     			///< parte integrale regolatore guida
float delta_teta;
float deltaS;
long  spdDxCnt, spdSxCnt; 		///< velocità encoder, impulsi per tempo di campionamento


int   lidarDistance;    		///< distance measured by lidar
float deltaErrore;    			///< variabili parte derivativa
float erroreK_1;      			///< variabili parte derivativa
float DerActive;      			///< variabili parte derivativa

char  occlusoDavanti;   		///< sensore IR anteriore, LOW in presenza di ostacolo


unsigned long   lastTime, lastTimeFast,timeLidar;
float     teta_;
float       actualTetaRef;
char      SM_R5 = 0;
unsigned long   inizioStabilita; //Timer


sensors_event_t event;    // ID_009

// prototipi funzione
//void displaySensorDetails (void);
//void getSensorReading     (void);


/*!
    @brief  setup della parte hardware e caricamento coefficienti da E2prom

*/
void setup() {
    // initialize serial:
    Serial.begin  (115200);
    Serial.print  ("setup starting! FW version: ");
    Serial.println  (V_FW_ATMEGA);

	pinMode(LED1,         OUTPUT);
	pinMode(LED2,         OUTPUT);
	pinMode(LED3,         OUTPUT);

	digitalWrite(LED1, HIGH);
	digitalWrite(LED2, HIGH);
	digitalWrite(LED3, HIGH);

	TFserial.begin  (TFMINI_BAUDRATE);  // seriale lidar
    tfmini.begin  (&TFserial);

    ESPserial.begin (115200);  // ID_002

    // LSM303_magn unit
    /* Enable auto-gain */
    mag.enableAutoRange(true);

    /* Initialise the sensor */
    Serial.println("init LSM");
	bussola = 1;
	if(!mag.begin()){
		/* There was a problem detecting the LSM303 ... check your connections */
		Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
		bussola = 0;
	}
	else{
		Serial.println("init LSM done");
		/* Display some basic information on this sensor */
		displaySensorDetails();
	}

    sensore_ost=1;
	if (!lox.begin()) {
		Serial.println(F("Failed to boot VL53L0X"));
		sensore_ost=0;
	}
	else{
		Serial.println("init VL53L0X done");
	}
	pinMode(ledPin,   OUTPUT);
	pinMode(laserPin,   OUTPUT);

	// setup digital inputs for IR sensors
	pinMode(R_SIDE_FRONT,     INPUT);     // non usato
	pinMode(L_SIDE_FRONT,     INPUT);
	pinMode(ENB_R_SIDE_FRONT,   OUTPUT);    // non usato
	pinMode(ENB_L_SIDE_FRONT,   OUTPUT);


	pinMode(GIRO_DX_PIN,      INPUT);
	pinMode(GIRO_SX_PIN,      INPUT);


	digitalWrite(laserPin, LOW);

	// attaches the servo on pin .. to the servo object
	servoPan.attach  (SERVO_PAN_PIN );
	servoTilt.attach (SERVO_TILT_PIN);

	servoPan.write ( 90);
	servoTilt.write( 90);


	// Attach motors to the input pins:
	driver.attachMotorA(MTR_A_DX_P1, MTR_A_DX_P2);
	driver.attachMotorB(MTR_B_SX_P1, MTR_B_SX_P2);

	differenziale(0);

	// odometro
	attachInterrupt(digitalPinToInterrupt(GIRO_DX_PIN), odometroDxMisuraHW, CHANGE);
	attachInterrupt(digitalPinToInterrupt(GIRO_SX_PIN), odometroSxMisuraHW, CHANGE);

	Serial.flush();
	ESPserial.flush();  // ID_002


	DataEEprom(LEGGI);  // carica coefficienti da e2prom
	//DataEEprom(5);    /// 5 ARI3, 4 ARI2 carica coefficienti da e2prom

	Serial.println( risposta   );

	firstRun = 1;

	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);
	digitalWrite(LED3, LOW);

	Serial.println("setup done!");

}



void loop() {

static long exeTime, tInit;


  mode = TEST_CONTROLLO;

  getCmd2();              /// gestione comunicazione con ESP
  rSeriale();             /// gestione comunicazione con ESP

  servoPan.write ( panAngle);     /// assegna angoli a pan e tilt
  servoTilt.write(tiltAngle);


  /**
    funzionamento corrente con anelli di controllo attivi
  */
    if (mode == TEST_CONTROLLO){

    if (firstRun){
      Serial.println("start TEST_CONTROLLO mode");
      Serial.println("controlloAttivo");
      raggiorSterzo = 0.0;
      servoPan.write( panAngle);
      servoTilt.write(tiltAngle);
      digitalWrite( ENB_L_SIDE_FRONT, HIGH);
      timeLidar=millis();
      firstRun = 0;
    }

    /*
      test e azione ostacoli frontali
    */

    testOstacoli();
    if (sensore_ost == 1) 	occlusoDavanti = vl53dist		< distOstacolo;
    else					occlusoDavanti = lidarDistance	< distOstacolo;
    

        if (occlusoDavanti && (statoRun != 5) &&(statoRun != 6) &&(statoRun != 0))
        {
			//motorSpeedRef = FERMO;
			statoRun      	= 99;   // senza rampa
			motorSpeed    	= 0;    // annullo rampa
			motorSpeedRef   = 0;
			risposta    	= "ostacolo";
			sendAnswer2(port);
			if (sensore_ost==1){
				risposta = "ost;" + String(vl53dist);
			}
			else{
				risposta = "ost;" + String(lidarDistance);
			}
			sendAnswer2(port);
        }


    /** statoRun

      il movimento è gestito da una macchina a stati. Lo stato iniziale è fermo.


      0: fermo
      1: controllo sterzo da sensore dx e distanza automatico (non attivo)
      1: controllo sterzo da sensore sx e distanza automatico (non attivo)
      2: controllo distanza, sterzo da parametro
      4: controllo distanza, sterzo da parametro da tetaRef vs teta odometria
      5: rotazione sul posto con una ruota ferma
      6: rotazione sul posto con ruote controrotanti

      99: comando arresto
    */

      // parte a 25 ms
      if ((millis()-lastTime) > TEMPO_CONTROLLO){

        exeTime = micros() - tInit;
        //Serial.println(lidarDistance);
        tInit = micros();

        lidarDistance = tfmini.getDistance()/divLidar;


        lastTime = millis();

        updatePosition();
        // ID_009 bussola

        if (bussola) compass();

        if (modoGuida == 0) tetaMisura = teta;      // ID_001
        if (modoGuida == 1) tetaMisura = tetaCompass; // ID_009
        if (modoGuida == 3) tetaMisura = tetaCompass; // ID_009
        if (modoGuida == 2) {
          tetaCompass = 0;
          modoGuida   = 1;
        }



        // controllo sterzo da lidar
        // R3: PAN  10 muro su lato DX
        // R1: PAN 170 muro su lato SX
        // kp usato 0.5
        // Se errore troppo alto andarci con S limitato
        if ((statoRun == 1)||(statoRun == 3)){

          errore = float(distRef - lidarDistance);
          // per come sono messi i motori inverto segno dell
          // deltaErrore
          deltaErrore = - errore + erroreK_1;
          erroreK_1   = errore;
          //tauD = 100e-3;
          //ts   = 25e-3
          //0.7788008 ktD  = exp(-ts/tauD)
          DerActive = DerActive*0.7788008 + kd_guida*deltaErrore*0.2211992;

          if (statoRun == 1) {
            raggiorSterzo =   kp_guida*errore  - DerActive;
          }
          else{
            raggiorSterzo =  -kp_guida*errore  + DerActive;
          }
          // errore +/- 0.5


          if (raggiorSterzo < -MAX_S) raggiorSterzo = -MAX_S;
          if (raggiorSterzo >  MAX_S) raggiorSterzo =  MAX_S;
        }

        if (statoRun == 2){
          motorSpeedRef = motorSpeedValue;
          direzione     = AVANTI;
        }

        // controllo sterzo da teta
        if (statoRun == 4){

          // regolatore PI

          errore = tetaRef - tetaMisura;
          raggiorSterzo =   kpTeta*errore;
          i_part += raggiorSterzo*kiTeta;

          // anti windup
          // limito la parte integrale se la parte proporzionale è già alta
          // questo evita di accumulare errore da scaricare
          // se la somma di parte P e I supera il massimo, la parte integrale viene
          // limitata a MAX - parte P
          if  ((raggiorSterzo + i_part) >  MAX_STERZO){
            i_part = MAX_STERZO - raggiorSterzo;
            if (i_part < 0) i_part = 0;
          }
          if  ((raggiorSterzo + i_part) < -MAX_STERZO){
            i_part = -MAX_STERZO - raggiorSterzo; // -2 - (-1) = -1, -2 -(-3) = 1
            if (i_part > 0) i_part = 0;
          }
          if (i_part>  0.5) i_part =  0.5;
          if (i_part< -0.5) i_part = -0.5;

          raggiorSterzo += i_part;

          // errore +/- 0.5

          if (raggiorSterzo < -2.0) raggiorSterzo = -2.0;   // ID_005
          if (raggiorSterzo >  2.0) raggiorSterzo =  2.0;   // ID_005
        }


        /** gestione raggiungimento angolo target
          in questo caso il robot deve ruotare per orientarsi con l'angolo dato.
          Questo vale per i modi 5 e 6.


          All'approsimarsi della destinazione la velocità viene diminuita per raggiungere il target senza superarlo.

        */
        if ((statoRun == 5)||(statoRun == 6)){

          errore = tetaRef -tetaMisura;

          switch (SM_R5){
            case 0:
                if ( tetaMisura < tetaRef ) SM_R5 = 1;  // salita
                if ( tetaMisura > tetaRef ) SM_R5 = 2;  // discesa
                if ( tetaMisura ==tetaRef ) {
                  SM_R5       = 0;
                  statoRun    = 0;  // arresto senza rampa
                  motorSpeed  = 0;  // annullo rampa
                  motorSpeedRef = 0;
                }
              break;

            case 1: // rampa salita
                motorSpeedRef =  MODERATA;
                direzione = AVANTI;

                // spazio di fermata 0.5*v*tf
                //

                if (errore < delta_teta*7.0){
                // fermo
                  SM_R5     = 3;
                  deltaS      = 10.8;
                }
              break;

            case 2: // rampa discesa

                motorSpeedRef =  MODERATA;
                direzione = INDIETRO;
                if (errore > delta_teta*7.0){
                  // fermo
                  SM_R5     = 3;
                  deltaS      = 10.8;
                }
              break;

            case 3:
                motorSpeedRef -= deltaS;

                if (motorSpeedRef< 0.0){
                  SM_R5     = 0;
                  statoRun    = 0;  // arresto senza rampa
                  motorSpeed  = 0;  // annullo rampa
                  motorSpeedRef = 0;
                }
              break;


          }
        }// SM_R5


        /** gestione raggiungimento distanza target
          in questo caso il robot deve arrivare alla distanza impostata.
          Questo vale per i modi (1, 3, 2, 4).
          Sono esclusi i 5 e 6 che ruotano il robot e sono gestiti nei modi 5 e 6 da una propria macchina a stati.

          All'approsimarsi della destinazione la velocità viene diminuita per raggiungere il target senza superarlo.

        */


        if (((statoRun != 5)&&(statoRun != 6))&&(statoRun != 0)){
          if ( distanza > odometro )  direzione = AVANTI;
          else            direzione = INDIETRO;

            if ( abs(distanza - odometro) > E_APPROCCIO){
              motorSpeedRef = motorSpeedValue;
            }
            else
              if ( abs(distanza - odometro) > E_POSIZIONAMENTO){
                motorSpeedRef = APPROCCIO;
              }
              else{
                motorSpeedRef = FERMO;
                statoRun      = 99;
              }
        }
        else{ // statoRun == 5 or 6 o zero (ruota su se stessa)
          ;
        }
      }// fine parte temporizzata TEMPO_CONTROLLO ms


      /** questa parte genera le rampe sulla tensione del motore. Sia in accelerazione che decelerazione.
      *   Viene fatta girare con un tempo piu' veloce per poter usare gradini di riferimento più piccoli.
      *   All'arrivo a velocità zero la macchina a stati del movimento viene messa nello stato iniziale.
      */
      if ((millis()-lastTimeFast) > 10){
        lastTimeFast = millis();

        if (statoRun == 99) motorSpeedRef = 0;

        // rampa sulla velocita'
        if (motorSpeedRef > motorSpeed) motorSpeed += 2;
        if (motorSpeedRef < motorSpeed) motorSpeed -= 4;  // 15

        if (motorSpeed > 250) motorSpeed = 250;
        if ((motorSpeed <   1)&&(statoRun == 99)){
          motorSpeed = 0;
          statoRun   = 0;
        }
      }// fine temporizzata veloce

      differenziale(motorSpeed);

  }


  /**
    modo di test. viene usato durante il debug dell'hw. viene configurato alla bisogna.
    I comandi arrivano diretti dalla seriale e codificati comenecessario.
  */
    if (mode == TEST_MOTORE){
        Serial.println("start TEST_MOTORE mode, enter motorSpeed, direzione");
    while (1){
      while (Serial.available() > 0) {

        // format x, y
        // look for the next valid integer in the incoming serial stream:
        motorSpeedRef = Serial.parseInt();
        // do it again:
        direzione = Serial.parseInt();

        //PDURATION = Serial.parseInt();

        // look for the newline. That's the end of your
        // sentence:
        if (Serial.read() == '\n') {
          // constrain the values to -50 - 50
      //                    x = constrain(x, -150, 150);
      //                    y = constrain(y, -200, 200);

          Serial.print(motorSpeedRef);
          Serial.print(',');
          Serial.println(direzione);

        }
      }

      if (motorSpeedRef < 0){ // == INDIETRO){
        driver.motorBForward(motorSpeedRef);
        driver.motorAReverse(motorSpeedRef);
      }
      else{
        driver.motorAForward(motorSpeedRef);
        driver.motorBReverse(motorSpeedRef);
      }

    }
  }

}

//------------------- fine main --------------------------------

/** @brief odometri

  la misura della posizione viene fatta leggendo gli impulsi prodotti dagli encoder rotativi montati sull'albero delle ruote.
  Gli encoder sono dischi con delle fessure regolari. Queste interrompono a tartti il fascio luminoso sul sensore optoelettronico.
  Ogni transizione genera un interrupt su Arduino.
  Contando il numero degli impulsi letti si misura lo spazio percorso.
  La direzione dipende dalla tensione applicata al motore. Gli impulsi vengono sommati in un caso e sottratti nell'altro.

  Il flag Vx_zero indica queando il motore è fermo. In questo caso si evitano i conteggi per evitarne di falsi.
  Se gli impulsi giugono troppo velocemente vengono considerati errori.

  La routine incremmenta contatori di interi per essere veloce.
*/
void odometroDxMisuraHW(void){
unsigned long pulseTime;

	if ((millis() - pulseTime) < MIN_TIME_TRA_PULSE) return;
	pulseTime = millis();

	//Serial.println("dx");
	digitalWrite(ledPin, !digitalRead(ledPin));

	if (VA_zero) return;    // se tensione zero non conta

	if  (statoRun == 0)          return;

	if (dirVA == 1)  odometroDxCnt ++;    // ID_005
	else             odometroDxCnt --;
}

/** @brief vedi Sx
*/
void odometroSxMisuraHW(void){
unsigned long pulseTime;

	if ((millis() - pulseTime) < MIN_TIME_TRA_PULSE) return;
	pulseTime = millis();

	//Serial.println("Sx");
	digitalWrite(ledPin, !digitalRead(ledPin));

	if (VB_zero) return;    // se tensione zero non conta

	if  (statoRun == 0) return;

	if (dirVB == 1)  odometroSxCnt ++;    // ID_005
	else             odometroSxCnt --;
}



/**
  @brief  sterzo con differenziale. Per curvare la velocità relativa delle due ruote viene modificata. Con le velocità uguali proseguo diritto, con una maggiore dell'altra curvo.

      raggiorSterzo (variabile globale) indica lo scorrimento che applichiamo alle ruote. E' un numero tra 0 e 2.

      ponendo s = raggiorSterzo abbiamo che le velocita' alle ruote valgono:

      VA = motorSpeed*(1-s)

      VB = motorSpeed*(1+s)

      In funzione di StatoRun si hanno diversi modi operativi

      StatoRun  .. : differenziale in funzione di raggiorSterzo

      StatoRun == 5: curva con una ruota ferma

      StatoRun == 6: curva con ruote controrotanti


    @param  motorSpeed. Il valore a cui limitare la velocita' massima applicata ai motori.

*/

void differenziale(float motorSpeed){
static float rs;
static float Vlimite;

  if ((statoRun != 5)&&(statoRun != 6)){

    rs = raggiorSterzo;

    /* quando viene fatta una curva con una ruota bloccata l'altra ruota va a 255.
       partendo da fermo ci può essere slittamento.
       In questo caso si limita la velocità della ruota che marcia.
    */
    Vlimite = 255;
    if (rs >  MAX_STERZO){      // ID_005
      rs =  MAX_STERZO;
      Vlimite = motorSpeed;
    }
    if (rs < -MAX_STERZO){
      rs = -MAX_STERZO;
      Vlimite = motorSpeed;
    }

    VA = motorSpeed*(1.0+rs);
    VB = motorSpeed*(1.0-rs);

    if (VA >  Vlimite) VA =  Vlimite;
    if (VA < -Vlimite) VA = -Vlimite;

    if (VB >  Vlimite) VB =  Vlimite;
    if (VB < -Vlimite) VB = -Vlimite;

    if (direzione == INDIETRO){
      if (VA >= 0 ){
        dirVA = -1;   // ID_005
        driver.motorAReverse(VA);
      }
      else{
        dirVA =  1;
        driver.motorAForward(VA);
      }
      if (VB >= 0 ){
        dirVB = -1;
        driver.motorBReverse(VB);
      }
      else{
        dirVB =  1;
        driver.motorBForward(VB);
      }
    }
    else{ // direzione == AVANTI

      if (VA >= 0 ){
        dirVA =  1;   // ID_005
        driver.motorAForward(VA);
      }
      else{
        dirVA = -1;
        driver.motorAReverse(VA);
      }
      if (VB >= 0 ){
        dirVB =  1;
        driver.motorBForward(VB);
      }
      else{
        dirVB = -1;
        driver.motorBReverse(VB);
      }
    }
  }
  else
    if (statoRun == 5){// con una ruota ferma ruota su se stesso

      if (direzione == AVANTI){
        VB = 0;
        VA = motorSpeed;
        dirVA = 1;
        dirVB = 0;
        driver.motorBForward(VB);
        driver.motorAForward(VA);
      }
      else{
        VA = 0;
        VB = motorSpeed;
        dirVB = 1;
        dirVA = 0;
        driver.motorBForward(VB);
        driver.motorAForward(VA);
      }
    }
  else
    if(statoRun == 6){// ruota su se stesso con ruote in direzioni opposte
      if (direzione == AVANTI){
        VA = motorSpeed;
        VB = motorSpeed;
        dirVB = -1;
        dirVA =  1;
        driver.motorAForward(motorSpeed);
        driver.motorBReverse(motorSpeed);
      }
      else{
        VA = motorSpeed;
        VB = motorSpeed;
        dirVB =  1;
        dirVA = -1;
        driver.motorBForward(motorSpeed);
        driver.motorAReverse(motorSpeed);
      }
    }

    // informazione per lettura encoder
  if (VA == 0)  VA_zero = 1;
  else      VA_zero = 0;
  if (VB == 0)  VB_zero = 1;
  else      VB_zero = 0;

}

/**
  @brief
*/
void updatePosition(void){

static long SxCnt_k_1 = 0;  // valore cnt a k-1
static long DxCnt_k_1 = 0;  // valore cnt a k-1
static long dDxCnt, dSxCnt;   // delta cnt
static long letturaDx;      // congelo cnt
static long letturaSx;      // congelo cnt
static float deltaC;      // delta cnt
static int  counter = 0;


	// valore complessivo: usato temporaneamente
	odometro = (odometroDxCnt + odometroSxCnt)*GIRO_RUOTA;

	// calcolo evoluzione nel periodo

	// congelo le letture per lavorare su valori coerenti

	noInterrupts();
	letturaDx= odometroDxCnt;
	letturaSx= odometroSxCnt;
	interrupts();

	dDxCnt   = letturaDx - DxCnt_k_1;       // delta sx e dx in count
	dSxCnt   = letturaSx - SxCnt_k_1;

	// esporto velocità encoder
	spdDxCnt = dDxCnt;
	spdSxCnt = dSxCnt;

	deltaC   = (dDxCnt + dSxCnt)*GIRO_RUOTA;// avanzamento del centro nel periodo in mm

	DxCnt_k_1= letturaDx;             // memoria per prossimo ciclo
	SxCnt_k_1= letturaSx;

	// integro teta
	// delta_teta è la velocità angolare
	delta_teta =((float)dDxCnt*GIRO_RUOTA_DX - (float)dSxCnt*GIRO_RUOTA_SX)*2.0/BASELINE;
	teta  += delta_teta;

	// integro posizioni
	if (modoGuida == 3) {
		xpos    +=  deltaC*cos(tetaCompass);
		ypos    +=  deltaC*sin(tetaCompass);
	}
	else{
		xpos    +=  deltaC*cos(teta);
		ypos    +=  deltaC*sin(teta);
	}
	//  getSensorReading(); // ID_009

	/* Get a new sensor event */
	counter ++;

	if (bussola) mag.getEvent(&event);

	// monitor dati
	if (monitorDati && (counter >=4) ){
		counter = 0;
		//risposta  = "dDxCnt; dSxCnt; deltaC; teta; xpos; ypos; i_part;X;Y;Z;"
		risposta  = "mon;";
		risposta += (dDxCnt);
		risposta += (";");
		risposta += (dSxCnt);
		risposta += (";");
		risposta += (deltaC);
		//    risposta += (actualTetaRef);
		risposta += (";");
		risposta += (teta);
		risposta += (";");
		risposta += (xpos);
		risposta += (";");
		risposta += (ypos);
		risposta += (";");
		risposta += (i_part);// raggiorSterzo
		risposta += (";");
		/* Display the results (magnetic vector values are in micro-Tesla (uT)) */

		risposta += event.magnetic.x;
		risposta += ";";
		risposta += event.magnetic.y;
		risposta += ";";
		risposta += event.magnetic.z;
		risposta += ";";
		//    dato += "uT";
		sendAnswer2(port);
	}

}



//ID_002 *********************ale

  void sendAnswer2(char port){
    //*********ale
    ESPserial.println("!"+risposta+"?");
    //Serial.println   ("!"+risposta+"?"); ID_010
    //*********ale
    risposta="";
  }

/** @brief richieste: 
		viene fatta una richiesta. riorna una o più variabili in risposta
    @param comando

    p: 
		risposta = "pos:"			+\
		String(millis())		+";"+\
		String(xpos)			+";"+\ 
		String(ypos)			+";"+\
		String(teta) 			+";"+\
		String(tetaCompass) 	+";"+\
		String(statoRun) 		+";"+\
		String(raggiorSterzo) 	+";"+\
		String(errore)			+";"+\
		String(vl53dist)		+";"+\
		String(inputString.substring(2)) ;

*/
void richieste(void)
{
static float x, y;
        Serial.println("richiesta");
        Serial.println(inputString);
		
        switch (char(inputString[1])) {
          case 'a':
              risposta = "a: " + String(xpos);
            break;

          case 'b':
              risposta = "b: " + String(ypos);
            break;

          case 'c':
              risposta = "c: " + String(teta);
            break;


          case 'd':
              risposta = "d: " + String(odometro);
            break;

          case 'e':
              risposta = "e: " + String(errore);
            break;

          case 'f':
              risposta = "f: ";   // ID_009
              risposta += event.magnetic.x;
              risposta += ";";
              risposta += event.magnetic.y;
              risposta += ";";
              risposta += event.magnetic.z;
            break;

          case 'g':
              risposta = "g: " + String(tetaCompass);
            break;

          case 'h':
              risposta = "h-V FW: " + String(V_FW_ATMEGA);
            break;

			case 'p':
				if (monitorDati) return;
				digitalWrite(LED2, !digitalRead(LED2));
				
				risposta = "pos:"			+\
				String(millis())		+";"+\
				String(inputString.substring(2)) +"+"+\
				String(xpos)			+";"+\ 
				String(ypos)			+";"+\
				String(teta) 			+";"+\
				String(tetaCompass) 	+";"+\
				String(statoRun) 		+";"+\
				String(raggiorSterzo) 	+";"+\
				String(errore)			+";"+\
				String(vl53dist);
				break;

			case 'q':
              // eseguo una scansione
              int range_ang1;
              int range_ang2;
              int passo_ang;

              // ricevo una stringa da sezionare
              range_ang1 = String(inputString.substring(2, 5)).toInt();
              range_ang2 = String(inputString.substring(5, 8)).toInt();
              passo_ang  = String(inputString.substring(8,10)).toInt();

              // limito il movimento causa limiti servo
              if (range_ang1 > 170) range_ang1 = 170;
              if (range_ang2 > 170)   range_ang2 = 170;
              if (range_ang1 < 10)  range_ang1 = 10;
              if (range_ang2 < 10)  range_ang2 = 10;

              // gestisce scan in accordo alla direzione
              if ( range_ang1 > range_ang2){

                passo_ang=passo_ang*-1;

                for (int ang=range_ang1; ang>=range_ang2; ang=ang+passo_ang)
                {
                  //settare angolo pan
                  servoPan.write ( ang);
                  delay(200);
                  risposta = lidarMisuraAmb(10,ang) ;
                  sendAnswer2(port);
                }
              }
              else{
                for (int ang=range_ang1; ang<=range_ang2; ang=ang+passo_ang){
                  //settare angolo pan
                  servoPan.write ( ang);
                  delay(200);
                  risposta = lidarMisuraAmb(10,ang) ;
                  sendAnswer2(port);
                }
              }
              risposta = "echf";
              sendAnswer2(port);
            break;

          case 'r':
              risposta = "r: " + String(statoRun)+";"+String(inputString.substring(2)); // ID_008
            break;

          case 's':
              risposta = "s: " + String(raggiorSterzo, 3);
            break;

          case 'v':
              risposta = "v: " + String(motorSpeed);
            break;

          case 'w':
              risposta = "w: " + String(VB);
            break;

          case 'z':
              risposta = "z: " + String(VA);
            break;

          case '2':
              risposta = "Sx: " + String(odometroSxCnt);
            break;

          case '3':
              risposta = "Dx: " + String(odometroDxCnt);
            break;
        }

    sendAnswer2(port);


  }


/** @brief comandi: i comandi assegnano valori o eseguono funzioni. iniziano con un 3 e hanno la lettera maiuscola, sono seguiti da uno o più numeri.
      non ci sono spazi di separazione.
      i numeri, se non diversamente indicato, sono floating point, la virgola è data col '.', es 3.14

  @param comando

    Axxx: Alfa. assegna Alfa, la direzione del robot. xxx è l'angolo in radianti, es 3.14. La direzione zero è definita all'accensione del robot. E' la direzione in avanti. Coincide con l'asse x.
        Alfa è positivo in senso antiorario.

    Cxxx: assegna "raggiorSterzo" in [m]. La variabile è usata nella movimento R2, stabilisce il raggio della circonferenza su cui ruota il robot.

    Dxxx: Distance. La distanza che viene percorsa nel prossimo Run espressa in [mm]. La distanza è sempre incrementale.

    Ex: EEprom.  Esegue operazioni su dei parametri di taratura. Vedi procedura DataEEprom.

      E0 SCRIVI i parametri in E2prom,
      E1 LEGGI i parametri in E2prom,
      E2 rispristina in valori di DEFAULT,
      E3 mostra i parametri CORRENTI,
      E4 carica i valori di DEFAULT per il modello ARI02,
      E5 carica i valori di DEFAULT per il modello ARI03,

    Fnxxx: imposta dei parametri del robot. "n" indica quale parametro, "xxx" è il valore.

      F0xx ED
      F1xx ED_BASE
      F2xx BASELINE   mm
      F3xx GIRO_RUOTA   mm  = sviluppo ruota[mm]/(4*ppr)
      F4xx Divisore lidar

      N.B. questi valori vanno attivati con un 3E3

    Gxxx: "Guide Mode". definisce il modo di guida nel run "R4". Il teta di feedback, tetaMisura, può provenire dall'odometria, dalla bussola, o altri mix possibili.
        Nel codice sotto teta è l'angolo ricavato dagli encoder.

        Il modo zero si affida alle misure fatte dagli encoder.
        Il modo 1 e 3 ricavano il teta direttamente dalla bussola. Nel modo 3 anche le posizioni x e y
        sono calcolate dal teta della bussola.
        con il modo 2 è possibile imporre l'angolo corrente come angolo zero. Altrimenti la direzione è
        assoluta come determinata dalla bussola.

        if (modoGuida == 0) tetaMisura = teta;      // ID_001
        if (modoGuida == 1) tetaMisura = tetaCompass; // ID_009
        if (modoGuida == 3) tetaMisura = tetaCompass; // ID_009
        if (modoGuida == 2) {
          tetaCompass = 0;  // definisco la direzione corrente come zero.
          modoGuida   = 1;
          }

        in updatePosition integro posizioni in funzione del modoGuida attivo

        if (modoGuida == 3) {
          xpos    +=  deltaC*cos(tetaCompass);
          ypos    +=  deltaC*sin(tetaCompass);
        }
        else{
          xpos    +=  deltaC*cos(teta);
          ypos    +=  deltaC*sin(teta);
        }

    Knxx:   assegna i guadagni
        K0 kpTeta   il guadagno proporzionale kp usato nel modo Run 4
        K1 kiTeta
        K2 kp_guida il guadagno proporzionale kp usato nel modo Run 1 e 3 (sensore di distanza laterale)
        K3 kd_guida il guadagno derivativo    kd usato nel modo Run 1 e 3 (sensore di distanza laterale)

    H0:   Homing, assegna lo posizione corrente (x, y, teta) = (0, 0, 0)

    Ixxx;   Integral part. assegna il guadagno della parte integrale kiTeta usato nel modo Run 4.

    Lx:   Led. puntatore a Led acceso, x=1, o spento x=0.

    Mx:   Monitor. Attiva, 1, o disattiva, 0, l'invio continuo di misure da parte del robot.
        le msiure sono inviate nella routine "updatePosition" ogni 100 ms.
        La stringa ha l'header "mon:"

        if (monitorDati && (counter >=4) ){
        counter = 0;
        risposta  = "mon;";
        risposta += (dDxCnt);
        risposta += (";");
        risposta += (dSxCnt);
        risposta += (";");
        risposta += (deltaC);
        risposta += (";");
        risposta += (teta);
        risposta += (";");
        risposta += (xpos);
        risposta += (";");
        risposta += (ypos);
        risposta += (";");
        risposta += (i_part);// raggiorSterzo
        risposta += (";");

        risposta += event.magnetic.x;
        risposta += ";";
        risposta += event.magnetic.y;
        risposta += ";";
        risposta += event.magnetic.z;
        risposta += ";";
        sendAnswer2(port);
      }

    Nnxxx:  assegna i coefficienti di correzione della bussola.
        N0: ox
        N1: oy
        N2: ky

    Onxx:
        O0: distRef      [cm]. riferimento per guida a distanza fissa nei modi R1 e R2
        O1: distOstacolo [cm]. distanza minima sotto la quale ARI si arresta

    Pxxx:   orienta il servo PAN della testa. xxx è in gradi, 90° guarda in avanti, 0 a sinistra e 180 a destra.
        in alcuni casi, a seconda del servo usato, è bene limitare l'escursione ad esempio tra 10 e 170°.

    Rx:   Run. Definisce in che modo effettuare la corsa "Run". Il run è definito con un obbiettivo (target).
        Il raggiungimento del target termina il run.

        x=4: R4. Il robot si muove sino a percorrere la distanza impostata con "D" nella direzione alfa impostata con "A".
        x=5: R5. Il robot si muove sino a orientarsi nella direzione alfa impostata con "A". La rotazione fa perno su una ruota che rimane ferma.
        x=6: R6. Il robot si muove sino a orientarsi nella direzione alfa impostata con "A". La rotazione avviene con perno al centro del robot con ruote controrotanti.
        x=2: R2. Il robot effettua un percorso circolare con raggio pari al parametro "C". Il raggiorSterzo può anche essere
             imposto direttamente con il comando "S". In questo caso sarà lo scorrimento tra le ruote dx e sx.

        R1 e 3. i modi 1 e 3 fanno viaggiare il robot a una distanza fissa dalla parete laterale. la misura è fatta orinetando il LIDAR.
            suggerimento, angolo lidar 45 rispetto alla parete
			leggere distanza e metterla come distRef (O0)
			mettere distOstacolo maggiore distRef (O1), per esempio + 30 cm
			gli angoli pan (P) sono P45  seguo parete dx con R3
			gli angoli pan (P) sono P135 seguo parete sx con R1

        x=99: provoca l'arresto del movimento

        Il movimento si arresta quando il lidar vede un ostacolo troppo vicino.

        if (lidardist<50 && (statoRun != 5) &&(statoRun != 6) &&(statoRun != 0))
          {
            statoRun      = 99;   // senza rampa
            motorSpeed    = 0;    // annullo rampa
            motorSpeedRef   = 0;
            risposta    = "ostacolo";
            sendAnswer2(port);
          }

    Sxxx: Scorrimento. Imposta direttamente la varibile "raggiDiSterzo". è un numero tra -1 e 1.
        Zero significa scorrimento nullo, cioè le due ruote hanno la stessa velocità comandata.
        vedi procedura "differenziale"

        ponendo s = raggiorSterzo abbiamo che le velocita' alle ruote valgono:

          VA = motorSpeed*(1-s)
          VB = motorSpeed*(1+s)

    Txxx:   orienta il servo TILT della testa. xxx è in gradi, 90° guarda in avanti, 0 a sinistra e 180 a destra.
        in alcuni casi, a seconda del servo usato, è bene limitare l'escursione ad esempio tra 10 e 170°.

    Vxxx: Velocità. Definisce la velocità del robot. é il valore assegnato al PWM.

    Zxxx:   definisce MAX_S. è il limite dello scorrimento (positivo e negativo) applicabile dal controllo.


*/
void comandi()
{
static float x, y;


//          x==(inputString.substring(2)).toInt();
    //with only one parameter looks for a given substring from the position given to the end of the string.
    x = (inputString.substring(2)).toFloat();
    /*
    Serial.print  ("comandi- ch[1]: ");
    Serial.print  (inputString[1]);
    Serial.print  (",xstr: ");
    Serial.print  (inputString.substring(2));
    Serial.print  (",x: ");
    Serial.println(x);
    */

    switch (char(inputString[1])) {
      case 'A':
			tetaRef  = x*3.14/180.0;
			risposta = "A: " + String( tetaRef, 3);
        break;

      case 'B':
			//sensore_ost = x;
			risposta = "B: libero";// + String(sensore_ost);
        break;

      case 'C':
		if (abs(x) < 1000)
			raggiorSterzo = LAGHEZZA_A_MEZZI/x;
		else
			raggiorSterzo = 0.0;
		
		risposta = "S: " + String(raggiorSterzo, 3);
        break;

      case 'D':
          distanza += x;
          risposta = "D: " + String( distanza, 3);
        break;


      case 'E':
          if (x == 0.0){
            DataEEprom(SCRIVI);
            risposta = "E: SCRIVI";
          }
          else if (x == 1.0){
            DataEEprom(LEGGI);
            risposta = "E: LEGGI";
          }
          else if (x == 2.0){
            DataEEprom(DEFAULT);
            risposta = "E: DEFAULT";
          }
          else if (x == 3.0){
            DataEEprom(3.0);
            printDatiCalibrazione();
          }
          else if (x == 4.0){
            DataEEprom(4.0);
            risposta = "E: DEFAULT ARI02";
          }
          else if (x == 5.0){
            DataEEprom(5.0);
            risposta = "E: DEFAULT ARI03";
          }
          else  risposta = "E0: SCRIVI, E1 LEGGI, E2 DEFAULT, E3 CORRENTI, E4 DEFAULT ARI02, E5 DEFAULT ARI03";
        break;

      /* ID_010
        la seconda cifra è un numero cosi
        che l'assegnamento di x all'inizio vada sempre bene
        mantiene la compatibilita
        F0xx ED
        F1xx ED_BASE
        F2xx BASELINE
        F3xx GIRO_RUOTA
        F4xx Divisore lidar

      */
      case 'F':
        x = (inputString.substring(3)).toFloat();
        switch (char(inputString[2])) {
          case '0':
            ED = x;
            risposta = "F0_ED: " + String(ED, 6);
            break;
          case '1':
            ED_BASE = x;
            risposta = "F1_ED_BASE: " + String(ED_BASE, 6);
            break;
          case '2':
            BASELINE = x;
            risposta = "F2_BASELINE: " + String(BASELINE, 6);
            break;
          case '3':
            GIRO_RUOTA = x;
            risposta = "F3_GIRORUOTA: " + String(GIRO_RUOTA, 6);
            break;
          case '4':
            divLidar = x;
            risposta = "div lidar: " + String(divLidar, 6);
            break;
        }
        break;


      case 'G':   // ID_009
          modoGuida = x;
          risposta = "G: " + String(modoGuida);
        break;

      // K0 kpTeta
      // K1 kiTeta
      // K2 kp_guida
      // K3 kd_guida
      case 'K':
        x = (inputString.substring(3)).toFloat();
        switch (char(inputString[2])) {
          case '4':
//            kp = x;
//            risposta = "kp: " + String(kp, 6);
            break;
          case '0':
            kpTeta = x;
            risposta = "kpTeta: " + String(kpTeta, 6);
            break;
          case '1':
            kiTeta = x;
            risposta = "kiTeta: " + String(kiTeta, 6);
            break;
          case '2':
            kp_guida = x;
            risposta = "kp_guida: " + String(kp_guida, 6);
            break;
          case '3':
            kd_guida = x;
            risposta = "kd_guida: " + String(kd_guida, 6);
            break;
        }
        break;

      case 'H':
        xpos = ypos = teta = 0.0;
        risposta = "H: ";
      break;

      case 'I':
          kiTeta = x;
          risposta = "I: " + String(kiTeta, 3);
        break;

      case 'L':
          Serial.println(x);
          if (x == 0.0){
            Serial.println("L0");
            digitalWrite(laserPin, LOW);
            digitalWrite(LED1, LOW);
            laser = 0;
          }
          else{
            Serial.println("L1");
            digitalWrite(laserPin, HIGH);
            digitalWrite(LED1, HIGH);
            laser = 1;
          }
          risposta = "L: " + String(x);
        break;

      case 'M':
          monitorDati = x;
          risposta = "M: " + String(monitorDati);
        break;

      case 'N':
        x = (inputString.substring(3)).toFloat();
        switch (char(inputString[2])) {
          case '0':
            ox = x;
            risposta = "N0: " + String(ox, 6);
            break;
          case '1':
            oy = x;
            risposta = "N1: " + String(oy, 6);
            break;
          case '2':
            ky = x;
            risposta = "N2: " + String(ky, 6);
            break;
        }
        break;

      case 'O':
        x = (inputString.substring(3)).toFloat();
        switch (char(inputString[2])) {
          case '0':
              distRef  =  (uint16_t)int(x);
              risposta = "distRef: " + String(distRef);
            break;
          case '1':
              distOstacolo  =  (uint16_t)int(x);
              risposta = "distOstacolo: " + String(distOstacolo);
            break;
        }
        break;

      case 'P':
          panAngle = x;
          risposta = "P: " + String(panAngle);
        break;

      case 'R':
          statoRun = x;
          risposta = "R: " + String(statoRun);
        break;

      case 'S':
          raggiorSterzo = x;
          risposta = "S: " + String( raggiorSterzo, 3);
        break;

      case 'T':
          tiltAngle = 180 - x;    // servomotore girato
          risposta = "T: " + String(x);
        break;

      case 'V':
          motorSpeedValue = x;
          risposta = "V: " + String(motorSpeedValue);
        break;

      case 'Z':
          MAX_S = x;
          risposta = "Z: " + String(MAX_S, 3);
        break;

      case '1':
          VA = x;
          risposta = "1: " + String(x, 3);
        break;

        default:
            risposta="mica capito";
        }
        sendAnswer2(port);
  }

/*!
    @brief  quando arriva una stringa da ESP la processa. Il protocollo usato è in ASCII permette di inviare comandi o richieste.
      Il primo carattere indica se richiesta (1) o comando (3).

      I caratteri successivi vengono passati alle routine dedicate che eseguono il comando e preparano la stringa "risposta".

      questa viene ritornata dalla funzione sendAnswer2
*/

void getCmd2(void){
static float x, y;
static int inByte;

  if (inputString!="")
  {
    Serial.print("getCmd2: ");
    Serial.println(inputString);
    switch (inputString.charAt(0)) {
      case '1':
      richieste();
      break;
      case '3':
      comandi();
      break;
      default:
      risposta="mica capito";
      sendAnswer2(port);    // l'argomento dovrebbe essere la porta su cui rispondere

    }

    // fine comandi
    inputString="";
  }

}


/*!
    @brief  riceve i caratteri dalla seriale dell'ESP. Il char '!' è l'inizio stringa, il '?' è il terminatore della stringa.
  il terminatore della stringa è ?

  All'arrivo dello start la stringa pulita e inizia l'accumolo dei caratteri

  All'arrivo del terminatore la stringa viene resa disponibile in "inputString"

  ! *flush*

  ? *terminatore*


  N.B. il codice su esp oltre a dispacciare i pacchetti giunti dal WiFi verso Arduino effettuata anche un filtraggio e una generazione di richieste.
  filtraggio: I pacchetti che iniziano !> sono destinati all'esp e sono quindi trattati sull'esp stesso.
  generazione di richieste: a tempo l'esp richiede a Arduino delle informazioni che poi invia al client.

  per dettagli su questa parte vedi codice esp..

*/
void rSeriale(void)
{
static unsigned long time, cmdTime;
static char c;
static int numero = 0;
  port = 0;
  if (ESPserial.available() ==0) return;

  c = (char)ESPserial.read();
  //Serial.write(c);

  // arrivano e non so da dove
    if (c == '\n' || c == '\r')   return;


  numero ++;

    if (c=='!')   // flush stringa
    {
      inputStringTmp = "";
      ESPserial.flush();
      okcomm  = 1;
      numero  = 0;
      Serial.println("flush");
    }
    if (c != '?' && c != '!' && okcomm==1)
    {
    cmdTime = millis();
      inputStringTmp += c;
    }
    if (c == '?') {
    inputString=inputStringTmp;
    Serial.println(inputString);
    Serial.print(millis());
    okcomm=0;
    }
}


/*!
    @brief  gestisce dei valori sulle E2prom. I vengono letti e scritti nello stesso ordine. La procedura gestisce anche il valore di default.
        letti i valori si procede ad aggiornare i parametri dipendenti dai valori assegnati.
      Questa procedura viene chiamata al termine dell' startUp con comando LEGGI. Vengono quindi attivati i parametri salvati in EEprom.


  @param comando

    i comandi sono:

    SCRIVI  0
    LEGGI   1
    DEFAULT 2
    ATTIVA  3 (attiva e ricalcola i valori GIRO_RUOTA DX, SX etc

    temporaneamente i valori 4 e 5 caricano dei valori di default calibrati su due macchine. 4 ARI_02, 5 ARI_03

*/

void DataEEprom(char comando){
/*
assegnamento dati in e2prom
0 ED_BASE   f
1   ED      f
2   BASELINE  f
3   giroRuota f
4 kpTeta    f
5 ox      f
6 oy      f
7 ky      f
8 kiTeta    f
9 kp_guida  f
10  kd_guida  f

ID_010
*/
int eeAddress = 0;   //Location we want the data to be put/get.

char endList = 0;
int  i = 0;

  while (endList == 0){

    switch (i) {

      case 0: //
          if (comando == SCRIVI)      EEPROM.put(eeAddress, ED);
          if (comando == LEGGI)     EEPROM.get(eeAddress, ED);
          if (comando == DEFAULT)     ED = 1.0;

            eeAddress += sizeof(float); //Move address to the next byte after float 'f'.
        break;

      case 1: //
          if (comando == SCRIVI)      EEPROM.put(eeAddress, ED_BASE);
          if (comando == LEGGI)     EEPROM.get(eeAddress, ED_BASE);
          if (comando == DEFAULT)     ED_BASE = 1.0;

            eeAddress += sizeof(float);
        break;

      case 2: //
          if (comando == SCRIVI)      EEPROM.put(eeAddress, BASELINE);
          if (comando == LEGGI)     EEPROM.get(eeAddress, BASELINE);
          if (comando == DEFAULT)     BASELINE = 220.0;

            eeAddress += sizeof(float);
        break;

      case 3: //
          if (comando == SCRIVI)      	EEPROM.put(eeAddress, GIRO_RUOTA);
          if (comando == LEGGI)     	EEPROM.get(eeAddress, GIRO_RUOTA);
          if (comando == DEFAULT)     	GIRO_RUOTA = 1.995;

            eeAddress += sizeof(float);
        break;

      case 4: //
          if (comando == SCRIVI)      	EEPROM.put(eeAddress, kpTeta);
          if (comando == LEGGI)       	EEPROM.get(eeAddress, kpTeta);
          if (comando == DEFAULT)     	kpTeta = 8.0;

            eeAddress += sizeof(float);
        break;

      case 5: //
          if (comando == SCRIVI)      	EEPROM.put(eeAddress, ox);
          if (comando == LEGGI)			EEPROM.get(eeAddress, ox);
          if (comando == DEFAULT)     	ox = 0.0;

            eeAddress += sizeof(float);
        break;

      case 6: //
          if (comando == SCRIVI)      	EEPROM.put(eeAddress, oy);
          if (comando == LEGGI)     	EEPROM.get(eeAddress, oy);
          if (comando == DEFAULT)     	oy = 0.0;

            eeAddress += sizeof(float);
        break;

      case 7: //
          if (comando == SCRIVI)      	EEPROM.put(eeAddress, ky);
          if (comando == LEGGI)     	EEPROM.get(eeAddress, ky);
          if (comando == DEFAULT)     	ky = 1.0;

            eeAddress += sizeof(float);
        break;

      case 8: //
          if (comando == SCRIVI)      EEPROM.put(eeAddress, kiTeta);
          if (comando == LEGGI)     EEPROM.get(eeAddress, kiTeta);
          if (comando == DEFAULT)     kiTeta = 0.2;

            eeAddress += sizeof(float);
        break;

      case 9: //
          if (comando == SCRIVI)      EEPROM.put(eeAddress, kp_guida);
          if (comando == LEGGI)     EEPROM.get(eeAddress, kp_guida);
          if (comando == DEFAULT)     kp_guida = 0.5;

            eeAddress += sizeof(float);
        break;

      case 10:  //
          if (comando == SCRIVI)      EEPROM.put(eeAddress, kd_guida);
          if (comando == LEGGI)     EEPROM.get(eeAddress, kd_guida);
          if (comando == DEFAULT)     kd_guida = 40.0;

            eeAddress += sizeof(float);
        break;

      case 11:  //
          if (comando == SCRIVI)      EEPROM.put(eeAddress, divLidar);
          if (comando == LEGGI)     EEPROM.get(eeAddress, divLidar);
          if (comando == DEFAULT)     divLidar = 1.0;

            eeAddress += sizeof(float);
        break;

      case 12:  //
          endList = 1;
        break;


    }

    i++;
  }

  // aggiornamento parametri dipendenti


  // valori default ARI 02
  if(comando == 4){
    GIRO_RUOTA  = 2.728;
    ED_BASE   = 1.0;
    ED      = 0.99347735;
    BASELINE    = 129.826;
  }

  // valori default ARI 03
  if(comando == 5){
    GIRO_RUOTA  = 1.131;
    ED_BASE   = 1.0;
    ED      = 0.95;
    BASELINE  = 130.0;
  }

  GIRO_RUOTA_SX   = GIRO_RUOTA*2.0/(1.0 +      ED*ED_BASE );
  GIRO_RUOTA_DX   = GIRO_RUOTA*2.0/(1.0 + 1.0/(ED*ED_BASE));

  LAGHEZZA_A_MEZZI= BASELINE/2000.0;
  printDatiCalibrazione();

}


void printDatiCalibrazione(void){

  risposta  = "ED:      " + String(ED         , 6) + '\n';
  risposta += "ED_BASE: " + String(ED_BASE      , 6)  + '\n';
  risposta += "BASELINE " + String(BASELINE     , 6)  + '\n';

  risposta += "GIRO_RUOTA:" + String(GIRO_RUOTA     , 6)  + '\n';
//  risposta += "GIRO_RUOTA_SX:   " + String(GIRO_RUOTA_SX    , 6)  + '\n';
//  risposta += "GIRO_RUOTA_DX:   " + String(GIRO_RUOTA_DX    , 6)  + '\n';
//  risposta += "LAGHEZZA_A_MEZZI:" + String(LAGHEZZA_A_MEZZI , 6)  + '\n';
  risposta += "kpTeta:  " + String(kpTeta) + '\n';
  risposta += "kiTeta:  " + String(kiTeta) + '\n';
  risposta += "kp_guida: " + String(kp_guida) + '\n';
  risposta += "kd_guida: " + String(kd_guida) + '\n';
  risposta += "ox: " + String(ox) + '\n';
  risposta += "oy: " + String(oy) + '\n';
  risposta += "ky: " + String(ky) + '\n';
}


/** @brief riporta al controller la distanza letta dal lidar, la risposta ha formato "mis;" + distanza in cm".
  aggiorna anche la il valore "lidardist" ogni 250 ms.

*/
void testOstacoli(){
static int i;
  if (monitorDati) return;


	// 
	if ((millis()-timeLidar) > 1000){
		timeLidar  = millis();
		if (sensore_ost==1){         //ho il laser anteriore attivo

			VL53L0X_RangingMeasurementData_t measure;
			lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
			
			if (measure.RangeStatus != 4) {  // phase failures have incorrect data
			
				vl53dist = measure.RangeMilliMeter/10;
				//risposta = "ost;" + String(measure.RangeMilliMeter/10);
				//sendAnswer2(port);
			}
			else{
				vl53dist=999;
			}
		}
		//risposta	= "mis;" + String(lidarDistance);
		//sendAnswer2(port);
	}
}

/** @brief libreria lettura lidar

*/
String lidar()
{

    // Take one TF Mini distance measurement
     uint16_t dist = tfmini.getDistance()/divLidar;
//     dist = tfmini.getDistance();
//     dist = tfmini.getDistance();
//     dist = tfmini.getDistance();
     uint16_t strength = tfmini.getRecentSignalStrength();
     //Serial.print("forza");
    //Serial.println(strength);
    return String(dist);

}

/** @brief legge il lidar e prepara la stringa di risposta.

  inizializza la stringa di risposta come "echo-"+String(ang)+"-";

  effettua dapprima 10 letture a vuoto,

  appende "numeroMisure" misure alla risposta ognuna separata da "-"


  @param numeroMisure su cui viene
  @param ang
*/
String lidarMisuraAmb(int numeroMisure,int ang ){
float  distance = 0;
String risp   = "echo-"+String(ang)+"-";

  for (int i = 0; i < numeroMisure+10; i++){
    // Read in times pulse
    if(i<10){// svuoto il lidar
      delay(25);  // tempo assestamento pan
      lidar();
    }
    else{
      delay(25);  // tempo assestamento pan
      risp += lidar();
      risp += "-";
    }
  }
  risp += "+";
  return(risp);
}


void displaySensorDetails(void)
{
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void getSensorReading(void){
static String dato;
  /* Get a new sensor event */
  mag.getEvent(&event);

  /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
  dato  = "X;Y;Z;";
  dato += event.magnetic.x;
  dato += ";";
  dato += event.magnetic.y;
  dato += ";";
  dato += event.magnetic.z;
  dato += ";";
  dato += "uT";
  /*
  Serial.print("X;Y;Z;");
  Serial.print(event.magnetic.x); Serial.print(";");
  Serial.print(event.magnetic.y); Serial.print(";");
  Serial.print(event.magnetic.z); Serial.print(";");
  Serial.println("uT");
*/
  /* Note: You can also get the raw (non unified values) for */
  /* the last data sample as follows. The .getEvent call populates */
  /* the raw values used below. */
  // Serial.print("X Raw: "); Serial.print(mag.raw.x); Serial.print("  ");
  // Serial.print("Y Raw: "); Serial.print(mag.raw.y); Serial.print("  ");
  // Serial.print("Z Raw: "); Serial.print(mag.raw.z); Serial.println("");
}


/*!
    @brief  Gets the most recent sensor values and transform in angle.

            This function reads only x and y magnetometer values. The coefficients for correction are applied.

      the output is tetaCompass which is a global variable.

      ox: offet on x axis

      oy: offset on y axis

      ky: gain on y axis


    @param  none
    @return none. The output valie is on tetaCompass global variable
*/

void compass(void){

  xc =  event.magnetic.x + ox;
  yc = (event.magnetic.y + oy)*ky;

//        tetaCompass = atan2 (yc, xc);     // arc tangent of y/x

  // ottengo angolo per integrazione
  // l'angolo andare oltre 2PI e può essere resettato
  // facilmente
  tetaCompassRead = atan2 (yc, xc);
  deltaCompass  = tetaCompassRead - tetaCompass_prev;
  tetaCompass_prev= tetaCompassRead;
  // considero che tra due campioni non ci siano oltre 3.28 rad
  // se ci sono c'è stato lo scaravolto dell'angolo
  if (deltaCompass >=  3.28) deltaCompass -= 6.28;
  if (deltaCompass <= -3.28) deltaCompass += 6.28;

  // meno per avere angolo positivo in senso antiorario
  // concorde con encoder

  tetaCompass -= deltaCompass;
}


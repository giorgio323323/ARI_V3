#define V_FW_ESP8266	"1.00.01"
/*@serverTcp_IR_Receiver_o1.ino
 * 	stuffcube.wordpress.com


	16dic18 1.00.01
		aggiunta versione
		si legge con >v
		
 
	04mag18
		update con udp discovery

		
	13ago18
		aggiunta interfaccia per configurazione WiFi
		messo un solo sensore IR

		dichiarando diversi ricevitori su pin diversi non sembrava funzionare
		ho inserito un circuito che mi permette di selezionare il ricevitore desiderato

		da fermo
		a comando faccio n letture, ritorno la percentuale di letture con A9 o C9
		su ogni sensore.

  28apr18
		porting su ESP (scheda wemos D1 R2 & mini)
		aggiunte librerie, vedi esempio ESP8266_IRreceive demo
		#include <IRremoteESP8266.h>
		#include <IRrecv.h>
		#include <IRutils.h> 
		aggiunto cast a long su Serial.println((long)results.value, HEX);
		variata mappatura pin
		merge conserertcp_arianna_2.ino    

	06apr18
		rimosso lettura continua poichè ogni tanto partiva da sola
		pezza


	29dic17 aggiunta gestione barriera di arrivo
		i codici letti possono essere diversi
		la lettura viene fatta da fermo. QUindi la barriera è occupata o no.
		il codice ricevuto è unico nella sessione
		ritorno il codice ricevuto e il numero di occorrenze
	  
	03dic17		IRrecvDemo_1.ino
		il comando R1 effettua una lettura multipla dei tre sensori
		e ritorna i risultati.
		è coordinato con IRsendDemo in docking folder

	23nov17 	IRrecv_00



 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 * 	
Starts listening for an IR code.
int irrecv.decode(&results)

Returns 1 if there is a received code available, and 0 otherwise.
The fields of results are:
decode_type 	NEC, SONY, RC5, RC6, or UNKNOWN
value 			the received code value
bits 			The number of bits received
rawbuf[] 		Raw durations
rawlen 			Number of records in rawbuf	


 */
#define COMPILA_PER_ESP 1

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

//for LED status
#include <Ticker.h>
Ticker ticker;

void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}



 
char* 		ssid;
char* 		password;
 
#ifdef COMPILA_PER_ESP 
	#ifndef UNIT_TEST
		#include <Arduino.h>
	#endif
	
	#include <IRremoteESP8266.h>
	#include <IRrecv.h>
	#include <IRutils.h>
#else
	//#include <IRremote.h>		// 
	#include <InfraredRemote.h>   // v3  https://github.com/z3t0/Arduino-IRremote/tree/v3
#endif

#define TAG_SX		0xA90
#define TAG_DX		0xC90

#define TAG_SX_NEAR	0x9A0
#define TAG_DX_NEAR	0x9C0

#define TAG_NOT_RECIVED	9999

#define LED			LED_BUILTIN //13


WiFiServer server(81);
WiFiClient serverClient;


//udp discovery
unsigned int udpport = 8888;
WiFiUDP UDP;
//udp discovery


float intervallopos	=	700.0;
float ultimopos		=	5000.0;
int   okcom1		=	0;
int   okcom2		=	0;
int   debug;
String buffr;
String _risposta = "";
char  appoggio[4];

int sensore 	= 1;


// mappatura pin di I/O
//int RECV_PIN  	= 14;    // gpio14, pin wemos D5
int RECV_PIN  	= 4;    // gpio4, pin wemos D2

IRrecv 	irRecv (RECV_PIN );

decode_results results;

char i;

String msg, tagCodeSx, tagCodeDx;
int  p_letture 			= 20;
long p_timeBtwReading 	= 176;	// ms
char verbose 			= 0;

int cntTagDx;
int cntTagSx;

int leggiSingle;

// prototype declaration
void mostraHelp	(void);
void getCmd		(void);
void readSensor	(char sensore);
void scanSensor	(void);
void getCmdFromClient(void);
void getFromArduino(void);


/**
	@brief gets called when WiFiManager enters configuration mode
*/
void configModeCallback (WiFiManager *myWiFiManager) {
	//Serial.println("Entered config mode");
	//Serial.println(WiFi.softAPIP());
	//if you used auto generated SSID, print it
	//Serial.println(myWiFiManager->getConfigPortalSSID());
	
	//entered config mode, make led toggle faster
	ticker.attach(0.2, tick);
}

/*********************************************/
/*
 * 
 */
void setup()
{
	Serial.begin(115200);

	pinMode(LED  ,  OUTPUT); 
	digitalWrite(LED, HIGH);

	// start ticker with 0.5 because we start in AP mode and try to connect
	ticker.attach(0.6, tick);

  
  
	Serial.println();  
	Serial.print("serverTcp_IR_Recerver_01.ino. V_FW_ESP8266: ");  
	Serial.println(V_FW_ESP8266);  

	irRecv.enableIRIn(); // Start the receiver

//	Serial.println("Enabled IRin");

//	mostraHelp();

	//WiFiManager
	//Local initialization. Once its business is done, there is no need to keep it around
	WiFiManager wifiManager;
	//reset settings - for testing
	//wifiManager.resetSettings();

	//set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
	wifiManager.setAPCallback(configModeCallback);

	//fetches ssid and pass and tries to connect
	//if it does not connect it starts an access point with the specified name
	//here  "AutoConnectAP"
	//and goes into a blocking loop awaiting configuration
	
	if (!wifiManager.autoConnect()) {
		//Serial.println("failed to connect and hit timeout");
		//reset and try again, or maybe put it to deep sleep
		ESP.reset();
		delay(1000);
	}	
	

	debug 					= 0;
	uint8_t i 				= 0;
	char connectionActive 	= 0;
	uint8_t tryConnectionState = 0;
	uint8_t indiceReti	 	= 0;
	
	
	server.begin();
	server.setNoDelay(true);

	if (verbose){
		Serial.print("Connected to: ");
		Serial.println(WiFi.localIP());
	}
	ticker.detach();
  //keep LED on
	digitalWrite(LED, LOW);
	
	if (verbose){
		Serial.println("end init");
	}
}

			



void loop() {

	//uint8_t i;
	//check if there are any new clients
	if (server.hasClient()){
		if (!serverClient || !serverClient.connected()){
			serverClient = server.available();
			
			if (verbose) Serial.println("New client");
	
		}
	}
	if (!serverClient.connected()) {
		delay(500);
		//Serial.println("connessione chiusa");
		  IPAddress broadcastIp = WiFi.localIP();
		// se non ci sono client connessi
		// invia nome sulla rete per essere trovato
		broadcastIp[3] = 255;
		
		if (verbose)	Serial.println(WiFi.localIP());
		
		UDP.beginPacket(broadcastIp, udpport);
		UDP.print("arianna:");
		UDP.endPacket();
		serverClient.stop();
	}
	else{
		getCmdFromClient();
		getFromArduino();
    } 

	if (irRecv.decode(&results)) {
		irRecv.resume();
	}
	delay(10);	// lascia tempo libero
}// end loop

/** @brief 	legge e conta i segnali ricevuti sul sensore
			ritorna una stringa con quali e quanti;

*/
void scanSensor(void){
int i;

		cntTagSx = 0; 
		cntTagDx = 0;
		
		tagCodeSx = TAG_NOT_RECIVED;
		tagCodeDx = TAG_NOT_RECIVED;
		
		
		digitalWrite(LED, HIGH);
		for (i = 0; i < p_letture; i++){
			if (irRecv.decode(&results)) {
				irRecv.resume(); // Receive the next value

				if (verbose){
					Serial.print  ("decode_type; ");
					Serial.println(results.decode_type);
					Serial.print  ("bits       ; ");
					Serial.println(results.bits);
					Serial.print  ("rawLen     ; ");
					Serial.println(results.rawlen);
					Serial.print  (msg);
					Serial.println((long)results.value, HEX);
					Serial.print  (msg);
					Serial.println((long)results.value, DEC);
				}
			
				if ((results.value == TAG_SX) | (results.value == TAG_SX_NEAR)){
					cntTagSx++;
					if (results.value == TAG_SX) tagCodeSx = TAG_SX;
					else 						 tagCodeSx = TAG_SX_NEAR;
				}
				if ((results.value == TAG_DX) | (results.value == TAG_DX_NEAR)){
					cntTagDx++;
					if (results.value == TAG_DX) tagCodeDx = TAG_DX;
					else 						 tagCodeDx = TAG_DX_NEAR;
				}
			}
			delay(p_timeBtwReading);
		}

		leggiSingle = 0;
		// ritorna risposta
		if (verbose) Serial.println("grand total");

		// la lettura della risposta finisce con \name
		// devo inviarlo solo alla fine
		msg = "!ir:";
		msg += tagCodeSx;
		msg += "; ";
		msg += String(cntTagSx) ;

		if (verbose) Serial.println(msg);
		else msg += ";";
		
		msg += tagCodeDx;
		msg += "; ";
		msg += String(cntTagDx) ;
		digitalWrite(LED, LOW);
}


static String risposta;


	

/** @brief	riceve comandi da client
	la forma è !cxxx?
	
	se c == > è un comando per il micro locale ESP
	
	altrimenti è destinato ad arduino

	es:
	
	!>pxxx  cambio tempo di ...
	
	!>dsss..  la stringa sss.. viene passata a espLocale tramite getCmd(buffr)
*/
void getCmdFromClient(){
	  
	//parte di comunicazione via socket
	char buffc;
	
	/*
	//a intervalli regolari chiedo dati a arduino sulla posizione  
	if (serverClient && serverClient.connected()){
		if(millis()-intervallopos>ultimopos){
			ultimopos=millis();
			if (debug==0) Serial.println("!1p?");
		}
	} 
	*/
	//qui mi arrivano le richieste da python
	if(serverClient.available()){
		buffc = serverClient.read();
		//Serial.println(buffc); debug
		
		// arriva carattere inizio comando
		if (buffc  == '!'){     //  //da python mi arriva il ! inizio ? fine come separatore
			buffr="";
			okcom1 = 1;
		}
		// accumulo caratteri
		if (buffc  != '?' && okcom1==1){ 
			buffr += buffc;     // buffr è una stringa
		}
		// arriva terminatore comando
		// viene intepretato
		if (buffc  == '?' ){ 
			okcom1 = 0;
			buffr += buffc;
			// il caratter > indica caratteri al micro locale (ESP)
			if (buffr[1]=='>'){
				
				if(buffr[2]=='d'){  // comando per docking station
					serverClient.print(getCmd(buffr));
				}
				if(buffr[2]=='v'){  // comando per docking station
					serverClient.print(V_FW_ESP8266);
				}
			}
				// altrimenti sono per il mcro sotto (Arduino)
			else {
				Serial.println(buffr);
			}
			buffr="";
		}
	}

}

/*
	riceve stringhe da arduino e quaando complete le invia alcliant
	
*/

void getFromArduino(){
		// dati in arrivo dalla porta seriale
        if(Serial.available() >0){
			digitalWrite(LED, !digitalRead(LED));	// lampeggio led
            char inChar= Serial.read();
            if (inChar  == '!'){     // carattere iniziale
				_risposta = "";
				okcom2 = 1;
            }
        
            if (inChar  != '?' && okcom2==1){ 
              _risposta += inChar;
            }
            if (inChar  == '?' ){ 
              _risposta += inChar;
              okcom2	= 0;
              serverClient.print(_risposta);
              _risposta = "";
            }
        }
}
/*
	help
	mostra 
*/
void mostraHelp(void){
  risposta = "--- elenco comandi ---\n\n";
  risposta +="R1 read single.\n";
  risposta +="Tx p_timeBtwReading. x in [ms].\n";
  risposta +="V  verbose. 0 off, 1 On.\n";
  //return(risposta);
}


/*
	interfaccia linea di comando
	ricevi comandi per IR
*/
String getCmd(String cmd){
static float x, y;
static char smComandi = 0;
static char inCmd;
static unsigned long time, cmdTime;
static char portCmd, inChar;



static int inByte;

        // cmd = !>dCnnnnn?
        // estrai numero e comando 
        cmd.remove(0, 3); // remove "!>d" che significa comando locale 
        cmd.remove(cmd.length(), 1);  //remove ?
        Serial.println(cmd);        //Cnnnnn comando pulito

        inCmd = cmd[0];   // C
        
        x     = cmd.toFloat();
        Serial.print("x: ");
        Serial.println(x);
                
        switch (inCmd) {
    
			case 'H': 
				
				mostraHelp(); // mostraHelp compone risposta
				break;

				case 'R': 
				// modo di funzionamento
				leggiSingle = (int)x;
				scanSensor();							
				risposta = msg; 
				break;

			case 'T': 
				// modo di funzionamento
				p_timeBtwReading = (int)x;
				smComandi = 0;      
				risposta = "p_timeBtwReading: " + String(p_timeBtwReading);
				break;
			case 'V': 
			
				// modo di funzionamento

				verbose = (int)x;
				risposta = "verbose: " + String(verbose);
				break;
			default:
				risposta = "cmd sconosciuto";
				break;
            }
    if (risposta != "")   return(risposta);
    else                  return("ok");
}


/*
	procedura ricerca stazione con modalità di misura su tre posizioni
	la procedura è simile a quella testata via python con tre IR
	in questo caso l'IR è montato sul pan che viene orientato
	si usa un IR con angolo limitato da schermi a circa 60 gradi
	
        counter  = IRcode['CodeDX']['dx']
        counter += IRcode['CodeDX']['sx']
        counter += IRcode['CodeDX']['ct']

        # counter == 0 serve nel caso non veda alcun IR
        if (IRcode['CodeSX']['codice'] == 0xA90) or (counter == 0):
            eDX = IRcode['CodeSX']['dx'] + IRcode['CodeDX']['dx']
            eSX = IRcode['CodeSX']['sx'] + IRcode['CodeDX']['sx']
            eCT = IRcode['CodeSX']['ct'] - IRcode['CodeDX']['ct']

            '''
            quando, durante il processo di ricerca, le letture sul sensore
            centrale sono significative (es > 2, 3 di mabo in codici)
            può essere ragionevole trascurare i laterali.
            '''
            # decidi
            if ((IRcode['CodeSX']['ct'] >=2) and (IRcode['CodeDX']['ct'] >= 2)):
                eSX = eDX = 0

            e   = 4*eSX + eCT - 4*eDX
            alfa += e
            stringa += str(e) + "; "
            stringa += str(alfa) + "\n"

            if (counter == 0):
                deltaSpace = 300
            else:
                deltaSpace = 150
            print ("stato: %s, alfa: %d" % (stato, alfa))
            pass
            # avanza
            cmd.sendCmd('A', alfa)
            ari.angle(alfa)
            cmd.sendCmd('D', deltaSpace)
            ari.forward(deltaSpace)
            cmd.sendCmd('R', 4)
            print('sent R')
            cmd.waitEndRun()

        elif IRcode['CodeSX']['codice'] == 0x9A0:
            print ("arrivato!")
            seekOn = False
	
*/
int proceduraDocking_0(void){

}

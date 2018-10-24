/*
 * IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 *
 *	03dic17
 *	coordinato con IRrecvDemo_1 nel folder docking
 *  28dic17
 *		aggiunta gestione barriera di arrivo e codice di arrivo
 *	04ago18	
 *    porting su esp WEMOS D1 R2       // ID_ESP
 *    come nomi pin uso quelli sulla serigrafia
 */

#include <IRremoteESP8266.h>  // ID_ESP
#include <IRsend.h>           // ID_ESP
#include <IRutils.h>          // ID_ESP

ADC_MODE(ADC_VCC);            // ID_ESP

  
#define LED				LED_BUILTIN

#define LED_BARRIERA		16		// gpio16 pin wemos D0
#define PHOTO_PIN			A0      // ID_ESP
#define IR_LED 				4  		// gpio4  pin wemos D2      // ID_ESP
#define SELECT_IRLED_DX	 	0		// gpio0  pin wemos D3
#define SELECT_IRLED_SX	 	5		// gpio5  pin wemos D1

IRsend irsend(IR_LED);      		// ID_ESP

// definizione condivisa con il ricevitore 
#define TAG_SX		0xA90
#define TAG_DX		0xC90

#define TAG_SX_NEAR	0x9A0
#define TAG_DX_NEAR	0x9C0

//int tag_sx, tag_dx;
unsigned int tag_sx, tag_dx;  // ID_ESP
long tempo;


int TIME_INTERVAL = 20;
int photo_value;

#define FAR				2
#define NEAR			3
#define WAIT4FREEDOM	4

#define HIGHER_BUSY	300

char statoRobot		= FAR;

void setup()
{
	Serial.begin(115200);
	Serial.println();
	Serial.println();
	Serial.println("IRsend02Esp.ino 16Ago18");
	
	Serial.println(D1);	//1
	Serial.println(D2);	// 16
	Serial.println(D3);	// 5
	Serial.println(D4);	// 4
	Serial.println(D5);	// 14

	irsend.begin();

	pinMode(SELECT_IRLED_DX,	OUTPUT); 
	pinMode(SELECT_IRLED_SX,	OUTPUT); 
	pinMode(LED,  				OUTPUT); 
	pinMode(LED_BARRIERA,  		OUTPUT); 

	Serial.println("setup end");
}

void loop() {
	
	
	photo_value = analogRead(PHOTO_PIN);
	Serial.println(photo_value);
	
	switch (statoRobot){
		
		case FAR:
				// far led bliking
				tag_sx = TAG_SX;
				tag_dx = TAG_DX;
				digitalWrite(LED_BARRIERA, !digitalRead(LED_BARRIERA));
				if (photo_value > HIGHER_BUSY){
					tag_sx = TAG_SX_NEAR;
					tag_dx = TAG_DX_NEAR;
					statoRobot = NEAR;
				}

			break;
			
		case NEAR:
				tag_sx = TAG_SX_NEAR;
				tag_dx = TAG_DX_NEAR;
				digitalWrite(LED_BARRIERA, HIGH);
				if (photo_value < HIGHER_BUSY){
					statoRobot = WAIT4FREEDOM;
					tempo = millis();
				}
			break;
			
		case WAIT4FREEDOM:
				if (photo_value > HIGHER_BUSY){
					statoRobot = NEAR;
				}
				if ((millis() - tempo) > 10000) statoRobot = FAR;
	}


	tag_sx = TAG_SX;
	tag_dx = TAG_DX;

	digitalWrite(LED, HIGH);
	digitalWrite(LED_BARRIERA, HIGH);
	

	// select chip and send
	digitalWrite	(SELECT_IRLED_DX, HIGH);
	digitalWrite	(SELECT_IRLED_SX, LOW);

	delay			(TIME_INTERVAL);
	irsend.sendSony	(tag_sx, 12);
	delay			(TIME_INTERVAL);
	
	digitalWrite(LED, LOW);
	digitalWrite(LED_BARRIERA, LOW);

	// select chip and send
	digitalWrite	(SELECT_IRLED_DX, LOW);
	digitalWrite	(SELECT_IRLED_SX, HIGH);

	delay			(TIME_INTERVAL);
	irsend.sendSony	(tag_dx, 12);
	delay			(TIME_INTERVAL);
	
}

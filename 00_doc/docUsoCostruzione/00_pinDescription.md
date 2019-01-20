# Documentazione elettrica

 

| Tabella aggiornamenti |         |                                  |
| --------------------- | ------- | -------------------------------- |
| data                  | autore  | modifica                         |
| 13gen19               | giorgio | portato documento in formato .md |
|                       |         |                                  |
|                       |         |                                  |
|                       |         |                                  |
|                       |         |                                  |
|                       |         |                                  |

##  

vengono descritti i pin dei vari componenti e come vengono usati.



## Wemos nomi pin

Per indirizzare i pin usare il nome della colonna GPIO. I nomi Wemos nella colonna di destra non tornano.

per verificare le corrispondenze ho scritto un codice che stampa i valori dei vari Dx, 

`Serial.println(D1);`

`...`

`Serial.println(D5);`

la colonna Valore print mostra il valore ritornato

 

| Print(Dx) | Valore print | GPIO | Nome wemos |
| --------- | ------------ | ---- | ---------- |
| D1        | 01           | 01   | -          |
| D2        | 16           | 16   | D0         |
| D3        | 05           | 05   | D1         |
| D4        | 04           | 04   | D2         |
| D5        | 14           | 14   | D5         |
|           |              | 15   | D8         |
|           |              | 13   | D7         |
|           |              | 12   | D6         |
|           |              | 02   | D4         |
|           |              | 00   | D3         |
|           |              |      |            |

 

# Dove sono usati i Wemos 

I Wemos sono usati sia sul robot che sulla base a terra. La base a terra è un radiofaro per permettere ad ARI di orientarsi.

La tabella riporta l'ssegnamento pin sui due Wemos



| **Pin   number** | **Pin** **name** | **function**                                                 | **Ir Tx   On ground**              | **Ir Rx      On robot**            |
| ---------------- | ---------------- | ------------------------------------------------------------ | ---------------------------------- | ---------------------------------- |
| 1                | RST              | Reset module                                                 |                                    |                                    |
| 2                | ADC              | A/d conversion result.   Input voltage range 0~1V, value range: 0~1024 | Fotocellula - in                   |                                    |
| 3                | EN               | Chip enable pin. Active   high                               |                                    |                                    |
| 4                | GPIO16           | GPIO16; can be used to wake   up the chipset from deep sleep mode | Stato barriera - D0 - out          |                                    |
| 5                | GPIO14           | GPIO14; HSPI_CLK                                             |                                    |                                    |
| 6                | GPIO12           | GPIO12; HSPI_MISO                                            |                                    |                                    |
| 7                | GPIO13           | GPIO13; HSPI_MOSI;   UART0_CTS                               |                                    |                                    |
| 8                | VCC              | 3.3V power supply (VDD)                                      |                                    | IR_chip pwr supply                 |
| 9                | GND              | GND                                                          |                                    | IR chip ground                     |
| 10               | GPIO15           | GPIO15; MTDO; HSPICS;   UART0_RTS                            |                                    |                                    |
| **11**           | **GPIO2**        | **GPIO2;   UART1_TXD**                                       | **Seriale   tx Debug – D4  - out** | **Seriale   tx Debug – D4  - out** |
| 12               | GPIO0            | GPIO0                                                        | Sx led sel  -    D3  - out         |                                    |
| 13               | GPIO4            | GPIO4                                                        | Ir tx           -    D2  -  out    | Ir rx (D5) In                      |
| 14               | GPIO5            | GPIO5                                                        | Dx led sel - D1 - out              |                                    |
| 15               | RXD0             | UART0_RXD; GPIO3                                             |                                    | Rx                                 |
| 16               | TXD0             | UART0_TXD; GPIO1                                             |                                    | Tx                                 |

 

 





# Wemos pin description



| **Interface**                                                | **Pin Name**                                                 | **Description**                                              |
| ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| HSPI                                                         | IO12(MISO),    IO13(MOSI)    IO14(CLK),    IO15(CS)          | SPI Flash , display screen,   and MCU can be connected using HSPI interface |
| PWM                                                          | IO12(R),    IO15(G),    IO13(B)                              | Currently the PWM interface   has four channels, but users can extend the channels according to their own   needs. PWM interface can be used to control LED lights, buzzers, relays,   electronic machines, and so on. |
| IR Remote Control                                            | IO14(IR_T),    IO5(IR_R)                                     | The functionality of Infrared   remote control interface can be implemented via software programming. NEC   coding, modulation, and demodulation are used by this interface. The   frequency of modulated carrier signal is 38KHz. |
| ADC                                                          | TOUT                                                         | ESP8266EX integrates a 10-bit   analog ADC. It can be used to test the power supply voltage of VDD3P3 (Pin3   and Pin4) and the input power voltage of TOUT (Pin 6). However, these two   functions cannot be used simultaneously. This interface is typically used in   sensor |
| I2C                                                          | IO14(SCL),    IO2(SDA)                                       | I2C interface can be used to connect external sensor products   and display screens, etc. |
| UART                                                         | UART0:    TXD(U0TXD),    RXD(U0RXD),    IO15(RTS),    IO13(CTS)    UART1:    IO2(TXD) | Devices with UART interfaces can be connected with the module.   Downloading: U0TXD+U0RXD or GPIO2+U0RXD Communicating: UART0: U0TXD, U0RXD,   MTDO (U0RTS), MTCK (U0CTS) Debugging: UART1_TXD (GPIO2) can be used to print   debugging information. |
| By default, UART0 will output some printed information when the   device is powered on and is booting up. If this issue exerts influence on   some specific applications, users can exchange the inner pins of UART when   initializing, that is to say, exchange U0TXD, U0RXD with U0RTS, U0CTS. |                                                              |                                                              |
| I2S                                                          | I2S Input：    IO12 (I2SI_DATA) ;    IO13 (I2SI_BCK );    IO14 (I2SI_WS); | I2S interface is mainly used for collecting, processing, and   transmission of audio data. |
| I2S Output:：    IO15 (I2SO_BCK );    IO3 (I2SO_DATA);    IO2 (I2SO_WS ). |                                                              |                                                              |





## Arduino ATmega 2560 pin assignement table

 

| **Used for** | **Schematic Name** | **Pin Name**             | **Mapped Pin Name**   |
| ------------ | ------------------ | ------------------------ | --------------------- |
|              |                    |                          |                       |
|              |                    |                          |                       |
|              |                    | PG5 ( OC0B )             | Digital pin 4 (PWM)   |
| Lidar rx     | PWMH.1             | PE0 ( RXD0/PCINT8 )      | Digital pin 0 (RX0)   |
| -            |                    | PE1 ( TXD0 )             | Digital pin 1 (TX0)   |
|              |                    | PE2 ( XCK0/AIN0 )        |                       |
| Motor A2     | PWML.6             | PE3 ( OC3A/AIN1 )        | Digital pin 5 (PWM)   |
| Motor B2     | PWMH.2             | PE4 ( OC3B/INT4 )        | Digital pin 2 (PWM)   |
| Motor B1     | PWMH.3             | PE5 ( OC3C/INT5 )        | Digital pin 3 (PWM)   |
|              |                    | PE6 ( T3/INT6 )          |                       |
|              |                    | PE7 ( CLKO/ICP3/INT7 )   |                       |
|              |                    | VCC                      | VCC                   |
|              |                    | GND                      | GND                   |
|              |                    | PH0 ( RXD2 )             | Digital pin 17 (RX2)  |
|              |                    | PH1 ( TXD2 )             | Digital pin 16 (TX2)  |
|              |                    | PH2 ( XCK2 )             |                       |
| Motor A1     | PWML.7             | PH3 ( OC4A )             | Digital pin 6 (PWM)   |
|              |                    | PH4 ( OC4B )             | Digital pin 7 (PWM)   |
|              |                    | PH5 ( OC4C )             | Digital pin 8 (PWM)   |
|              |                    | PH6 ( OC2B )             | Digital pin 9 (PWM)   |
|              |                    | PB0 ( SS/PCINT0 )        | Digital pin 53 (SS)   |
|              |                    | PB1 ( SCK/PCINT1 )       | Digital pin 52 (SCK)  |
|              |                    | PB2 ( MOSI/PCINT2 )      | Digital pin 51 (MOSI) |
|              |                    | PB3 ( MISO/PCINT3 )      | Digital pin 50 (MISO) |
|              |                    | PB4 ( OC2A/PCINT4 )      | Digital pin 10 (PWM)  |
|              |                    | PB5 ( OC1A/PCINT5 )      | Digital pin 11 (PWM)  |
|              |                    | PB6 ( OC1B/PCINT6 )      | Digital pin 12 (PWM)  |
|              |                    | PB7 ( OC0A/OC1C/PCINT7 ) | Digital pin 13 (PWM)  |
|              |                    | PH7 ( T4 )               |                       |
|              |                    | PG3 ( TOSC2 )            |                       |
|              |                    | PG4 ( TOSC1 )            |                       |
|              |                    | RESET                    | RESET                 |
|              |                    | VCC                      | VCC                   |
|              |                    | GND                      | GND                   |
|              |                    | XTAL2                    | XTAL2                 |
|              |                    | XTAL1                    | XTAL1                 |
|              |                    | PL0 ( ICP4 )             | Digital pin 49        |
|              |                    | PL1 ( ICP5 )             | Digital pin 48        |
|              |                    | PL2 ( T5 )               | Digital pin 47        |
|              |                    | PL3 ( OC5A )             | Digital pin 46 (PWM)  |
|              |                    | PL4 ( OC5B )             | Digital pin 45 (PWM)  |
|              |                    | PL5 ( OC5C )             | Digital pin 44 (PWM)  |
|              |                    | PL6                      | Digital pin 43        |
|              |                    | PL7                      | Digital pin 42        |
|              |                    | PD0 ( SCL/INT0 )         | Digital pin 21 (SCL)  |
|              |                    | PD1 ( SDA/INT1 )         | Digital pin 20 (SDA)  |
|              |                    | PD2 ( RXDI/INT2 )        | Digital pin 19 (RX1)  |
|              |                    | PD3 ( TXD1/INT3 )        | Digital pin 18 (TX1)  |
|              |                    | PD4 ( ICP1 )             |                       |
|              |                    | PD5 ( XCK1 )             |                       |
|              |                    | PD6 ( T1 )               |                       |
|              |                    | PD7 ( T0 )               | Digital pin 38        |
|              |                    | PG0 ( WR )               | Digital pin 41        |
|              |                    | PG1 ( RD )               | Digital pin 40        |
|              |                    | PC0 ( A8 )               | Digital pin 37        |
|              |                    | PC1 ( A9 )               | Digital pin 36        |
|              |                    | PC2 ( A10 )              | Digital pin 35        |
| Led 3        | XIOH.4             | PC3 ( A11 )              | Digital pin 34        |
| Led 2        | XIOH.5             | PC4 ( A12 )              | Digital pin 33        |
| Led 1        | XIOH.6             | PC5 ( A13 )              | Digital pin 32        |
|              |                    | PC6 ( A14 )              | Digital pin 31        |
|              |                    | PC7 ( A15 )              | Digital pin 30        |
|              |                    | VCC                      | VCC                   |
|              |                    | GND                      | GND                   |
|              |                    | PJ0 ( RXD3/PCINT9 )      | Digital pin 15 (RX3)  |
|              |                    | PJ1 ( TXD3/PCINT10 )     | Digital pin 14 (TX3)  |
|              |                    | PJ2 ( XCK3/PCINT11 )     |                       |
|              |                    | PJ3 ( PCINT12 )          |                       |
|              |                    | PJ4 ( PCINT13 )          |                       |
|              |                    | PJ5 ( PCINT14 )          |                       |
|              |                    | PJ6 ( PCINT 15 )         |                       |
|              |                    | PG2 ( ALE )              | Digital pin 39        |
|              |                    | PA7 ( AD7 )              | Digital pin 29        |
|              |                    | PA6 ( AD6 )              | Digital pin 28        |
|              |                    | PA5 ( AD5 )              | Digital pin 27        |
|              |                    | PA4 ( AD4 )              | Digital pin 26        |
|              |                    | PA3 ( AD3 )              | Digital pin 25        |
|              |                    | PA2 ( AD2 )              | Digital pin 24        |
|              |                    | PA1 ( AD1 )              | Digital pin 23        |
|              |                    | PA0 ( AD0 )              | Digital pin 22        |
|              |                    | PJ7                      |                       |
|              |                    | VCC                      | VCC                   |
|              |                    | GND                      | GND                   |
|              |                    | PK7 ( ADC15/PCINT23 )    | Analog pin 15         |
|              |                    | PK6 ( ADC14/PCINT22 )    | Analog pin 14         |
|              |                    | PK5 ( ADC13/PCINT21 )    | Analog pin 13         |
|              |                    | PK4 ( ADC12/PCINT20 )    | Analog pin 12         |
|              |                    | PK3 ( ADC11/PCINT19 )    | Analog pin 11         |
|              |                    | PK2 ( ADC10/PCINT18 )    | Analog pin 10         |
|              |                    | PK1 ( ADC9/PCINT17 )     | Analog pin 9          |
|              |                    | PK0 ( ADC8/PCINT16 )     | Analog pin 8          |
|              |                    | PF7 ( ADC7 )             | Analog pin 7          |
|              |                    | PF6 ( ADC6 )             | Analog pin 6          |
|              |                    | PF5 ( ADC5/TMS )         | Analog pin 5          |
|              |                    | PF4 ( ADC4/TMK )         | Analog pin 4          |
|              |                    | PF3 ( ADC3 )             | Analog pin 3          |
|              |                    | PF2 ( ADC2 )             | Analog pin 2          |
|              |                    | PF1 ( ADC1 )             | Analog pin 1          |
|              |                    | PF0 ( ADC0 )             | Analog pin 0          |
|              |                    | AREF                     | Analog Reference      |
|              |                    | GND                      | GND                   |
|              |                    | AVCC                     | VCC                   |

 
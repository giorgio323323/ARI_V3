/**@file readMe.ino */
/*note

	portPrint("--- elenco comandi ---\n\n");
	portPrint("Sn set sensor.      n: 1 sx  , 2 cnentrale, 3 dx\n");
	portPrint("Cn read countinuos. n: 0 stop, 1 start\n");
	portPrint("R1 read single.\n");
	portPrint("Tx p_timeBtwReading. x in [ms].\n");
	portPrint("V  verbose. 0 off, 1 On.\n");

	
N.B.

al primo avvio vanno inseriti i dati meccanici.
questo si fa con i comandi E4 oppure E5, questi caricano i valori di default, seguiti
da E0 che li scrive in EEprom. All'avvio vengono caricati i valori dalla EEprom.

per ARI2 fare

E4
E0

per ARI3 fare

E5
E0



*/

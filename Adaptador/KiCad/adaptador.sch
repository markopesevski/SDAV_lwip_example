EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:adaptador-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_02X06 P1
U 1 1 582E5962
P 2300 3250
F 0 "P1" H 2300 3600 50  0000 C CNN
F 1 "CONN_02X06" H 2300 2900 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_2x06" H 2300 2050 50  0000 C CNN
F 3 "" H 2300 2050 50  0000 C CNN
	1    2300 3250
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P2
U 1 1 582E5A87
P 7350 3300
F 0 "P2" H 7350 3500 50  0000 C CNN
F 1 "CONN_01X03" V 7450 3300 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Angled_1x03" H 7350 3300 50  0000 C CNN
F 3 "" H 7350 3300 50  0000 C CNN
	1    7350 3300
	1    0    0    -1  
$EndComp
NoConn ~ 5250 3100
Wire Wire Line
	2550 3000 2750 3000
Text Label 1800 3000 0    60   ~ 0
3V3
Text Label 2750 3000 0    60   ~ 0
3V3
Text Label 2750 3100 0    60   ~ 0
GND
Text Label 1800 3100 0    60   ~ 0
GND
Text Label 2750 3300 0    60   ~ 0
SCK
Text Label 2750 3400 0    60   ~ 0
MISO
Wire Wire Line
	2550 3100 2750 3100
$Comp
L MCP3002 U1
U 1 1 582E58B9
P 4750 3300
F 0 "U1" H 4500 3750 50  0000 C CNN
F 1 "MCP3002" H 5200 3750 50  0000 C CNN
F 2 "Housings_DIP:DIP-8_W7.62mm" H 4750 3200 50  0000 C CNN
F 3 "" H 4750 3500 50  0000 C CNN
	1    4750 3300
	-1   0    0    1   
$EndComp
Text Label 2750 3200 0    60   ~ 0
/CS
Text Label 2750 3500 0    60   ~ 0
MOSI
Wire Wire Line
	3800 3200 4050 3200
Wire Wire Line
	3800 3100 4050 3100
Wire Wire Line
	3800 3400 4050 3400
Wire Wire Line
	3800 3500 4050 3500
Wire Wire Line
	4750 3900 4750 3800
Wire Wire Line
	4750 2700 4750 2800
Text Label 3800 3100 0    60   ~ 0
MOSI
Text Label 3800 3200 0    60   ~ 0
MISO
Text Label 3800 3400 0    60   ~ 0
SCK
Text Label 3800 3500 0    60   ~ 0
/CS
Text Label 4750 3900 0    60   ~ 0
3V3
Text Label 4750 2700 0    60   ~ 0
GND
Wire Wire Line
	5350 3500 5250 3500
Text Label 5350 3500 0    60   ~ 0
SENS
Wire Wire Line
	6850 3400 7150 3400
Wire Wire Line
	6850 3300 7150 3300
Wire Wire Line
	6850 3200 7150 3200
Text Label 6850 3400 0    60   ~ 0
SENS
Text Label 6850 3300 0    60   ~ 0
3V3
Text Label 6850 3200 0    60   ~ 0
GND
Wire Wire Line
	2050 3100 1800 3100
Wire Wire Line
	1800 3000 2050 3000
Wire Wire Line
	2550 3300 2750 3300
Wire Wire Line
	2550 3400 2750 3400
Wire Wire Line
	2750 3500 2550 3500
Wire Wire Line
	2550 3200 2750 3200
$EndSCHEMATC

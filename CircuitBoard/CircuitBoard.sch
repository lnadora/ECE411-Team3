EESchema Schematic File Version 4
EELAYER 30 0
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
L Transistor_Array:ULN2003A U?
U 1 1 6170940C
P 2400 2150
F 0 "U?" H 2400 2817 50  0000 C CNN
F 1 "ULN2003A" H 2400 2726 50  0000 C CNN
F 2 "" H 2450 1600 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/uln2003a.pdf" H 2500 1950 50  0001 C CNN
	1    2400 2150
	1    0    0    -1  
$EndComp
$Comp
L Motor:Stepper_Motor_unipolar_5pin M?
U 1 1 6170B152
P 4700 1900
F 0 "M?" V 4888 2024 50  0000 L CNN
F 1 "Stepper_Motor_unipolar_5pin" V 5000 1250 50  0000 L CNN
F 2 "" H 4710 1890 50  0001 C CNN
F 3 "http://www.infineon.com/dgdl/Application-Note-TLE8110EE_driving_UniPolarStepperMotor_V1.1.pdf?fileId=db3a30431be39b97011be5d0aa0a00b0" H 4710 1890 50  0001 C CNN
	1    4700 1900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2800 1950 2950 1950
Wire Wire Line
	3700 1950 3700 2000
Wire Wire Line
	3700 2000 4400 2000
Wire Wire Line
	2800 2050 3150 2050
Wire Wire Line
	3600 2050 3600 1800
Wire Wire Line
	3600 1800 4400 1800
Wire Wire Line
	2800 2150 3350 2150
Wire Wire Line
	3750 2150 3750 2300
Wire Wire Line
	3750 2300 4600 2300
Wire Wire Line
	4600 2300 4600 2200
Wire Wire Line
	2800 2250 3500 2250
Wire Wire Line
	4800 2250 4800 2200
$Comp
L Device:C 100nF
U 1 1 61715830
P 3200 2900
F 0 "100nF" H 3315 2946 50  0000 L CNN
F 1 "C" H 3315 2855 50  0000 L CNN
F 2 "" H 3238 2750 50  0001 C CNN
F 3 "~" H 3200 2900 50  0001 C CNN
	1    3200 2900
	1    0    0    -1  
$EndComp
$Comp
L power:GNDREF #PWR?
U 1 1 61716F3D
P 3200 3050
F 0 "#PWR?" H 3200 2800 50  0001 C CNN
F 1 "GNDREF" H 3205 2877 50  0000 C CNN
F 2 "" H 3200 3050 50  0001 C CNN
F 3 "" H 3200 3050 50  0001 C CNN
	1    3200 3050
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 61717DE3
P 4250 3200
F 0 "J?" V 4122 3380 50  0000 L CNN
F 1 "Conn_01x04 (4-Pin Header)" V 4213 3380 50  0000 L CNN
F 2 "" H 4250 3200 50  0001 C CNN
F 3 "~" H 4250 3200 50  0001 C CNN
	1    4250 3200
	0    1    1    0   
$EndComp
$Comp
L power:VCC #PWR?
U 1 1 61719285
P 4150 2800
F 0 "#PWR?" H 4150 2650 50  0001 C CNN
F 1 "VCC" H 4165 2973 50  0000 C CNN
F 2 "" H 4150 2800 50  0001 C CNN
F 3 "" H 4150 2800 50  0001 C CNN
	1    4150 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 3000 4150 3000
Wire Wire Line
	4150 2800 4150 2850
Connection ~ 4150 3000
$Comp
L Device:LED D?
U 1 1 6171D613
P 2950 1350
F 0 "D?" V 2989 1232 50  0000 R CNN
F 1 "LED" V 2898 1232 50  0000 R CNN
F 2 "" H 2950 1350 50  0001 C CNN
F 3 "~" H 2950 1350 50  0001 C CNN
	1    2950 1350
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D?
U 1 1 617207E8
P 3150 1350
F 0 "D?" V 3189 1232 50  0000 R CNN
F 1 "LED" V 3098 1232 50  0000 R CNN
F 2 "" H 3150 1350 50  0001 C CNN
F 3 "~" H 3150 1350 50  0001 C CNN
	1    3150 1350
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D?
U 1 1 61721DB2
P 3350 1350
F 0 "D?" V 3389 1232 50  0000 R CNN
F 1 "LED" V 3298 1232 50  0000 R CNN
F 2 "" H 3350 1350 50  0001 C CNN
F 3 "~" H 3350 1350 50  0001 C CNN
	1    3350 1350
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D?
U 1 1 61723682
P 3550 1350
F 0 "D?" V 3589 1232 50  0000 R CNN
F 1 "LED" V 3498 1232 50  0000 R CNN
F 2 "" H 3550 1350 50  0001 C CNN
F 3 "~" H 3550 1350 50  0001 C CNN
	1    3550 1350
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_US 500ohm
U 1 1 61724082
P 2950 950
F 0 "500ohm" H 3018 996 50  0000 L CNN
F 1 "R_US" H 3018 905 50  0000 L CNN
F 2 "" V 2990 940 50  0001 C CNN
F 3 "~" H 2950 950 50  0001 C CNN
	1    2950 950 
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US 500ohm
U 1 1 61724EBC
P 3150 950
F 0 "500ohm" H 3218 996 50  0000 L CNN
F 1 "R_US" H 3218 905 50  0000 L CNN
F 2 "" V 3190 940 50  0001 C CNN
F 3 "~" H 3150 950 50  0001 C CNN
	1    3150 950 
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US 500ohm
U 1 1 61725778
P 3350 950
F 0 "500ohm" H 3418 996 50  0000 L CNN
F 1 "R_US" H 3418 905 50  0000 L CNN
F 2 "" V 3390 940 50  0001 C CNN
F 3 "~" H 3350 950 50  0001 C CNN
	1    3350 950 
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US 500ohm
U 1 1 617260BC
P 3550 950
F 0 "500ohm" H 3618 996 50  0000 L CNN
F 1 "R_US" H 3618 905 50  0000 L CNN
F 2 "" V 3590 940 50  0001 C CNN
F 3 "~" H 3550 950 50  0001 C CNN
	1    3550 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	4350 3000 5500 3000
Wire Wire Line
	5500 3000 5500 800 
Wire Wire Line
	5500 800  4250 800 
Connection ~ 3150 800 
Wire Wire Line
	3150 800  2950 800 
Connection ~ 3350 800 
Wire Wire Line
	3350 800  3150 800 
Connection ~ 3550 800 
Wire Wire Line
	3550 800  3350 800 
Wire Wire Line
	3550 1100 3550 1200
Wire Wire Line
	3350 1100 3350 1200
Wire Wire Line
	3150 1100 3150 1200
Wire Wire Line
	2950 1100 2950 1200
Wire Wire Line
	4400 2100 4250 2100
Wire Wire Line
	4250 2100 4250 800 
Connection ~ 4250 800 
Wire Wire Line
	4250 800  3550 800 
Wire Wire Line
	3800 3000 3800 3050
Wire Wire Line
	3800 3050 3200 3050
Wire Wire Line
	3800 3000 4050 3000
Connection ~ 3200 3050
Wire Wire Line
	3200 3050 2400 3050
Wire Wire Line
	2400 3050 2400 2750
Wire Wire Line
	3700 2850 3700 2750
Wire Wire Line
	3700 2750 3200 2750
Wire Wire Line
	3700 2850 4150 2850
Connection ~ 4150 2850
Wire Wire Line
	4150 2850 4150 3000
Wire Wire Line
	2800 1750 3200 1750
Wire Wire Line
	3200 1750 3200 2750
Connection ~ 3200 2750
Wire Wire Line
	3550 1500 3500 1500
Wire Wire Line
	3500 1500 3500 2250
Connection ~ 3500 2250
Wire Wire Line
	3500 2250 4800 2250
Wire Wire Line
	3350 1500 3350 2150
Connection ~ 3350 2150
Wire Wire Line
	3350 2150 3750 2150
Wire Wire Line
	3150 1500 3150 2050
Connection ~ 3150 2050
Wire Wire Line
	3150 2050 3600 2050
Wire Wire Line
	2950 1500 2950 1950
Connection ~ 2950 1950
Wire Wire Line
	2950 1950 3700 1950
$Comp
L doit-esp32-devkit-v1:DOIT-ESP32-DEVKIT-V1 U?
U 1 1 61730A6B
P 1700 3900
F 0 "U?" H 1675 4465 50  0000 C CNN
F 1 "DOIT-ESP32-DEVKIT-V1" H 1675 4374 50  0000 C CNN
F 2 "" H 1650 4350 50  0001 C CNN
F 3 "" H 1650 4350 50  0001 C CNN
	1    1700 3900
	1    0    0    -1  
$EndComp
$EndSCHEMATC

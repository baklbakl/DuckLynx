# DuckLynx
Change EH address https://github.com/FIRST-Tech-Challenge/FtcRobotController/wiki/Using-Two-Expansion-Hubs#checking-the-address-of-an-expansion-hub
How to ghidra!!! With registers https://www.youtube.com/watch?app=desktop&v=q4CxE5P6RUE

<!-- FIX: Update the programming instructions to make them not specific to my machine -->
## Programming
Connect to debug serial `picocom -b 115200 --imap lfcrlf --echo /dev/ttyUSB1`  
Program chip `openocd -f openocd\ configs/ftdi_lynx.cfg -c "program /home/polarbub/Documents/saves/hardware/lynx/firmware/build/expansionHubFW.bin reset; shutdown"`  
Read flash `openocd -f openocd\ configs/ftdi_lynx.cfg -c "flash read_bank 0 /home/polarbub/Documents/saves/code/c/expansionHubFW/info/readLynxFW.bin; shutdown"`  
Start openocd `openocd -f openocd\ configs/ftdi_lynx.cfg`
To restore the MCU to factory connect the SRST line and run `openocd -f openocd\ configs/ftdi_lynx.cfg -c "reset_config srst_only; stellaris recover"`

## Interesting files from RC app
Extracted-RC/RobotCore/src/main/java/org/firstinspires/ftc/robotcore/internal/stellaris/ - bootloader interaction code
Extracted-RC/RobotCore/src/main/java/org/firstinspires/ftc/robotcore/internal/hardware/ - which pins things are connected to on the android header. Also has the model number of what the android board was based off of.
Extracted-RC/FtcCommon/src/main/java/org/firstinspires/ftc/ftccommon/internal/manualcontrol/ - RHSP manual control?
Extracted-RC/Hardware/src/main/java/com/qualcomm/hardware/lynx/ - rc rhsp implementation

## FTDI
Remove the 74HC595 to turn off the obnoxious leds
Remove the pullup to 5v for the reset line

## Launchpad
Link: https://www.ti.com/tool/EK-TM4C123GXL#tech-d5 4 ocs
MCU: https://www.ti.com/product/TM4C123GH6PM
Program chip openocd -f board/ti_ek-tm4c123gxl.cfg -c "program /home/polarbub/Documents/saves/code/c/expansionHubFW/build/expansionHubFW.bin verify reset"

## Android Board
This makes an Expansion Hub into a Control Hub. Seems to be based off of the Dragonboard 410c

## Lynx HIB (Hardware Interface Board)
### Components
A large chunk of the resistors are 0603
Aux Shunt resistors are 20 milli ohm in a 2512 package

#### ICs
Main MCU [TM4C123GH6PGEI7](https://www.ti.com/product/TM4C123GH6PGE/part-details/TM4C123GH6PGEI7) [docs](https://www.ti.com/product/TM4C123GH6PGE#tech-docs)
Motor controller VNH5050AE full bridge motor controller
USB to UART FT230XQ
RS485 transceiver ST3485EB
Bus transceiver SN74LVC8T245
Op Amp for shunts K176 SOT 23-5 ST LMV321RILT
5V Buck converter driver tps54527
3.3V Buck converter driver TPS562209
701 IMU Bosch BNO055
AAXXX U17 Adjustable shunt voltage regulator TL431ASA
High side current monitor U25 ZXCT1010E5TA
USB OTG ESD Diodes U19 PUSBM5V5X4-TL

#### Diodes
Big zener diode for motor drives D4 1SMB5931BT3G
Tiny ESD diode D5 PESD15VS1UL
ESD Protection E5U Chip CPDV5-5V0U
ESD Protection E3V3 Chip CPDV5-3V3UP

#### Transistors
Reverse current protection BIG mosfet FDD9409-F085
NPN Transistor 43 DTC143XKA
N channel MOSFET WJ3 BSH103
P channel mosfet KFH 5V servo power enabe SSM3J328R
P channel mosfet phone charging X73L AO3407A
N channel MOSFET K38 BSS138W
N channel MOSFET N32 GN (Last few characters may be different) Q13 and others AO3434A

#### Connectors
XT30 Male Amass [XT30UPB-M](https://www.tme.com/ux/en-us/details/xt30upb-m/dc-power-connectors/amass/)
XT30 Female Amass [XT30UPB-F](https://www.tme.com/ux/en-us/details/xt30upb-f/dc-power-connectors/amass/)
Mini USB B [MUSBS5FBM1RA](https://leoco.com.tw/product/mini-usb-connector/)
 - That part number exists nowhere on the internet. 
 - It also is Leoco product series 0850 and P/N 0850BFBD111. The F could be J or K as the gold plating thickness is unknown
 - TE Connectivity P/N 1734035-1 seems to have the same pad dimensions, so it may work
JTAG Connector Female Molex [53398-0671](https://www.molex.com/en-us/products/part-detail/533980671)
JTAG Connector Male Cable Molex [15134-0605](https://www.molex.com/en-us/products/part-detail/151340605)
All of shrouded external connectors are JST PH
All the motor connectors are JST VH

#### Fuses
All from Bel Power? Bel Fuse?
They are green
b2 Fuse - 2A hold current - 0ZCJ0200FF2C
bS Fuse - 1.5 A hold current - 0ZCH0150FF2E
b1 Fuse - 1A hold current - 0ZCJ0100FF2E
bM Fuse - 0.5A hold current - 0ZCK0050FF2E

#### Mystery Components
D2
D3 Maybe ESD Protection Diode LittelFuse SMF17A

Don't ask - doesn't matter:
LED 3825 package. About 1.6 mm tall. 6 pins

#### To order
XT30
3 pin JST
Motor controllers?

### Datasheet questions
- External Ram? 
- How much does bit banding help performance?
- Instrumentation Trace Macrocell (ITM) for support of printf style debugging
– Embedded Trace Macrocell (ETM) for instruction trace capture
– Trace Port Interface Unit (TPIU) for bridging to a Trace Port Analyzer
    - What is system trace?
- How does profiling work?

on page 599

### Hardware questions
 - Does 3V3 get measured?
 - What does PJ7 do? It goes between CBUS2, the MCU and maybe somewhere else.
 - Find where CBUS3 on the FTDI goes - likely nowhere as it is just a test pad
 - J16 Android Module uart - what does this do?
 - Find out what PB0 does - related informing the MCU when the IMU is ready. Also seems to go to the android header. Why?
 - Check if PM2 is a pullup on the MCU or driven high because the buck driver may try and pull it down if there is a fault generating a conflict
 - Check if ENA/B to the motor drivers is an open drain pin on the MCU
 - Figure out how to setup the FTDI reset on MCU reset thing - where did I get the idea that this happens? Does it happen? Does the FTDI get reset at other times? - It seems that the MCU can reset the FTDI, but I doubt that it ever happens. More likely it is used to tell when there is an android module plugged in. Check PC6
 - The circuit around PJ6 and the android header seems weird. It seems to only pull down PJ6 when both the android header control enable line is pulled low and a pin on the android header is too. Also what does it indicate?

#### Answered Questions
 - Why is the VREF+ 3 volts not 3.3V? - It is under vcc because the stable regulator regulates from VCC
 - Does the 32678 Hz crystal go to the MCU or just the IMU? - Just IMU
 - Where does 12V get measured - See schematic
 - How does phone charging work? Does it work? - According to rev it doesn't with the phones that are around today, so we won't be trying to use it at all
 - Is there anything that prevents the Android Module and the FTDI from trying to talk at the same time? - Yes. When the enable line for the level shifters goes low it also places the FTDI in reset
 
### Todo
End goal: really fast PID and feedforward loop times from java
Ideas:
    - Up the RHSP baud rate
    - Remove message / reference number
    - Motion read/write packet that sends new motor powers and retrieves encoder positions, battery current, battery voltage, and anything else important.

Implement motor DEKA commands
    mode
    encoder
    encoder reset
Add voltage too low to run motors failsafe
  Does the stock firmware stop trying to run the motors if the 12 is removed while they are running?
Add RHSP timeout
Setup the automatic LED

Add motor controller fault monitoring
Coulomb counting for battery state of charge
Enable 5V
Check that all of the hardware initialization explicitly disables the hardware before making changes
Add servo RHSP commands
Update this document to match the schematic
Print out reset cause (page 254) on reset
Print out hard fault cause
WFI instruction to wait for an interrupt instead of a tiny loop 
Setup flashing via UART. See https://github.com/REVrobotics/node-expansion-hub-ftdi/blob/main/src/EnterFirmwareUpdateMode.cc
Make the button switch wifi bands - no idea how to do that - probably not nessisary as the android header has the button broken out to it

### Done
RHSP
    Enable UART0
    Test TX/RX on UART0
    Parse RHSP Frame
    Respond to basic commands from hubDriver
    Make handler for the different types of packet
    Implement the discovery command
    Implement the commandSet handlers ("DEKA")
    Use uDMA to speed up serial transfers
    Implement the address set command
    Implement important parts of ADC DEKA command
    Implement version DEKA commands

Enable faults
    See page 114 for what faults exist.
    See page 175 for how to enable them
    See page 170 for to enable fault trapping
    See page 179 to see what the faults mean
    Check to see without them being enabled they trigger a hard fault

Before reading eeprom check EESUPP for errors
    There may be an errata for this. Check.

Get rid of driverlib and headers
    All the TI code that should be left is linker config and startup code

External oscillator 
    Enable
    set up with PLL to 80 MHz
    Make sure eeprom is done before doing so
    See page 233
    Do we need to wait for it to be stable? (page 246)

Change GPIOs to AHB (page 260)
Find out what is in User Registers 1-4 - nothing
Enable main oscillator verification - doesn't work - see errata
Update the schematic with the new part numbers from rev
Check if the RC will send another packet before waiting for the response of the previous one - it will

### Won't do
Make the button put the device into rom bootloader mode. Check bootcfg register on page 594 - the button isn't physically hooked up to the pin that the BOOTCFG register is configured to use. BOOTCFG should stay not be modified by DuckLynx, so it is impossible
Enable brown out protection?
Sleep mode while waiting to do something? - it will pause the core until it gets an interrupt (page 230) - There are a bunch of errata, so working with sleep mode is a pain
  Make sure eeprom is done before doing so
  WFI instruction
  Maybe turn on auto clock gating so that it turns off unneeded peripherals?

### What to flash to a new chip
EEPROM
Flash
BOOTCFG - ffff24fe
    Port B pin 1 for bootloader entry
User registers 1-4 - all just F
    They don't seem important

### Stock firmware
All GPIO peripherals are enabled
Ports A-J are on the older APB
Ports K-P are on the newer AHP
I2C 4 and 5 aren't used on the original FW

### Connector Pinouts
#### Internal UART (J16)
Seems to go to the Android Header. May go other places
Pins:
```
1 - Rx
2 - Tx
3 - GND
```

#### JTAG Connector
The connector is a Molex Picoblade P/N 15134-0600 for a cable
Pinout:
Pin 1 has a dot next to it on the PCB
```
1 - 3V3
2 - TCK
3 - TMS
4 - TDI
5 - TDO
6 - GND
```

#### Android Header (J??)
The header that connects to a Android device (if equipped)
Pin numbering:
```
 1 - - 40
 2 - - 39
 3 - - 38
 :     :
 :     :
20 - - 21
```
 5 - R67
19 - +12V
20 - GND
21 - GND
23 - +1.8V to bus transceivers
40 - GND

### Voltage Rails
There seems to be something between the 3.3V rail for the core / jtag and the one for everything else 3.3V. There is about 17 ohms between them. Maybe filtering?
The 1.8V rail seems to come from the android module. There is no power on it when the android module is disconnected

### Bus Transceivers
A is Android Header - 1.8V  
B is System - 3.3V
Eight channels

HIB to Android Header (B to A) (Dir is low)
- U21 
- CH 1
    - B U0Tx
    - A Android Header 37
- CH 2
    - B FX230X Pin 4 (CTS)
    - A Android Header 39
- CH 3
    - B Internal J16 UART Rx (Pin 1)
    - A Android Header 34
- CH 4
    - B PB0 ?? Is this just a GPIO?
    - A Android Header 27
- CH 5-8 are unused and connected to GND through a 10 kΩ resistor

Android Header to HIB (A to B) (Dir is high)
- U22
- CH 1
    - A Android Header 38
    - B U0Rx
- CH 2
    - A Android Header 36
    - B FT230X Pin 16 (RTS)
- CH 3
    - A Android Header 35
    - B Internal J16 UART Tx (Pin 2) 
- CH 4
    - A Android Header 4
    - B PC7
- CH 5
    - A Android Header 29
    - B MCU PB1 and TP3 (NPRG), and FTDI CBUS1
- CH 6
    - A Android Header 28
    - B MCU RST, TP2 (NRST) and FTDI CBUS0
- CH 7-8 are unused and connected to GND through a 10 kΩ resistor

### Current sensors
https://github.com/WestsideRobotics/FTC-Power-Monitoring/wiki
https://github.com/OpenFTC/RevExtensions2/tree/master/rev-extensions-2/src/main/java/org/openftc/revextensions2
https://github.com/OpenFTC/Extracted-RC/blob/ce0a67af28c966c7a166ae51765e115a853734fc/Hardware/src/main/java/com/qualcomm/hardware/lynx/commands/core/LynxGetADCCommand.java#L66
B

### MCU Pin Connections
Overall temp sensor
3.3V voltage sense?

#### FT230XQ
PB1 (Bootconfig enter bootloader) FT230X Pin 11 (CBUS1) and TP3 (NPRG)
RST FT230X Pin 12 (CBUS0) and TP2 (NRST)
PJ7 FT230X Pin 5 (CBUS2)
PK5 FT230X Pin 16 (RTS) and Android Header 36

#### UART
U0Tx (PA1) FT230X Rx  
U0Rx (PA0) FT230X Tx
PK5 FT230X CTS and Android Header 39
PK4 FT230X RTS and Android Header 36

U1Tx (PC5) RS485 Tx  
U1Rx (PC4) RS485 Rx  
PF1 RS485 Read enable
PF0 RS485 Output enable  

U2Tx (PD7) External UART upper - seems dead on the test hub  
U2Rx (PD6) External UART upper

U4Tx (PJ1) External UART lower  
U4Rx (PJ0) External UART lower

#### LEDS
PM3 Blue Led - Pin 86 (WT5CCP1)
PM6 Red led - Pin 83 (WT0CCP0)
PM7 Green Led - Pin 82 (WT0CCP1) 
All used general timers
About a 3.21 nS pulse length (About 312 KHz)
0x100 is the value that it counts down from
0x20 is transition for on
0x0 prescaler

Colors:
#ff2000 - no power flashing orange
#000020 - just powered on blue
#00007f - flashing blue for rhsp timeout
#002000 - rhsp connected green
For flashing modes leds blink every 600 ms (300 ms on 300 ms off). 
Expansion hubs blink their address every so often, but control hubs don't. This seems to be determined by if the address is 173 or it is a low number

#### Button
PB4 Button by RS458  

#### I2C
Current sense opamp out AIN7 (PD4)

I2C0SDA (PB3) External I2C 0 SDA  
I2C0SCL (PB2) External I2C 0 SCL

I2C1SDA (PA7) External I2C 1 SDA
I2C1SCL (PA6) External I2C 1 SCL

I2C2SDA (PE5) External I2C 2 SDA
I2C2SCL (PE4) External I2C 2 SCL

I2C3SDA (PD1) External I2C 3 SDA
I2C3SCL (PD0) External I2C 3 SCL

#### Digital GPIO
Current sense opamp out AIN6 (PD5)
GPIOs are set as open drain pins and have pull up resistors
The GPIO has a pull up resistor
The upper resistor goes to the gate of a N channel mosfet with an unknown part number that will pull the GPIO down.
The lower one comes from the pin and goes through a resistor to a pin on the MCU for input
Output and input are from the perspective of the lynx

GPIO 0:
PN4 R38 Left (output)
PL0 R39 Left (input)

GPIO 1:
PN5 R36 Left (output)
PL1 R37 Left (input)

GPIO 2:
PN6 R34 Left (output)
PL2 R35 Left (input)

GPIO 3:
PN7 R32 Left (output)
PL3 R33 Left (input)

GPIO 4:
PJ4 R30 Left (output)
PL4 R31 Left (input)

GPIO 5:
PJ5 R28 Left (output)
PL5 R29 Left (input)

GPIO 6:
PN2 R26 Left (output)
PM0 R27 Left (input)

GPIO 7:
PP2 R24 Left (output)
PM1 R25 Left (input)

#### Analog GPIO
VCCA and GNDA are just connected to VCC and GND
VREFA- is connected to GND
VREFA+ is connected to a circuit that makes 2.994V. This is derived from the TL431A datasheet by using the formula Vout = Vref * (1 + (R1/R2)) which in the case of the HIB is 2.994 = 2.495 * (1 + (2000 / 10000))

AGPIO 0 AIN0 (PE3)
AGPIO 1 AIN1 (PE2)
AGPIO 2 AIN2 (PE1)
AGPIO 3 AIN3 (PE0)

#### 5V rail
Buck converter enable Pin 1 PM2
Current Sense opamp out AIN12 (PD3)
Voltage monitoring AIN23 (PP0) - Through resistor divider with two 10K resistors to divide voltage by 2. 

#### Servo
Servo 5 PWM M0PWM7 (PH7)
Servo 4 PWM M0PWM6 (PH6)
Servo 3 PWM M0PWM3 (PH3)
Servo 2 PWM M0PWM2 (PH2)
Servo 1 PWM M1PWM1 (PG3)
Servo 0 PWM M0PWM0 (PG2)
Servo 5V enable PC6 - Has pull down

#### Motors
M0:
 - CS AIN16 (PK0)
 - EN A/B PN0
 - INA PK6
 - INB PF2
 - PWM M0PWM0 (PH0)

M1:
 - CS AIN17 (PK1)
 - EN A/B PN1
 - INA PK7
 - INB PF3
 - PWM M0PWM1 (PH1)

M2:
 - CS AIN20 (PE7)
 - EN A/B PM4
 - INA PG6
 - INB PK2
 - PWM M1PWM2 (PG4)

M3:
 - CS AIN21 (PE6)
 - EN A/B PM5
 - INA PG7
 - INB PK3
 - PWM M1PWM3 (PG5)

#### Encoder
CH0
 - A PhA0 (PH4)
 - B PhB0 (PH5)

CH1
 - A PF6
 - B PF7

CH2
 - A PF4
 - B PF5

CH3
 - A PhA1 (PG0)
 - B PhB1 (PG1)

#### Battery
Voltage - connected to AIN22 (PP1) via a 1/6 resistor divider with 10K and 2K resistors
Current - Uses a high side current monitor IC with a 0.003 ohm shunt and 3K Rout resistor into AIN13 (PD2)

#### External oscillator
Called main oscillator by the datasheet
Connected to pins 92 and 93 which are OSC0 and OSC1
16 MHz

#### SSI 0
Goes to J19 SPI
J19 SPI:
```
Pin 1 - VCC
Pin 2 - CLK - SSI0Clk (PA2)
Pin 3 - CS - SSI0Fss (PA3)
Pin 4 - MISO - SSI0Rx (PA4)
Pin 5 - MOSI - SSI0Tx (PA5)
Pin 6 - GND
```

### Failures
#### 4
RS485 B is shorted to ground

#### 3 
Analog 3.3v / Digital 3.3v / I2C 3.3v / Encoder 3.3v
SCL 3
Upper external UART Rx
Lower external UART Rx

#### MCU replacment
To check
  All ESD diodes
  Motor drives

To fix
  External clock shorted
  Blue LED resistor
  Red LED

Won't fix
  5V enable PC6
  Android board sense PC7
  Software encoder PF4
  Software encoder PF5
  RS485 Plug #2
  UART 2

### Serial numbers
Serial number is the serial number of the FTDI

### EEPROM
The first byte of EEPROM stores the address of the module. It seems like there is nothing else of importance in it.
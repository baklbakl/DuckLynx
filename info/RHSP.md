# RHSP
RHSP is the REV Hub Serial Protocol. Is the protocol that the Lynx and other REV hubs communicate with. RHSP runs over serial. Between hubs it runs over RS485. For the most part it is half duplex. When running over USB to an FTDI that then goes to the main chip it will run at full duplex. Peripheral devices only output if they receive a message from their controller device.

## Resources
https://github.com/OpenFTC/Extracted-RC
https://github.com/REVrobotics/REV-Hub-Serial-Protocol-Analyzer-For-Saleae/blob/main/HighLevelAnalyzer.py
https://github.com/REVrobotics/node-rhsplib/tree/main/packages/rhsplib/librhsp

## Terminology
- Hub = a lynx or other REV hub
- Controller = either librhsp or the FTC Robot Controller App
- Parent (Hub) = the hub that is connected to the controller via UART 0
- Child (Hub) = a hub that is connected to the parent device via RS485 on UART 1
- Command = a packet send by the controller to a hub
- Reply = a packet send in response to a command from controller by a hub

## Packet format

```
[0] = 0x44
[1] = 0x4B
[2] = LSB of packet size
[3] = MSB of packet Size
[4] = Dest address
[5] = Src address
[6] = Message number
[7] = Reference number
[8] = LSB of command ID
[9] = MSB of command ID
[10, 10 + Payload size - 1] = payload
[10 + Payload size] = checksum
```

#### Example Packet
Keep alive:  
`44 4B 0B 00 AD 00 04 00 04 7F CE`

#### Src address
This is always zero for the controller. It may always be zero, but not if the parent hub messes with it when repeating packets. It is `0xFF` for broadcast to all hub
#### Message number
- Message number should never equal zero. It starts at one. If it overflows to zero it should be reset to one - test overflowing
- Any response packets should use the same message number as their request

#### Reference number
- The way that the child device indicates which packet it is responding to. Must be the same as the message number in any response packets for librhsp
- On the stock firmware it seems to go up by one whenever it receives a packet with a higher Message Number than the last packet

#### Command ID
- Bit 15 indicates if the packet is a response (1) or a command (0). A response is where the hub sends back data to the controller with the same command id except for bit 15. An example is the `GET_MODULE_STATUS` command. An `ACK` or` NACK` is not a response because it doesn't have the same command id as the initial command and it is instead a command, so bit 15 of the command id should be 0.

#### Payload
- 512 Byte max size in librhsp

#### Checksum
- Last eight bits of the sum of all bytes of the packet except the checksum
- If the checksum is incorrect no response should be issued

## Commands

### ACK = 0x7f01
Only ever a reply
#### Payload
```
[0] (bool) Attention Needed - I assume that this is one when any of the status bits are set 'SET_MODULE_STATUS' command
```

### NACK = 0x7f02
Only ever a reply
#### Payload
```
[0] NACK Code
```

### GET_MODULE_STATUS = 0x7f03
Gets the status of a module
#### Payload 
```
[0] (bool) Clear status after response
```

#### Response
Should be sent back in a packet with the same command ID, but the return bit is set
Payload:
For the lynx at least, maybe not the servo hub
```
[0] Module Status
   [0] Keep Alive Timeout
   [1] Device Reset - device has come up from reset?
   [2] Fail Safe - should be on when the battery level is too low to run, when there is a keep alive timeout 
   [3] Controller Over Temp
   [4] Battery Low
   [5] HIB Fault
   [6-7] Reserved 
[1] Motor Status
   [0] Motor 0 lost counts 
   [1] Motor 1 lost counts
   [2] Motor 2 lost counts
   [3] Motor 3 lost counts
   [4] Motor 0 driver overheat
   [5] Motor 1 driver overheat
   [6] Motor 2 driver overheat
   [7] Motor 3 driver overheat
```

### KEEP_ALIVE = 0x7f04
Only ever send as a command
No payload
ACK sent as response

### FAIL_SAFE = 0x7f05
Tells the hub to disable all motion
No payload
ACK as response

### SET_NEW_MODULE_ADDRESS = 0x7f06

### QUERY_INTERFACE = 0x7f07

### START_DOWNLOAD = 0x7f08

### DOWNLOAD_CHUNK = 0x7f09

### SET_MODULE_LED_COLOR = 0x7f0a
Sets the color of the multicolor LED on the lynx

#### Payload
```
[0] Red value
[1] Green value
[2] Blue value
```

### GET_MODULE_LED_COLOR = 0x7f0b

### SET_MODULE_LED_PATTERN = 0x7f0c

### GET_MODULE_LED_PATTERN = 0x7f0d

### DEBUG_LOG_LEVEL = 0x7f0e

### DISCOVERY = 0x7f0f
This packet is sent by the controller when it wants to find all of the hubs that are connected. First the controller sends a discovery packet to the broadcast address. The parent hub then sends back its address by replying to the discovery packet, but changing the src address from the broadcast address to the one of the parent hub. The parent hub also then proceeds to send out a discovery packet to every possible child address (0-254, but not the address of the parent (verify)) over RS485 and forwards their responses back to the controller.

No Payload

#### Response payload
```
[0] (bool) Packet from parent
```

Packet from parent indicates if the parent is the originator of the packet or if it was retransmitted after being received from RS485
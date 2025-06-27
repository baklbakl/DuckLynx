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

#### Src address
This is always zero for the controller. It may always be zero, but not if the parent hub messes with it when repeating packets

#### Message number
- Message number should never equal zero. It starts at one. If it overflows to zero it should be reset to one
- Any response packets should use the same message number as their request

#### Reference number
- Always seems to be zero

#### Command ID
- Bit 15 indicates if the packet is a response (1) or a command (0). 

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
[0] = (bool) Attention Needed
```

### NACK = 0x7f02
Only ever a reply
#### Payload
```
[0] = NACK Code
```

### GET_MODULE_STATUS = 0x7f03

### KEEP_ALIVE = 0x7f04
Only ever send as a command  
No payload

### FAIL_SAFE = 0x7f05

### SET_NEW_MODULE_ADDRESS = 0x7f06

### QUERY_INTERFACE = 0x7f07

### START_DOWNLOAD = 0x7f08

### DOWNLOAD_CHUNK = 0x7f09

### SET_MODULE_LED_COLOR = 0x7f0a

### GET_MODULE_LED_COLOR = 0x7f0b

### SET_MODULE_LED_PATTERN = 0x7f0c

### GET_MODULE_LED_PATTERN = 0x7f0d

### DEBUG_LOG_LEVEL = 0x7f0e

### DISCOVERY = 0x7f0f
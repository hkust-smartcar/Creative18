# Creative18

## Specifications

### I. Coodinate System

Global Origin at top left corner of game field, unit mm (integer), x-axis right going, y axis down going. Global rotational angle defined by x-axis to y-axis direction, in radian(float)

### II. Protocol

#### A. Constants

```
//Package Anotations
HEAD = 0xAA,
END = 0xFF,

//Package Types
kACK = 0x00,
kRequestSetMotor = 0x01,
kRequestEncoder = 0x02,
kResponseEncoder = 0x03,
kRequestMove = 0x04,
kRequestSetMotorById = 0x05,
kRequestEncoderById = 0x06,
kResponseEncoderById = 0x07,
kRequestEncoders = 0x08,
kResponseEncoders = 0x09,
kFeedGlobalRotation = 0x0A,
kFeedGlobalTranslation = 0x0B,
kFeedCorners = 0xA0
```

#### B. Package structure

```
|HEAD|LENGTH|PACKAGE_TYPE|PACKAGE_ID|DATA|...|CHECKSUM|END|
```

where:
`HEAD`	 	= `0xAA`

`LENGTH` 		= length of whole package including header and end

`PACKAGE_TYPE` = package Type, determine the function of package

`PACKAGE_ID` 	= unique id of package over a machine, for ACK packages

`CHECKSUM` 	= sum of all bytes in package mod 256

`END` 		= `0xFF`

#### C. Communication Flow

##### 1. Roles

Master: the tempory camera17 coard

Slave: the tempory balance17 board

Super: the UI/ OpenMV/ other high level instances...

OpenMV

Ui

#### 2. Flow

- the `sendPackageImmediate` function send the package only once and will not take care the acknowledge
- the `queuePackage` function will keep sending the package until corresponding acknowledgement is received. It is done by pushing the package to a queue, 
- `peroid` function will call  `sendPackageImmediate` function for each package inside the queue, which should be done periodically
- `listen` function will parse the incoming bytes from Rx
- `buildBufferPackage` convert byte array to package
- `packageHandler` is called when package is built. 
  - If the package is not an acknowledgement package, it will send an acknowledgement package for that; 
  - if the package is an acknowledgement package, it will remove corresponding package from queue

### D. Protocol

##### 1. Motor Control

###### Data:

`int16_t` `motorpower` (PWM)

###### Response:

`<none>`

```sequence
Master -> Slave: kRequestSetMotor(Power)
Slave -> Master: kACK
Note over Slave: motor0 setpower
```

##### 2. Request Encoder

###### Data:

`<none>`

###### Response:

`int32_t` `encoder_count` (accumunative, from `kResponseEncoder` Type)

###### Note:

if calling await version will halt the master until recieve the count

```sequence
Master->Slave: kRequestEncoder
Slave->Master: kACK
Note over Slave: get encoder0 count
Slave->Master: kResponseEncoder(count)
Master->Slave: kACK
```

##### 3. Move

###### Data:

`int16_t` `x_speed` (TBC)

`int16_t` `y_speed` (TBC)

###### Response:

`<none>`

```sequence
Super->Master: kRequestMove(x,y)
Master->Super: kACK
Note over Master: Set Moter0, Motor1
Master->Slave: kRequestSetMotor(power)
Slave->Master: kACK
Note over Slave: Set Motor0
```

##### 4. Set Motor By Id

###### Data: 

`uin8_t` `id`

###### Return:

`<none>`

```sequence
Super->Master: kRequestSetMotorById(id, power)
Master->Super: kACK
Note over Master: if id<2: Set Motor by id
Note over Master: if id==2: request slave
Master-->Slave: kRequestSetMotor(power)
Slave-->Master: kACK
```

##### 5. Request Encoders by id

###### data

`uint8_t` `id`

###### return 

`int32_t` `count`

```sequence
Super->Master: kRequestEncoderById(id)
Master->Super: kACK
Note over Master: if id<2: Get Encoder
Note over Master: if id==2: Request Encoder
Master-->Slave: kRequestEncoder
Slave-->Master: kACK
Note over Slave: encoder0 get count
Slave-->Master: kResponseEncoder(count)
Master-->Slave: kACK
Master->Super: kResponseEncoderById(id, count)
Super->Master: kACK
```

##### 6. Request Encoders

###### data:

`none`

###### response:

`int32_t` `encoder0_count, encoder1_count, encoder2_count`

```sequence
Super->Master: kRequestEncoders
Master->Super: kACK
Note over Master: Encoder0,1 Get Count
Master->Slave: kRequestEncoder
Slave->Master: kACK
Note over Slave: encoder0 get count
Slave->Master: kResponseEncoder(count)
Master->Slave: kACK
Master->Super: kResponseEncoders(count0, count1, count2)
Super->Master: kACK
```
##### 7. Feed Global Rotation

###### data

`float` rotation angle

`uint8_t` time_spent_on_calculation

###### response

`none`

```sequence
Note over Super: new frame
Super->Master: kFeedGlobalRotation(angle,time)
Master->Super: kACK
```

##### 8. Feed Global Translation

###### data:

`int32_t` `x_translation, y_translation`

`uint8_t` time_spent_on_calculation

###### return:

`none`

```sequence
Note over Super: new frame
Super->Master: kFeedTranslation
Master->Super: kACK
```

##### 9. Feed Corners

###### data:

`uint16_t` `frame_id`

`uint8_t` `chunk_id` _(the buffer length should be limited below 100)_

`uint16_t[]` `x0, y0, x1, y1, ...`

###### response:

`<none>`

```sequence
Note over OpenMV: on frame
OpenMV -> Ui: kFeedCorners(frame_id, chunk_id ...)
OpenMV -> Ui: kFeedCorners(frame_id, chunk_id ...)
Ui -> OpenMV: kACK
Ui -> OpenMV: kACK
```


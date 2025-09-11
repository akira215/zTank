# zTank PCB
A dedicated PCB using ESP32C6 µC as been designed to optimize reliability and costs.

![Iso view of zTank PCB](<screenshot/zTankV1_1 iso.png>)

## Tools

 The design of the PCB has been done with KiCad 9.0. Particular footprint and 3D STEP file are included in the `/footprints` folder for sake of maintenance

## Design

 The design is based on ESP32C6 WROOM with the following peripherials:
 - USB for power, flash device and output serial debug / info.
 - Powering circuit is done using MCP1826S 
 - A 4-20mA loop is built based on a LM2735X (1.6MHz) to raise voltage to about 20V
 - 4 channels 4-20mA sensors is provided and converted using ADS1115IDGS. It communicate with µC with i2c port
 - a debounced contact is provided for pulse counter
 - A push button is also provided for very simple HMI
 - PCB is also equipped by a LED for simpl HMI
 - Finally PCB is equipped with header that can be used for a touchscreen device in case of future needs (software lib already written and tested)

 Component size has been selected to 0805, except ESD protective diodes that are smaller.

### USB

  ![USB schematic](<screenshot/zTank USB.png>)

  USB socket is grounded to the main ground planes (2 sides).
  USB Lines `USB-` and `USB+` are directly connected to ESP32C6 to flash and monitor µC.
  `VBUS` is used to provide `+5V` (which is in fact lower than +5V) via a protective diode `D3`.

  All this 3 lines are protected by ESD protective diode.

### Voltage regulator

  ![Regulator schematic](<screenshot/zTank MCP1826S.png>)

  All the board is powered using `+3.3V`. This voltage is provided by a fixed low-voltage regulator MCP1826S-3302E that can provide up to 1000mA.
  Fixed voltage regulator requires only few external component, in this case only capacitors are installed as per datasheet.


### 4-20mA Voltage circuit

  ![LM2735X schematic](<screenshot/zTank LM2735X.png>)

  To avoid noise on lines and due to voltage drop at each stage, 4-20mA loop voltage shall be at around 20V. Care shall be taken when changing this voltage as it may require adjustment (in the shunt values for instance) to avoid damaging other components.

  This voltage is produced from `+5V` using a boost type regulator LM2735X (@1.6MHz). Computation of the external components are provided in the LibreOffice Spreadsheet [PCB comput spreadsheet](<2024-06-29 Comput.ods>). A 33µH inductance has been selected to get a clean output voltage to avoid noise on sensors line.

### 4-20mA conversion

 ![4 channel connector schematic](<screenshot/zTank 4channel connector.png>)

  4 channels are available via Phoenix contact 8 poles connector. Each channel input current is injected in a 133R shunt (low tolerance resistor) to output a voltage below 3.3V. A 100nF capacitor is provided to reduce noise.

  ![ADS1115 schematic](<screenshot/zTank ADS1115.png>)

  Each channel voltage is then converted in a digital signal using an ADS1115IDGS, that communicate with µC using i2c protocol. A ready pin is used to trigger an interrupt to the µC when data are available.
  External components are:
   - 10k resistor connected to the ground for the address of the device (only one on board so gnd is the common one)
   - 10k pull-up resistor on each signal (`SCL`, `SDA`, `RDY`) connected to µC
   - decoupling capacitor for power lines and rearmable fuse to protect circuit

### Debounced contact

![Debounced pulse schematic](<screenshot/zTank Pulse contact.png>)

### µC ESP32C6 WROOM

![µC schematic](<screenshot/zTank ESP32C6.png>)

### Push Button

### Led

### Header



## PCB fabrication

## Assembly


## Usage


### To be developped




## Credits



# zTank
Tank survey, embedded sofware on dedicated PCB using ESP32C6 µC

# Description

This repository contain all the development for an electronic device that will monitor a water tank (such as rain collect tank) using zigbee to transmit data to a coordinator. It contains: 
 * A PCB design [PCB design](PCB/README.md)
 * An enclosure design to be printed in 3D [Enclosure design](3Dprint/README.md)
 * Source code of the embedded software on the board (in an ESP32C6 µC)
 * An External converter for Zigbee2MQTT service [external converter](external_converters)

# Principle

The board will collect data from various sensors surch as:
 * Upstream static pressure (pump side)
 * Downstream static pressure (home side) - difference between upstream and downstream static pressure is used to estimate filters clogging
 * Water level sensor, to estimate the available water volume in the tank
 * water pulse meter to count the consumption in the home
 * Additionally on board temperature sensor could be monitor

 ![zTank connections](<img/zTank connections.png>)

 The 3 first sensors shall be 4-20mA sensor directly connected to the board. The water counter shall be a dry contact meter, connected to the board

 All these sensors have a calibration factor that can be updated directly form Zigbee2MQTT GUI. To avoid losing calibration, the device itself will keep updated theses factor in its Non Volatile Storage.

![z2m settings (specific)](<img/zTank z2m settings specific.png>)

A 4-20mA input is available for any other futher sensor requirement. Adding a sensor will require to :
 * Update the embedded software
 * Update the external converter on Zigbee2MQTT side

## Usage

  * Short press button to join the zigbee network (coordinator shall grant pairing) -> LED shall flash, until network is joined, it then shutoff
  * Long press button to leave network. When the LED is fixed on, the device is not connected to the network neither is trying to join

## Sensor selection

To develop

## Zigbee2MQTT integration

To develop

## Build Instructions

```
git submodule add https://github.com/akira215/esp-ash-components.git components
```
Create a partition.csv for zigbee and configure menuconfig with custom partition table

## Current Status

Still work in progress, but is completely running. All bugs 
could be reported to try to improve it.

## PCB design

[PCB design](PCB/README.md)

## Enclosure design design

[Enclosure design](3Dprint/README.md)

## Caveats

### To be developped

OTA update


## Usage and Examples



## Changes

### New in version 0.0.0


## Credits

 * [Zigbee2MQTT](https://github.com/Koenkk/zigbee2mqtt)


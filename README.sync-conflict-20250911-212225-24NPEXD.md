# zTank
Tank survey, embedded sofware on dedicated PCB using ESP32C6 µC

## Requirements

## Usage
 - short press button to join the zigbee network (coordinator shall grant pairing) -> LED shall flash, until network is joined, it then shutoff
  - long press button to leave network. When the LED is fixed on, the device is not connected to the network neither is trying to join


## Build Instructions

```
git submodule add https://github.com/akira215/esp-ash-components.git components
```
Create a partition.csv for zigbee and configure menuconfig with custom partition table

## Current Status

Still work in progress, but is completely running. All bugs 
could be reported to try to improve it.

## PCB design

![Alt text](PCB/screenshot/TankMonitorV1_1.png?raw=true "3D view of PCB board")

## PCB design

[text](3Dprint/README.md)

## Caveats

### To be developped

OTA update


## Usage and Examples



## Changes

### New in version 0.0.0


## Credits



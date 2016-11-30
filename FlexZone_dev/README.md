# FlexZone

This is the CCS workspace for the FlexZone project which runs on a CC2640. It should be portable between machines and OSes as long as you import the correct vars.ini file for Linked Resources to point to the correct local directories of required packages.

The required packages are below:
- BLE-Stack v2.01.00
- TI-RTOS v2.13.00
- CC26XXware v2.21.01
- SYSBIOS v6.42.00

**Sample 'vars.ini' for Windows (BLE-Stack v2.01):**
```
CC26XXWARE = C:\ti\tirtos_simplelink_2_13_00_06\products\cc26xxware_2_21_01_15600
TI_RTOS_DRIVERS_BASE = C:\ti\tirtos_simplelink_2_13_00_06\packages
DRIVERLIB_PATH = C:\ti\tirtos_simplelink_2_13_00_06\products\cc26xxware_2_21_01_15600\driverlib\bin\ccs
XDCTOOLS_PATH = C:\ti\tirex-content\xdctools_3_32_00_06_core\packages
BIOS_PATH = C:\ti\tirtos_simplelink_2_13_00_06\products\bios_6_42_00_08\packages
TI_RTOS_BASE = C:\ti\tirtos_simplelink_2_13_00_06\packages
```

The two projects provided are
- FlexZoneApp
- FlexZoneStack
- FlexApp (Currently just test project to port functionality to CC2640 on PCB)

The Stack project just needs to be loaded to flash once and any future changes can be made to the App project.

##Required Changes
###Drivers
- [x] Integrate EMG
- [x] Integrate Accelerometer
- [x] Clean up FlexZone.c/.h to remove all ProjectZero references

###Processing
- [ ] Packetization
- [ ] Communication Stack/Workflow

###Power Management
- [ ] BLE Power Management
- [x] AUX/ADC Power Management
- [ ] System Clock Reduction
- [ ] AUX Clock Reduction
# FlexZone

This is the CCS workspace for the FlexZone project which runs on a CC2640. It should be portable between machines and OSes as long as you import the correct vars.ini file for Linked Resources to point to the correct local directories of required packages.

The required packages are below:
- BLE-Stack v2.01.00
- TI-RTOS v2.13.00
- CC26XXware v2.21.01
- SYSBIOS v6.42.00

A new updated version will require the following packages based on the BLE-Stack v2.02:
- BLE-Stack v2.02.00
- TI-RTOS v2.18.00
- CC26XXware v2.23.03
- SYSBIOS v6.45.02

**Sample 'vars.ini' for Windows (BLE-Stack v2.01):**
```
CC26XXWARE = C:\ti\tirtos_simplelink_2_13_00_06\products\cc26xxware_2_21_01_15600
TI_RTOS_DRIVERS_BASE = C:\ti\tirtos_simplelink_2_13_00_06\packages
DRIVERLIB_PATH = C:\ti\tirtos_simplelink_2_13_00_06\products\cc26xxware_2_21_01_15600\driverlib\bin\ccs
XDCTOOLS_PATH = C:\ti\tirex-content\xdctools_3_32_00_06_core\packages
BIOS_PATH = C:\ti\tirtos_simplelink_2_13_00_06\products\bios_6_42_00_08\packages
TI_RTOS_BASE = C:\ti\tirtos_simplelink_2_13_00_06\packages
```

**Sample 'vars.ini' for Windows (BLE-Stack v2.02):**
```
EXT_TI_PRODUCTS_DIR = C:\ti
CC26XXWARE = ${EXT_TI_PRODUCTS_DIR}\tirtos_cc13xx_cc26xx_2_18_00_03\products\cc26xxware_2_23_03_17162
TI_RTOS_DRIVERS_BASE = ${EXT_TI_PRODUCTS_DIR}\tirtos_cc13xx_cc26xx_2_18_00_03\products\tidrivers_cc13xx_cc26xx_2_16_01_13\packages
DRIVERLIB_PATH = ${EXT_TI_PRODUCTS_DIR}\tirtos_cc13xx_cc26xx_2_18_00_03\products\cc26xxware_2_23_03_17162\driverlib\bin\ccs
XDCTOOLS_PATH = ${EXT_TI_PRODUCTS_DIR}\tirex-content\xdctools_3_32_00_06_core\packages
BIOS_PATH = ${EXT_TI_PRODUCTS_DIR}\tirtos_cc13xx_cc26xx_2_18_00_03\products\bios_6_45_02_31\packages
TI_RTOS_BASE = ${EXT_TI_PRODUCTS_DIR}\tirtos_cc13xx_cc26xx_2_18_00_03\packages
```

The two projects provided are
- FlexZoneApp
- FlexZoneStack

The Stack project just needs to be loaded to flash once and any future changes can be made to the App project.

##Required Changes
###Drivers
- [ ] Integrate EMG
- [ ] Integrate Accelerometer
- [ ] Clean up FlexZone.c/.h to remove all ProjectZero references

###Processing
- [ ] Packetization
- [ ] Communication Stack/Workflow

###Power Management
- [ ] BLE Power Management
- [ ] AUX/ADC Power Management
- [ ] System Clock Reduction
- [ ] AUX Clock Reduction
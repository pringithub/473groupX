################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
build-752113807: C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Projects/ble/ProjectZero/CC26xx/CCS/Config/appBLE_UartLog.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"C:/ti/tirex-content/xdctools_3_31_01_33_core/xs" --xdcpath="C:/ti/tirex-content/tirtos_simplelink_2_13_00_06/packages;C:/ti/tirex-content/tirtos_simplelink_2_13_00_06/products/bios_6_42_00_08/packages;C:/ti/tirex-content/tirtos_simplelink_2_13_00_06/products/uia_2_00_02_39/packages;C:/ti/ccsv6/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M3 -p ti.platforms.simplelink:CC2650F128 -r release -c "C:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS" --compileOptions "-mv7M3 --code_state=16 -me -O4 --opt_for_speed=0 --include_path=\"C:/Users/Rohit/Documents/TI/FlexZoneApp/Profiles\" --include_path=\"C:/Users/Rohit/Documents/TI/FlexZoneApp/Application\" --include_path=\"C:/Users/Rohit/Documents/TI/FlexZoneApp/ICallBLE\" --include_path=\"C:/Users/Rohit/Documents/TI/FlexZoneApp/Startup\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Projects/ble/Profiles/DevInfo\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Projects/ble/ProjectZero/CC26xx/Source/Application\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Components/uart_log\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Components/applib/heap\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Components/services/sdata\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Projects/ble/Profiles/Roles/CC26xx\" --include_path=\"C:/ti/tirex-content/tirtos_simplelink_2_13_00_06/products/cc26xxware_2_21_01_15600\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Components/ble/controller/CC26xx/include\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Components/ble/hci\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Components/ble/host\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Components/ble/include\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Components/hal/include\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Components/hal/target/CC2650TIRTOS\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Components/hal/target/_common/cc26xx\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Components/icall/include\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Components/osal/include\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Components/services/saddr\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Projects/ble/ICall/Include\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Projects/ble/Include\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Projects/ble/Profiles/Roles\" --include_path=\"C:/ti/tirex-content/simplelink/ble_cc26xx_2_01_00_44423_cloud/Projects/ble/common/cc26xx\" --include_path=\"C:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/include\" -g --gcc --define=CC26XX --define=CC26XXWARE --define=USE_ICALL --define=xdc_runtime_Assert_DISABLE_ALL --define=Xxdc_runtime_Log_DISABLE_ALL --define=POWER_SAVING --define=DEBUG --define=HEAPMGR_SIZE=3000 --define=ICALL_MAX_NUM_ENTITIES=6 --define=ICALL_RAM0_ADDR=0x200043E8 --define=ICALL_STACK0_ADDR=0x0000E000 --define=MAX_NUM_BLE_CONNS=1 --define=ccs --define=GAPROLE_TASK_STACK_SIZE=520 --define=ICALL_MAX_NUM_TASKS=3 --define=xdc_FILE=\"\"\"\" --define=UARTLOG_NUM_EVT_BUF=32 --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi  " "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: build-752113807
configPkg/compiler.opt: build-752113807
configPkg/: build-752113807



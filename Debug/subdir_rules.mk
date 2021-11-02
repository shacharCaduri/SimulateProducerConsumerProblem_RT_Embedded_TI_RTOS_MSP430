################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
configPkg/linker.cmd: ../empty.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"C:/ti/xdctools_3_31_01_33_core/xs" --xdcpath="C:/ti/tirtos_msp43x_2_14_03_28/packages;C:/ti/tirtos_msp43x_2_14_03_28/products/bios_6_42_03_35/packages;C:/ti/tirtos_msp43x_2_14_03_28/products/uia_2_00_02_39/packages;C:/ti/ccsv6/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.msp430.elf.MSP430X -p ti.platforms.msp430:MSP430F5529 -r release -c "C:/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.5" --compileOptions "-vmspx --abi=eabi --data_model=restricted --use_hw_mpy=F5 --include_path=\"C:/ti/ccsv6/ccs_base/msp430/include\" --include_path=\"C:/TI_RTOS/Workspace/RT_FinProj_Part1_HadadYanousYazdiKaduri\" --include_path=\"C:/ti/tirtos_msp43x_2_14_03_28/products/MSPWare_2_00_00_40c/driverlib/MSP430F5xx_6xx\" --include_path=\"C:/ti/tirtos_msp43x_2_14_03_28/packages/ti/drivers/wifi/cc3100/Simplelink\" --include_path=\"C:/ti/tirtos_msp43x_2_14_03_28/packages/ti/drivers/wifi/cc3100/Simplelink/Include\" --include_path=\"C:/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.5/include\" -g --define=__MSP430F5529__ --define=ccs --define=MSP430WARE --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal  " "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/compiler.opt: | configPkg/linker.cmd
configPkg/: | configPkg/linker.cmd

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.5/bin/cl430" -vmspx --abi=eabi --data_model=restricted --use_hw_mpy=F5 --include_path="C:/ti/ccsv6/ccs_base/msp430/include" --include_path="C:/TI_RTOS/Workspace/RT_FinProj_Part1_HadadYanousYazdiKaduri" --include_path="C:/ti/tirtos_msp43x_2_14_03_28/products/MSPWare_2_00_00_40c/driverlib/MSP430F5xx_6xx" --include_path="C:/ti/tirtos_msp43x_2_14_03_28/packages/ti/drivers/wifi/cc3100/Simplelink" --include_path="C:/ti/tirtos_msp43x_2_14_03_28/packages/ti/drivers/wifi/cc3100/Simplelink/Include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.5/include" -g --define=__MSP430F5529__ --define=ccs --define=MSP430WARE --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
fram-utilities/ctpl/ctpl.obj: ../fram-utilities/ctpl/ctpl.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="C:/Users/Steven/Documents/MIDI(DINO)" --include_path="C:/Users/Steven/Documents/MIDI(DINO)/fram-utilities/nvs" --include_path="C:/Users/Steven/Documents/MIDI(DINO)/fram-utilities/ctpl" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE --define=CTPL_STACK_SIZE=160 -g --printf_support=full --diag_suppress=10420 --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="fram-utilities/ctpl/ctpl.d_raw" --obj_directory="fram-utilities/ctpl" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

fram-utilities/ctpl/ctpl_low_level.obj: ../fram-utilities/ctpl/ctpl_low_level.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="C:/Users/Steven/Documents/MIDI(DINO)" --include_path="C:/Users/Steven/Documents/MIDI(DINO)/fram-utilities/nvs" --include_path="C:/Users/Steven/Documents/MIDI(DINO)/fram-utilities/ctpl" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE --define=CTPL_STACK_SIZE=160 -g --printf_support=full --diag_suppress=10420 --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="fram-utilities/ctpl/ctpl_low_level.d_raw" --obj_directory="fram-utilities/ctpl" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

fram-utilities/ctpl/ctpl_low_level_macros.obj: ../fram-utilities/ctpl/ctpl_low_level_macros.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="C:/ti/ccsv7/ccs_base/msp430/include" --include_path="C:/Users/Steven/Documents/MIDI(DINO)" --include_path="C:/Users/Steven/Documents/MIDI(DINO)/fram-utilities/nvs" --include_path="C:/Users/Steven/Documents/MIDI(DINO)/fram-utilities/ctpl" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.6.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ --define=_MPU_ENABLE --define=CTPL_STACK_SIZE=160 -g --printf_support=full --diag_suppress=10420 --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="fram-utilities/ctpl/ctpl_low_level_macros.d_raw" --obj_directory="fram-utilities/ctpl" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '



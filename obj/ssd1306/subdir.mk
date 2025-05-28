################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ssd1306/ssd1306.c 

OBJS += \
./ssd1306/ssd1306.o 

C_DEPS += \
./ssd1306/ssd1306.d 


# Each subdirectory must supply rules for building sources it contributes
ssd1306/%.o: ../ssd1306/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"/home/master/Workspace/MRS/poic-i2c/Debug" -I"/home/master/Workspace/MRS/poic-i2c/ssd1306" -I"/home/master/Workspace/MRS/poic-i2c/Core" -I"/home/master/Workspace/MRS/poic-i2c/User" -I"/home/master/Workspace/MRS/poic-i2c/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Ext_ADC/ads1115_Driver.c 

OBJS += \
./Core/Src/Ext_ADC/ads1115_Driver.o 

C_DEPS += \
./Core/Src/Ext_ADC/ads1115_Driver.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Ext_ADC/%.o Core/Src/Ext_ADC/%.su Core/Src/Ext_ADC/%.cyclo: ../Core/Src/Ext_ADC/%.c Core/Src/Ext_ADC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -IC:/Users/Ethan/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.2/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IC:/Users/Ethan/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.2/Drivers/STM32F4xx_HAL_Driver/Inc -IC:/Users/Ethan/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.2/Drivers/CMSIS/Include -IC:/Users/Ethan/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.2/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -IC:/Users/user/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Inc -IC:/Users/user/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -IC:/Users/user/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IC:/Users/user/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/CMSIS/Include -I"C:/Ethan/workshop/mcu/STM32-2/Markscat_prj01_git/M-STM32F407/Core/Inc/OLED" -O0 -ffunction-sections -fdata-sections -Wall -u _printf_float -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Ext_ADC

clean-Core-2f-Src-2f-Ext_ADC:
	-$(RM) ./Core/Src/Ext_ADC/ads1115_Driver.cyclo ./Core/Src/Ext_ADC/ads1115_Driver.d ./Core/Src/Ext_ADC/ads1115_Driver.o ./Core/Src/Ext_ADC/ads1115_Driver.su

.PHONY: clean-Core-2f-Src-2f-Ext_ADC


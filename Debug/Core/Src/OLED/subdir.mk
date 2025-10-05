################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/OLED/I2C_OLED_SH1106.c \
../Core/Src/OLED/I2C_OLED_fonts.c \
../Core/Src/OLED/I2C_OLED_fonts_Dimensions.c \
../Core/Src/OLED/I2C_OLED_horse_anim.c 

OBJS += \
./Core/Src/OLED/I2C_OLED_SH1106.o \
./Core/Src/OLED/I2C_OLED_fonts.o \
./Core/Src/OLED/I2C_OLED_fonts_Dimensions.o \
./Core/Src/OLED/I2C_OLED_horse_anim.o 

C_DEPS += \
./Core/Src/OLED/I2C_OLED_SH1106.d \
./Core/Src/OLED/I2C_OLED_fonts.d \
./Core/Src/OLED/I2C_OLED_fonts_Dimensions.d \
./Core/Src/OLED/I2C_OLED_horse_anim.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/OLED/%.o Core/Src/OLED/%.su Core/Src/OLED/%.cyclo: ../Core/Src/OLED/%.c Core/Src/OLED/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -IC:/Users/Ethan/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.2/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IC:/Users/Ethan/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.2/Drivers/STM32F4xx_HAL_Driver/Inc -IC:/Users/Ethan/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.2/Drivers/CMSIS/Include -IC:/Users/Ethan/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.2/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -IC:/Users/user/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Inc -IC:/Users/user/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -IC:/Users/user/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/CMSIS/Device/ST/STM32F4xx/Include -IC:/Users/user/STM32Cube/Repository/STM32Cube_FW_F4_V1.28.1/Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -u _printf_float -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-OLED

clean-Core-2f-Src-2f-OLED:
	-$(RM) ./Core/Src/OLED/I2C_OLED_SH1106.cyclo ./Core/Src/OLED/I2C_OLED_SH1106.d ./Core/Src/OLED/I2C_OLED_SH1106.o ./Core/Src/OLED/I2C_OLED_SH1106.su ./Core/Src/OLED/I2C_OLED_fonts.cyclo ./Core/Src/OLED/I2C_OLED_fonts.d ./Core/Src/OLED/I2C_OLED_fonts.o ./Core/Src/OLED/I2C_OLED_fonts.su ./Core/Src/OLED/I2C_OLED_fonts_Dimensions.cyclo ./Core/Src/OLED/I2C_OLED_fonts_Dimensions.d ./Core/Src/OLED/I2C_OLED_fonts_Dimensions.o ./Core/Src/OLED/I2C_OLED_fonts_Dimensions.su ./Core/Src/OLED/I2C_OLED_horse_anim.cyclo ./Core/Src/OLED/I2C_OLED_horse_anim.d ./Core/Src/OLED/I2C_OLED_horse_anim.o ./Core/Src/OLED/I2C_OLED_horse_anim.su

.PHONY: clean-Core-2f-Src-2f-OLED


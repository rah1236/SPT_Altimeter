################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/commands/ush_cmd.c \
../Core/Src/commands/ush_cmd_baro.c \
../Core/Src/commands/ush_cmd_cat.c \
../Core/Src/commands/ush_cmd_cd.c \
../Core/Src/commands/ush_cmd_echo.c \
../Core/Src/commands/ush_cmd_gps.c \
../Core/Src/commands/ush_cmd_help.c \
../Core/Src/commands/ush_cmd_ls.c \
../Core/Src/commands/ush_cmd_pwd.c \
../Core/Src/commands/ush_cmd_rfm95.c \
../Core/Src/commands/ush_cmd_servo.c \
../Core/Src/commands/ush_cmd_xxd.c 

OBJS += \
./Core/Src/commands/ush_cmd.o \
./Core/Src/commands/ush_cmd_baro.o \
./Core/Src/commands/ush_cmd_cat.o \
./Core/Src/commands/ush_cmd_cd.o \
./Core/Src/commands/ush_cmd_echo.o \
./Core/Src/commands/ush_cmd_gps.o \
./Core/Src/commands/ush_cmd_help.o \
./Core/Src/commands/ush_cmd_ls.o \
./Core/Src/commands/ush_cmd_pwd.o \
./Core/Src/commands/ush_cmd_rfm95.o \
./Core/Src/commands/ush_cmd_servo.o \
./Core/Src/commands/ush_cmd_xxd.o 

C_DEPS += \
./Core/Src/commands/ush_cmd.d \
./Core/Src/commands/ush_cmd_baro.d \
./Core/Src/commands/ush_cmd_cat.d \
./Core/Src/commands/ush_cmd_cd.d \
./Core/Src/commands/ush_cmd_echo.d \
./Core/Src/commands/ush_cmd_gps.d \
./Core/Src/commands/ush_cmd_help.d \
./Core/Src/commands/ush_cmd_ls.d \
./Core/Src/commands/ush_cmd_pwd.d \
./Core/Src/commands/ush_cmd_rfm95.d \
./Core/Src/commands/ush_cmd_servo.d \
./Core/Src/commands/ush_cmd_xxd.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/commands/%.o Core/Src/commands/%.su Core/Src/commands/%.cyclo: ../Core/Src/commands/%.c Core/Src/commands/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../LoRaMac/radio/sx1276 -I../Core/Inc/inc -I../Core/Inc/config -I../Core/Inc -I../LoRaMac/boards -I../LoRaMac/mac -I../LoRaMac/radio -I../LoRaMac/system -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -u _printf_float -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-commands

clean-Core-2f-Src-2f-commands:
	-$(RM) ./Core/Src/commands/ush_cmd.cyclo ./Core/Src/commands/ush_cmd.d ./Core/Src/commands/ush_cmd.o ./Core/Src/commands/ush_cmd.su ./Core/Src/commands/ush_cmd_baro.cyclo ./Core/Src/commands/ush_cmd_baro.d ./Core/Src/commands/ush_cmd_baro.o ./Core/Src/commands/ush_cmd_baro.su ./Core/Src/commands/ush_cmd_cat.cyclo ./Core/Src/commands/ush_cmd_cat.d ./Core/Src/commands/ush_cmd_cat.o ./Core/Src/commands/ush_cmd_cat.su ./Core/Src/commands/ush_cmd_cd.cyclo ./Core/Src/commands/ush_cmd_cd.d ./Core/Src/commands/ush_cmd_cd.o ./Core/Src/commands/ush_cmd_cd.su ./Core/Src/commands/ush_cmd_echo.cyclo ./Core/Src/commands/ush_cmd_echo.d ./Core/Src/commands/ush_cmd_echo.o ./Core/Src/commands/ush_cmd_echo.su ./Core/Src/commands/ush_cmd_gps.cyclo ./Core/Src/commands/ush_cmd_gps.d ./Core/Src/commands/ush_cmd_gps.o ./Core/Src/commands/ush_cmd_gps.su ./Core/Src/commands/ush_cmd_help.cyclo ./Core/Src/commands/ush_cmd_help.d ./Core/Src/commands/ush_cmd_help.o ./Core/Src/commands/ush_cmd_help.su ./Core/Src/commands/ush_cmd_ls.cyclo ./Core/Src/commands/ush_cmd_ls.d ./Core/Src/commands/ush_cmd_ls.o ./Core/Src/commands/ush_cmd_ls.su ./Core/Src/commands/ush_cmd_pwd.cyclo ./Core/Src/commands/ush_cmd_pwd.d ./Core/Src/commands/ush_cmd_pwd.o ./Core/Src/commands/ush_cmd_pwd.su ./Core/Src/commands/ush_cmd_rfm95.cyclo ./Core/Src/commands/ush_cmd_rfm95.d ./Core/Src/commands/ush_cmd_rfm95.o ./Core/Src/commands/ush_cmd_rfm95.su ./Core/Src/commands/ush_cmd_servo.cyclo ./Core/Src/commands/ush_cmd_servo.d ./Core/Src/commands/ush_cmd_servo.o ./Core/Src/commands/ush_cmd_servo.su ./Core/Src/commands/ush_cmd_xxd.cyclo ./Core/Src/commands/ush_cmd_xxd.d ./Core/Src/commands/ush_cmd_xxd.o ./Core/Src/commands/ush_cmd_xxd.su

.PHONY: clean-Core-2f-Src-2f-commands


################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/bmp280.c \
../Core/Src/main.c \
../Core/Src/rfm95.c \
../Core/Src/sam_m8q.c \
../Core/Src/servo.c \
../Core/Src/shell.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c \
../Core/Src/ush.c \
../Core/Src/ush_autocomp.c \
../Core/Src/ush_autocomp_state.c \
../Core/Src/ush_autocomp_utils.c \
../Core/Src/ush_commands.c \
../Core/Src/ush_file.c \
../Core/Src/ush_node.c \
../Core/Src/ush_node_mount.c \
../Core/Src/ush_node_utils.c \
../Core/Src/ush_parse.c \
../Core/Src/ush_parse_char.c \
../Core/Src/ush_parse_utils.c \
../Core/Src/ush_process.c \
../Core/Src/ush_prompt.c \
../Core/Src/ush_read.c \
../Core/Src/ush_read_char.c \
../Core/Src/ush_read_utils.c \
../Core/Src/ush_reset.c \
../Core/Src/ush_utils.c \
../Core/Src/ush_write.c \
../Core/Src/ush_write_utils.c 

OBJS += \
./Core/Src/bmp280.o \
./Core/Src/main.o \
./Core/Src/rfm95.o \
./Core/Src/sam_m8q.o \
./Core/Src/servo.o \
./Core/Src/shell.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o \
./Core/Src/ush.o \
./Core/Src/ush_autocomp.o \
./Core/Src/ush_autocomp_state.o \
./Core/Src/ush_autocomp_utils.o \
./Core/Src/ush_commands.o \
./Core/Src/ush_file.o \
./Core/Src/ush_node.o \
./Core/Src/ush_node_mount.o \
./Core/Src/ush_node_utils.o \
./Core/Src/ush_parse.o \
./Core/Src/ush_parse_char.o \
./Core/Src/ush_parse_utils.o \
./Core/Src/ush_process.o \
./Core/Src/ush_prompt.o \
./Core/Src/ush_read.o \
./Core/Src/ush_read_char.o \
./Core/Src/ush_read_utils.o \
./Core/Src/ush_reset.o \
./Core/Src/ush_utils.o \
./Core/Src/ush_write.o \
./Core/Src/ush_write_utils.o 

C_DEPS += \
./Core/Src/bmp280.d \
./Core/Src/main.d \
./Core/Src/rfm95.d \
./Core/Src/sam_m8q.d \
./Core/Src/servo.d \
./Core/Src/shell.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d \
./Core/Src/ush.d \
./Core/Src/ush_autocomp.d \
./Core/Src/ush_autocomp_state.d \
./Core/Src/ush_autocomp_utils.d \
./Core/Src/ush_commands.d \
./Core/Src/ush_file.d \
./Core/Src/ush_node.d \
./Core/Src/ush_node_mount.d \
./Core/Src/ush_node_utils.d \
./Core/Src/ush_parse.d \
./Core/Src/ush_parse_char.d \
./Core/Src/ush_parse_utils.d \
./Core/Src/ush_process.d \
./Core/Src/ush_prompt.d \
./Core/Src/ush_read.d \
./Core/Src/ush_read_char.d \
./Core/Src/ush_read_utils.d \
./Core/Src/ush_reset.d \
./Core/Src/ush_utils.d \
./Core/Src/ush_write.d \
./Core/Src/ush_write_utils.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../LoRaMac/radio/sx1276 -I../Core/Inc/inc -I../Core/Inc/config -I../Core/Inc -I../LoRaMac/boards -I../LoRaMac/mac -I../LoRaMac/radio -I../LoRaMac/system -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -u _printf_float -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/bmp280.cyclo ./Core/Src/bmp280.d ./Core/Src/bmp280.o ./Core/Src/bmp280.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/rfm95.cyclo ./Core/Src/rfm95.d ./Core/Src/rfm95.o ./Core/Src/rfm95.su ./Core/Src/sam_m8q.cyclo ./Core/Src/sam_m8q.d ./Core/Src/sam_m8q.o ./Core/Src/sam_m8q.su ./Core/Src/servo.cyclo ./Core/Src/servo.d ./Core/Src/servo.o ./Core/Src/servo.su ./Core/Src/shell.cyclo ./Core/Src/shell.d ./Core/Src/shell.o ./Core/Src/shell.su ./Core/Src/stm32f1xx_hal_msp.cyclo ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su ./Core/Src/ush.cyclo ./Core/Src/ush.d ./Core/Src/ush.o ./Core/Src/ush.su ./Core/Src/ush_autocomp.cyclo ./Core/Src/ush_autocomp.d ./Core/Src/ush_autocomp.o ./Core/Src/ush_autocomp.su ./Core/Src/ush_autocomp_state.cyclo ./Core/Src/ush_autocomp_state.d ./Core/Src/ush_autocomp_state.o ./Core/Src/ush_autocomp_state.su ./Core/Src/ush_autocomp_utils.cyclo ./Core/Src/ush_autocomp_utils.d ./Core/Src/ush_autocomp_utils.o ./Core/Src/ush_autocomp_utils.su ./Core/Src/ush_commands.cyclo ./Core/Src/ush_commands.d ./Core/Src/ush_commands.o ./Core/Src/ush_commands.su ./Core/Src/ush_file.cyclo ./Core/Src/ush_file.d ./Core/Src/ush_file.o ./Core/Src/ush_file.su ./Core/Src/ush_node.cyclo ./Core/Src/ush_node.d ./Core/Src/ush_node.o ./Core/Src/ush_node.su ./Core/Src/ush_node_mount.cyclo ./Core/Src/ush_node_mount.d ./Core/Src/ush_node_mount.o ./Core/Src/ush_node_mount.su ./Core/Src/ush_node_utils.cyclo ./Core/Src/ush_node_utils.d ./Core/Src/ush_node_utils.o ./Core/Src/ush_node_utils.su ./Core/Src/ush_parse.cyclo ./Core/Src/ush_parse.d ./Core/Src/ush_parse.o ./Core/Src/ush_parse.su ./Core/Src/ush_parse_char.cyclo ./Core/Src/ush_parse_char.d ./Core/Src/ush_parse_char.o ./Core/Src/ush_parse_char.su ./Core/Src/ush_parse_utils.cyclo ./Core/Src/ush_parse_utils.d ./Core/Src/ush_parse_utils.o ./Core/Src/ush_parse_utils.su ./Core/Src/ush_process.cyclo ./Core/Src/ush_process.d ./Core/Src/ush_process.o ./Core/Src/ush_process.su ./Core/Src/ush_prompt.cyclo ./Core/Src/ush_prompt.d ./Core/Src/ush_prompt.o ./Core/Src/ush_prompt.su ./Core/Src/ush_read.cyclo ./Core/Src/ush_read.d ./Core/Src/ush_read.o ./Core/Src/ush_read.su ./Core/Src/ush_read_char.cyclo ./Core/Src/ush_read_char.d ./Core/Src/ush_read_char.o ./Core/Src/ush_read_char.su ./Core/Src/ush_read_utils.cyclo ./Core/Src/ush_read_utils.d ./Core/Src/ush_read_utils.o ./Core/Src/ush_read_utils.su ./Core/Src/ush_reset.cyclo ./Core/Src/ush_reset.d ./Core/Src/ush_reset.o ./Core/Src/ush_reset.su ./Core/Src/ush_utils.cyclo ./Core/Src/ush_utils.d ./Core/Src/ush_utils.o ./Core/Src/ush_utils.su ./Core/Src/ush_write.cyclo ./Core/Src/ush_write.d ./Core/Src/ush_write.o ./Core/Src/ush_write.su ./Core/Src/ush_write_utils.cyclo ./Core/Src/ush_write_utils.d ./Core/Src/ush_write_utils.o ./Core/Src/ush_write_utils.su

.PHONY: clean-Core-2f-Src


################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/bsp/epaper/epaper_demo.c \
../lib/bsp/epaper/epd4in2.c \
../lib/bsp/epaper/epdif.c \
../lib/bsp/epaper/epdpaint.c \
../lib/bsp/epaper/imagedata.c 

OBJS += \
./lib/bsp/epaper/epaper_demo.o \
./lib/bsp/epaper/epd4in2.o \
./lib/bsp/epaper/epdif.o \
./lib/bsp/epaper/epdpaint.o \
./lib/bsp/epaper/imagedata.o 

C_DEPS += \
./lib/bsp/epaper/epaper_demo.d \
./lib/bsp/epaper/epd4in2.d \
./lib/bsp/epaper/epdif.d \
./lib/bsp/epaper/epdpaint.d \
./lib/bsp/epaper/imagedata.d 


# Each subdirectory must supply rules for building sources it contributes
lib/bsp/epaper/%.o: ../lib/bsp/epaper/%.c lib/bsp/epaper/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DSTM32F1 -DNUCLEO_F103RB -DSTM32F103RBTx -DSTM32 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../lib/bsp -I../lib//middleware/MIDI/ -I../lib/bsp/VL53L0X -I../lib/bsp/MPU6050 -I../lib/bsp/iks01a1 -I../lib/bsp/iks01a1/Common -I../lib/bsp/iks01a1/hts221 -I../lib/bsp/iks01a1/lis3mdl -I../lib/bsp/iks01a1/lps22hb -I../lib/bsp/iks01a1/lps25hb -I../lib/bsp/iks01a1/lsm6ds0 -I../lib/bsp/iks01a1/lsm6ds3 -I../lib/bsp/tft_ili9341 -I../lib/bsp/tft_pcd8544 -I../lib/bsp/MCP23S17 -I../lib/hal/inc -I../lib/bsp/Common -I../lib/bsp/lcd2x16 -I../lib/bsp/MLX90614 -I../lib/bsp/MatrixKeyboard -I../lib/bsp/MatrixLed -I../lib/CMSIS/core -I../lib/CMSIS/device -I../lib/middleware/FatFs -I../lib/middleware/FatFs/src -I../lib/middleware/FatFs/src/drivers -I../appli -O0 -ffunction-sections -fdata-sections -Wall -Wextra -Wconversion -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfloat-abi=soft -mthumb -o "$@"

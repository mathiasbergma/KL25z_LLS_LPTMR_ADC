################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/LPTMR.c \
../source/Sleep.c \
../source/adc_irq.c \
../source/color_rgb.c \
../source/mtb.c \
../source/semihost_hardfault.c \
../source/uart.c 

OBJS += \
./source/LPTMR.o \
./source/Sleep.o \
./source/adc_irq.o \
./source/color_rgb.o \
./source/mtb.o \
./source/semihost_hardfault.o \
./source/uart.o 

C_DEPS += \
./source/LPTMR.d \
./source/Sleep.d \
./source/adc_irq.d \
./source/color_rgb.d \
./source/mtb.d \
./source/semihost_hardfault.d \
./source/uart.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MKL25Z128VLK4 -DCPU_MKL25Z128VLK4_cm0plus -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\Mathias\Desktop\ISD\Kode\Sleep\board" -I"C:\Users\Mathias\Desktop\ISD\Kode\Sleep\source" -I"C:\Users\Mathias\Desktop\ISD\Kode\Sleep" -I"C:\Users\Mathias\Desktop\ISD\Kode\Sleep\startup" -I"C:\Users\Mathias\Desktop\ISD\Kode\Sleep\CMSIS" -I"C:\Users\Mathias\Desktop\ISD\Kode\Sleep\drivers" -I"C:\Users\Mathias\Desktop\ISD\Kode\Sleep\utilities" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/unused/echo.c \
../src/unused/rxperf.c \
../src/unused/tftpserver.c \
../src/unused/tftputils.c \
../src/unused/txperf.c \
../src/unused/urxperf.c \
../src/unused/utxperf.c 

OBJS += \
./src/unused/echo.o \
./src/unused/rxperf.o \
./src/unused/tftpserver.o \
./src/unused/tftputils.o \
./src/unused/txperf.o \
./src/unused/urxperf.o \
./src/unused/utxperf.o 

C_DEPS += \
./src/unused/echo.d \
./src/unused/rxperf.d \
./src/unused/tftpserver.d \
./src/unused/tftputils.d \
./src/unused/txperf.d \
./src/unused/urxperf.d \
./src/unused/utxperf.d 


# Each subdirectory must supply rules for building sources it contributes
src/unused/%.o: ../src/unused/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MicroBlaze gcc compiler'
	mb-gcc -Wall -O0 -g3 -c -fmessage-length=0 -I../../standalone_bsp_0/microblaze_0/include -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mno-xl-soft-div -mcpu=v8.50.c -mno-xl-soft-mul -Wl,--no-relax -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



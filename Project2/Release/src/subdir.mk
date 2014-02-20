################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/BigQ.cc \
../src/Comparison.cc \
../src/ComparisonEngine.cc \
../src/DBFile.cc \
../src/File.cc \
../src/Pipe.cc \
../src/Record.cc \
../src/Schema.cc \
../src/TwoWayList.cc \
../src/test.cc 

OBJS += \
./src/BigQ.o \
./src/Comparison.o \
./src/ComparisonEngine.o \
./src/DBFile.o \
./src/File.o \
./src/Pipe.o \
./src/Record.o \
./src/Schema.o \
./src/TwoWayList.o \
./src/test.o 

CC_DEPS += \
./src/BigQ.d \
./src/Comparison.d \
./src/ComparisonEngine.d \
./src/DBFile.d \
./src/File.d \
./src/Pipe.d \
./src/Record.d \
./src/Schema.d \
./src/TwoWayList.d \
./src/test.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



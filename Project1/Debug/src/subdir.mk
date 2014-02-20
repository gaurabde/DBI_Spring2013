################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../src/Comparison.o \
../src/ComparisonEngine.o \
../src/DBFile.o \
../src/File.o \
../src/Record.o \
../src/Schema.o \
../src/lex.yy.o \
../src/main.o \
../src/test.o \
../src/y.tab.o 

C_SRCS += \
../src/lex.yy.c \
../src/y.tab.c 

CC_SRCS += \
../src/Comparison.cc \
../src/ComparisonEngine.cc \
../src/DBFile.cc \
../src/File.cc \
../src/Record.cc \
../src/Schema.cc \
../src/TwoWayList.cc \
../src/main.cc \
../src/test.cc 

OBJS += \
./src/Comparison.o \
./src/ComparisonEngine.o \
./src/DBFile.o \
./src/File.o \
./src/Record.o \
./src/Schema.o \
./src/TwoWayList.o \
./src/lex.yy.o \
./src/main.o \
./src/test.o \
./src/y.tab.o 

C_DEPS += \
./src/lex.yy.d \
./src/y.tab.d 

CC_DEPS += \
./src/Comparison.d \
./src/ComparisonEngine.d \
./src/DBFile.d \
./src/File.d \
./src/Record.d \
./src/Schema.d \
./src/TwoWayList.d \
./src/main.d \
./src/test.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



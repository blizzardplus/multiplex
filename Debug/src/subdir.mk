################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Aggregator.cpp \
../src/Dispatcher.cpp \
../src/Linked_list.cpp \
../src/Main.cpp \
../src/Mutex.cpp \
../src/Proxifier.cpp \
../src/ProxyShuffler.cpp \
../src/Relay.cpp \
../src/RelayStream.cpp \
../src/SocksStream.cpp \
../src/UProxy.cpp \
../src/Util.cpp \
../src/Util_converter.cpp \
../src/debug.cpp \
../src/socks4.cpp 

OBJS += \
./src/Aggregator.o \
./src/Dispatcher.o \
./src/Linked_list.o \
./src/Main.o \
./src/Mutex.o \
./src/Proxifier.o \
./src/ProxyShuffler.o \
./src/Relay.o \
./src/RelayStream.o \
./src/SocksStream.o \
./src/UProxy.o \
./src/Util.o \
./src/Util_converter.o \
./src/debug.o \
./src/socks4.o 

CPP_DEPS += \
./src/Aggregator.d \
./src/Dispatcher.d \
./src/Linked_list.d \
./src/Main.d \
./src/Mutex.d \
./src/Proxifier.d \
./src/ProxyShuffler.d \
./src/Relay.d \
./src/RelayStream.d \
./src/SocksStream.d \
./src/UProxy.d \
./src/Util.d \
./src/Util_converter.d \
./src/debug.d \
./src/socks4.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



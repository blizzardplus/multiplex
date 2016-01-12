################################################################################
# Automatically-generated file. Do not edit!
################################################################################

LIBS := -lboost_system


# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
./Aggregator.cpp \
./Dispatcher.cpp \
./Linked_list.cpp \
./Main.cpp \
./Mutex.cpp \
./Proxifier.cpp \
./ProxyShuffler.cpp \
./Relay.cpp \
./RelayStream.cpp \
./SocksStream.cpp \
./UProxy.cpp \
./Util.cpp \
./Util_converter.cpp \
./debug.cpp \
./socks4.cpp 

OBJS += \
./Aggregator.o \
./Dispatcher.o \
./Linked_list.o \
./Main.o \
./Mutex.o \
./Proxifier.o \
./ProxyShuffler.o \
./Relay.o \
./RelayStream.o \
./SocksStream.o \
./UProxy.o \
./Util.o \
./Util_converter.o \
./debug.o \
./socks4.o 

CPP_DEPS += \
./Aggregator.d \
./Dispatcher.d \
./Linked_list.d \
./Main.d \
./Mutex.d \
./Proxifier.d \
./ProxyShuffler.d \
./Relay.d \
./RelayStream.d \
./SocksStream.d \
./UProxy.d \
./Util.d \
./Util_converter.d \
./debug.d \
./socks4.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ./%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Proxifier

# Tool invocations
Proxifier: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -L/usr/lib/x86_64-linux-gnu -pthread -o "Proxifier" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) Proxifier
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:




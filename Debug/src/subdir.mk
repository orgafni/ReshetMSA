################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Broker.cpp \
../src/MThread.cpp \
../src/MultipleTCPSocketListener.cpp \
../src/PeersRequestsDispatcher.cpp \
../src/TCPMessengerServer.cpp \
../src/TCPSocket.cpp \
../src/main.cpp 

OBJS += \
./src/Broker.o \
./src/MThread.o \
./src/MultipleTCPSocketListener.o \
./src/PeersRequestsDispatcher.o \
./src/TCPMessengerServer.o \
./src/TCPSocket.o \
./src/main.o 

CPP_DEPS += \
./src/Broker.d \
./src/MThread.d \
./src/MultipleTCPSocketListener.d \
./src/PeersRequestsDispatcher.d \
./src/TCPMessengerServer.d \
./src/TCPSocket.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



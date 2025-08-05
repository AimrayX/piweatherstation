# Compiler and flags
CC = gcc
CFLAGS = -g -Wall -Wextra -std=c11 -I./bme68x -I./grove-sunlight-sensor-c -I./PMSA003I-hardware-driver -I./ads1015-driver -I./RS485-wind-direction-transmitter

# Source files
SRC = main.c ./bme68x/bme68x.c ./bme68x/bme68x_platform.c ./grove-sunlight-sensor-c/si1151.c ./PMSA003I-hardware-driver/PMSA003I.c ./ads1015-driver/ads1015.c ./ads1015-driver/ads1015_platform.c ./RS485-wind-direction-transmitter/RS485_wind_direction.c

# Output executable name
TARGET = main

# Libraries to link (i2c-dev for I2C)
LDLIBS = -li2c -lpigpio -lmodbus -lm

# Default target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

# Clean build artifacts
clean:
	rm -f $(TARGET)

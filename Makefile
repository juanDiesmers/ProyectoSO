# Define compiler
CC=gcc
# Define flags
CFLAGS=-Wall -pthread

# Target for sensor executable
sensor: main_sensor.c common.h
	$(CC) $(CFLAGS) main_sensor.c -o sensor

# Target for monitor executable
monitor: main_monitor.c common.h
	$(CC) $(CFLAGS) main_monitor.c -o monitor

# Clean up the build
clean:
	rm -f sensor monitor

# All targets
all: sensor monitor

CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-pthread

# Nombres de los ejecutables
SENSOR_EXEC=Sensor
MONITOR_EXEC=Monitor

# Fuentes y objetos
SENSOR_SRC=main_sensor.c
SENSOR_OBJ=$(SENSOR_SRC:.c=.o)

MONITOR_SRC=main_monitorTest.c
MONITOR_OBJ=$(MONITOR_SRC:.c=.o)

all: $(SENSOR_EXEC) $(MONITOR_EXEC)

$(SENSOR_EXEC): $(SENSOR_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(MONITOR_EXEC): $(MONITOR_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(SENSOR_EXEC) $(MONITOR_EXEC) *.o

.PHONY: all clean

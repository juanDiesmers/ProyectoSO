#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define BUFFER_SIZE 256

typedef struct {
    int type;   // 1 para temperatura, 2 para pH
    double value;
} SensorData;

void generate_data(SensorData *data) {
    double base_value;
    double noise = ((double)rand() / RAND_MAX) * 2 - 1;  // Noise between -1 and 1
    if (data->type == 1) {
        base_value = 25.0 + noise;  // Base temperature around 25°C
        if (rand() % 100 < 5) {  // 5% chance to generate an outlier
            base_value = (rand() % 2) ? base_value + 10 : base_value - 10;
        }
    } else {
        base_value = 7.0 + noise;  // Base pH around 7.0
        if (rand() % 100 < 5) {  // 5% chance to generate an outlier
            base_value = (rand() % 2) ? base_value + 2 : base_value - 2;
        }
    }
    data->value = base_value;
}

int main(int argc, char *argv[]) {
    if (argc != 9) {
        fprintf(stderr, "Uso: %s -s [tipo_sensor] -t [tiempo] -f [archivo] -p [pipe_nominal]\n", argv[0]);
        return 1;
    }

    int sensorType = atoi(argv[2]);
    int interval = atoi(argv[4]);
    char *pipeName = argv[8];

    srand(time(NULL));  // Seed the random number generator

    int fd = open(pipeName, O_WRONLY);
    if (fd == -1) {
        perror("Error al abrir el pipe");
        return 1;
    }

    SensorData data;
    data.type = sensorType;

    while (1) {
        generate_data(&data);
        write(fd, &data, sizeof(data));
        printf("Enviando valor: %f al pipe\n", data.value);
        sleep(interval);
    }

    close(fd);
    return 0;
}

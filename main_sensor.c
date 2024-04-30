// main_sensor.c
// author:  Juan Diego Palacios Toledo
// fecha: 29/04/2024
// tema: Sensor principal

#include "common.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <bits/getopt_core.h>

void *sensor_thread(void *arg) {
    SensorData *data = (SensorData *)arg;
    char line[1024];
    int pipe_fd;

    // Abrir el pipe para enviar datos
    if ((pipe_fd = open(data->pipeName, O_WRONLY)) < 0) {
        perror("Failed to open pipe");
        pthread_exit(NULL);
    }

    FILE *file = fopen(data->fileName, "r");
    if (!file) {
        perror("Failed to open file");
        close(pipe_fd);
        pthread_exit(NULL);
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        double value = atof(line);
        if (value < 0) { // Ignorar valores negativos
            continue;
        }
        
        data->value = value;
        write(pipe_fd, data, sizeof(SensorData));
        sleep(data->interval);
    }

    fclose(file);
    close(pipe_fd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t tid;
    SensorData data = {0};  // Inicializar todos los campos de data a cero/NULL
    int opt;

    // Valores por defecto (opcional, dependiendo de la lógica que desees)
    data.type = -1;
    data.interval = -1;
    data.fileName = NULL;
    data.pipeName = NULL;

    // Procesar argumentos
    while ((opt = getopt(argc, argv, "s:t:f:p:")) != -1) {
        switch (opt) {
            case 's':
                data.type = atoi(optarg);
                break;
            case 't':
                data.interval = atoi(optarg);
                break;
            case 'f':
                data.fileName = optarg;
                break;
            case 'p':
                data.pipeName = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s -s sensorType -t interval -f fileName -p pipeName\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Validar los datos iniciales antes de proceder
    if (data.type == -1 || data.interval == -1 || data.fileName == NULL || data.pipeName == NULL) {
        fprintf(stderr, "Invalid or insufficient parameters provided\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&tid, NULL, sensor_thread, &data) != 0) {
        perror("Failed to create thread");
        exit(EXIT_FAILURE);
    }

    pthread_join(tid, NULL);
    return 0;
}

// main_sensor.c
// author:  Juan Diego Palacios Toledo
// fecha: 29/04/2024
// tema: Sensor principal

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {
    if (argc != 9) {
        fprintf(stderr, "Uso: %s -s [tipo_sensor] -t [tiempo] -f [archivo] -p [pipe_nominal]\n", argv[0]);
        return 1;
    }

    int sensorType = atoi(argv[2]);
    int interval = atoi(argv[4]);
    char *filename = argv[6];
    char *pipeName = argv[8];

    int fd = open(pipeName, O_WRONLY);
    if (fd == -1) {
        perror("Error al abrir el pipe");
        return 1;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error al abrir archivo de mediciones");
        close(fd);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
        double value = atof(buffer);
        if (value < 0) {
            printf("Ignorando valor negativo: %f\n", value);
            continue; // Ignorar valores negativos
        }
        
        printf("Enviando valor: %f al pipe\n", value);
        write(fd, &value, sizeof(value));
        sleep(interval);
    }

    fclose(file);
    close(fd);

    return 0;
}

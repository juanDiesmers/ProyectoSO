// main_monitor.c
// author:  Juan Diego Palacios Toledo
// fecha: 29/04/2024
// tema: Monitor

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 256
#define MAX_MEASUREMENTS 100

typedef struct {
    double measurements[MAX_MEASUREMENTS];
    int count;
} MeasurementBuffer;

MeasurementBuffer tempBuffer, phBuffer;
pthread_mutex_t tempMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t phMutex = PTHREAD_MUTEX_INITIALIZER;

void* temp_handler(void *arg) {
    while (1) {
        pthread_mutex_lock(&tempMutex);
        if (tempBuffer.count > 0) {
            double temp = tempBuffer.measurements[--tempBuffer.count];
            pthread_mutex_unlock(&tempMutex);

            // Registrar la temperatura
            printf("Temperatura registrada: %f°C\n", temp);
            FILE *f = fopen("archivo_temperaturasTomadas.txt", "a");
            if (f) {
                fprintf(f, "%f°C\n", temp);
                fclose(f);
            }

            // Verificar si la temperatura está fuera del rango
            if (temp < 20.0 || temp > 31.6) {
                printf("Alerta: Temperatura fuera de rango: %f°C\n", temp);
                
            }

            sleep(1); // Simular procesamiento
        } else {
            pthread_mutex_unlock(&tempMutex);
        }
    }
    return NULL;
}
void* ph_handler(void *arg) {
    while (1) {
        pthread_mutex_lock(&phMutex);
        if (phBuffer.count > 0) {
            double ph = phBuffer.measurements[--phBuffer.count];
            pthread_mutex_unlock(&phMutex);

            // Registrar el pH
            printf("pH registrado: %f\n", ph);
            FILE *f = fopen("archivo_phTomados.txt", "a");
            if (f) {
                fprintf(f, "%f\n", ph);
                fclose(f);
            }

            // Verificar si el pH está fuera del rango
            if (ph < 6.0 || ph > 8.0) {
                printf("Alerta: pH fuera de rango: %f\n", ph);
                
            }

            sleep(1); // Simular procesamiento
        } else {
            pthread_mutex_unlock(&phMutex);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 9) {
        fprintf(stderr, "Uso: %s -b [tam_buffer] -t [file_temp] -h [file_ph] -p [pipe_nominal]\n", argv[0]);
        return 1;
    }

    char *pipeName = argv[8];
    int fd = open(pipeName, O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir el pipe");
        return 1;
    }

    pthread_t tempThread, phThread;
    pthread_create(&tempThread, NULL, temp_handler, NULL);
    pthread_create(&phThread, NULL, ph_handler, NULL);

    double value;
    while (read(fd, &value, sizeof(value)) > 0) {
        printf("Recibido valor: %f\n", value);
        if (value < 20 || value > 40) { 
            pthread_mutex_lock(&tempMutex);
            tempBuffer.measurements[tempBuffer.count++] = value;
            pthread_mutex_unlock(&tempMutex);
        } else if (value < 6 || value > 15) {
            pthread_mutex_lock(&phMutex);
            phBuffer.measurements[phBuffer.count++] = value;
            pthread_mutex_unlock(&phMutex);
        } 
    }

    close(fd);
    pthread_join(tempThread, NULL);
    pthread_join(phThread, NULL);

    return 0;
}

// main_monitorTest.c
#include "common.h"
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>

// Estructura para los buffers
SensorData phBuffer[BUFFER_SIZE];
SensorData tempBuffer[BUFFER_SIZE];
int phIndex = 0;
int tempIndex = 0;

// locks para los buffers
pthread_mutex_t phMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tempMutex = PTHREAD_MUTEX_INITIALIZER;

// Condicion de terminacion
bool running = true;

// Hilo recolector
void* recolector(void* arg) {
    int pipe_fd = *(int*)arg;
    SensorData data;

    while (running) {
        if(read(pipe_fd, &data, sizeof(data) > 0)) {
            if(data.value < 0) continue; // Descartar valores negativos

            // Clasificar los datos en los buffers
            if (data.type == SENSOR_TEMP) { // Temperatura
                pthread_mutex_lock(&tempMutex);
                if(tempIndex < BUFFER_SIZE) {
                    tempBuffer[tempIndex++] = data;
                }
                pthread_mutex_unlock(&tempMutex);
            } else if (data.type == SENSOR_PH) { // PH
                pthread_mutex_lock(&phMutex);
                if(phIndex < BUFFER_SIZE) {
                    phBuffer[phIndex++] = data;
                }
                pthread_mutex_unlock(&phMutex);
            } 
        } else {
            running = false; // Terminar si no hay mas datos
        }
    }
    pthread_exit(NULL);
}

// Hilo para PH
void* phHandler(void* arg) {
    while (running || phIndex > 0) {
        pthread_mutex_lock(&phMutex);
        if(phIndex > 0) {
            SensorData data = phBuffer[--phIndex];
            printf("PH: %f\n", data.value);
        }
        pthread_mutex_unlock(&phMutex);
    }
    pthread_exit(NULL);
}

// Hilo para temperatura
void* tempHandler(void* arg) {
    while (running || tempIndex > 0) {
        pthread_mutex_lock(&tempMutex);
        if(tempIndex > 0) {
            SensorData data = tempBuffer[--tempIndex];
            printf("Temperatura: %f\n", data.value);
        }
        pthread_mutex_unlock(&tempMutex);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t recolectorThread, phThread, tempThread;
    int pipe_fd;
    char *pipeName = "sensorPipe";

    // Crear el hilo si no existe
    mkfifo(pipeName, 0666);
    pipe_fd = open(pipeName, O_RDONLY);

    // Crear los hilos
    pthread_create(&recolectorThread, NULL, recolector, &pipe_fd);
    pthread_create(&phThread, NULL, phHandler, NULL);
    pthread_create(&tempThread, NULL, tempHandler, NULL);

    // Esperar a que terminen los hilos
    pthread_join(recolectorThread, NULL);
    pthread_join(phThread, NULL);
    pthread_join(tempThread, NULL);

    // Cerrar y eliminar el pipe
    close(pipe_fd);
    unlink(pipeName);

    return 0;
}
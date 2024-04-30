// common.h
// Created on: 29/04/2024
// Created by: Juan Diego Palacios Toledo

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Definiciones de los tipos de sensor
#define SENSOR_TEMP 1
#define SENSOR_PH 2

// Rangos para los sensores
#define MIN_PH 6.0
#define MAX_PH 8.0
#define MIN_TEMP 20.0
#define MAX_TEMP 31.6

// Definiciones de tamaño del buffer
#define BUFFER_SIZE 10

// Estructura para los mensajes entre sensores y monitor
typedef struct {
    int type;        // 1 para temperatura, 2 para PH
    double value;    // Valor medido
    int interval;    // Intervalo de tiempo en segundos entre mediciones (usado por el sensor)
    char* fileName;  // Nombre del archivo de donde se leen los datos (usado por el sensor)
    char* pipeName;  // Nombre del pipe para enviar los datos (usado por el sensor)
} SensorData;

#endif // COMMON_H

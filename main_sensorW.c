#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <windows.h>

#define BUFFER_SIZE 256
#define STOP_SIGNAL -1.0

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

    HANDLE hPipe = CreateFile(
        pipeName,          // pipe name
        GENERIC_WRITE,     // write access
        0,                 // no sharing
        NULL,              // default security attributes
        OPEN_EXISTING,     // opens existing pipe
        0,                 // default attributes
        NULL);             // no template file

    if (hPipe == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error al abrir el pipe. Código de error: %lu\n", GetLastError());
        return 1;
    }

    SensorData data;
    data.type = sensorType;

    while (1) {
        generate_data(&data);
        DWORD bytesWritten;
        if (!WriteFile(hPipe, &data, sizeof(data), &bytesWritten, NULL)) {
            fprintf(stderr, "Error al escribir en el pipe. Código de error: %lu\n", GetLastError());
            break;
        }
        printf("Enviando valor: %f al pipe\n", data.value);
        Sleep(interval * 1000);

        // Verificar si hay una señal de parada en el pipe
        DWORD bytesAvailable;
        if (PeekNamedPipe(hPipe, NULL, 0, NULL, &bytesAvailable, NULL) && bytesAvailable >= sizeof(SensorData)) {
            SensorData stopCheck;
            DWORD bytesRead;
            if (ReadFile(hPipe, &stopCheck, sizeof(stopCheck), &bytesRead, NULL) && bytesRead > 0 && stopCheck.value == STOP_SIGNAL) {
                printf("Received stop signal. Terminating sensor.\n");
                break;
            }
        }
    }

    CloseHandle(hPipe);
    return 0;
}

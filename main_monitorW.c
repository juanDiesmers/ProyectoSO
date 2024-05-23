#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

typedef struct {
    int type;   // 1 para temperatura, 2 para pH
    double value;
} SensorData;

#define PIPE_NAME "\\\\.\\pipe\\MyPipe"
#define TEMP_FILE "temperature_data.csv"
#define PH_FILE "ph_data.csv"
#define ALERT_FILE "alert_log.txt"
#define TEMP_MIN 20.0
#define TEMP_MAX 31.6
#define PH_MIN 6.0
#define PH_MAX 8.0
#define STOP_SIGNAL -1.0

unsigned __stdcall session_thread(void *data) {
    HANDLE hPipe = (HANDLE)data;
    DWORD bytesRead;
    SensorData sensorData;

    while (ReadFile(hPipe, &sensorData, sizeof(sensorData), &bytesRead, NULL) && bytesRead > 0) {
        if (sensorData.value == STOP_SIGNAL) {
            printf("Received stop signal. Terminating session.\n");
            break;
        }
        FILE *file;
        FILE *alertFile;
        if (sensorData.type == 1) {
            file = fopen(TEMP_FILE, "a");
            if (file) {
                fprintf(file, "Temperature,%.2f\n", sensorData.value);
                fclose(file);
            }
            if (sensorData.value < TEMP_MIN || sensorData.value > TEMP_MAX) {
                printf("ALERTA: Temperatura fuera de rango: %.2f\n", sensorData.value);
                alertFile = fopen(ALERT_FILE, "a");
                if (alertFile) {
                    fprintf(alertFile, "ALERTA: Temperatura fuera de rango: %.2f\n", sensorData.value);
                    fclose(alertFile);
                }
            }
        } else if (sensorData.type == 2) {
            file = fopen(PH_FILE, "a");
            if (file) {
                fprintf(file, "pH,%.2f\n", sensorData.value);
                fclose(file);
            }
            if (sensorData.value < PH_MIN || sensorData.value > PH_MAX) {
                printf("ALERTA: pH fuera de rango: %.2f\n", sensorData.value);
                alertFile = fopen(ALERT_FILE, "a");
                if (alertFile) {
                    fprintf(alertFile, "ALERTA: pH fuera de rango: %.2f\n", sensorData.value);
                    fclose(alertFile);
                }
            }
        }
    }

    CloseHandle(hPipe);
    return 0;
}

int main() {
    HANDLE hPipe;
    unsigned int threadId;

    // Ensure the headers are set for new files
    FILE *file = fopen(TEMP_FILE, "w");
    fprintf(file, "Type,Value\n");
    fclose(file);
    file = fopen(PH_FILE, "w");
    fprintf(file, "Type,Value\n");
    fclose(file);
    file = fopen(ALERT_FILE, "w");
    fclose(file);

    while (1) {
        hPipe = CreateNamedPipe(
            PIPE_NAME,
            PIPE_ACCESS_INBOUND,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            0,
            sizeof(SensorData),
            0,
            NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "Failed to create pipe. Error: %lu\n", GetLastError());
            return 1;
        }

        if (ConnectNamedPipe(hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) {
            _beginthreadex(NULL, 0, session_thread, (void*)hPipe, 0, &threadId);
        } else {
            CloseHandle(hPipe);
        }
    }

    // Send stop signal to sensors before exiting
    HANDLE hPipeWrite = CreateFile(PIPE_NAME, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hPipeWrite != INVALID_HANDLE_VALUE) {
        SensorData stopSignal = {0, STOP_SIGNAL};
        DWORD bytesWritten;
        WriteFile(hPipeWrite, &stopSignal, sizeof(stopSignal), &bytesWritten, NULL);
        CloseHandle(hPipeWrite);
    }

    return 0;
}

# Sistema de Monitoreo de Sensores

Este sistema simula el monitoreo de parámetros ambientales como la temperatura y el pH, utilizando hilos y comunicación interprocesos en C. El sistema está compuesto por un programa sensor que envía datos a un monitor a través de un pipe nombrado.

## Componentes

- `main_monitorTest.c`: Contiene la lógica del monitor que recibe y procesa los datos.
- `main_sensor.c`: Simula los sensores enviando datos al monitor.

## Descripción de los Componentes

### main_monitorTest.c
El monitor maneja la recepción de datos de los sensores. Utiliza múltiples hilos para procesar los datos de temperatura y pH almacenados en buffers separados.

### main_sensor.c
El sensor envía datos simulados de temperatura o pH al monitor a través de un pipe. Puede ser configurado para enviar datos a diferentes intervalos.


## Compilación y Ejecución

### Compilación

Utiliza el siguiente comando para compilar el monitor y el sensor. Asegúrate de estar en el directorio que contiene los archivos fuente.

```bash
make all
```

## Ejecucion
1. Ejecutar el monitor primero para asegurar que el pipe está disponible:
```bash
./Monitor -b [tam_buffer] -t [file_temp] -h [file_ph] -p [pipe_nominal]
```
2. Ejecutar el sensor
./monitorTest
```bash
./Sensor -s [tipo_sensor] -t [tiempo] -f [archivo] -p [pipe_nominal]
```
Reemplaza [tipo_sensor] (1 para temperatura, 2 para pH), [tirmpo] (en segundos), [archivo] (path al archivo de datos), y [pipe_nominal] es el nombre del pipe.

## Notas Adicionales

- **Inicio del Monitor**: Es crucial que el monitor esté corriendo antes de iniciar los sensores para evitar errores de "pipe no encontrado".
- **Argumentos Necesarios**: Asegúrate de proporcionar todos los argumentos necesarios al ejecutar el sensor para evitar errores de ejecución. Esto incluye especificar correctamente el tipo de sensor, el intervalo entre mediciones, el archivo de datos de donde se leen las mediciones, y el nombre del pipe.

## Problemas Comunes

- **Pipe no encontrado**: Este error ocurre si intentas iniciar los sensores antes de que el monitor esté activo. Asegúrate de que el monitor esté ejecutando antes de iniciar cualquier sensor para establecer el pipe necesario para la comunicación.
- **Datos no recibidos**: Si los datos no están siendo correctamente recibidos por el monitor, verifica que los nombres de los pipes y los archivos de datos sean consistentes y correctos en ambos programas. Además, asegúrate de que el formato y la integridad de los datos en los archivos de datos cumplan con lo esperado por el programa sensor.

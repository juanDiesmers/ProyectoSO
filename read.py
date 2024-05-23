import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

TEMP_MIN = 20.0
TEMP_MAX = 31.6
PH_MIN = 6.0
PH_MAX = 8.0

def animate(i):
    plt.cla()  # Clear the current axes
    try:
        temp_data = pd.read_csv('temperature_data.csv')
        if 'Temperature' in temp_data['Type'].values:
            temp_values = temp_data[temp_data['Type'] == 'Temperature']['Value'][-25:]  # Últimos 25 puntos
            plt.plot(temp_values, label='Temperature (°C)', color='blue')
            plt.axhline(y=TEMP_MIN, color='green', linestyle='--', label='Temp Min')
            plt.axhline(y=TEMP_MAX, color='green', linestyle='--', label='Temp Max')
    except (FileNotFoundError, pd.errors.EmptyDataError) as e:
        print(f"Temperature data is not available or corrupt: {e}")

    try:
        ph_data = pd.read_csv('ph_data.csv')
        if 'pH' in ph_data['Type'].values:
            ph_values = ph_data[ph_data['Type'] == 'pH']['Value'][-25:]  # Últimos 25 puntos
            plt.plot(ph_values, label='pH', color='red')
            plt.axhline(y=PH_MIN, color='orange', linestyle='--', label='pH Min')
            plt.axhline(y=PH_MAX, color='orange', linestyle='--', label='pH Max')
    except (FileNotFoundError, pd.errors.EmptyDataError) as e:
        print(f"pH data is not available or corrupt: {e}")

    plt.xlabel('Time Step')
    plt.ylabel('Sensor Value')
    plt.title('Real-Time Sensor Data Visualization')
    plt.legend()

ani = FuncAnimation(plt.gcf(), animate, interval=1000, save_count=50, cache_frame_data=False)
plt.show()

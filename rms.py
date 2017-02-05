#!/usr/bin/python3

import numpy as np
import scipy as sp
import scipy.signal as signal
import matplotlib.pyplot as plt



def waveform(t, duty):
	gate = signal.square(2 * np.pi * t, duty = duty) / 2 + 0.5
	y = np.sin(np.pi * t) * gate
	return y

def rms(y):
	return np.sqrt(np.mean(y**2))

def normalize(x):
	return x / np.max(x);

def rms_at_duty(duty):
	t = np.linspace(0, 1, 5000, endpoint=False)
	return rms(waveform(t, duty))


duty = np.linspace(0, 1, 5000)
f = np.vectorize(rms_at_duty)
volts_duty = normalize(f(duty))






volts = [0, 0.003126031508, 0.006251312828, 0.009377344336, 0.01250562641, 0.01875468867, 0.02813203301, 0.03750937734, 0.05626406602, 0.07501875469, 0.09377344336, 0.112528132, 0.1500375094, 0.1875468867, 0.2250562641, 0.2625656414, 0.3000750188, 0.3750937734, 0.4501125281, 0.5251312828, 0.6001500375, 0.6751687922, 0.7501875469, 0.8252063016, 0.9002250563, 1]
lumen_volts = [0, 0, 0, 0, 0, 0, 0, 1.86E-10, 2.90E-08, 0.0000007656612529, 0.000005800464037, 0.00002784222738, 0.000222737819, 0.0008584686775, 0.002459396752, 0.005568445476, 0.01067285383, 0.02900232019, 0.06218097448, 0.1120649652, 0.1851508121, 0.2825986079, 0.4060324826, 0.5580046404, 0.7331786543, 1]

bv = sp.interpolate.interp1d(volts, lumen_volts, kind='cubic')

bright_duty = bv(volts_duty)




# import datetime
# import pandas as pd
# import dateutil.parser


# filename = "data/Actual_37.05_-113.45_2006_UPV_172MW_5_Min.csv"
# solar_data = pd.read_csv(filename)
# solar_data['DateTime'] = solar_data['LocalTime'].apply(lambda d:dateutil.parser.parse(d))
# solar_data['Date'] = solar_data['DateTime'].apply(lambda d:d.date())
# solar_data['Time'] = solar_data['DateTime'].apply(lambda d:d.time())




f = sp.interpolate.interp1d(duty, bright_duty, fill_value="extrapolate")

lutx = normalize(np.linspace(0,1,32))
luty = normalize(f(lutx))

print(np.array2string(lutx, separator=', '))
print(np.array2string(luty, separator=', '))

plt.plot(lutx, luty, label='lut')
plt.plot(bright_duty, duty, label='data')
plt.legend()
plt.show()
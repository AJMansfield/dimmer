#!/usr/bin/python2.7

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


duty = np.linspace(0, 1, 5000, endpoint=True)
f = np.vectorize(rms_at_duty)
volts_duty = normalize(f(duty))



volts = [0, 0.003126031508, 0.006251312828, 0.009377344336, 0.01250562641, 0.01875468867, 0.02813203301, 0.03750937734, 0.05626406602, 0.07501875469, 0.09377344336, 0.112528132, 0.1500375094, 0.1875468867, 0.2250562641, 0.2625656414, 0.3000750188, 0.3750937734, 0.4501125281, 0.5251312828, 0.6001500375, 0.6751687922, 0.7501875469, 0.8252063016, 0.9002250563, 1]
bright_volts = [0, 0, 0, 0, 0, 0, 0, 0.00001362405407, 0.0001703006758, 0.0008750207157, 0.002408415254, 0.00527657345, 0.01492440347, 0.02929963613, 0.04959230537, 0.07462201737, 0.103309505, 0.1703006758, 0.2493611327, 0.3347610569, 0.4302915431, 0.531600045, 0.6372067817, 0.7469970819, 0.8562585207, 1 ]

bv = sp.interpolate.interp1d(volts, bright_volts, kind='cubic')


bright_duty = bv(volts_duty)


f = sp.interpolate.interp1d(duty, bright_duty, fill_value="extrapolate")

lutx = normalize(np.linspace(0,1,32))
luty = normalize(f(lutx))

print np.array2string(lutx, separator=', ')
print np.array2string(luty, separator=', ')

plt.plot(lutx, luty, label='lut')
plt.plot(bright_duty, duty, label='data')
plt.legend()
plt.show()
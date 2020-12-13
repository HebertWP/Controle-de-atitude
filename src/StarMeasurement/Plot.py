import time
from telnetlib import Telnet
import winsound
frequency = 2500  # Set Frequency To 2500 Hertz
duration = 300  # Set Duration To 1000 ms == 1 second
#from serial import Serial
import matplotlib.pyplot as plt
import numpy as np
import re

host = '192.168.1.106'
port = 555
ang = 1
comport = Telnet(host, port)
time.sleep(2)
    
def SerialReadFloat():
    read = comport.read_until(b',')
    read = str(read)
    read = float(read[2:-2])
    return read

p1=[]
n1=[]
p2=[]
n2=[]
t=[]

print("IN progress")
comport.write(b'C')
winsound.Beep(frequency, duration*10)
    
for i in range(int(360/ang)):
    print()
    input('Digite qualquer coisa e mova ' + str(ang))
    comport.write(b'C')
    winsound.Beep(frequency, duration)
    p1.append(SerialReadFloat())
    n1.append(SerialReadFloat())
    p2.append(SerialReadFloat())
    n2.append(SerialReadFloat())
    t.append(i*ang))
    print(i*ang)
   
comport.close()

fig,axs=plt.subplots(3,3)
fig.suptitle('Saida dos LDR ')

axs[0,0].axis('off')
axs[0,1].plot(t, p1)
axs[0,2].axis('off')

axs[1,0].plot(t, p2)
axs[1,1].axis('off')
axs[1,2].plot(t, n2)

axs[2,0].axis('off')
axs[2,1].plot(t, n1)
axs[2,2].axis('off')

plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, right=False)
plt.xlabel("Angulo [°]")
plt.show()
print('int StarSensor::_p1[12] = {')

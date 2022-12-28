# -*- coding: utf-8 -*-
""" 
22.07.2020 

Programa creado para el Canal de YouTobe Entec Tutoriales
Este programa es creado con fines de didacticos de desarrollo y educacion

"""

# Importamos los módulos necesarios
from threading import Thread
import time , collections
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import serial

isReceive = False #bandera para comenzar a recibir datos
isRun = True #bandera para recibir datos
value = 0.0


def getData():
	time.sleep(1.0) # da tiempo para resivir datos
	Conexion.reset_input_buffer() # resetea el bufer de entrada
	
	while (isRun): # lee datos
		global isReceive
		global value
		value = float(Conexion.readline().strip()) # leer el sensor
		isReceive = True
		


def plotData(self,Samples,Conexion,lines,lineValueText,lineLabel):
	data.append(value) #guarda lectura en ultima posicion
	lines.set_data(range(Samples),data) #Dibuja nueva linea
	lineValueText.set_text(lineLabel+'-' + str(round(value,2)))# muestra el valor del sensor 


SerialPort = '/dev/ttyUSB0'
baudRate = 9600

try:
	Conexion = serial.Serial(SerialPort,baudRate) 

except:
	print("No se puede Conectar el Arduino al puerto")
	print("Verifique que el Arduino este conectado")
	
		
Samples = 100 # Muestras
data = collections.deque([0] * Samples, maxlen = Samples) # vector de muestra
sampleTime = 100 #tiempo de muestreo

#limite de los ejes
xmin = 0
xmax = Samples
ymin = 0
ymax = 6


fig = plt.figure(figsize = (13,6)) #crea una figura
ax  = plt.axes(xlim=(xmin, xmax), ylim = (ymin , ymax))
plt.title("Real-time Sensor reading") # Titulo de la figura
ax.set_xlabel("Samples")
ax.set_ylabel("Voltaje value")

lineLabel = "Voltage" 
lines = ax.plot([], [], label = lineLabel) [0] # grafica datos iniciales y retorna lineas que representa la grafica
lineValueText = ax.text(0.85, 0.95, '', transform = ax.transAxes) # agrega texto en las cordenadas x,y

thread = Thread(target=getData) #crea objeto de la clase Thread
thread.start() #inicia subproseso


while isReceive != True: #espera hasta resivir datos
	print("Iniciando....")
	time.sleep(0.1)
	control = False

anim = animation.FuncAnimation(fig,plotData, fargs=(Samples,Conexion,lines,lineValueText,lineLabel), interval = sampleTime) #da movimineto a la grafica
plt.show() # muestra la grafica

isRun = False
thread.join() #finaliza subproseso antes de que el programa ejecute la siguiente instruccion
Conexion.close() # finaliza la conexion serial

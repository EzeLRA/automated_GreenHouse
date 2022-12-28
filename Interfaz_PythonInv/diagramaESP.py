from cgitb import text
import mimetypes
from signal import SIG_DFL
from turtle import width
import serial
import collections
import time
import numpy as np
import tkinter as tk
import matplotlib.pyplot as plt
import matplotlib.animation as animation 
from matplotlib.lines import Line2D

puertoSerial = ['COM1','COM2','COM3','COM4','COM5']
baudios = [9600,115200]


miVentana = tk.Tk()

miVentana.title("Ventana")
miVentana.geometry("500x500")

miVentana.config(bg="lightgray")

var1=tk.IntVar(miVentana)
#var1.set(0)
var2=tk.IntVar(miVentana)
#var2.set(0)

def lecturaSensores(self,Muestras,numData,esp,lineas1,lineas2):
    
    cadena = esp.readline().decode('ascii').strip()
    if cadena:
        pos = cadena.index(':')
        etiqueta= cadena[:pos]
        valor = cadena[pos+1:]
        
        if etiqueta == 'Temperatura(1)':
            temp1=valor
            for i in range(numData):
                data1[i].append(temp1)
                lineas1[i].set_data(range(Muestras),data1[i])

        if etiqueta == 'Humedad(1)':
            hum1=valor
            for i in range(numData):
                data2[i].append(hum1)
                lineas2[i].set_data(range(Muestras),data2[i])


Muestras = 50
tiempoMuestras = 0
numData = 4

xmin = 0
xmax = Muestras
ymin = [-10,0]
ymax = [30,100]
lineas1=[]
lineas2=[]
data1=[]
data2=[]

for i in range(numData):
            data1.append(collections.deque([0] * Muestras,maxlen=Muestras))
            lineas1.append(Line2D([],[],color='blue'))

            data2.append(collections.deque([0] * Muestras,maxlen=Muestras))
            lineas2.append(Line2D([],[],color='blue'))

def conectar():
    try:
        esp = serial.Serial(puertoSerial,baudios)
    except:
        print("Error de conexion") 


def Mostrar(esp):
        fig = plt.figure()
        ax1 = fig.add_subplot(2,2,1,xlim=(xmin,xmax),ylim=(ymin[0],ymax[0]))
        ax1.title.set_text('Temperatura')
        ax1.set_xlabel("Muestras")
        ax1.set_ylabel("Temperatura(Cº)")
        ax1.add_line(lineas1[0])

        ax2 = fig.add_subplot(2,2,2,xlim=(xmin,xmax),ylim=(ymin[1],ymax[1]))
        ax2.title.set_text('Humedad')
        ax2.set_xlabel("Muestras")
        ax2.set_ylabel("Humedad(%)")
        ax2.add_line(lineas2[0])
        anim = animation.FuncAnimation(fig,lecturaSensores,fargs=(Muestras,numData,esp,lineas1,lineas2),interval=tiempoMuestras)

        plt.show()

def Limpiar():
    plt.clf()
    plt.close()



botonEncender1=tk.Button(miVentana,text="Ver Grafica",command=Mostrar)
botonEncender1.pack()
botonEncender1.place(x=150,y=150)

botonEncender2=tk.Button(miVentana,text="Limpiar",command=Limpiar)
botonEncender2.pack()
botonEncender2.place(x=150,y=190)

botonEncender2=tk.Button(miVentana,text="Conectar",command=conectar)
botonEncender2.pack()
botonEncender2.place(x=150,y=190)

opciones1 = tk.OptionMenu(miVentana,var1,*baudios)
opciones1.config(width=6)
opciones1.pack(side='right',padx=10,pady=10)

opciones2 = tk.OptionMenu(miVentana,var2,*puertoSerial)
opciones2.config(width=6)
opciones2.pack(side='right',padx=10,pady=10)

miVentana.mainloop()
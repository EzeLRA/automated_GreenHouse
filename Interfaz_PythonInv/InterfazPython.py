#from mainFrame import MainFrame
from tkinter import Canvas, Frame,Label,StringVar,Button,Toplevel
import serial
import time
import tkinter as tk
import threading
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

class MainFrame(Frame):

    #Constructor de la clase
    def __init__(self, master=None,width=None, height=None):
        super().__init__(master, width=800, height=480)                
        self.master = master    
        self.master.protocol('WM_DELETE_WINDOW',self.askQuit)
        self.pack()
        self.hilo1 = threading.Thread(target=self.getSensorValues,daemon=True)
        self.arduino = serial.Serial("COM4",115200,timeout=1.0)
        time.sleep(1)
        
        self.estadoDHT22N1 = StringVar()
        self.tempN1 = StringVar()
        self.humN1 = StringVar()

        self.estadoDHT22N2 = StringVar()
        self.tempN2 = StringVar()
        self.humN2 = StringVar()

        self.humS1 = StringVar()
        self.humS2 = StringVar()
        
        self.create_widgets()
        self.isRun=True
        self.hilo1.start()

    #Metodo de cierre
    def askQuit(self):
        self.isRun=False
        time.sleep(1.1)
        self.arduino.close()
        self.hilo1.join(0.1)
        self.master.quit()
        self.master.destroy()
        print("*** finalizando...")

    #Metodo de lectura
    def getSensorValues(self):
        while self.isRun:
            cad =self.arduino.readline().decode('ascii').strip()
            if cad:         
                pos=cad.index(":")
                label=cad[:pos]
                value=cad[pos+1:]
                if label== 'EstadoDHT22(1)':
                    self.estadoDHT22N1.set(value)
                if label == 'Humedad(1)':
                    self.humN1.set(value)                   
                if label == 'Temperatura(1)':
                    self.tempN1.set(value)
                
                if label== 'EstadoDHT22(2)':
                    self.estadoDHT22N2.set(value)
                if label == 'Humedad(2)':
                    self.humN2.set(value)                   
                if label == 'Temperatura(2)':
                    self.tempN2.set(value)

                if label == 'humedadSuperficial1':
                    self.humS1.set(value) 
                if label == 'humedadSuperficial2':
                    self.humS2.set(value) 


    #Metodo para agregar la estructura    
    def create_widgets(self):
        #Estetica
        canvas=Canvas(self,width=800, height=480) 
        canvas.place(x=0,y=0)

        canvas.create_line(700,-1,700,480,fill='#c8c8c8',width=500)

        #Sensores

        #DHT22
        Label(self,text="Conexion con DHT22(1): ").place(x=30,y=20)
        Label(self,width=10,textvariable=self.estadoDHT22N1).place(x=180,y=20)
        
        Label(self,text="Temperatura: ").place(x=30,y=40)
        Label(self,width=6,textvariable=self.tempN1).place(x=180,y=40)

        Label(self,text="Humedad: ").place(x=30,y=60)
        Label(self,width=6,textvariable=self.humN1).place(x=180,y=60) 

        Label(self,text="Conexion con DHT22(2): ").place(x=30,y=100)
        Label(self,width=10,textvariable=self.estadoDHT22N2).place(x=180,y=100)
        
        Label(self,text="Temperatura: ").place(x=30,y=120)
        Label(self,width=6,textvariable=self.tempN2).place(x=180,y=120)

        Label(self,text="Humedad: ").place(x=30,y=140)
        Label(self,width=6,textvariable=self.humN2).place(x=180,y=140)


        #YL-69
        Label(self,text="Conexion con YL-69(1): ").place(x=30,y=180)
        #Label(self,width=10,textvariable=self.estadoDHT22).place(x=180,y=180)

        Label(self,text="Humedad Superficial: ").place(x=30,y=200)
        Label(self,width=10,textvariable=self.humS1).place(x=180,y=200)
        
        Label(self,text="Conexion con YL-69(2): ").place(x=30,y=220)
        #Label(self,width=10,textvariable=self.estadoDHT22).place(x=180,y=220)

        Label(self,text="Humedad Superficial: ").place(x=30,y=240)
        Label(self,width=10,textvariable=self.humS2).place(x=180,y=240)

        #Boton de graficas
        Button(self,text="Mostrar Graficas",command=self.graficas).place(x=320,y=20)
       
    

     
def main():
    root = tk.Tk()
    root.wm_title("Interfaz Principal del Invernadero")
    app = MainFrame(root)
    app.mainloop()

if __name__=="__main__":
    main()
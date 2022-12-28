from tkinter import Frame,Label,Button,Checkbutton,Scale,StringVar,IntVar
import serial
import time
import threading


class MainFrame(Frame):

    def __init__(self, master=None):
        super().__init__(master, width=800, height=480)                
        self.master = master    
        self.master.protocol('WM_DELETE_WINDOW',self.askQuit)
        self.pack()
        self.hilo1 = threading.Thread(target=self.getSensorValues,daemon=True)
        self.arduino = serial.Serial("COM3",9600,timeout=1.0)
        time.sleep(1)
        
        self.estadoDHT22 = StringVar()
        self.temp = StringVar()
        self.hum = StringVar()
    #    self.valor_led = IntVar()
        
        self.create_widgets()
        self.isRun=True
        self.hilo1.start()

    def askQuit(self):
        self.isRun=False
    #    self.arduino.write('mot:0'.encode('ascii'))
        time.sleep(1.1)
    #    self.arduino.write('led:0'.encode('ascii'))
        self.arduino.close()
        self.hilo1.join(0.1)
        self.master.quit()
        self.master.destroy()
        print("*** finalizando...")

    def getSensorValues(self):
        while self.isRun:
            cad =self.arduino.readline().decode('ascii').strip()
            if cad:         
                pos=cad.index(":")
                label=cad[:pos]
                value=cad[pos+1:]
                if label== 'EstadoDHT22':
                    self.estadoDHT22.set(value)
                if label == 'Humedad':
                    self.hum.set(value)                   
                if label == 'Temperatura':
                    self.temp.set(value)
        

        
    #def fEnviaLed(self):
    #    cad='led:' + str(self.value_led.get())
    #    self.arduino.write(cad.encode('ascii'))
    #    print(cad)

        
    def create_widgets(self):
        Label(self,text="Conexion con DHT22: ").place(x=30,y=20)
        Label(self,width=10,textvariable=self.estadoDHT22).place(x=180,y=20)
        
        Label(self,text="Temperatura: ").place(x=30,y=50)
        Label(self,width=6,textvariable=self.temp).place(x=120,y=50)

        Label(self,text="Humedad: ").place(x=30,y=80)
        Label(self,width=6,textvariable=self.hum).place(x=120,y=80)

    #    Checkbutton(self, text="Encender/Apagar Led", variable=self.value_led,
    #    onvalue=1, offvalue=0,command=self.fEnviaLed).place(x=30, y=90)                 
        
        

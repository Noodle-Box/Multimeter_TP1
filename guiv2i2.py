from tkinter import *
from tkinter import messagebox
import threading
import serial.tools.list_ports;
import serial
import tkinter as tk

root = tk.Tk()

hold = 0
currentmode = "DC"
minimum = 0
maximum = 0
current = 0 #current reading
offset = 1 #dc offset data
continuity = 2.0 #continuity value

ports = [comport.device for comport in serial.tools.list_ports.comports()]
port = serial.Serial()
baudrate = 9600
foundport = False
resetflag = 0


def readline():
    while 1:
        current = port.readline()
        print(current)
        currentmeas.set(current)

readings = threading.Thread(target=readline)




def read_serial():
    global hold
    global resetflag
    global port

    while True:
        if port:
            ser_string = str(port.readline())
            print(ser_string)
        else:
            print("no reading")
    
    

#Relevant menus for buttons
#backlight level
LEVELS = [
            "1",
            "2",
            "3",
            "4",
            "5"
        ]

#Create buttons for modes
DCButton = Button(root, text="DC", command=lambda: dc_voltage())
ACButton = Button(root, text="AC", command=lambda: AC())
ResButton = Button(root, text="Resistance", command=lambda: Resistance())
ContButton = Button(root, text="Continuity", command=lambda: Cont())
HoldButton = Button(root, text="Hold", command=lambda: toggleHold())
ResetButton = Button(root, text="Reset")
PortButton = tk.Button(root, text="Connect",command= lambda: connectport())

        #OPTIONMENU FOR BRIGHTNESS
var = StringVar()
var.set(LEVELS[4])
lightmenu = tk.OptionMenu(root, var, *LEVELS)
var_port = tk.StringVar()
portoption = tk.OptionMenu(root, var_port, *ports)
        
#LABELS
modelabel = Label(root,text="Current Mode: DC")
lightlabel = Label(root,text="Brightness")
minlabel = Label(root, text="Minimum: " + str(minimum))
maxlabel = Label(root, text ="Maximum: " + str(maximum))
data = Label(root, text= "Current Reading: ")
portnamelabel = tk.Label(root, text="Select Port: ")
currentmeas = tk.StringVar()
currentmeas.set('0')
reading = tk.Label(root, textvariable= currentmeas)
conport = tk.StringVar()
portconlabel = tk.Label(root, textvariable=conport)

#Grid the buttons on
lightlabel.grid(row=0,column=0)
lightmenu.grid(row=0,column=1)
portnamelabel.grid(row=0,column=3)
portoption.grid(row=0,column=4)
PortButton.grid(row=0,column=5)
DCButton.grid(row=1,column=1)
ACButton.grid(row=1,column=2)
ResButton.grid(row=1,column=3)
ContButton.grid(row=1,column=4)
modelabel.grid(row=1,column=0)
data.grid(row=3,column=0)
minlabel.grid(row=4,column=0)
maxlabel.grid(row=5,column=0)
ResetButton.grid(row=6,column=0)
HoldButton.grid(row=6,column=1)
reading.grid(row=3,column=1)
portconlabel.grid(row=6,column=3)


def dc_voltage():
        currentmode = "DC"
        modelabel.config(text= "Current Mode: DC")
        s = port.readline()
        print(s)

def AC():
        currentmode = "AC"
        modelabel.config(text= "Current Mode: AC")
        data.config(text=   "DC Offset: " + str(offset)   + "   AC Voltage (Vrms): ")


def Resistance():
        currentmode = "Resistance"
        modelabel.config(text= "Current Mode: Resistance")
        data.config(text="Resistance: ")

def Cont():
        currentmode = "Contintuity"
        modelabel.config(text= "Current Mode: Continuity")
        data.config(text="Current reading: ")

def toggleHold():
        global hold
        hold ^= 1
        print(hold)



def connectport():
        #initialise port params
        global port
        global baudrate

        portname = var_port.get()

        try:
            port= serial.Serial(portname,baudrate)
            conport.set(str(portname))
            readings.start()
        except serial.serialutil.SerialException:
            port = None



if __name__ == "__main__":
    root.update()
    root.mainloop() 

from tkinter import *
from tkinter import messagebox
from threading import Thread
import serial.tools.list_ports;
import serial
import tkinter as tk

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
ser = NONE

foundport = False
resetflag = 0

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
    
    

class GUI:
    def __init__(self,master):
        self.meas = 0
        self.master = master
        self.currentMeas = StringVar()
        self.currentMeas.set("0")

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
        self.DCButton = Button(master, text="DC", command=lambda: self.dc_voltage())
        self.ACButton = Button(master, text="AC", command=lambda: self.AC())
        self.ResButton = Button(master, text="Resistance", command=lambda: self.Resistance())
        self.ContButton = Button(master, text="Continuity", command=lambda: self.Cont())
        self.HoldButton = Button(master, text="Hold", command=lambda: self.toggleHold())
        self.ResetButton = Button(master, text="Reset")
        PortButton = tk.Button(master, text="Connect",command=self.connectport)

        #OPTIONMENU FOR BRIGHTNESS
        self.var = StringVar()
        self.var.set(LEVELS[4])
        self.lightmenu = tk.OptionMenu(master, self.var, *LEVELS)
        self.var_port = tk.StringVar()
        portoption = tk.OptionMenu(master, self.var_port, *ports)
        
        #LABELS
        self.modelabel = Label(master,text="Current Mode: DC")
        self.lightlabel = Label(master,text="Brightness")
        self.minlabel = Label(master, text="Minimum: " + str(minimum))
        self.maxlabel = Label(master, text ="Maximum: " + str(maximum))
        self.data = Label(master, text= "Current Reading: " + str(current))
        portnamelabel = tk.Label(master, text="Select Port: ")
        self.updatelabel = tk.Label(master, textvariable=self.currentMeas)

        #Grid the buttons on
        self.lightlabel.grid(row=0,column=0)
        self.lightmenu.grid(row=0,column=1)
        portnamelabel.grid(row=0,column=3)
        portoption.grid(row=0,column=4)
        PortButton.grid(row=0,column=5)
        self.DCButton.grid(row=1,column=1)
        self.ACButton.grid(row=1,column=2)
        self.ResButton.grid(row=1,column=3)
        self.ContButton.grid(row=1,column=4)
        self.modelabel.grid(row=1,column=0)
        self.data.grid(row=3,column=0)
        self.minlabel.grid(row=4,column=0)
        self.maxlabel.grid(row=5,column=0)
        self.ResetButton.grid(row=6,column=0)
        self.HoldButton.grid(row=6,column=1)
        self.updatelabel.grid(row=7,column=1)

    def dc_voltage(self):
        currentmode = "DC"
        current = str(port.readline())
        self.modelabel.config(text= "Current Mode: DC" + current)

    def AC(self):
        currentmode = "AC"
        self.modelabel.config(text= "Current Mode: AC")
        self.data.config(text=   "DC Offset: " + str(offset)   + "   AC Voltage: " + str(current) + " Vrms")


    def Resistance(self):
        currentmode = "Resistance"
        self.modelabel.config(text= "Current Mode: Resistance")
        self.data.config(text="Resistance: " + str(current))

    def Cont(self):
        currentmode = "Contintuity"
        self.modelabel.config(text= "Current Mode: Continuity")
        self.data.config(text="Current reading: " + str(current))

    def toggleHold(self):
        global hold
        hold ^= 1
        print(hold)

    def connectport(self):
        #initialise port params
        global port
        global baudrate

        portname = self.var_port.get()

        try:
            port= serial.Serial(portname,baudrate)
            portconlabel = tk.Label(self.master, text="Connected to\n" + portname)
            portconlabel.grid(row=6,column=3)
        except serial.serialutil.SerialException:
            port = None


if __name__ == "__main__":
    root = tk.Tk()
    GUI(root)
    root.update()
    root.mainloop() 

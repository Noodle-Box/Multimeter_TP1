from tkinter import *
import serial
from serial import Serial


#Declaration for root

class GUI():
    def __init__(self,master):
        #super().__init__()
        #Relevant drop down menus

        #for mode
        MODES = [
            "DC Voltage",
            "AC Voltage",
            "Resistance",
            "Continuity"
        ]
        variable1 = StringVar()
        variable1.set("Mode")
        mode = OptionMenu(master, variable1, *MODES)
        mode.grid(row=0, column=1)
        modelabel = Label(master, text="Current mode")
        modelabel.grid(row=0,column=0)

        #for hold
        OPTIONS = [
            'on',
            'off'
        ]
        variable2 = StringVar()
        variable2.set("status")
        holdlabel = Label(master, text= "Hold")
        hold = OptionMenu(master, variable2, *OPTIONS )
        hold.grid(row=1, column=1)
        holdlabel.grid(row=1, column=0)

        #backlight level
        LEVELS = [
            "1",
            "2",
            "3",
            "4",
            "5"
        ]
        variable3 = StringVar(master)
        variable3.set("Level")
        mode = OptionMenu(master, variable3, *LEVELS)
        mode.grid(row=2, column=1)
        lightlabel = Label(master, text="Backlight Level")
        lightlabel.grid(row=2, column=0)

        #readings, going to add relevant comands after pyserial implementation
        readinglabel = Label(master, text="Current reading")
        readinglabel.grid(row=3,column=0)
        readingentry = Entry(master, width = 10)
        readingentry.grid(row=3, column=1)

        Minlabel = Label(master,text="Current Minimum")
        Minlabel.grid(row=5,column=0)
        minentry = Entry(master, width=10)
        minentry.grid(row=5,column=1)


        Maxlabel = Label(master, text="Current Maximum")
        Maxlabel.grid(row=6,column=0)
        maxentry = Entry(master,width=10)
        maxentry.grid(row=6,column=1)

        #reset
        resetButton = Button(master, text="RESET")
        resetButton.grid(row=7,column=0)

        #current message
        messagelabel = Label(master, text="Current Message")
        messagelabel.grid(row=8,column=0)
        messageentry = Entry(master, width=10) 
        messageentry.grid(row=8,column=1)

        
        

if __name__ == "__main__":
    root = Tk()
    GUI(root)
    root.mainloop()
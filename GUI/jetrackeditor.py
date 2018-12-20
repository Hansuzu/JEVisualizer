from tkinter import *

class JETrackEditor:
    def __init__(self, parent, mediadir):
        self.parent = parent
        self.mediadir = mediadir
        self.master = None
        self.stuff = {}

        self.TYPE="WAV"
        self.FILE=""
        self.SHIFT="0.0"
        self.F0="16.35159783"
        self.F1="10000"
        self.FK="1.0594630943592953"
        self.CHLEN="1.0"
        self.THR="0.001"
        self.TRACK="0"

        self.vTYPE=StringVar()
        self.vFILE=StringVar()
        self.vSHIFT=StringVar()
        self.vF0=StringVar()
        self.vF1=StringVar()
        self.vFK=StringVar()
        self.vCHLEN=StringVar()
        self.vTHR=StringVar()
        self.vTRACK=StringVar()

    def save(self):
        self.TYPE=self.vTYPE.get()
        self.FILE=self.vFILE.get()
        self.SHIFT=self.vSHIFT.get()
        self.F0=self.vF0.get()
        self.F1=self.vF1.get()
        self.FK=self.vFK.get()
        self.CHLEN=self.vCHLEN.get()
        self.THR=self.vTHR.get()
        self.TRACK=self.vTRACK.get()
        
    def load(self):
        self.vTYPE.set(self.TYPE)
        self.vFILE.set(self.FILE)
        self.vSHIFT.set(self.SHIFT)
        self.vF0.set(self.F0)
        self.vF1.set(self.F1)
        self.vFK.set(self.FK)
        self.vCHLEN.set(self.CHLEN)
        self.vTHR.set(self.THR)
        self.vTRACK.set(self.TRACK)

    def view(self, master):
        self.master = master
        self.master.title("JEVGUI Track Editor")

        self.load()

        self.header = Label(self.master, text="Track editor", font=("Times", 12, "bold"))
        self.header.grid(row=0, columnspan=2)
        
        self.typelabel = Label(self.master, text="Type")
        self.typelabel.grid(row=1, column=0)
        self.typemenu = OptionMenu(self.master, self.vTYPE, "WAV", "SPCTR", "MMP", command=self.change_type)
        self.typemenu.grid(row=1, column=1)

        self.filelabel = Label(self.master, text="File")
        self.filelabel.grid(row=2, column=0)
        self.fileentry = Entry(self.master, textvariable=self.vFILE)
        self.fileentry.grid(row=2, column=1)
        self.filebutton = Button(self.master, text="Browse", command=self.browse_file)
        self.filebutton.grid(row=2, column=2)

        self.shiftlabel = Label(self.master, text="Shift")
        self.shiftlabel.grid(row=3, column=0)
        self.shiftentry = Entry(self.master, textvariable=self.vSHIFT)
        self.shiftentry.grid(row=3, column=1)


        self.okbutton = Button(master, text="Ok", command=self.ok)
        self.okbutton.grid(row=100, column=0)

        self.change_type(None)

    

    def name(self):
        return self.FILE.split("/")[-1]

    def browse_file(self):
        current = "/".join(self.fileentry.get().split("/")[:-1])
        if current=="":current=self.mediadir
        filetypes=()
        if self.vTYPE.get()=="WAV":filetypes=[("wav sound files", "*.wav")]
        if self.vTYPE.get()=="SPCTR":filetypes=[("spctr spectrum files", "*.spctr")]
        if self.vTYPE.get()=="MMP":filetypes=[("unpacked LMMS files", "*.mmp")]
        fname = filedialog.askopenfilename(initialdir=current, filetypes=filetypes)
        self.fileentry.delete(0, END)
        self.fileentry.insert(0, fname)
        self.master.lift()
        self.master.focus()

    
    def write_config(self, main, visualizer, row):
        visualizer.write("DOWNSPEED#"+str(row)+"=0.04\n")
        visualizer.write("UPSPEED#"+str(row)+"=0.04\n")

        main.write("    TYPE="+self.TYPE+"\n")
        main.write("    FILE=\""+self.FILE+"\"\n")
        main.write("    SHIFT="+self.SHIFT+"\n")
        if self.TYPE=="WAV":
            main.write("    F0="+self.F0+"\n")
            main.write("    F1="+self.F1+"\n")
            main.write("    FK="+self.FK+"\n")
            main.write("    CHLEN="+self.CHLEN+"\n")
            main.write("    THR="+self.THR+"\n")
        elif self.TYPE=="MMP":
            main.write("    TRACK="+self.TRACK)

    def ok(self):
        self.save()
        self.parent.edited()
        self.master.destroy()
        self.master=None

    def change_type(self, event):
        for i in self.stuff: self.stuff[i].destroy()
        self.stuff={}
        if self.vTYPE.get()=="WAV":
            self.stuff["WAVParamlabel"] = Label(self.master, text="Spectrum extraction settings", font=("bold"))
            self.stuff["WAVParamlabel"].grid(row=4, columnspan=2)

            self.stuff["f0label"] = Label(self.master, text="Lowest frequency")
            self.stuff["f0label"].grid(row=5, column=0)
            self.stuff["f0entry"]  = Entry(self.master, textvariable=self.vF0)
            self.stuff["f0entry"].grid(row=5, column=1)

            self.stuff["f1label"] = Label(self.master, text="Highest frequency")
            self.stuff["f1label"].grid(row=6, column=0)
            self.stuff["f1entry"]  = Entry(self.master, textvariable=self.vF1)
            self.stuff["f1entry"].grid(row=6, column=1)

            self.stuff["fklabel"] = Label(self.master, text="Frequency multiplier")
            self.stuff["fklabel"].grid(row=7, column=0)
            self.stuff["fkentry"]  = Entry(self.master, textvariable=self.vFK)
            self.stuff["fkentry"].grid(row=7, column=1)
            
            self.stuff["chlenlabel"] = Label(self.master, text="Maximum sample length")
            self.stuff["chlenlabel"].grid(row=8, column=0)
            self.stuff["chlenentry"]  = Entry(self.master, textvariable=self.vCHLEN)
            self.stuff["chlenentry"].grid(row=8, column=1)

            self.stuff["thrlabel"] = Label(self.master, text="Thresold")
            self.stuff["thrlabel"].grid(row=9, column=0)
            self.stuff["threntry"]  = Entry(self.master, textvariable=self.vTHR)
            self.stuff["threntry"].grid(row=9, column=1)
        elif self.vTYPE.get()=="SPCTR":
            pass
        elif self.vTYPE.get()=="MMP":
            self.stuff["tracklabel"] = Label(self.master, text="MMP Track")
            self.stuff["tracklabel"].grid(row=4, column=0)
            self.stuff["trackentry"] = Entry(self.master, textvariable=self.vTRACK)
            self.stuff["trackentry"].grid(row=4, column=1)


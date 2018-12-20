from tkinter import *
from tkinter import filedialog
from tkinter import messagebox
import os

class JECommonSettings:
    def __init__(self, parent):
        self.parent = parent

    def view(self, master):
        self.master=master
        EW = 40

        self.header = Label(self.master, text="Common settings", font=("Times", 12, "bold"))
        self.header.grid(columnspan=3)

        self.confplabel = Label(master, text="Config files directory ($CDIR$)")
        self.confplabel.grid(row=1, column=0)
        self.confpentry = Entry(master, width=EW)
        self.confpentry.insert(0, "../bin/config")
        self.confpentry.grid(row=1, column=1)
        self.confpentrybutton = Button(master, text="Browse", command=self.browse_cf_dir)
        self.confpentrybutton.grid(row=2, column=2)

        self.commandlabel = Label(master, text="JEVisualizer command")
        self.commandlabel.grid(row=2, column=0)
        self.commandentry = Entry(master, width=EW)
        self.commandentry.insert(0, "../bin/JEVisualizer --config $CDIR$/main.config $PARAMS$")
        self.commandentry.grid(row=2, column=1)

        self.medialabel = Label(master, text="Media directory")
        self.medialabel.grid(row=3, column=0)
        self.mediaentry = Entry(master, width=EW)
        self.mediaentry.insert(0, "../bin/media")
        self.mediaentry.grid(row=3, column=1)
        self.mediabutton = Button(master, text="Browse", command=self.browse_media_dir)
        self.mediabutton.grid(row=3, column=2)

        self.sfilelabel = Label(master, text="Soundfile for video ($SOUNDS$)")
        self.sfilelabel.grid(row=4, column=0)
        self.sfileentry = Entry(master, width=EW)
        self.sfileentry.insert(0, "")
        self.sfileentry.grid(row=4, column=1)
        self.sfilebutton = Button(master, text="Browse", command=self.browse_soundfile)
        self.sfilebutton.grid(row=4, column=2)

        self.outputlabel = Label(master, text="Output directory ($ODIR$)")
        self.outputlabel.grid(row=5, column=0)
        self.outputentry = Entry(master, width=EW)
        self.outputentry.insert(0, "../bin/output")
        self.outputentry.grid(row=5, column=1)
        self.outputbutton = Button(master, text="Browse", command=self.browse_output_dir)
        self.outputbutton.grid(row=5, column=2)

        self.outputfilelabel = Label(master, text="JEVisualizer output file ($OFILE$)")
        self.outputfilelabel.grid(row=6, column=0)
        self.outputfileentry = Entry(master, width=EW)
        self.outputfileentry.insert(0, "output_no_sounds.ogv")
        self.outputfileentry.grid(row=6, column=1)

        self.outputsfilelabel = Label(master, text="On-end-output file ($OSFILE$)")
        self.outputsfilelabel.grid(row=7, column=0)
        self.outputsfileentry = Entry(master, width=EW)
        self.outputsfileentry.insert(0, "output.ogv")
        self.outputsfileentry.grid(row=7, column=1)

        self.oecommandlabel = Label(master, text="On-end-command")
        self.oecommandlabel.grid(row=8, column=0)
        self.oecommandentry = Entry(master, width=EW)
        self.oecommandentry.insert(0, "ffmpeg -i '$ODIR$/$OFILE$' -i '$SOUNDS$' -shortest -vcodec copy '$ODIR$/$OSFILE$' -y")
        self.oecommandentry.grid(row=8, column=1)

    def getmediadir(self):
        return self.mediaentry.get()

    def get_config_path(self):
        path=self.confpentry.get()
        if len(path) and path[-1]!="/":path+="/"
        return path

    def get_ofile_path(self):
        ofilepath=self.outputentry.get()
        if ofilepath[-1]!="/":ofilepath+="/"
        return ofilepath

    def get_ofilename(self):
        return self.outputfileentry.get()
    
    def get_osfilename(self):
        return self.outputsfileentry.get()

    def get_oec_command(self):
        oec=self.oecommandentry.get()
        oec=self.replace_vars(oec)
        return oec

    def replace_vars(self, s):
        CDIR=self.confpentry.get()
        SOUNDS=self.sfileentry.get()
        ODIR=self.outputentry.get()
        OFILE=self.outputfileentry.get()
        OSFILE=self.outputsfileentry.get()
        return s.replace("$CDIR$", CDIR).replace("$SOUNDS$", SOUNDS).replace("$ODIR$", ODIR).replace("$OFILE$", OFILE).replace("$OSFILE$", OSFILE)

    def get_jevisualizer_command(self):
        jec=self.commandentry.get()
        jec=self.replace_vars(jec)
        return jec


    def browse_dir(self, entry):
        current = entry.get()
        dirname = filedialog.askdirectory(initialdir=current)
        entry.delete(0, END)
        entry.insert(0, dirname)
    def browse_soundfile(self):
        current = self.sfileentry.get()
        if current=="": current=self.mediaentry.get()
        dirname = filedialog.askopenfilename(initialdir=current)
        self.sfileentry.delete(0, END)
        self.sfileentry.insert(0, dirname)
        
    def browse_cf_dir(self):
        self.browse_dir(self.confpentry)
    def browse_media_dir(self):
        self.browse_dir(self.mediaentry)
    def browse_output_dir(self):
        self.browse_dir(self.outputentry)


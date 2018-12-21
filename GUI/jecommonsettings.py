from tkinter import *
from tkinter import filedialog
from tkinter import messagebox
import os

class JECommonSettings:
    def __init__(self, parent, settings):
        self.parent = parent
        self.settings = settings
        
        self.vConfp=StringVar()
        self.vCommand=StringVar()
        self.vMedia=StringVar()
        self.vSfile=StringVar()
        self.vOutput=StringVar()
        self.vOutputfile=StringVar()
        self.vOutputsfile=StringVar()
        self.vOecommand=StringVar()

        self.loadsettings()

    def loadsettings(self):
        self.vConfp.set(self.settings.get("CONFIG_FILES_DIRECTORY", ""))
        self.vCommand.set(self.settings.get("JEVISUALIZER_COMMAND", ""))
        self.vMedia.set(self.settings.get("MEDIA_DIRECTORY", ""))
        self.vSfile.set(self.settings.get("SOUNDFILE_FOR_VIDEO", ""))
        self.vOutput.set(self.settings.get("OUTPUT_DIRECTORY", ""))
        self.vOutputfile.set(self.settings.get("JEV_OUTPUT_FILE", ""))
        self.vOutputsfile.set(self.settings.get("OEC_OUTPUT_FILE", ""))
        self.vOecommand.set(self.settings.get("OEC", ""))

    def view(self, master):
        self.master=master
        EW = 40

        self.header = Label(self.master, text="Common settings", font=("Times", 12, "bold"))
        self.header.grid(columnspan=3)

        self.confplabel = Label(self.master, text="Config files directory ($CDIR$)")
        self.confplabel.grid(row=1, column=0)
        self.confpentry = Entry(self.master, width=EW, textvariable=self.vConfp)
        self.confpentry.grid(row=1, column=1)
        self.confpentrybutton = Button(self.master, text="Browse", command=self.browse_cf_dir)
        self.confpentrybutton.grid(row=2, column=2)

        self.commandlabel = Label(self.master, text="JEVisualizer command")
        self.commandlabel.grid(row=2, column=0)
        self.commandentry = Entry(self.master, width=EW, textvariable=self.vCommand)
        self.commandentry.grid(row=2, column=1)

        self.medialabel = Label(self.master, text="Media directory")
        self.medialabel.grid(row=3, column=0)
        self.mediaentry = Entry(self.master, width=EW, textvariable=self.vMedia)
        self.mediaentry.grid(row=3, column=1)
        self.mediabutton = Button(self.master, text="Browse", command=self.browse_media_dir)
        self.mediabutton.grid(row=3, column=2)

        self.sfilelabel = Label(self.master, text="Soundfile for video ($SOUNDS$)")
        self.sfilelabel.grid(row=4, column=0)
        self.sfileentry = Entry(self.master, width=EW, textvariable=self.vSfile)
        self.sfileentry.grid(row=4, column=1)
        self.sfilebutton = Button(self.master, text="Browse", command=self.browse_soundfile)
        self.sfilebutton.grid(row=4, column=2)

        self.outputlabel = Label(self.master, text="Output directory ($ODIR$)")
        self.outputlabel.grid(row=5, column=0)
        self.outputentry = Entry(self.master, width=EW, textvariable=self.vOutput)
        self.outputentry.grid(row=5, column=1)
        self.outputbutton = Button(self.master, text="Browse", command=self.browse_output_dir)
        self.outputbutton.grid(row=5, column=2)

        self.outputfilelabel = Label(self.master, text="JEVisualizer output file ($OFILE$)")
        self.outputfilelabel.grid(row=6, column=0)
        self.outputfileentry = Entry(self.master, width=EW, textvariable=self.vOutputfile)
        self.outputfileentry.grid(row=6, column=1)

        self.outputsfilelabel = Label(self.master, text="On-end-output file ($OSFILE$)")
        self.outputsfilelabel.grid(row=7, column=0)
        self.outputsfileentry = Entry(self.master, width=EW, textvariable=self.vOutputsfile)
        self.outputsfileentry.grid(row=7, column=1)

        self.oecommandlabel = Label(self.master, text="On-end-command")
        self.oecommandlabel.grid(row=8, column=0)
        self.oecommandentry = Entry(self.master, width=EW, textvariable=self.vOecommand)
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
        return self.vOutputfile.get()

    def set_ofilename(self, ofilename):
        return self.vOutputfile.set(ofilename)
    
    def get_osfilename(self):
        return self.outputsfileentry.get()

    def get_oec_command(self):
        oec=self.vOecommand.get()
        oec=self.replace_vars(oec)
        return oec
    def set_oec_command(self, oec):
        self.vOecommand.set(oec)
        

    def replace_vars(self, s):
        CDIR=self.vConfp.get()
        SOUNDS=self.vSfile.get()
        ODIR=self.vOutput.get()
        OFILE=self.vOutputfile.get()
        OSFILE=self.vOutputsfile.get()
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
        current = self.vSfile.get()
        if current=="": current=self.mediaentry.get()
        dirname = filedialog.askopenfilename(initialdir=current)
        self.vSfile.set(dirname)
        
    def browse_cf_dir(self):
        self.browse_dir(self.confpentry)
    def browse_media_dir(self):
        self.browse_dir(self.mediaentry)
    def browse_output_dir(self):
        self.browse_dir(self.outputentry)


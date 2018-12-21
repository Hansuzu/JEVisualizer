from tkinter import *

class JEVideoSettings:
    def __init__(self, parent, settings):
        self.parent = parent
        self.settings = settings
        self.master = None

        self.vW=StringVar()
        self.vH=StringVar()
        self.vFPS=StringVar()
        self.vFirstframe=StringVar()
        self.vLastframe=StringVar()
        self.vEnablefirstframe=IntVar()
        self.vEnablelastframe=IntVar()

        self.loadsettings()

    def loadsettings(self):
        self.vW.set(self.settings.get("WIDTH", ""))
        self.vH.set(self.settings.get("HEIGHT", ""))
        self.vFPS.set(self.settings.get("FPS", ""))
        self.vFirstframe.set(self.settings.get("FIRST_FRAME", ""))
        self.vLastframe.set(self.settings.get("LAST_FRAME", ""))
        try:    self.vEnablefirstframe.set(int(self.settings.get("FIRST_FRAME_ENABLED", "0")))
        except: self.vEnablefirstframe.set(0)
        try:    self.vEnablelastframe.set(int(self.settings.get("LAST_FRAME_ENABLED", "0")))
        except: self.vEnablelastframe.set(0)

    def view(self, master):
        self.master = master

        self.label = Label(self.master, text="Output video settings", font=("bold"))
        self.label.grid(row=0, columnspan=2)
        self.settingsframe = Frame(self.master)
        self.settingsframe.grid(row=1, columnspan=2)
        
        self.widthlabel = Label(self.settingsframe, text="Width: ")
        self.widthlabel.grid(row=0, column=0)
        self.widthentry = Entry(self.settingsframe, textvariable=self.vW)
        self.widthentry.grid(row=0, column=1)

        self.heightlabel = Label(self.settingsframe, text="Height: ")
        self.heightlabel.grid(row=1, column=0)
        self.heightentry = Entry(self.settingsframe, textvariable=self.vH)
        self.heightentry.grid(row=1, column=1)

        self.resolutionframe = Frame(self.settingsframe)
        self.resolutionframe.grid(row=2, columnspan=2)

        self.nHDButton = Button(self.resolutionframe, text="nHD (640x360)", command = lambda : self.setResolution(640, 360))
        self.nHDButton.grid(row=0, column=0)
        self.qHDButton = Button(self.resolutionframe, text="qHD (960x540)", command = lambda : self.setResolution(960, 540))
        self.qHDButton.grid(row=0, column=1)
        self.HDButton = Button(self.resolutionframe, text="HD (1280x720)", command = lambda : self.setResolution(1280, 720))
        self.HDButton.grid(row=0, column=2)
        self.fullHDButton = Button(self.resolutionframe, text="Full HD (1920x1080)", command = lambda : self.setResolution(1920, 1080))
        self.fullHDButton.grid(row=0, column=3)

        self.fpslabel = Label(self.settingsframe, text="FPS: ")
        self.fpslabel.grid(row=3, column=0)
        self.fpsentry = Entry(self.settingsframe, textvariable=self.vFPS)
        self.fpsentry.grid(row=3, column=1)

        self.firstframelabel = Label(self.settingsframe, text="First frame: ")
        self.firstframelabel.grid(row=4, column=0)
        self.firstframeentry = Entry(self.settingsframe, textvariable=self.vFirstframe)
        self.firstframeentry.grid(row=4, column=1)
        self.firstframecheckbox = Checkbutton(self.settingsframe, text="Enable", variable=self.vEnablefirstframe, command=self.change_enable_firstframe)
        self.firstframecheckbox.grid(row=4, column=2)

        self.lastframelabel = Label(self.settingsframe, text="Last frame: ")
        self.lastframelabel.grid(row=5, column=0)
        self.lastframeentry = Entry(self.settingsframe, textvariable=self.vLastframe)
        self.lastframeentry.grid(row=5, column=1)
        self.lastframecheckbox = Checkbutton(self.settingsframe, text="Enable", variable=self.vEnablelastframe, command=self.change_enable_lastframe)
        self.lastframecheckbox.grid(row=5, column=2)

        self.change_enable_firstframe()
        self.change_enable_lastframe()
    

    def getmediadir(self):
        return self.parent.getmediadir()


    def disen(self, value):
        if value: return "normal"
        return "readonly"

    def change_enable_firstframe(self):
        self.firstframeentry.config(state=self.disen(self.vEnablefirstframe.get()))

    def change_enable_lastframe(self):
        self.lastframeentry.config(state=self.disen(self.vEnablelastframe.get()))
        

    def setResolution(self, w, h):
        self.widthentry.delete(0, END)
        self.widthentry.insert(0, str(w))
        self.heightentry.delete(0, END)
        self.heightentry.insert(0, str(h))


    def write_config(self, visualizer):
        visualizer.write("W="+self.vW.get()+"\n")
        visualizer.write("H="+self.vH.get()+"\n")
        visualizer.write("FPS="+self.vFPS.get()+"\n")  
        if not self.vEnablefirstframe.get(): visualizer.write("// ")
        visualizer.write("FIRST-FRAME="+self.vFirstframe.get()+"\n")
        if not self.vEnablelastframe.get(): visualizer.write("// ")
        visualizer.write("LAST-FRAME="+self.vLastframe.get()+"\n")
        visualizer.write("\nFPV#0=frame\n")

    def load_config(self, visualizer):
        got_ff=0
        fot_lf=0
        for i in visualizer:
            if i[0] and (i[1][0]=="W" or i[1][0]=="WIDTH"): self.vW.set(i[2])
            if i[0] and (i[1][0]=="H" or i[1][0]=="HEIGHT"): self.vW.set(i[2])
            if i[0] and i[1][0]=="FPS": self.vFPS.set(i[2])
            if i[1][0]=="FIRST-FRAME":
                if i[0]:
                   self.vFirstframe.set(i[2])
                   self.vEnablefirstframe.set(1)
                   got_ff=True
                elif not got_ff:
                   self.vFirstframe.set(i[2])
                   self.vEnablefirstframe.set(0)
            if i[1][0]=="LAST-FRAME":
                if i[0]:
                    self.vLastframe.set(i[2])
                    self.vEnablelastframe.set(1)
                elif not got_ff:
                    self.vLastframe.set(i[2])
                    self.vEnablelastframe.set(0)
            

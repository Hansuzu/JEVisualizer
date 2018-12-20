from tkinter import *

class JEVideoSettings:
    def __init__(self, parent):
        self.parent = parent

    def view(self, master):
        self.master = master

        self.label = Label(self.master, text="Output video settings", font=("bold"))
        self.label.grid(row=0, columnspan=2)
        self.settingsframe = Frame(self.master)
        self.settingsframe.grid(row=1, columnspan=2)
        
        self.widthlabel = Label(self.settingsframe, text="Width: ")
        self.widthlabel.grid(row=0, column=0)
        self.widthentry = Entry(self.settingsframe)
        self.widthentry.insert(0, "1280")
        self.widthentry.grid(row=0, column=1)

        self.heightlabel = Label(self.settingsframe, text="Height: ")
        self.heightlabel.grid(row=1, column=0)
        self.heightentry = Entry(self.settingsframe)
        self.heightentry.insert(0, "720")
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
        self.fpsentry = Entry(self.settingsframe)
        self.fpsentry.insert(0, "40")
        self.fpsentry.grid(row=3, column=1)

        self.firstframelabel = Label(self.settingsframe, text="First frame: ")
        self.firstframelabel.grid(row=4, column=0)
        self.firstframeentry = Entry(self.settingsframe)
        self.firstframeentry.insert(0, "2000")
        self.firstframeentry.config(state="readonly")
        self.firstframeentry.grid(row=4, column=1)
        self.firstframe=IntVar()
        self.firstframecheckbox = Checkbutton(self.settingsframe, text="Enable", variable=self.firstframe, command=self.change_enable_firstframe)
        self.firstframecheckbox.grid(row=4, column=2)

        self.lastframelabel = Label(self.settingsframe, text="Last frame: ")
        self.lastframelabel.grid(row=5, column=0)
        self.lastframeentry = Entry(self.settingsframe)
        self.lastframeentry.insert(0, "2400")
        self.lastframeentry.config(state="readonly")
        self.lastframeentry.grid(row=5, column=1)
        self.lastframe=IntVar()
        self.lastframecheckbox = Checkbutton(self.settingsframe, text="Enable", variable=self.lastframe, command=self.change_enable_lastframe)
        self.lastframecheckbox.grid(row=5, column=2)

    

    def getmediadir(self):
        return self.parent.getmediadir()


    def disen(self, value):
        if value: return "normal"
        return "readonly"

    def change_enable_firstframe(self):
        self.firstframeentry.config(state=self.disen(self.firstframe.get()))

    def change_enable_lastframe(self):
        self.lastframeentry.config(state=self.disen(self.lastframe.get()))
        

    def setResolution(self, w, h):
        self.widthentry.delete(0, END)
        self.widthentry.insert(0, str(w))
        self.heightentry.delete(0, END)
        self.heightentry.insert(0, str(h))


    def write_config(self, visualizer, ofilename):
        visualizer.write("OUTPUT-FILE=\""+ofilename+"\"\n")
        visualizer.write("W="+self.widthentry.get()+"\n")
        visualizer.write("H="+self.heightentry.get()+"\n")
        visualizer.write("FPS="+self.fpsentry.get()+"\n")  
        if not self.firstframe.get(): visualizer.write("// ")
        visualizer.write("FIRST-FRAME="+self.firstframeentry.get()+"\n")
        if not self.lastframe.get(): visualizer.write("// ")
        visualizer.write("LAST-FRAME="+self.lastframeentry.get()+"\n")
        visualizer.write("\nFPV#0=frame\n")


from tkinter import *
from jetrack import *

class JETracks:
    def __init__(self, parent, settings):
        self.parent = parent
        self.settings = settings
        self.master = None
        self.tracks=[]

    def view_tracks(self):
        for i in range(len(tracks)): tracks[i].view(self.frame)

    def view(self, master):
        self.master = master
        self.label = Label(self.master, text="Input Tracks", font=("bold"))
        self.label.grid(row=0, column=0)
        self.frame = Frame(self.master)
        self.frame.grid(row=1, column=0)
        self.newbutton = Button(self.master, text="New input track", command=self.newtrack)
        self.newbutton.grid(row=2, column=0)

    def getmediadir(self):
        return self.parent.getmediadir()

    def updatetracks(self):
        for i in range(len(self.tracks)):
            self.tracks[i].set_row(i)

    def removetrack(self, row):
        del self.tracks[row]
        self.updatetracks()

    def newtrack(self):
        self.tracks.append(JETrack(self, self.frame, len(self.tracks), self.settings))

    def write_config(self, main, visualizer):
        main.write("\n// Tracks\n")
        visualizer.write("\n// Track settings\n")
        for track in self.tracks:
            track.write_config(main, visualizer)
        
        

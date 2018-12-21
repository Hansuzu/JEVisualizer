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

    def addtrack(self, track):
        self.tracks.append(track)
        self.tracks[-1].view(self.frame)
        ix = len(self.tracks)-1
        self.tracks[ix].set_row(ix)
        self.tracks[ix].edited()

    def newtrack(self):
        self.tracks.append(JETrack(self, len(self.tracks), self.settings))
        self.tracks[-1].view(self.frame)
        self.tracks[-1].edit()

    def write_config(self, main, visualizer):
        main.write("\n// Tracks\n")
        visualizer.write("\n// Track settings\n")
        for track in self.tracks:
            track.write_config(main, visualizer)

    def load_config(self, main, visualizer):
        while len(self.tracks): del self.tracks[-1]
        tmptracks=[]
        for i in main:
            if i[0] and i[1][0]=="TRACK":
                tmptracks.append([i[1][1], i[2]])
        tmptracks.sort()
        for i in range(len(tmptracks)):
            track=JETrack(self, i, self.settings)
            track.load_config(tmptracks[i][0], tmptracks[i][1], visualizer)
            self.addtrack(track)
        


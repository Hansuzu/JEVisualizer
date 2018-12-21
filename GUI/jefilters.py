from tkinter import *
from tkinter.colorchooser import *
from jefilter import *
from PIL import Image, ImageTk, ImageDraw
import os.path

class JEFilters:
    def __init__(self, parent, settings):
        self.parent = parent
        self.settings = settings
        self.master = None
        self.filters = []
        
    def save(self):
        pass
    def load(self):
        pass

    def copy_from(self, o):
        self.filters = []
        for i in range(len(o.filters)):
            self.filters.append(JEFilter(self, self.frame, len(self.filters), self.settings))
            self.filters[i].copy_from(o.filters[i])

    def view(self, master):
        if self.master: return
        self.master = master
        
        self.label = Label(master, text="Layer filters", font=("bold"))
        self.label.grid(row=0)
        self.frame = Frame(master)
        self.frame.grid(row=1)
        self.newbutton = Button(master, text="New Filter", command=self.newfilter)
        self.newbutton.grid(row=2)

        self.updatefilters(True)


    def window_closed(self):
        for filtr in self.filters: filtr.window_closed()
        self.master = None

    def updatefilters(self, update_master=False):
        for i in range(len(self.filters)):
            if update_master: self.filters[i].set_master(self.frame)
            self.filters[i].set_row(i)

    def newfilter(self):
        self.filters.append(JEFilter(self, self.frame, len(self.filters), self.settings))

    def removefilter(self, index):
        del self.filters[index]
        self.updatefilters()


    def update_preview(self, img, track_values):
        return img

    def create_preview(self, img, track_values):
        return img


    def write_config(self, f):
        for filtr in self.filters: filtr.write_config(self, f, "FILTER")

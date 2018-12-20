from tkinter import *
from tkinter.colorchooser import *
from jefilter import *
from PIL import Image, ImageTk, ImageDraw
import os.path

class JEIFilters:
    def __init__(self, parent):
        self.parent = parent
        self.master = None
        self.ifilters = []
        
    def save(self):
        pass
    def load(self):
        pass

    def copy_from(self, o):
        self.ifilters = []
        for i in range(len(o.ifilters)):
            self.ifilters.append(JEFilter(self))
            self.ifilters[i].copy_from(o.ifilters[i])

    def view(self, master):
        if self.master: return
        self.master = master
        
        self.label = Label(master, text="Image filters", font=("bold"))
        self.label.grid(row=11, columnspan=4)
        self.frame = Frame(master)
        self.frame.grid(row=12, columnspan=4)
        self.newbutton = Button(master, text="New Filter", command=self.newifilter)
        self.newbutton.grid(row=13, columnspan=3)

        self.updateifilters(True)
        

    def window_closed(self):
        for filtr in self.ifilters: filtr.window_closed()
        self.master = None

    def updateifilters(self, update_master=False):
        for i in range(len(self.ifilters)):
            if update_master: self.filters[i].set_master(self.frame)
            self.ifilters[i].set_row(i)

    def newifilter(self):
        self.ifilters.append(JEFilter(self, self.frame, len(self.ifilters)))

    def removeifilter(self, index):
        del self.ifilters[index]
        self.updateifilters()


    def update_preview(self, img, track_values):
        return img

    def create_preview(self, img, track_values):
        return img


    def write_config(self, f):
        for filtr in self.ifilters: filtr.write_config(self, f, "IMAGE-FILTER")

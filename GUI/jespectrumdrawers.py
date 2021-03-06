from tkinter import *
from jespectrumdrawer import *

class JESpectrumDrawers:
    def __init__(self, parent, settings):
        self.parent = parent
        self.settings = settings
        self.master = None
        self.drawers=[]

    
    def save(self):
        pass
    def load(self):
        pass

    def copy_from(self, o):
        self.drawers=[]
        for i in range(len(o.drawers)):
            self.drawers.append(JESpectrumDrawer(self, i, self.settings))
            self.drawers[i].copy_from(o.drawers[i])

    def view(self, master):
        if self.master: return
        self.master = master
        self.label = Label(self.master, text="Spectrum Drawers", font=("bold"))
        self.label.grid(row=0, columnspan=4)
        self.frame = Frame(self.master)
        self.frame.grid(row=1,columnspan=4)
        self.newbutton = Button(self.master, text="New Spectrum Drawer", command=self.newdrawer)
        self.newbutton.grid(row=2, columnspan=3)

        self.updatedrawers(True)

    def window_closed(self):
        for drawer in self.drawers: drawer.window_closed()
        self.master = None

    def get_W(self): return self.parent.get_W()
    def get_H(self): return self.parent.get_H()

    def updatedrawers(self, update_master=False):
        for i in range(len(self.drawers)):
            if update_master: self.drawers[i].set_master(self.frame)
            self.drawers[i].set_row(i)

    def newdrawer(self):
        self.drawers.append(JESpectrumDrawer(self, len(self.drawers), self.settings))
        self.drawers[-1].view(self.frame)
        self.drawers[-1].edit()

    def removedrawer(self, index):
        del self.drawers[index]
        self.updatedrawers()

    def adddrawer(self, drawer):
        self.drawers.append(drawer)
        self.drawers[-1].view(self.frame)
        ix = len(self.drawers)-1
        self.drawers[ix].set_row(ix)
        self.drawers[ix].edited()

    def edited(self):
        self.parent.update_preview()

    def update_preview(self, img, track_values):
        for drawer in self.drawers: img = drawer.update_preview(img, track_values)
        return img

    def create_preview(self, img, track_values):
        for drawer in self.drawers: img = drawer.create_preview(img, track_values)
        return img

    def write_config(self, f):
        for drawer in self.drawers: drawer.write_config(f)

    def load_config(self, conf):
        while len(self.drawers): del self.drawers[-1]
        for i in conf:
            if i[0] and i[1][0]=="DRAWER":
                self.drawers.append(JESpectrumDrawer(self, len(self.drawers), self.settings))
                self.drawers[-1].load_config(i[2])


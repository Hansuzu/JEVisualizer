from tkinter import *
from tkinter.colorchooser import *
from jelayersettings import *
from jespectrumdrawers import *
from jefilters import *
from jeifilters import *
from PIL import Image, ImageTk, ImageDraw
import os.path

class JELayerEditor:
    def __init__(self, parent, mediadir):
        self.parent = parent
        self.mediadir = mediadir
        self.master = None
        self.disable_update_preview = False

        self.settings = JELayerSettings(self, self.mediadir)
        self.drawers =  JESpectrumDrawers(self)
        self.filters = JEFilters(self)
        self.ifilters = JEIFilters(self)

        self.track_values=[]
        for i in range(20):
            self.track_values.append([])
            for i in range(500):
                self.track_values[-1].append((random.random()*0.4)**3)


    def save(self):
        self.settings.save()
        self.drawers.save()
        self.filters.save()
        self.ifilters.save()

    def load(self):
        self.settings.load()
        self.drawers.load()
        self.filters.load()
        self.ifilters.load()

    def copy_from(self, o):
        self.settings.copy_from(o.settings)
        self.drawers.copy_from(o.drawers)
        self.filters.copy_from(o.filters)
        self.ifilters.copy_from(o.ifilters)

    def close_master(self):
        if not self.master: return
        self.master.destroy()
        self.master=None
        self.settings.window_closed()
        self.drawers.window_closed()
        self.filters.window_closed()
        self.ifilters.window_closed()

    def view(self, master):
        if self.master: return
        self.disable_update_preview = True
        self.master = master
        self.master.protocol("WM_DELETE_WINDOW", self.close_master)
        self.master.title("JEVGUI Layer Editor")

        self.settingsframe = Frame(master)
        self.settingsframe.grid(row=0, column=0)

        self.drawerframe = Frame(master)
        self.drawerframe.grid(row=1, column=0)

        self.filterframe = Frame(master)
        self.filterframe.grid(row=2, column=0)

        self.ifilterframe = Frame(master)
        self.ifilterframe.grid(row=3, column=0)

        self.settings.view(self.settingsframe)
        self.drawers.view(self.drawerframe)
        self.filters.view(self.filterframe)
        self.ifilters.view(self.ifilterframe)

        self.okbutton = Button(master, text="Ok", command=self.ok)
        self.okbutton.grid(row=100, column=0)
        
        self.load()

        self.disable_update_preview = False
        self.update_preview()

    def update_preview(self):
        self.preview_base = Image.new('RGBA', (128, 72), (0, 0, 0, 0))
        if self.disable_update_preview: return self.preview_base
        img = self.settings.update_preview(self.preview_base, self.track_values)
        img = self.drawers.update_preview(img, self.track_values)
        img = self.filters.update_preview(img, self.track_values)
        img = self.ifilters.update_preview(img, self.track_values)
        return img

    def create_preview(self, img, track_values):
        img = self.settings.create_preview(img, track_values)
        img = self.drawers.create_preview(img, track_values)
        img = self.filters.create_preview(img, track_values)
        img = self.ifilters.create_preview(img, track_values)
        return img

    def get_W(self): return self.settings.get_W()
    def get_H(self): return self.settings.get_H()

    def ok(self):
        self.save()
        self.parent.edited()
        self.close_master()

    def write_config(self, f):
        self.settings.write_config(f)
        self.drawers.write_config(f)
        self.filters.write_config(f)
        self.ifilters.write_config(f)
        
        

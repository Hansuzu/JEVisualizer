from tkinter import *
from jefiltereditor import *
import configreader

class JEFilter:
    def __init__(self, parent, masterframe, row, settings):
        self.parent = parent
        self.masterframe = masterframe
        self.settings = settings
        self.row=row
        self.frame = Frame(masterframe)
        self.frame.grid(row=row, sticky=N)

        self.title=StringVar()
        self.title.set("Filter")
        self.header = Label(self.frame, textvariable=self.title, font=("Times", 12, "bold"))
        self.header.grid(row=0, column=0)

        self.edit_button = Button(self.frame, text="Edit", command=self.edit)
        self.edit_button.grid(row=0, column=1)
        self.delete_button = Button(self.frame, text="Delete", command=self.delete)
        self.delete_button.grid(row=0, column=2)


        self.edit()

    def master_closed(self):
        self.master = None

    def set_row(self, row):
        self.row=row
        self.frame.grid(row=row)
     
    def edit(self):
        window = Toplevel(self.masterframe)
        editor = JEFilterEditor(window, self, self.variables)

    def delete(self):
        self.parent.removedrawer(self.row)
        self.frame.destroy()
        del self

    def write_config(self, f, name):
        f.write(name+"={\n\n}\n")

    def load_config(self, confstr):
        conf=configreader.readConfigFromString(confstr)
        

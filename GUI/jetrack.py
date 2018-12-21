from tkinter import *
from jetrackeditor import *
import random

class JETrack:
    def __init__(self, parent, masterframe, row, settings):
        self.parent = parent
        self.masterframe = masterframe
        self.row = row
        self.settings = settings
        self.frame = Frame(masterframe)
        self.frame.grid(row=row, sticky=N)

        self.title=StringVar()
        self.header = Label(self.frame, textvariable=self.title, font=("Times", 12, "bold"))
        self.header.grid(row=0, column=0)

        self.edit_button = Button(self.frame, text="Edit", command=self.edit)
        self.edit_button.grid(row=0, column=1)
        self.delete_button = Button(self.frame, text="Delete", command=self.delete)
        self.delete_button.grid(row=0, column=2)

        self.editor = JETrackEditor(self, self.parent.getmediadir(), self.settings)

        self.update_name()
        self.edit()

        
    def update_name(self):
        self.title.set("TRACK#"+str(self.row)+" ("+self.editor.name()+")")


    def edit(self):
        window = Toplevel(self.masterframe)
        self.editor.view(window)
        # editor...

    def edited(self):
        self.update_name()

    def delete(self):
        self.parent.removetrack(self.row)
        self.frame.destroy()
        del self

    def set_row(self, row):
        self.row=row
        self.update_name()
        self.frame.grid(row=row)

    def write_config(self, main, visualizer):
        main.write("TRACK#"+str(self.row)+"={\n")
        self.editor.write_config(main, visualizer, self.row)
        main.write("}\n\n")

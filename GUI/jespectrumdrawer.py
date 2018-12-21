from tkinter import *
from jespectrumdrawereditor import *

class JESpectrumDrawer:
    def __init__(self, parent, row, settings):
        self.parent = parent
        self.row = row
        self.settings = settings
        self.master = None

        self.editor = JESpectrumDrawerEditor(self, self.settings)

    def view(self, master):
        if self.master: return
        self.master = master
        self.frame = Frame(self.master)
        self.frame.grid(row=self.row, sticky=N)

        self.title=StringVar()
        self.title.set("Drawer")
        self.header = Label(self.frame, textvariable=self.title, font=("Times", 12, "bold"))
        self.header.grid(row=0, column=0)

        self.edit_button = Button(self.frame, text="Copy", command=self.create_copy)
        self.edit_button.grid(row=0, column=1)
        self.edit_button = Button(self.frame, text="Edit", command=self.edit)
        self.edit_button.grid(row=0, column=2)
        self.delete_button = Button(self.frame, text="Delete", command=self.delete)
        self.delete_button.grid(row=0, column=3)

        self.preview = Canvas(self.master, width=128, height=72)
        self.preview.grid(row=self.row, column=3)

    def copy_from(self, o):
        self.editor.copy_from(o.editor)

    def create_copy(self):
        cop = JESpectrumDrawer(self.parent, self.row, self.settings)
        cop.copy_from(self)
        self.parent.adddrawer(cop)

    def window_closed(self):
        self.master = None
        
    def get_W(self): return self.parent.get_W()
    def get_H(self): return self.parent.get_H()

    def update_preview(self, img, track_values):
        self.final_img = self.editor.create_preview(img, track_values)
        self.imgp = ImageTk.PhotoImage(self.final_img)

        self.preview.delete("all")
        self.preview.create_image(0, 0, image=self.imgp, anchor=NW)
        return self.final_img

    def create_preview(self, img, track_values):
        self.final_img = self.editor.create_preview(img, track_values)
        return self.final_img

    def edited(self):
        self.parent.edited()
    
    def set_row(self, row):
        self.row=row
        self.frame.grid(row=row)
        self.preview.grid(row=row, column=3)

    def set_master(self, master):
        self.view(master)
     
    def edit(self):
        window = Toplevel(self.master)
        self.editor.view(window)

    def delete(self):
        self.parent.removedrawer(self.row)
        self.frame.destroy()
        self.preview.destroy()
        del self


    def write_config(self, f):
        self.editor.write_config(f)

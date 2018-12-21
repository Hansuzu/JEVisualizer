from tkinter import *
from jelayereditor import *
import configreader

class JELayer:
    def __init__(self, parent, row, settings):
        self.parent = parent
        self.row=row
        self.settings = settings
        self.master =  None

        self.title=StringVar()

        self.editor = JELayerEditor(self, self.parent.getmediadir(), self.settings)



    def view(self, master):
        if self.master: return
        self.master = master

        self.frame = Frame(self.master)
        self.frame.grid(row=self.row, sticky=N)

        self.header = Label(self.frame, textvariable=self.title, font=("Times", 12, "bold"))
        self.header.grid()

        self.edit_button = Button(self.frame, text="Edit", command=self.edit)
        self.edit_button.grid(row=0, column=1)
        self.delete_button = Button(self.frame, text="Delete", command=self.delete)
        self.delete_button.grid(row=0, column=2)
        self.delete_button = Button(self.frame, text="Copy", command=self.create_copy)
        self.delete_button.grid(row=0, column=3)
        self.delete_button = Button(self.frame, text="^", command=self.move_up)
        self.delete_button.grid(row=0, column=4)
        self.delete_button = Button(self.frame, text="v", command=self.move_down)
        self.delete_button.grid(row=0, column=5)
        
        self.preview_fr = Canvas(self.master, width=128, height=72)
        self.preview_fr.grid(row=self.row, column=1)
        self.preview_tot = Canvas(self.master, width=128, height=72)
        self.preview_tot.grid(row=self.row, column=2)

    
        

    def update_name(self):
        self.title.set("LAYER#"+str(self.row))


    def update_preview(self, track_values):
        self.preview_base = Image.new('RGBA', (128, 72), (0, 0, 0, 255))
        self.img = self.editor.create_preview(self.preview_base, track_values)
        self.imgp = ImageTk.PhotoImage(self.img)

        self.preview_fr.update()
        self.preview_fr.delete("all")
        self.preview_fr.create_image(0, 0, image=self.imgp, anchor=NW)
        return self.img

    def update_total_preview(self, img, track_values):
        self.final_img = self.editor.create_preview(img, track_values)
        self.final_imgp = ImageTk.PhotoImage(self.final_img)

        self.preview_tot.update()
        self.preview_tot.delete("all")
        self.preview_tot.create_image(0, 0, image=self.final_imgp, anchor=NW)
        return self.final_img
    
        


    def edit(self):
        window = Toplevel(self.master)
        self.editor.view(window)

    def edited(self):
        self.update_name()
        self.parent.update_preview()


    def move_up(self):
        self.parent.move_layer(self.row, self.row-1)

    def move_down(self):
        self.parent.move_layer(self.row, self.row+1)

    def delete(self):
        self.parent.removelayer(self.row)
        self.frame.destroy()
        self.preview_fr.destroy()
        self.preview_tot.destroy()
        del self

    def copy_from(self, o):
        self.editor.copy_from(o.editor)
        
    def create_copy(self):
        cop = JELayer(self.parent, self.row, self.settings)
        cop.copy_from(self)
        self.parent.addlayer(cop)

    def set_row(self, row):
        self.row=row
        self.preview_fr.grid(row=self.row, column=1)
        self.preview_tot.grid(row=self.row, column=2)
        self.frame.grid(row=self.row)

    def write_config(self, visualizer, path):
        layername="layer"+str(self.row)
        layerfname=path+layername+".config"
        visualizer.write("LAYER#"+str(self.row)+"=\""+layerfname+"\"\n")
        f=open(layerfname, "w")
        f.write("/*\n * JEVisualizer config file generated by JEVisualizer GUI\n */\n\n")
        self.editor.write_config(f)
        f.close()

    def load_config(self, filename):
        conf=configreader.readConfigFromFile(filename)
        self.editor.load_config(conf)


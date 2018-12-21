from tkinter import *
from jelayer import *
import random

class JELayers:
    def __init__(self, parent, settings):
        self.parent = parent
        self.settings = settings
        self.master = None
        self.layers=[]

        self.track_values=[]
        for i in range(20):
            self.track_values.append([])
            for i in range(500):
                self.track_values[-1].append((random.random()*0.4)**3)


    def view_layers(self):
        for i in range(len(self.layers)): self.layers[i].view(self.frame)

    def view(self, master):
        if self.master: return
        self.master = master
        self.label = Label(self.master, text="Output layers", font=("bold"))
        self.label.grid(row=0, column=0)
        self.frame = Frame(self.master)
        self.frame.grid(row=1, column=0)
        self.newbutton = Button(self.master, text="New layer", command=self.newlayer)
        self.newbutton.grid(row=2, column=0)
        self.view_layers()

    
    def getmediadir(self):
        return self.parent.getmediadir()

    def update_preview(self):
        self.img = Image.new('RGBA', (128, 72), (0, 0, 0, 255))
        for layer in self.layers: 
            layer.update_preview(self.track_values)
            self.img = layer.update_total_preview(self.img, self.track_values)

    def updatelayers(self):
        for i in range(len(self.layers)):
            self.layers[i].set_row(i)
            self.layers[i].edited()

    def removelayer(self, row):
        del self.layers[row]
        self.updatelayers()

    def addlayer(self, layer):
        self.layers.append(layer)
        self.layers[-1].view(self.frame)
        ix = len(self.layers)-1
        self.layers[ix].set_row(ix)
        self.layers[ix].edited()
    
    def move_layer(self, old_row, new_row):
        self.layers.insert(new_row, self.layers.pop(old_row))
        self.updatelayers()

    def newlayer(self):
        self.layers.append(JELayer(self, len(self.layers), self.settings))
        self.layers[-1].view(self.frame)
        self.layers[-1].edit()

    def write_config(self, visualizer, path):
        for layer in self.layers: layer.write_config(visualizer, path)

    def load_config(self, visualizer):
        while len(self.layers): del self.layers[-1]
        tmplayers=[]
        for i in visualizer:
            if i[0] and i[1][0]=="LAYER":
                tmplayers.append([i[1][1], i[2]])
        tmplayers.sort()
        for i in range(len(tmplayers)):
            layer=JELayer(self, i, self.settings)
            self.addlayer(layer)
            layer.load_config(tmplayers[i][1])
        


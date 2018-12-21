from tkinter import *
from tkinter.colorchooser import *
import random
from PIL import Image, ImageTk, ImageDraw

class JESpectrumDrawerEditor:
    def __init__(self, parent, settings):
        self.parent = parent
        self.master = None
        self.settings = settings
        self.stuff = {}
        self.disable_update_preview = False


        self.iTRACK=self.settings.get("LAYER_SPECTRUM_DRAWER_TRACK", "")
        self.iINDICES=self.settings.get("LAYER_SPECTRUM_DRAWER_INDICES", "")
        self.FC0=self.settings.get("LAYER_SEPCTRUM_DRAWER_FILL_COLOR_0", "")
        self.LC0=self.settings.get("LAYER_SEPCTRUM_DRAWER_LINE_COLOR_0", "")
        self.FC1=self.settings.get("LAYER_SEPCTRUM_DRAWER_FILL_COLOR_1", "")
        self.LC1=self.settings.get("LAYER_SEPCTRUM_DRAWER_LINE_COLOR_1", "")
        self.W=self.settings.get("LAYER_SPECTRUM_DRAWER_COLUMN_WIDTH", "")
        self.H=self.settings.get("LAYER_SPECTRUM_DRAWER_COLUMN_HEIGHT", "")
        self.COLUMN=self.settings.get("LAYER_SPECTRUM_DRAWER_COLUMN_TYPE", "")
        try:   self.ENABLEC1=int(self.settings.get("LAYER_SPECTRUM_DRAWER_ENABLE_SECOND_COLOR", "0"))
        except:self.ENABLEC1=0
        self.TYPE=self.settings.get("LAYER_SPECTRUM_DRAWER_TYPE", "")
        self.X0="0.0"
        self.Y0=str(self.parent.get_H()/2)
        self.X1=str(self.parent.get_W())
        self.Y1=str(self.parent.get_H()/2)
        
        
        self.viTRACK=StringVar()
        self.viINDICES=StringVar()
        self.vFC0=StringVar()
        self.vLC0=StringVar()
        self.vFC1=StringVar()
        self.vLC1=StringVar()
        self.vW=StringVar()
        self.vH=StringVar()
        self.vCOLUMN=StringVar()
        self.vENABLEC1=IntVar()
        self.vTYPE=StringVar()
        self.vX0=StringVar()
        self.vY0=StringVar()
        self.vX1=StringVar()
        self.vY1=StringVar()
        
        self.vFC0.trace("w", lambda name, index, mode: self.update_column0_preview(True))
        self.vLC0.trace("w", lambda name, index, mode: self.update_column0_preview(True))
        self.vFC1.trace("w", lambda name, index, mode: self.update_column1_preview())
        self.vLC1.trace("w", lambda name, index, mode: self.update_column1_preview())
        self.vW.trace("w", lambda name, index, mode: self.update_column_previews())
        self.vCOLUMN.trace("w", lambda name, index, mode: self.update_column_previews())
        self.vENABLEC1.trace("w", lambda name, index, mode: self.update_column1_preview())

        self.vH.trace("w", lambda name, index, mode: self.update_preview())
        self.vTYPE.trace("w", lambda name, index, mode: self.update_preview())
        self.vX0.trace("w", lambda name, index, mode: self.update_preview())
        self.vY0.trace("w", lambda name, index, mode: self.update_preview())
        self.vX1.trace("w", lambda name, index, mode: self.update_preview())
        self.vY1.trace("w", lambda name, index, mode: self.update_preview())

    def copy_from(self, o):
        self.iTRACK=o.iTRACK
        self.iINDICES=o.iINDICES
        self.FC0=o.FC0
        self.LC0=o.LC0
        self.FC1=o.FC1
        self.LC1=o.LC1
        self.W=o.W
        self.H=o.H
        self.COLUMN=o.COLUMN
        self.ENABLEC1=o.ENABLEC1
        self.TYPE=o.TYPE
        self.X0=o.X0
        self.Y0=o.Y0
        self.X1=o.X1
        self.Y1=o.Y1
        
    def save(self):
        self.iTRACK=self.viTRACK.get()
        self.iINDICES=self.viINDICES.get()
        self.FC0=self.vFC0.get()
        self.LC0=self.vLC0.get()
        self.FC1=self.vFC1.get()
        self.LC1=self.vLC1.get()
        self.W=self.vW.get()
        self.H=self.vH.get()
        self.COLUMN=self.vCOLUMN.get()
        self.ENABLEC1=self.vENABLEC1.get()
        self.TYPE=self.vTYPE.get()
        self.X0=self.vX0.get()
        self.Y0=self.vY0.get()
        self.X1=self.vX1.get()
        self.Y1=self.vY1.get()
        
    def load(self):
        self.viTRACK.set(self.iTRACK)
        self.viINDICES.set(self.iINDICES)
        self.vFC0.set(self.FC0)
        self.vLC0.set(self.LC0)
        self.vFC1.set(self.FC1)
        self.vLC1.set(self.LC1)
        self.vW.set(self.W)
        self.vH.set(self.H)
        self.vCOLUMN.set(self.COLUMN)
        self.vENABLEC1.set(self.ENABLEC1)
        self.vTYPE.set(self.TYPE)
        self.vX0.set(self.X0)
        self.vY0.set(self.Y0)
        self.vX1.set(self.X1)
        self.vY1.set(self.Y1)
        
    def close_master(self):
        self.master.destroy()
        self.master=None

    def view(self, master):
        if self.master: return
        self.disable_update_preview = True
        self.master = master
        self.master.protocol("WM_DELETE_WINDOW", self.close_master)
        self.master.title("JEVGUI Spectrum Drawer Editor")

        self.header = Label(self.master, text="Spectrum Drawer editor", font=("Times", 12, "bold"))
        self.header.grid(row=0, columnspan=2)
        
        
        self.tracklabel = Label(self.master, text="Track#")
        self.tracklabel.grid(row=1, column=0)
        self.trackentry = Entry(self.master, textvariable=self.viTRACK)
        self.trackentry.grid(row=1, column=1)

        self.trackindiceslabel = Label(self.master, text="Indices")
        self.trackindiceslabel.grid(row=2, column=0)
        self.trackindicesentry = Entry(self.master, textvariable=self.viINDICES)
        self.trackindicesentry.grid(row=2, column=1)

        self.fillcolor0label = Label(self.master, text="Fill color 0")
        self.fillcolor0label.grid(row=3, column=0)
        self.fillcolor0entry = Entry(self.master, textvariable=self.vFC0)
        self.fillcolor0entry.grid(row=3, column=1)
        self.fillcolor0button = Button(self.master, text="Select", command=self.fillcolor0_chooser)
        self.fillcolor0button.grid(row=3, column=2)

        self.linecolor0label = Label(self.master, text="Line color 0")
        self.linecolor0label.grid(row=4, column=0)
        self.linecolor0entry = Entry(self.master, textvariable=self.vLC0)
        self.linecolor0entry.grid(row=4, column=1)
        self.linecolor0button = Button(self.master, text="Select", command=self.linecolor0_chooser)
        self.linecolor0button.grid(row=4, column=2)

        self.previewcolumn0 = Canvas(self.master, width=30, height=60)
        self.previewcolumn0.grid(row=2, rowspan=3, column=3)

        self.swapcolorsbutton = Button(self.master, text="Swap colors", command=self.swap_colors)
        self.swapcolorsbutton.grid(row=5, column=0)
        self.enablecolor1checkbox = Checkbutton(self.master, text="Enable second color", variable=self.vENABLEC1, command=self.change_enablecolor1)
        self.enablecolor1checkbox.grid(row=5, column=1, columnspan=2)

        self.fillcolor1label = Label(self.master, text="Fill color 1")
        self.fillcolor1label.grid(row=6, column=0)
        self.fillcolor1entry = Entry(self.master, textvariable=self.vFC1)
        self.fillcolor1entry.grid(row=6, column=1)
        self.fillcolor1button = Button(self.master, text="Select", command=self.fillcolor1_chooser)
        self.fillcolor1button.grid(row=6, column=2)

        self.linecolor1label = Label(self.master, text="Line color 1")
        self.linecolor1label.grid(row=7, column=0)
        self.linecolor1entry = Entry(self.master, textvariable=self.vLC1)
        self.linecolor1entry.grid(row=7, column=1)
        self.linecolor1button = Button(self.master, text="Select", command=self.linecolor1_chooser)
        self.linecolor1button.grid(row=7, column=2)


        self.previewcolumn1 = Canvas(self.master, width=30, height=60)
        self.previewcolumn1.grid(row=5, rowspan=3, column=3)

        self.columnwidthlabel = Label(self.master, text="Column width:")
        self.columnwidthlabel.grid(row=8, column=0)
        self.columnwidthentry = Entry(self.master, textvariable=self.vW)
        self.columnwidthentry.grid(row=8, column=1)

        self.columnheightlabel = Label(self.master, text="Column height:")
        self.columnheightlabel.grid(row=9, column=0)
        self.columnheightentry = Entry(self.master, textvariable=self.vH)
        self.columnheightentry.grid(row=9, column=1)

        self.columntypelabel = Label(self.master, text="Column type")
        self.columntypelabel.grid(row=10, column=0)
        self.columntypemenu = OptionMenu(self.master, self.vCOLUMN, "RECTANGLE", "TRIANGLE", command=self.change_column_type)
        self.columntypemenu.grid(row=10, column=1)

        self.preview = Canvas(self.master, width=128, height=72)
        self.preview.grid(row=10, rowspan=20, column=3, columnspan=2)

        # row 11: reserved for column type POLYGON


        self.typelabel = Label(self.master, text="Type")
        self.typelabel.grid(row=12, column=0)
        self.typemenu = OptionMenu(self.master, self.vTYPE, "LINE", command=self.change_type)
        self.typemenu.grid(row=12, column=1)

        self.okbutton = Button(master, text="Ok", command=self.ok)
        self.okbutton.grid(row=100, column=0)

        self.load()
        self.change_column_type(None)
        self.change_enablecolor1()
        self.change_type(None)
        self.disable_update_preview = False
        self.update_preview()

    def create_view(self):
        pass


    def ok(self):
        self.save()
        self.parent.edited()
        self.master.destroy()
        self.master=None

    def draw_column(self, x0, y0, x1, y1, height, img, column, fc, lc):
        d=ImageDraw.Draw(img)
        coords=[(x0, y0), (x1, y1)]
        dx=x1-x0
        dy=y1-y0
        x2=x0+height*dy
        y2=y0-height*dx
        if column=="RECTANGLE":
            coords.append((x2+dx, y2+dy))
            coords.append((x2, y2))
        else:
            coords.append((x2+dx/2, y2+dy/2))
        d.polygon(coords, fill=fc, outline=lc)
        return img

    def cint(self, v, d=0):
        try: return int(v)
        except: return d
    def cfloat(self, v, d=0.0):
        try: return float(v)
        except: return d
    def parse_color(self, c):
        c=c.replace("[", "").replace("]", "").split(",")
        while len(c)<4:c.append(0)
        return (self.cint(c[0]), self.cint(c[1]), self.cint(c[2]), self.cint(c[3]))

    def update_column0_preview(self, can2=False):
        if not self.master: return
        self.previewcolumn0.update()
        self.previewcolumn0.delete('all')
        w=self.previewcolumn0.winfo_width()
        h=self.previewcolumn0.winfo_height()
        try:    W=float(self.vW.get())
        except: W=0.8
        dW=(1-W)/2*w
        H=0
        if W: H=1.5/W

        fc=self.parse_color(self.vFC0.get())
        lc=self.parse_color(self.vLC0.get())
        
        self.c0img = Image.new('RGBA', (w, h), (0, 0, 0, 0))
        self.c0img = self.draw_column(dW, h-1, w-1-dW, h-1, H, self.c0img, self.vCOLUMN.get(), fc, lc)

        self.c0imgp = ImageTk.PhotoImage(self.c0img)
        self.previewcolumn0.create_image(0, 0, image=self.c0imgp, anchor=NW)
        if can2 and not self.vENABLEC1.get():
            self.update_column1_preview()

    def update_column1_preview(self):
        if not self.master: return
        self.previewcolumn1.update()
        self.previewcolumn1.delete('all')
        w=self.previewcolumn1.winfo_width()
        h=self.previewcolumn1.winfo_height()

        try:    W=float(self.vW.get())
        except: W=0.8
        dW=(1-W)/2*w
        H=0
        if W: H=1.5/W


        if self.vENABLEC1.get():
            fc=self.parse_color(self.vFC1.get())
            lc=self.parse_color(self.vLC1.get())
        else:
            fc=self.parse_color(self.vFC0.get())
            lc=self.parse_color(self.vLC0.get())

        self.c1img = Image.new('RGBA', (w, h), (0, 0, 0, 0))
        self.c1img = self.draw_column(dW, h-1, w-1-dW, h-1, H, self.c1img, self.vCOLUMN.get(), fc, lc)
        self.c1imgp = ImageTk.PhotoImage(self.c1img)
        self.previewcolumn1.create_image(0, 0, image=self.c1imgp, anchor=NW)

        self.update_preview()

    def mix_colors(self, c0, c1, k):
        return (int((1-k)*c0[0]+k*c1[0]), int((1-k)*c0[1]+k*c1[1]), int((1-k)*c0[2]+k*c1[2]), int((1-k)*c0[3]+k*c1[3]))

    def draw_preview(self, img, x0, y0, x1, y1, width, height, heights, fc0, lc0, fc1, lc1, column):
        dx=x1-x0
        dy=y1-y0
        H=height
        if width: H/=width
        dW=(1-width)/2
        for i in range(len(heights)):
            h=heights[i]
            pos=float(i)/len(heights)
            fc = self.mix_colors(fc0, fc1, pos)
            lc = self.mix_colors(lc0, lc1, pos)
            ix0=x0+dx*(pos+1.0/len(heights)*dW)
            iy0=y0+dy*(pos+1.0/len(heights)*dW)
            ix1=x0+dx*(pos+1.0/len(heights)-1.0/len(heights)*dW)
            iy1=y0+dy*(pos+1.0/len(heights)-1.0/len(heights)*dW)
            img = self.draw_column(ix0, iy0, ix1, iy1, h*H, img, column, fc, lc)
        return img
            
    
    # draw preview
    def cpreview(self, img, TYPE, X0, Y0, X1, Y1, W, H, FC0, LC0, FC1, LC1, COLUMN, ENABLEC1, heights):
        w=img.width
        h=img.height
        if TYPE=="LINE":
            x0=self.cfloat(X0, 0)
            y0=self.cfloat(Y0, 0)
            x1=self.cfloat(X1, 1)
            y1=self.cfloat(Y1, 1)
            W=self.cfloat(W, 0.8)
            H=self.cfloat(H, 100)
            xk=1.0/self.parent.get_W()*w
            yk=1.0/self.parent.get_H()*h
            fc0=self.parse_color(FC0)
            lc0=self.parse_color(LC0)
            if ENABLEC1:
                fc1=self.parse_color(FC1)
                lc1=self.parse_color(LC1)
            else:
                fc1=fc0
                lc1=lc0
            l=(((x0-x1)**2+(y0-y1)**2)**0.5)*(1.0/len(heights))*W*0.6
            if l<0:l=2.0
            c0=self.mix_colors(fc0, lc0, 2.0/l)
            c1=self.mix_colors(fc1, lc1, 2.0/l)
            #Use fill color as a border color to make the preview look more what it will
            return self.draw_preview(img, x0*xk, y0*yk, x1*xk, y1*yk, W, H, heights, c0, c0, c1, c1, COLUMN)
            #return self.draw_preview(img, x0*xk, y0*yk, x1*xk, y1*yk, W, H, heights, fc0, lc0, fc1, lc1, COLUMN)
        return img

    def random_heights(self, iINDICES):
        heights=[]
        t=iINDICES.split("-")
        if len(t)<2: t.append(t[0])
        a=self.cint(t[0], 0)
        b=self.cint(t[1], 0)
        if a>b: a,b=b,a
        for i in range(a, b+1): heights.append((random.random()*0.4)**3)
        return heights

    def create_heights(self, iTRACK, iINDICES, track_values):
        ti=self.cint(iTRACK)
        heights=[]
        t=iINDICES.split("-")
        if len(t)<2: t.append(t[0])
        a=self.cint(t[0], 0)
        b=self.cint(t[1], 0)
        if a<=b: 
            for i in range(a, b+1):     heights.append(track_values[ti][i])
        else:   
            for i in range(a, b-1, -1): heights.append(track_values[ti][i])
        return heights


    def create_preview(self, img, track_values):
        #parent calls this...
        #print self.TYPE, self.X0, self.Y0, self.X1, self.Y1, self.W, self.H, self.FC0, self.LC0, self.FC1, self.LC1, self.COLUMN, self.ENABLEC1, self.iINDICES
        self.img = self.cpreview(img, self.TYPE, self.X0, self.Y0, self.X1, self.Y1, self.W, self.H, self.FC0, self.LC0, self.FC1, self.LC1, self.COLUMN, self.ENABLEC1, self.create_heights(self.iTRACK, self.iINDICES, track_values))
        return self.img

    def update_preview(self):
        if not self.master: return
        if self.disable_update_preview: return

        self.preview.update()
        self.preview.delete('all')
        w=self.preview.winfo_width()
        h=self.preview.winfo_height()
        self.cimg = Image.new('RGBA', (w, h), (0, 0, 0, 255))
        self.img = self.cpreview(self.cimg, self.vTYPE.get(), self.vX0.get(), self.vY0.get(), self.vX1.get(), self.vY1.get(), self.vW.get(), self.vH.get(), self.vFC0.get(), self.vLC0.get(), self.vFC1.get(), self.vLC1.get(), self.vCOLUMN.get(), self.vENABLEC1.get(), self.random_heights(self.viINDICES.get()))
        self.imgp = ImageTk.PhotoImage(self.img)
        self.preview.create_image(0, 0, image=self.imgp, anchor=NW)

        
    def update_column_previews(self):
        self.update_column0_preview()
        self.update_column1_preview()

    def change_enablecolor1(self):
        if not self.master: return
        if not self.vENABLEC1.get():
            self.fillcolor1entry.config(state="readonly")
            self.fillcolor1button.config(state="disabled")
            self.linecolor1entry.config(state="readonly")
            self.linecolor1button.config(state="disabled")
        else:
            self.fillcolor1entry.config(state="normal")
            self.fillcolor1button.config(state="normal")
            self.linecolor1entry.config(state="normal")
            self.linecolor1button.config(state="normal")

    def swap_colors(self):
        fc0=self.vFC0.get()
        lc0=self.vLC0.get()
        self.vFC0.set(self.vFC1.get())
        self.vLC0.set(self.vLC1.get())
        self.vFC1.set(fc0)
        self.vLC1.set(lc0)

    def change_type(self, e):
        if not self.master: return
        for i in self.stuff: self.stuff[i].destroy()
        self.stuff={}
        if self.vTYPE.get()=="LINE":
            self.stuff["x0label"] = Label(self.master, text="X0")
            self.stuff["x0label"].grid(row=13, column=0)
            self.stuff["x0entry"]  = Entry(self.master, textvariable=self.vX0)
            self.stuff["x0entry"].grid(row=13, column=1)
            self.stuff["y0label"] = Label(self.master, text="Y0")
            self.stuff["y0label"].grid(row=14, column=0)
            self.stuff["y0entry"]  = Entry(self.master, textvariable=self.vY0)
            self.stuff["y0entry"].grid(row=14, column=1)
            self.stuff["x1label"] = Label(self.master, text="X1")
            self.stuff["x1label"].grid(row=15, column=0)
            self.stuff["x1entry"]  = Entry(self.master, textvariable=self.vX1)
            self.stuff["x1entry"].grid(row=15, column=1)
            self.stuff["y1label"] = Label(self.master, text="Y1")
            self.stuff["y1label"].grid(row=16, column=0)
            self.stuff["y1entry"]  = Entry(self.master, textvariable=self.vY1)
            self.stuff["y1entry"].grid(row=16, column=1)
            self.stuff["mirrorbutton"] = Button(self.master, text="Mirror", command=self.mirror)
            self.stuff["mirrorbutton"].grid(row=17, column=1)

        
    def mirror(self):
        self.disable_update_preview = True
        self.viINDICES.set("-".join(reversed(self.viINDICES.get().split("-"))))
        if self.vTYPE.get()=="LINE":
            x0=self.vX0.get()
            y0=self.vY0.get()
            self.vX0.set(self.vX1.get())
            self.vY0.set(self.vY1.get())
            self.vX1.set(x0)
            self.vY1.set(y0)
        self.disable_update_preview = False
        self.update_preview()
        self.swap_colors()
        

    def change_column_type(self, e):
        pass
    
    def focs(self):
        self.master.lift()
        self.master.focus()

    def colorchooser(self, entry):
        color = askcolor()
        strcolor="["+str(color[0][0])+","+str(color[0][1])+","+str(color[0][2])+",255]"
        entry.delete(0, END)
        entry.insert(0, strcolor)
        self.focs()

    def fillcolor0_chooser(self):
        if not self.master: return
        self.colorchooser(self.fillcolor0entry)

    def linecolor0_chooser(self):
        if not self.master: return
        self.colorchooser(self.linecolor0entry)

    def fillcolor1_chooser(self):
        if not self.master: return
        self.colorchooser(self.fillcolor1entry)

    def linecolor1_chooser(self):
        if not self.master: return
        self.colorchooser(self.linecolor1entry)

    def convert_color(self, color):
        pc=self.parse_color(color)
        return str("["+str(float(pc[0])/255)+","+str(float(pc[1])/255)+","+str(float(pc[2])/255)+","+str(float(pc[3])/255)+"]")

    def write_config(self, f):
        f.write("DRAWER={\n")
        f.write("    COLUMN-TYPE="+self.COLUMN+"\n")
        f.write("    X0="+self.X0+"\n")
        f.write("    Y0="+self.Y0+"\n")
        f.write("    X1="+self.X1+"\n")
        f.write("    Y1="+self.Y1+"\n")
        f.write("    W="+self.W+"\n")
        f.write("    H="+self.H+"\n")
        f.write("    i="+self.iTRACK+":"+self.iINDICES+"\n")
        f.write("    LINE-COLOR-0="+self.convert_color(self.LC0)+"\n")
        f.write("    LINE-COLOR-1="+self.convert_color(self.LC1)+"\n")
        f.write("    FILL-COLOR-0="+self.convert_color(self.FC0)+"\n")
        f.write("    FILL-COLOR-1="+self.convert_color(self.FC1)+"\n")
        f.write("    THICKNESS=1"+"\n")
        f.write("}\n\n")

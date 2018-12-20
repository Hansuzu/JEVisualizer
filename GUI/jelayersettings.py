from tkinter import *
from PIL import Image, ImageTk, ImageDraw
import os

class JELayerSettings:
    def __init__(self, parent, mediadir):
        self.parent = parent
        self.mediadir = mediadir
        self.master = None

        self.bgtype="NONE"
        self.bgvalue=""
        self.W="1280"
        self.H="720"

        self.vbgtype = StringVar()
        self.vbgvalue = StringVar()
        self.vW = StringVar()
        self.vH = StringVar()

        self.vbgvalue.trace("w", lambda name, index, mode: self.chbgvalue())

    def save(self):
        self.bgtype=self.vbgtype.get()
        self.bgvalue=self.vbgvalue.get()
        self.W=self.vW.get()
        self.H=self.vH.get()

    def load(self):
        self.vbgtype.set(self.bgtype)
        self.vbgvalue.set(self.bgvalue)
        self.vW.set(self.W)
        self.vH.set(self.H)

    def copy_from(self, o):
        self.bgtype = o.bgtype
        self.bgvalue = o.bgvalue
        self.W = o.W
        self.H = o.H
        self.load()
        print self.vW.get()


    def view(self, master):
        if self.master: return
        self.master = master

        self.widthlabel = Label(self.master, text="Width")
        self.widthlabel.grid(row=0, column=0)
        self.widthentry = Entry(master, textvariable = self.vW)
        self.widthentry.grid(row=0, column=1)

        self.heightlabel = Label(self.master, text="Height")
        self.heightlabel.grid(row=1, column=0)
        self.heightentry = Entry(master, textvariable = self.vH)
        self.heightentry.grid(row=1, column=1)

        self.bgtypelabel = Label(master, text="Background type")
        self.bgtypelabel.grid(row=2, column=0)
        self.bgtypemenu = OptionMenu(master, self.vbgtype, "COLOR", "IMAGE", "VIDEO", "NONE", command=self.change_bgtype)
        self.bgtypemenu.grid(row=2, column=1)

        self.bglabel = Label(master)
        self.bglabel.grid(row=3, column=0)
        self.bgentry = Entry(master, textvariable=self.vbgvalue)
        self.bgentry.grid(row=3, column=1)
        self.bgbrowsebutton = Frame(master)
        self.bgbrowsebutton.grid(row=3, column=2)
        self.bgpreview = Canvas(master, width=128, height=72)
        self.bgpreview.grid(row=3, column=3)

        self.change_bgtype(None)

        self.load()


    def window_closed(self):
        self.master = None


    def get_W(self):
        try:    return float(self.vW.get())
        except: return 1.0
    def get_H(self):
        try:    return float(self.vH.get())
        except: return 1.0

    def edited(self):
        self.parent.update_preview()

    def cint(self, no):
        try:    return int(no)
        except: return 0

    def drawbg(self, img, bgtype, bgvalue):
        if bgtype=="COLOR":
            bgcolor = Image.new('RGBA', (img.width, img.height), (0, 0, 0, 0))
            d = ImageDraw.Draw(bgcolor)
            #TODO: different colors...
            t=bgvalue[1:-1].split(",")
            while len(t)<4:t.append(0)
            r=self.cint(t[0])
            g=self.cint(t[1])
            b=self.cint(t[2])
            a=self.cint(t[3])
            print r,
            d.rectangle(((0, 0), (img.width, img.height)), fill=(r, g, b, a)) 
            return Image.alpha_composite(img, bgcolor)
        if bgtype=="IMAGE":
            if not os.path.isfile(bgvalue):
                d = ImageDraw.Draw(img)
                d.text((0,0), "No file", fill=(255,255,255,255))
                return img
            try:
                nimg = Image.open(bgvalue).convert('RGBA')
                self.nimg=nimg.resize((img.width, img.height), Image.ANTIALIAS)
                return Image.alpha_composite(img, self.nimg)
            except Exception, e:
                d = ImageDraw.Draw(img)
                d.text((0,0), "Opening failed", fill=(255,255,255,255))
                print e
                return img
        if bgtype=="VIDEO":
            d = ImageDraw.Draw(img)
            d.text((0,0), "No preview for videos", fill=(255,255,255,255))
            return img
        return img
        
    def canvasbg(self):
        canvas.update()

    # updates all 
    def update_preview(self, img, track_values):
        self.bgpreview.update()
        self.bgpreview.delete("all")
        self.bgimg = self.drawbg(img, self.vbgtype.get(), self.vbgvalue.get())
        self.bgimgp = ImageTk.PhotoImage(self.bgimg)
        self.bgpreview.create_image(0, 0, image=self.bgimgp, anchor=NW)
        return self.bgimg

    # called from parent: draws preview on img given as a parameter
    def create_preview(self, img, track_values):
        self.bgimg = self.drawbg(img, self.bgtype, self.bgvalue)
        return self.bgimg
 
    def change_bgtype(self, event):
        self.bglabel.destroy()
        self.bgentry.destroy()
        self.bgbrowsebutton.destroy()
        old_bgtype=self.bgtype
        bgtype=self.vbgtype.get()
        if bgtype!="NONE":
            self.bgentry = Entry(self.master, textvariable=self.vbgvalue)
            self.bgentry.grid(row=3, column=1)
        else:
            self.bgentry = Frame(self.master)
            self.bgentry.grid(row=3, column=1)

        if bgtype=="COLOR":
            self.bglabel = Label(self.master, text="Background-color")
            self.bglabel.grid(row=3, column=0)
            if old_bgtype != bgtype: self.vbgvalue.set("[0,0,0,255]")
            self.bgbrowsebutton = Button(self.master, text='Select Color', command=self.bgcolor_chooser)
            self.bgbrowsebutton.grid(row=3, column=2)
        elif bgtype=="IMAGE":
            self.bglabel = Label(self.master, text="Background image")
            self.bglabel.grid(row=3, column=0)
            if old_bgtype != bgtype: self.vbgvalue.set("")
            self.bgbrowsebutton = Button(self.master, text="Browse", command=self.browse_bgimage_dir)
            self.bgbrowsebutton.grid(row=3, column=2)
        elif bgtype=="VIDEO":
            self.bglabel = Label(self.master, text="Background video")
            self.bglabel.grid(row=3, column=0)
            if old_bgtype != bgtype: self.vbgvalue.set("")
            self.bgbrowsebutton = Button(self.master, text="Browse", command=self.browse_bgvideo_dir)
            self.bgbrowsebutton.grid(row=3, column=2)
        self.edited()

    def chbgvalue(self):
        if self.master:
            self.bgpreview.delete("all")
            self.edited()

    def focs(self):
        self.master.master.lift()
        self.master.master.focus()

    def bgcolor_chooser(self):
        color = askcolor() 
        if not color: return
        strcolor="["+str(color[0][0])+","+str(color[0][1])+","+str(color[0][2])+",255]"
        self.bgentry.delete(0, END)
        self.bgentry.insert(0, strcolor)
        self.focs()
        self.bgpreview.delete("all")
        self.edited()

    def browse_bgimage_dir(self):
        current = "/".join(self.bgentry.get().split("/")[:-1])
        if current=="":current=self.mediadir
        dirname = filedialog.askopenfilename(initialdir=current)
        self.bgentry.delete(0, END)
        self.bgentry.insert(0, dirname)
        self.focs()
        self.bgpreview.delete("all")
        self.edited()

    def browse_bgvideo_dir(self):
        current = "/".join(self.bgentry.get().split("/")[:-1])
        if current=="":current=self.mediadir
        dirname = filedialog.askopenfilename(initialdir=current)
        self.bgentry.delete(0, END)
        self.bgentry.insert(0, dirname)
        self.focs()
        self.bgpreview.delete("all")
        self.canvasbg(self.bgpreview)

    def write_config(self, f):
        if self.bgtype=="COLOR": f.write("BG-COLOR=\""+self.bgvalue+"\"\n")
        elif self.bgtype=="IMAGE": f.write("BG-IMAGE=\""+self.bgvalue+"\"\n")
        elif self.bgtype=="VIDEO": f.write("BG-VIDEO=\""+self.bgvalue+"\"\n")
        f.write("W="+self.W+"\n")
        f.write("H="+self.H+"\n")
        

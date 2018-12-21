from tkinter import *
from subprocess import Popen, PIPE
import threading

class JEVisualizerRunner:
    def __init__(self, parent, master, settings):
        self.parent = parent
        self.master = master
        self.settings = settings
        self.window = None
    
    def destroy_window(self): 
        self.window.destroy()
        self.window=None
           

    def stop(self):
        self.stopped=True
        self.process.kill()
        self.destroy_window()
        messagebox.showinfo("JEVGUI: JEVisualizer stopped", "JEVisualizer stopped")

    def done(self):
        if not self.master: return
        self.destroy_window()
        messagebox.showinfo("JEVGUI: JEVisualizer done", "JEVisualizer done")

    def run(self, command):
        command=command.replace("$PARAMS$", "-G").split(" ")
        self.stopped=False
        self.process = Popen(command, stdout=PIPE, universal_newlines=True)
        for stdout_line in iter(self.process.stdout.readline, ""):
            msg=stdout_line.split(" ")
            if msg[0][0]=="#":
                self.parti = int(msg[0][1:])
                if self.parti==1:  self.part.set(str(self.parti)+"/3 creating spectrums...")
                if self.parti==2:  self.part.set(str(self.parti)+"/3 rendering video...")
                if self.parti==3:  self.part.set(str(self.parti)+"/3 executing command...")
                self.percents.set("0%")
                self.estimated_time_left.set("N/A")
                self.fps_value.set("")
            elif self.parti==2:
                firstFrame=int(msg[0])
                currentFrame=int(msg[1])
                lastFrame=int(msg[2])
                currentFrameTime=msg[3]
                percents=round(float(msg[4]),1)
                self.percents.set(str(percents)+"%")
                fps=round(float(msg[5]), 1)
                self.fps_value.set(str(fps))
                et=int(round(float(msg[6])))
                es=et%60
                et/=60
                em=et%60
                et/=60
                t=""
                if et: t+=str(et)+"h "
                if em: t+=str(em)+"m "
                t+=str(es)+"s"
                self.estimated_time_left.set(t)
        if not self.stopped: self.done()

    def run_JEVisualizer(self, command):
        if self.window: return

        self.window = Toplevel(self.master)
        self.window.title("JEVisualizer is running...")

        self.parti = 0
        self.part = StringVar()
        self.part_text = Label(self.window, textvariable = self.part)
        self.part_text.grid(row=1, column=0)

        self.percents_title = Label(self.window, text="Progress", font=("bold"))
        self.percents_title.grid(row=2, column=0)

        self.percents = StringVar()
        self.progress_text = Label(self.window, textvariable = self.percents)
        self.progress_text.grid(row=3, column=0)
        

        
        self.fps_title = Label(self.window, text="FPS", font=("bold"))
        self.fps_title.grid(row=2, column=1)

        self.fps_value=StringVar()
        self.fps_text = Label(self.window, textvariable = self.fps_value)
        self.fps_text.grid(row=3, column=1)


        self.time_left_title = Label(self.window, text="Estimated time Left", font=("bold"))
        self.time_left_title.grid(row=2, column=2)

        self.estimated_time_left=StringVar()
        self.estimated_text = Label(self.window, textvariable = self.estimated_time_left)
        self.estimated_text.grid(row=3, column=2)

        self.stop_button = Button(self.window, text="Stop", command=self.stop)
        self.stop_button.grid(row=4, column=0)

        self.process_thread = threading.Thread(target=self.run, args=(command,))
        self.process_thread.start()

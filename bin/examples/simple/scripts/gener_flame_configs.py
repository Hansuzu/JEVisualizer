import random

flames=7

def randomChangingColor():
  return "(0, [], 0, 1, 1, ("+str(random.random())+", ["+str(random.random()/50)+"]))"

for i in range(flames):
  f=open("flame"+str(i)+".config", "w")
  f.write("BG-IMAGE=\"examples/simple/media/flame"+str(i)+".png\"\n")
  f.write("FPV#1=track-values0:0-112\n")
  ccSpeed=random.random()
  f.write("""FILTER={
  TYPE=SUPER-COLOR
  RULE=\{  
      R=(1.0, """+randomChangingColor()+""", """+randomChangingColor()+""", 0)
      G=("""+randomChangingColor()+""", 1.0, """+randomChangingColor()+""", 0)
      B=("""+randomChangingColor()+""", """+randomChangingColor()+""", 1.0, 0)
      A=(0, 0, 0, """+randomChangingColor()+""")
      //SHIFT-X=(0, [], -4, 4, """+str(4*(random.random()-0.5))+""", (-210, [0, 300], 0))
      //SHIFT-Y=(0, [], -4, 4, """+str(4*(random.random()-0.5))+""", (-210, [0, 300], 0))
  \}
}\n""")
  f.close()

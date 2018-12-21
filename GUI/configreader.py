
# LOL, This file is only pythonized version of the code of config.cpp...
# HMM... The config format is quite cool but reading it is pain
# At least having two different readers in different programming languagesis pain...]

def readConfig(lines):
    ans=[]
    line=""
    istr=0
    isComment=0
    previous='\0'
    b=""
    v=""
    ec=0
    esc=0
    for line in lines:
        if len(line)==0: continue
        if line[-1]=="\n":line=line[:-1]
        if len(line)==0: continue
        if not istr:
            previous='\0'
            b=""
            v=""
            ec=0
        else:
            if not ec: b+="\n"  
            else:      v+="\n"
        for i in range(len(line)):
            if not istr and not isComment:
                if line[i]==" " or line[i]=="\t":       continue
                if line[i]=="/" and previous=="/": 
                    #GUI wants to use commented values ()
                    t=line[i+1:].split("=")
                    var=t[0]
                    val="=".join(t[1:])
                    ans.append([0, [var.strip(), ""], val.strip()])
                    break
            if not isComment and not istr and line[i]=="*" and i and line[i-1]=="/": isComment=1
            elif isComment and line[i]=="/" and i and line[i-1]=="*": isComment=0
            elif not isComment:
                if line[i]=='"' and not istr:
                    istr=1
                    continue
                if line[i]=='"' and istr==1 and not esc:
                    istr=0
                    continue
                if line[i]=="'" and not istr:
                    istr=2
                    continue
                if line[i]=="'" and istr==2 and not esc:
                    istr=0
                    continue
                if line[i]=="{" and not istr and not esc:
                    istr=3
                    continue
                if line[i]=="}" and istr==3 and not esc:
                    istr=0
                    continue
                if istr and not esc and line[i]=="\\":
                    esc=True
                    continue
                else:   
                    esc=False
                if not istr and line[i]=="=":
                    ec=1
                else:
                    if not ec: b+=line[i]
                    else:      v+=line[i]
                previous=line[i]
        if ec and not istr:
            if b=="#include":
                ans+=readConfigFromFile(v.c_str())
            else:
                t=b.split("#")
                if len(t)==1: no=""
                else:         no="#".join(t[1:])
            ans.append([1, [t[0], no], v])
    return ans

def readConfigFromString(s):
    return readConfig(s.split("\n"))

def readConfigFromFile(fn):
    f=open(fn)
    t=f.readlines()
    f.close()
    return readConfig(t)


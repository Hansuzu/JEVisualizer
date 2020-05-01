# JEVisualizer
JEVisualizer is a program that creates a visualization video for music. The development started, when we wanted to add visualizations for our music on YouTube. One important goal is to develop JEVisualizer in such a way that it can be configured to create very different visualizations - only imagination should be the limit.

Some examples about visualizations made with JEVisualizer can be found on our YouTube channel, [John Egestas](https://www.youtube.com/channel/UCSA9SAK2T-KkLAR59Jwy78w)

Current version is working, but not very "shippable". (poor and not up-to-date documentation, a bit difficult to use, only few file formats supported...)

[TODO-list](TODO.md)

## How to build
Requirements:
```
make
g++
OpenCV 3 
ffmpeg or some other program to add sounds
```
Build project:
```
mkdir build
make all
```
## How to use
Craete and edit config files on bin/ to make the visualizer do what you want and run bin/JEVisualizer .

~~Run example 'simple'~~
example had large files and I removed them to make this faster. TODO: modify example and make it work without large media files. ALSO: make the example actually "simple"
```
./JEVisualizer --config examples/simple/main.config
```

## GUI
With GUI it is possible to create simple configurations for JEVisualizer. Current version of GUI supports only very few simple features.

Requirements for GUI:
```
python
tkinter
PIL
```
Executing GUI: in directory GUI
```
python gui.py
```

## Documentation

[Documentation](https://github.com/Hansuzu/JEVisualizer/blob/master/doc/documentation.pdf) is here. It is not covering everything and it is not necessarily up to date. I will update it and add more documentation when I have time and motivation, and when I believe that the features I'm documenting won't be changed on the very next commit.


## Things to note

- Currently supports only 16-bits wav files as an input.
- Not sure if any other output-format than ogv currently works
- JEVisualizer doesn't currently add sounds to the output video. Use for example ffmpeg to add sounds.
- In config files the paths to other files are relative to the directory from which the JEVisualizer is executed (this means for example that configurations created with GUI work only when JEVisualizer is executed from the GUI directory) Fixing this is on TODO list


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
```
Build project:
```
mkdir build
make all
```
## How to use
Craete and edit config files on bin/ to make the visualizer do what you want and run bin/JEVisualizer .

Run example 'simple'
```
./JEVisualizer --config examples/simple/main.config
```


[Documentation](https://github.com/Hansuzu/JEVisualizer/blob/master/doc/documentation.pdf) is here. It is not covering everything and it is not necessarily up to date. I will update it and add more documentation when I have time and motivation, and when I believe that the features I'm documenting won't be changed on the very next commit.


## Things to note

- Currently supports only 16-bits wav files as an input.
- Not sure if any other output-format than ogv currently works
- JEVisualizer doesn't currently add sounds to the output video. Use for example ffmpeg to add sounds.
- 

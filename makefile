CC=g++
OPENCV_FLAGS=-I/usr/local/include/opencv -I/usr/local/include -L/usr/local/lib -lopencv_ml -lopencv_stitching -lopencv_superres -lopencv_dnn -lopencv_objdetect -lopencv_videostab -lopencv_photo -lopencv_shape -lopencv_video -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_imgproc -lopencv_flann -lopencv_core
CFLAGS=-std=c++17 -O3 -march=native -Wall -Iinclude -Wextra -Wshadow

SRCDIR=src
TARGETDIR=bin
BUILDDIR=build
HEADERDIR=include

all:$(TARGETDIR)/main

$(TARGETDIR)/main: $(SRCDIR)/main.cpp  $(BUILDDIR)/controller.o
	$(CC) $(CFLAGS) $(SRCDIR)/main.cpp $(BUILDDIR)/controller.o $(BUILDDIR)/wav.o $(BUILDDIR)/config.o $(BUILDDIR)/visualizer.o $(BUILDDIR)/layer.o $(BUILDDIR)/mmp.o -o $(TARGETDIR)/main  $(OPENCV_FLAGS)

$(BUILDDIR)/controller.o: $(SRCDIR)/controller.cpp $(BUILDDIR)/config.o $(HEADERDIR)/controller.h $(BUILDDIR)/wav.o $(BUILDDIR)/visualizer.o $(BUILDDIR)/mmp.o 
	$(CC) $(CFLAGS) $(SRCDIR)/controller.cpp -o $(BUILDDIR)/controller.o -c  $(OPENCV_FLAGS)

$(BUILDDIR)/wav.o: $(SRCDIR)/wav.cpp $(HEADERDIR)/wav.h
	$(CC) $(CFLAGS) $(SRCDIR)/wav.cpp -o $(BUILDDIR)/wav.o -c  $(OPENCV_FLAGS)

$(BUILDDIR)/mmp.o: $(SRCDIR)/mmp.cpp $(HEADERDIR)/mmp.h
	$(CC) $(CFLAGS) $(SRCDIR)/mmp.cpp -o $(BUILDDIR)/mmp.o -c  $(OPENCV_FLAGS)

$(BUILDDIR)/visualizer.o: $(SRCDIR)/visualizer.cpp $(HEADERDIR)/visualizer.h $(BUILDDIR)/config.o $(BUILDDIR)/layer.o
	$(CC) $(CFLAGS) $(SRCDIR)/visualizer.cpp -o $(BUILDDIR)/visualizer.o -c  $(OPENCV_FLAGS)

$(BUILDDIR)/layer.o: $(SRCDIR)/layer.cpp $(HEADERDIR)/layer.h $(BUILDDIR)/config.o
	$(CC) $(CFLAGS) $(SRCDIR)/layer.cpp -o $(BUILDDIR)/layer.o -c  $(OPENCV_FLAGS)

$(BUILDDIR)/config.o: $(SRCDIR)/config.cpp $(HEADERDIR)/config.h
	$(CC) $(CFLAGS) $(SRCDIR)/config.cpp -o $(BUILDDIR)/config.o -c $(OPENCV_FLAGS)



clean:
	rm -f $(BUILDDIR)/*

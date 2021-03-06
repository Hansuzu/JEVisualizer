CC=g++
OPENCV_FLAGS=-I/usr/local/include/opencv -I/usr/local/include -L/usr/local/lib -lopencv_ml -lopencv_stitching -lopencv_superres -lopencv_dnn -lopencv_objdetect -lopencv_videostab -lopencv_photo -lopencv_shape -lopencv_video -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_imgproc -lopencv_flann -lopencv_core
CFLAGS=-std=c++17 -O3 -march=native -Wall -Iinclude -Wextra -Wshadow -pthread

SRCDIR=src
TARGETDIR=bin
BUILDDIR=build
HEADERDIR=include

all:$(TARGETDIR)/JEVisualizer

$(TARGETDIR)/JEVisualizer: $(SRCDIR)/main.cpp $(BUILDDIR)/controller.o $(BUILDDIR)/wav.o $(BUILDDIR)/mmp.o $(BUILDDIR)/spctr.o $(BUILDDIR)/extractor.o $(BUILDDIR)/visualizer.o $(BUILDDIR)/layer.o $(BUILDDIR)/drawer.o $(BUILDDIR)/filter.o $(BUILDDIR)/particlesource.o $(BUILDDIR)/formulacolor.o $(BUILDDIR)/formula.o $(BUILDDIR)/fpe.o $(BUILDDIR)/trackcontroller.o $(BUILDDIR)/drawingfunctions.o $(BUILDDIR)/config.o $(BUILDDIR)/log.o $(BUILDDIR)/globalsettings.o
	$(CC) $(CFLAGS) $(SRCDIR)/main.cpp $(BUILDDIR)/controller.o $(BUILDDIR)/wav.o $(BUILDDIR)/mmp.o $(BUILDDIR)/spctr.o $(BUILDDIR)/extractor.o $(BUILDDIR)/visualizer.o $(BUILDDIR)/layer.o $(BUILDDIR)/drawer.o $(BUILDDIR)/filter.o $(BUILDDIR)/particlesource.o $(BUILDDIR)/formulacolor.o $(BUILDDIR)/formula.o $(BUILDDIR)/fpe.o $(BUILDDIR)/trackcontroller.o $(BUILDDIR)/drawingfunctions.o $(BUILDDIR)/config.o $(BUILDDIR)/log.o $(BUILDDIR)/globalsettings.o -o $(TARGETDIR)/JEVisualizer  $(OPENCV_FLAGS)


$(BUILDDIR)/controller.o: $(SRCDIR)/controller.cpp $(HEADERDIR)/globalsettings.h $(HEADERDIR)/log.h $(BUILDDIR)/config.o $(HEADERDIR)/controller.h $(BUILDDIR)/wav.o $(BUILDDIR)/mmp.o $(BUILDDIR)/spctr.o  $(BUILDDIR)/extractor.o $(BUILDDIR)/visualizer.o
	$(CC) $(CFLAGS) $(SRCDIR)/controller.cpp -o $(BUILDDIR)/controller.o -c  $(OPENCV_FLAGS)


$(BUILDDIR)/wav.o: $(SRCDIR)/wav.cpp $(HEADERDIR)/wav.h $(HEADERDIR)/globalsettings.h $(HEADERDIR)/log.h
	$(CC) $(CFLAGS) $(SRCDIR)/wav.cpp -o $(BUILDDIR)/wav.o -c  $(OPENCV_FLAGS)

$(BUILDDIR)/mmp.o: $(SRCDIR)/mmp.cpp $(HEADERDIR)/mmp.h $(HEADERDIR)/globalsettings.h $(HEADERDIR)/log.h
	$(CC) $(CFLAGS) $(SRCDIR)/mmp.cpp -o $(BUILDDIR)/mmp.o -c  $(OPENCV_FLAGS)

$(BUILDDIR)/spctr.o: $(SRCDIR)/spctr.cpp $(HEADERDIR)/spctr.h $(HEADERDIR)/globalsettings.h $(HEADERDIR)/log.h
	$(CC) $(CFLAGS) $(SRCDIR)/spctr.cpp -o $(BUILDDIR)/spctr.o -c  $(OPENCV_FLAGS)



$(BUILDDIR)/extractor.o: $(SRCDIR)/extractor.cpp $(HEADERDIR)/extractor.h $(HEADERDIR)/globalsettings.h $(HEADERDIR)/log.h $(HEADERDIR)/config.h $(HEADERDIR)/trackcontroller.h
	$(CC) $(CFLAGS) $(SRCDIR)/extractor.cpp -o $(BUILDDIR)/extractor.o -c  $(OPENCV_FLAGS)



$(BUILDDIR)/visualizer.o: $(SRCDIR)/visualizer.cpp $(HEADERDIR)/visualizer.h $(HEADERDIR)/globalsettings.h $(HEADERDIR)/log.h $(HEADERDIR)/config.h $(HEADERDIR)/trackcontroller.h $(HEADERDIR)/fpe.h $(HEADERDIR)/layer.h $(HEADERDIR)/formula.h $(HEADERDIR)/formulacolor.h $(HEADERDIR)/drawer.h $(HEADERDIR)/filter.h $(HEADERDIR)/particlesource.h
	$(CC) $(CFLAGS) $(SRCDIR)/visualizer.cpp -o $(BUILDDIR)/visualizer.o -c  $(OPENCV_FLAGS)

$(BUILDDIR)/layer.o: $(SRCDIR)/layer.cpp $(HEADERDIR)/layer.h $(HEADERDIR)/globalsettings.h $(HEADERDIR)/log.h $(HEADERDIR)/config.h $(HEADERDIR)/trackcontroller.h $(HEADERDIR)/fpe.h $(HEADERDIR)/formula.h $(HEADERDIR)/formulacolor.h $(HEADERDIR)/drawer.h $(HEADERDIR)/filter.h $(HEADERDIR)/particlesource.h
	$(CC) $(CFLAGS) $(SRCDIR)/layer.cpp -o $(BUILDDIR)/layer.o -c  $(OPENCV_FLAGS)

$(BUILDDIR)/drawer.o: $(SRCDIR)/drawer.cpp $(HEADERDIR)/drawer.h $(HEADERDIR)/drawingfunctions.h $(HEADERDIR)/globalsettings.h $(HEADERDIR)/log.h $(HEADERDIR)/config.h $(HEADERDIR)/trackcontroller.h $(HEADERDIR)/fpe.h $(HEADERDIR)/formula.h $(HEADERDIR)/formulacolor.h
	$(CC) $(CFLAGS) $(SRCDIR)/drawer.cpp -o $(BUILDDIR)/drawer.o -c  $(OPENCV_FLAGS)

$(BUILDDIR)/filter.o: $(SRCDIR)/filter.cpp $(HEADERDIR)/filter.h $(HEADERDIR)/globalsettings.h $(HEADERDIR)/log.h $(HEADERDIR)/config.h $(HEADERDIR)/trackcontroller.h $(HEADERDIR)/fpe.h $(HEADERDIR)/formula.h $(HEADERDIR)/formulacolor.h
	$(CC) $(CFLAGS) $(SRCDIR)/filter.cpp -o $(BUILDDIR)/filter.o -c  $(OPENCV_FLAGS)

$(BUILDDIR)/particlesource.o: $(SRCDIR)/particlesource.cpp $(HEADERDIR)/particlesource.h $(HEADERDIR)/drawingfunctions.h $(HEADERDIR)/globalsettings.h $(HEADERDIR)/log.h $(HEADERDIR)/config.h $(HEADERDIR)/trackcontroller.h $(HEADERDIR)/fpe.h $(HEADERDIR)/formula.h $(HEADERDIR)/formulacolor.h
	$(CC) $(CFLAGS) $(SRCDIR)/particlesource.cpp -o $(BUILDDIR)/particlesource.o -c  $(OPENCV_FLAGS)

$(BUILDDIR)/formulacolor.o: $(SRCDIR)/formulacolor.cpp $(HEADERDIR)/formulacolor.h $(HEADERDIR)/globalsettings.h $(HEADERDIR)/log.h $(HEADERDIR)/fpe.h $(HEADERDIR)/formula.h
	$(CC) $(CFLAGS) $(SRCDIR)/formulacolor.cpp -o $(BUILDDIR)/formulacolor.o -c  $(OPENCV_FLAGS)

$(BUILDDIR)/formula.o: $(SRCDIR)/formula.cpp $(HEADERDIR)/formula.h $(HEADERDIR)/globalsettings.h $(HEADERDIR)/log.h $(HEADERDIR)/fpe.h
	$(CC) $(CFLAGS) $(SRCDIR)/formula.cpp -o $(BUILDDIR)/formula.o -c  $(OPENCV_FLAGS)

$(BUILDDIR)/fpe.o: $(SRCDIR)/fpe.cpp $(HEADERDIR)/fpe.h $(HEADERDIR)/log.h
	$(CC) $(CFLAGS) $(SRCDIR)/fpe.cpp -o $(BUILDDIR)/fpe.o -c  $(OPENCV_FLAGS)\

$(BUILDDIR)/trackcontroller.o: $(SRCDIR)/trackcontroller.cpp $(HEADERDIR)/trackcontroller.h $(HEADERDIR)/globalsettings.h $(HEADERDIR)/log.h
	$(CC) $(CFLAGS) $(SRCDIR)/trackcontroller.cpp -o $(BUILDDIR)/trackcontroller.o -c  $(OPENCV_FLAGS)


$(BUILDDIR)/drawingfunctions.o: $(SRCDIR)/drawingfunctions.cpp $(HEADERDIR)/drawingfunctions.h $(HEADERDIR)/globalsettings.h $(HEADERDIR)/log.h
	$(CC) $(CFLAGS) $(SRCDIR)/drawingfunctions.cpp -o $(BUILDDIR)/drawingfunctions.o -c $(OPENCV_FLAGS)


$(BUILDDIR)/config.o: $(SRCDIR)/config.cpp $(HEADERDIR)/config.h $(HEADERDIR)/globalsettings.h $(HEADERDIR)/log.h
	$(CC) $(CFLAGS) $(SRCDIR)/config.cpp -o $(BUILDDIR)/config.o -c $(OPENCV_FLAGS)

$(BUILDDIR)/log.o: $(SRCDIR)/log.cpp $(HEADERDIR)/log.h $(HEADERDIR)/globalsettings.h
	$(CC) $(CFLAGS) $(SRCDIR)/log.cpp -o $(BUILDDIR)/log.o -c $(OPENCV_FLAGS)
	
$(BUILDDIR)/globalsettings.o: $(SRCDIR)/globalsettings.cpp $(HEADERDIR)/globalsettings.h
	$(CC) $(CFLAGS) $(SRCDIR)/globalsettings.cpp -o $(BUILDDIR)/globalsettings.o -c $(OPENCV_FLAGS)



clean:
	rm -f $(BUILDDIR)/*

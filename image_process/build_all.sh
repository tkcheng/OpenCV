#!/bin/sh

g++ main.cpp image_process.cpp fft.cpp kernel_extract.cpp pixel_process.cpp util.cpp -o main -I /usr/include/opencv -L /usr/local/lib -lm -lcv -lhighgui


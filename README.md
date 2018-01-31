0. Make sure that both your x86 machine and Rpi have OpenCV installed under /usr/local/ respectively.
Rpi should also have raspicam,
	  https://github.com/cedricve/raspicam
installed under /usr/local on Rpi.

1. makefile_tmpl is a template of makefile. Copy this file to every individual project and replace [] with actual names.
When building on x86, simply type
     'make'
When building on Rpi, type
     'make RPI=1'

2. platformcam.h privides a unified API for default camera on PC and Rpi camera. Include this header in all source files that manipulates cameras.

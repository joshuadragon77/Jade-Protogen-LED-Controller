
![IMG_5323](https://github.com/user-attachments/assets/bca06bf4-59e6-4e12-bc44-04f0d5bd5718)
[IMG_5322](https://github.com/user-attachments/assets/81365bc8-6e06-40c2-9486-c091a643e569)
# Jade Protogen LED Controller!


Written in C and runs on a Raspberry Pi 4 with the following hardware accessories: Adafruit HUB 75 Hat, WS2818 Led Strips. 
It is a custom made program that runs my Protogen Head and will have more features coming soon as development goes on.

You may use the code here to help on your Protogen Endeavours.

In the future, once I have time, I will document and comment and organize everything.

## How to use:
There are several build scripts I have made within the scripts directory that will do the following...
- `build.sh` Builds the program.
- `start.sh` Builds the program and runs it.
- `assest` Takes all the SVGs from the faces directory and creates a header file storing the SVG points.

If it was confusing, you can simply use the `start.sh` script to do whatever you need.

## Modules:
- - `animator` Handles my own animation systems I have developed over the years in Music Visualizer. It utilizes a TimeFactor and AnimationFactor scheme to handle transitions. Supports many modes and ensures everything is timed properly.
  - `color` Handles the color scheme throughout the LEDs. It can also applies colour transitions over time which is dependant on the `animator`.
  - `jadeled` Is the main entry point and sets up all the module to work
  - `ledmatrix` Handles the interface to the `rgb-rpi-led-matrix` library so it can render the face to the RGB matrices.
  - `prerender` Handles the system that pre-renders all the face animation ahead of time to reduce CPU load during run-time. "was very complicated to make"
  - `transition` Holds and stores all the face states and determines how to transition them using `animator`, `prerender` and `color`.
  - `wsled` Handles the interface to the `rpi_ws281x` library so it can control the WS2818 LED strips.

## Dependencies
You will need to build the following libraries before you can build and utilize this program.

- https://github.com/jgarff/rpi_ws281x
- https://github.com/hzeller/rpi-rgb-led-matrix
- GraphicsMagick (Done by using Raspberry PI APT)
  


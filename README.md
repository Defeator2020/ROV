# ROV

This repository contains the code for my ongoing Romotely Operated (Underwater) Vehicle (ROV) project. This includes both the primary programs (those of the control and submersible boards), as well as any test sketches used to develop the equipment interfaces.

Designed for a maximum operating depth of 100m, the submersible is primarily constructed of PVC pipe. Clear components (portholes for cameras and lights) are Lexan (polycarbonate). The design has three primary pressure vessels, a large-diameter central one for the camera, primary electronics, and batteries, as well as two smaller-diameter (but much longer) vessels on either side, which contain the lights, periphery cameras, and the ESCs for the horizontal thrusters.

Control, on both the control panel and vessel sides, is managed with Arduino Uno's (one on either end), utilizing MAX485 breakouts to send serial between the two. Data and video (using a pair of composite video baluns) are

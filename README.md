# ROV

This repository contains the code for my ongoing Remotely Operated (Underwater) Vehicle (ROV) project. This includes both the primary programs (those of the control and submersible boards), as well as any test sketches used to develop the equipment interfaces.

Designed for a maximum operating depth of 100m, the submersible is primarily constructed of PVC pipe. Clear components (portholes for cameras and lights) are Lexan (polycarbonate). The design has three primary pressure vessels, a large-diameter central one for the camera, primary electronics, and batteries, as well as two smaller-diameter (but much longer) vessels on either side, which contain the lights, periphery cameras, and the ESCs for the horizontal thrusters.

Control, on both the control panel and vessel sides, is managed with Arduino Uno's (one on either end), utilizing MAX485 breakouts to send serial between the two. Data and video (using a pair of composite video baluns) are sent up two of four twisted pairs in a Cat 5e ethernet cable that connects the vehicle to the surface, which is stored on a custom spool when the full length isn't needed.

Propulsion is achieved with a 3-thruster layout, two horizontal (one on either side) and one vertical. This allows for forward and backward movement, rotation about the z-axis (turning), and vertical adjustment. The motors are mixed differentially, to allow for different rates and radii of turning. The thrusters are 3D-printed housings (and props!), with 1000kV brushless electric motors (like those used for RC airplanes and multirotors). These are driven with 60A (plenty of safety margin) RC car ESC's, which allow for forward and backward rotation of the motors by simply varying the PWM control signal sent to them.

The tether itself is (stranded) Cat 5e ethernet cable, 500' in length (currently only 250', for testing), that has been pulled through the center of a 3/8" hollow braid polypropylene rope, which both protects the ethernet cable from nicks and cuts as well as providing mechanical support, allowing the ROV to be manually pulled in, if need be.

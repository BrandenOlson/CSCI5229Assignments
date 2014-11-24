OpenPong (by Branden Olson)
===========================

Author: Branden Olson  
Email: branden.olson@colorado.edu
Source file: pong.c

Description
-----------
This assignment makes use of textures to display a can of Pabst Blue
Ribbon surrounded by several bumpy-textured Red Solo Cups.

Key Bindings
------------
* ESC     -> Exits the program
* 0       -> Resets the angle to default: (th, ph) = (0, 25)
* '-'     -> Decreases dimension which causes a zoom-in effect
* '+'     -> Increases dimension which causes a zoom-out effect
* 'a'/'A' -> Toggles axes
* 'l'/'L' -> Toggles lighting
* Arrows change view angle

Acknowledgements
----------------
I referred to mrmoo's code on opengl.org for help with my drawCylinder
function. 
 
I made use of the CSCIx229.h library and snagged some code from some
class examples. I also used loadtexbmp.c, created by Dr. Schreuder

I modified the source of the glut torus function to work with my
requirements for the drawCup() function. 

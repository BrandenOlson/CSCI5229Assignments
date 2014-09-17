The Lorenz Visualizer
=====================
Author: Branden Olson
E-mail: branden.olson@colorado.edu
Source file: lorenzVisualizer.c

Basic Controls
==============

* The left and right arrow keys change the azimuth of the view angle
* The up and down arrow keys change the elevation of the view angle
* To change any of the parameters s, b, or r, hit the key whose character 
   (upper or lower case) matches the parameter. For example, to edit the b
   parameter, hit either 'b' or "B", which will bring you into "B Mode"
* Once in a parameter mode, press "+" to increase the parameter by a
   predetermined amount, or press "-" to increase the parameter by a
   predetermed amount.
* Press "0" to reset the view angle and all parameters to their default values.
* Press ESC to close the window and terminate the program.

Interesting Things about the Lorenz Attractor
=============================================

It doesn't take much toying around with the program to realize that this
attractor can take on many vastly different paths in space.
I'll describe a few cases that personally piqued my interest.

* If b and r are held constant, then if we decrease s, once s becomes negative
   the attractor begins some strange behavior. Once s < -3.0, the attractor
   starts to "shoot outwards" and oscillate in the direction it shoots.
   This oscillation slows down significantly as s decreases.

* If r is decreased to 0, the attractor becomes almost too small to visualize.
   If r is increased with s and b held constant, the attractor more or less
   holds its form, but increases in size indefinitely. 

* For s about -5.6, b about 11.4, and r about 25.2, you see an interestingly
   shaped limit cycle, which looks like a square-ish spiral.

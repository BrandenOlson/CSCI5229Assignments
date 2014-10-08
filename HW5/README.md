Ancient Egypt in Technicolor, III
=================================

Author: Branden Olson  
Email: branden.olson@colorado.edu  
Source file: hw5.c

Description
-----------
This program draws a few pyramids and a few cacti, with light enabled upon user
request. The user can switch between orthogonal and perspective viewing modes.

Basic Controls
--------------
(I tried to get them all...)

* The left and right arrow keys change the azimuth of the view angle
* The up and down arrow keys change the elevation of the view angle 
* 'M' or 'm' toggles the viewing mode. Default is orthogonal
* 'L' or 'l' toggles whether or not light is enabled
* 'T' or 't' toggles the motion of the light
* '<' and '>' allow the user to move the light source if the toggle is off
* '[' decreases light elevation
* ']' increases light elevation
* 'a' decreases ambience
* 'A' increases ambience
* 'd' decreases diffuse level
* 'D' increases diffuse level
* 's' decreases specular level
* 'S' increases specular level
* 'e' decreases emission level
* 'E' increases emission level
* 'n' decreases shininess level
* 'N' increases shininess level
* "0" resets the view angle to its default value
* "ESC" closes the window and terminates the program. 

Acknowledgements
----------------
I referred to mrmoo's code on opengl.org for help with my drawCylinder
function. I also made use of the sphere2 and Vertex functions from the class
examples.

I made use of the Project(), Vertex(), ball(), and idle() functions, among
other snippets of code, from class examples.

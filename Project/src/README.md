OpenPong (by Branden Olson)
===========================

Author: Branden Olson  
Email: branden.olson@colorado.edu
Source file: pong.c

Description
-----------
This program creates a rendition of a collegiate backyard beer-pong session.
There is heavy emphasis on the scenery, whereas future work will include
game play and more interactive features. My goal was to draw a realistic and
aesthetically pleasing scene, incorporating light, shadows, and textures.

Key Bindings
------------
* ESC     -> Exits the program
* 0       -> Resets the angle to default: (th, ph) = (0, 25)
* '-'     -> Decreases dimension which causes a zoom-in effect
* '+'     -> Increases dimension which causes a zoom-out effect
* Mouseclick -> Shoots a ball. (Currently this is slow and incomplete)
* Arrows change view angle

*Note:* I don't suggest actually changing the dimension size with + and -. I 
added this functionality mainly for the purposes of debugging. I will work on 
a way to "zoom-in" and "zoom-out" without actually changing the dimensions of
the program (which is used for a lot of calculations).

Logistics
---------
* I keep all of my texture images in an "images" folder. They are read from
  this folder in the code, so there is no need to configure anything.
* I created and linked a library called "OlsonLib.h", in which I plan to move
  much of the basic functions in order to decrease the size of the main file,
  pong.c. 

Interesting Things
------------------
* Each shape seen in the graphics was created from scratch by me, with the
  exception of the cubes, which I got from a class example. This includes
  functions for a torus, sphere, cylinder, and sliced cone.
* If you look closely, the cups are more complicated than a simple sliced cone.
  They consist of about 10 different radii corresponding to 10 heights. I
  tried to make them look like an actual Red Solo Cup in form.
* I incorporated more than one layer of shadow: On the table and on the ground.

Work To Be Done
---------------
* I plan on drawing a door and window(s) on the house for a more realistic
  effect.
* I plan on assigning materials to objects for a better appearance.
* I want to draw an extension chord from the lamp to the house, using a
  parametric equation.
* I want to add keg hoses.
* I need to clean up the aesthetics of the head of the lamp.
* If I have time, I plan on trying to project a shadow onto the house and 
  possibly the fence (although the latter idea seems quite tricky).

Acknowledgements
----------------
I referred to mrmoo's code on opengl.org for help with my drawCylinder
function. 
 
I made use of the CSCIx229.h library and snagged some code from some
class examples. I also used loadtexbmp.c, created by Dr. Schreuder.


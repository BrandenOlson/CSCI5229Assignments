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
  pong.c. (I actually didn't get around to moving all the code I wanted into
  this library. At the moment it's slightly superfluous. But not harmful. 

Interesting Things
------------------
* Each shape seen in the graphics was created from scratch by me, with the
  exception of the cubes, which I got from a class example. This includes
  functions for a torus, sphere, cylinder, and sliced cone, among others.
* If you look closely, the cups are more complicated than a simple sliced cone.
  They consist of about 10 different radii corresponding to 10 heights. I
  tried to make them look like an actual Red Solo Cup in form.
* I incorporated more than one layer of shadow: On the table and on the ground.
  This consisted of making the items on the table part of a second "scene",
  and doing a separate projection of this table scene onto the table. The table
  scene is still part of the scene as a whole so that the table items can still
  cast shadows onto the ground.
* The beer kegs are a conglomeration of various surfaces and shapes. They are
  a nice example of how you can use fairly basic building blocks to build
  things that look decently realistic.
* The power cord looks convincing as I created a piecewise parametrization and
  ensured that the ends lined up. The result is a nice-looking cord that
  follows a realistic, curvy pattern and plugs into an outlet on the wall.
* I was able to incorporate the light into the scene without too much oddity.
  To do this, I had to create a separate conic shape with the normals assuming
  that the light came from within the cone. This is why the lamp has a "glow"
  effect. This was the best I could come up with in terms of dealing with a
  point light as an actual object.
* The sky is a spherical object with a spherical texture of the milky way.
* I did my best to assign material properties to the objects. You can tell that
  the kegs are "metal", for example.

Acknowledgements
----------------
I referred to mrmoo's code on opengl.org for help with my drawCylinder
function. 
 
I made use of the CSCIx229.h library and snagged some code from some
class examples. I also used loadtexbmp.c, created by Dr. Schreuder.

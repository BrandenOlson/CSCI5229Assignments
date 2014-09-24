/*
 *  hw3.c
 *  Author: Branden Olson
 *  Email: Branden.Olson@colorado.edu
 *
 *  Key bindings:
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//  Globals
int theta = 0;       // Azimuth of view angle
int phi = 0;         // Elevation of view angle
int mode = 0;        // Dimension (1-4)
double z = 0;        // Z variable
double w = 1;        // W variable
double dim = 60;     // Dimension of orthogonal box

const int NUM_POINTS = 50000;
// These are the default parameters. They can be changed at runtime
double s = 10;
double b = 2.6666;
double r = 28;

const int S_MODE = 1;
const int B_MODE = 2;
const int R_MODE = 3;

char* text[] = {"", "S", "B", "R"};

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  // Maximum length of text string
void Print(const char* format, ...)
{
   char buf[LEN];
   char* ch = buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args, format);
   vsnprintf(buf, LEN, format, args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *ch++);
}

static void triangle(double x1, double y1, double z1,
                     double x2, double y2, double z2,
                     double x3, double y3, double z3)
{
   glPushMatrix();
   glBegin(GL_POLYGON);
   glVertex3d(x1, y1, z1);
   glVertex3d(x2, y2, z2);
   glVertex3d(x3, y3, z3);
   glEnd();
   glPopMatrix(); 
}
 
static void square(double x1, double z1, double x2, double z2, 
                   double x3, double z3, double x4, double z4)
{
   glPushMatrix();
   glBegin(GL_POLYGON);
   glVertex3d(x1, 0, z1);
   glVertex3d(x2, 0, z2);
   glVertex3d(x3, 0, z3);
   glVertex3d(x4, 0, z4);
   glEnd();
   glPopMatrix();
}

// Invariant: Pyramids can only have their base at y = 0 for simplicity
static void pyramid(double x1, double z1, double x2, double z2,
                    double x3, double z3, double x4, double z4,
                    double height )
{
   glPushMatrix();
   double xtop = sqrt(x1*x1 - x3*x3);
   double ztop = sqrt(z1*z1 - z3*z3);
   glColor3f(0, 0, 1);
   triangle(x1, 0, z1, xtop, height, ztop,
            x2, 0, z2);
   glColor3f(1, 1, 0);
   triangle(x2, 0, z2, xtop, height, ztop,
            x3, 0, z3);
   glColor3f(1, 0, 0);
   triangle(x3, 0, z3, xtop, height, ztop,
            x4, 0, z4);
   glColor3f(0, 1, 0);
   triangle(x4, 0, z4, xtop, height, ztop,
            x1, 0, z1);
   glColor3f(1, 0, 1);
   square(x1, z1, x2, z2, x3, z3, x4, z4);
   glPopMatrix();
}

void display()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   // Reset previous transforms
   glLoadIdentity();
   // Set view angle
   glRotated(phi, 1, 0, 0);
   glRotated(theta, 0, 1, 0);

   // Draw stuff
   double x1 = 20;
   double height = 20;
   double z1 = 20;
   pyramid(x1, 0, 0, z1, -x1, 0, 0, -z1, height); 


   //  Draw axes in white
   glColor3f(1, 1, 1);

   double AXIS_LENGTH = dim/1.2; 
   glBegin(GL_LINES);
   glVertex3d(0, 0, 0);
   glVertex3d(AXIS_LENGTH, 0, 0);
   glVertex3d(0, 0, 0);
   glVertex3d(0, AXIS_LENGTH, 0);
   glVertex3d(0, 0, 0);
   glVertex3d(0, 0, AXIS_LENGTH);
   glEnd();
   //  Label axes
   glRasterPos3d(AXIS_LENGTH, 0, 0);
   Print("X");
   glRasterPos3d(0, AXIS_LENGTH, 0);
   Print("Y");
   glRasterPos3d(0, 0, AXIS_LENGTH);
   Print("Z");
   //  Display parameters
   glWindowPos2i(5, 5);
   Print("View Angle=%d, %d, ", theta, phi);
   if( mode > 0)
   {
      Print("Mode = %s, ", text[mode]);
   }
   Print("s = %.1f, b = %.1f, r = %.1f", s, b, r);
   glFlush();
   glutSwapBuffers();
}

void key(unsigned char ch, int x, int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   else if (ch == '0')
   {
      //  Reset view angle and default paramter values for the attractor
      theta = phi = 0;
      s = 10;
      b = 2.6666;
      r = 28;
   }
   else if (ch == 's' || ch == 'S')
   {
      mode = S_MODE;
   }
   else if (ch == 'b' || ch == 'B')
   {
      mode = B_MODE;
   }
   else if (ch == 'r' || ch == 'R')
   {
      mode = R_MODE;
   }

   else if (ch == '+')
   {
      // Increase the paramter corresponding to the current mode
      if ( S_MODE == mode )
      {
         s += 0.1; 
      }
      else if ( B_MODE == mode )
      {
         b += 0.1111; 
      }
      else if ( R_MODE == mode )
      {
         r += 0.1; 
      }
   }
   else if ('-' == ch)
   {
      // Decrease the paramter corresponding to the current mode
      if ( S_MODE == mode )
      {
         s -= 0.1; 
      }
      else if ( B_MODE == mode )
      {
         b -= 0.1111; 
      }
      else if ( R_MODE == mode )
      {
         r -= 0.1; 
      }
   }

   glutPostRedisplay();
}

void special(int key, int x, int y)
{
   if (key == GLUT_KEY_RIGHT)
      theta += 5;
   else if (key == GLUT_KEY_LEFT)
      theta -= 5;
   else if (key == GLUT_KEY_UP)
      phi += 5;
   else if (key == GLUT_KEY_DOWN)
      phi -= 5;
   //  Keep angles to +/-360 degrees
   theta %= 360;
   phi %= 360;
   glutPostRedisplay();
}

void reshape(int width, int height)
{
   //  Ratio of the width to the height of the window
   double w2h = (height > 0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0, 0, width, height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection box adjusted for the
   //  aspect ratio of the window
   glOrtho(-dim*w2h, +dim*w2h, -dim, +dim, -dim, +dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

int main(int argc,char* argv[])
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize(500,500);
   glutCreateWindow("Visualizing the Lorenz Attractor");
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutMainLoop();
   return 0;
}

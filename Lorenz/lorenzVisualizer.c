/*
 *  lorenzVisualizer.c
 *  Author: Branden Olson
 *  Email: Branden.Olson@colorado.edu
 *
 *  Key bindings:
 *  1      2D coordinates
 *  2      2D coordinates with fixed Z value
 *  3      3D coordinates
 *  4      4D coordinates
 *  +/-    Increase/decrease z or w
 *  arrows Change view angle
 *  0      Reset view angle
 *  ESC    Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//  Globals
int th=0;       // Azimuth of view angle
int ph=0;       // Elevation of view angle
int mode=0;     // Dimension (1-4)
double z=0;     // Z variable
double w=1;     // W variable
double dim=60;   // Dimension of orthogonal box

const int NUM_POINTS = 50000;
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
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Display the scene
 */
void display()
{
   double AXIS_LENGTH = dim/1.2; 
   // Clear image
   glClear(GL_COLOR_BUFFER_BIT);
   // Reset previous transforms
   glLoadIdentity();
   // Set view angle
   glRotated(ph, 1, 0, 0);
   glRotated(th, 0, 1, 0);
   //Draw Lorenz attractor
   glColor3f(1,1,0);
   glPointSize(0.5);
   glBegin(GL_LINE_STRIP);
   glColor3f(0,0,0);
   // Set the initial vertex to (1, 1, 1)
   double x = 1;
   double y = 1;
   double z = 1;
   double dt = 0.001;
   double color = 0.0;
   int step = 0;
   int up = 1;
   // Go through the algorithm to generate all desired Lorenz points/lines
   for(int i = 1; i < NUM_POINTS; i++)
   {
      if( !up )
      {
         glColor3f(1, 1 - color, 0);
      }
      else
      {
         glColor3f(1, color, 0);
      }
      double dx = s*(y-x);
      double dy = x*(r - z) - y;
      double dz = x*y - b*z;
      x += dt*dx;
      y += dt*dy;
      z += dt*dz;
      glVertex3d(x, y, z);
      step++;
      if( step > 5 )
      {
         color += 0.01;
         step = 0;
      }
      if ( color > 1.0 ) 
      {
         color = 0.0;
         up = !up;
      }
   }
   glColor3f(1,1,1);
   glEnd();
   //  Draw axes in white
   glColor3f(1,1,1);
   glBegin(GL_LINES);
   glVertex3d(0,0,0);
   glVertex3d(AXIS_LENGTH,0,0);
   glVertex3d(0,0,0);
   glVertex3d(0, AXIS_LENGTH, 0);
   glVertex3d(0,0,0);
   glVertex3d(0,0,AXIS_LENGTH);
   glEnd();
   //  Label axes
   glRasterPos3d(AXIS_LENGTH,0,0);
   Print("X");
   glRasterPos3d(0,AXIS_LENGTH,0);
   Print("Y");
   glRasterPos3d(0,0,AXIS_LENGTH);
   Print("Z");
   //  Display parameters
   glWindowPos2i(5,5);
   Print("View Angle=%d, %d, ", th, ph);
   if( mode > 0)
   {
      Print("Mode = %s, ", text[mode]);
   }
   Print("s = %.1f, b = %.1f, r = %.1f", s, b, r);
   //  Flush and swap
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle and default paramter values for the attractor
   else if (ch == '0')
   {
      th = ph = 0;
      s = 10;
      b = 2.6666;
      r = 28;
   }
   // Allow the paramter s to vary
   else if (ch == 's' || ch == 'S')
   {
      mode = S_MODE;
   }
   // Allow the paramter b to vary 
   else if (ch == 'b' || ch == 'B')
   {
      mode = B_MODE;
   }
   // Allow the paramter r to vary
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

   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase azimuth by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease azimuth by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection box adjusted for the
   //  aspect ratio of the window
   glOrtho(-dim*w2h,+dim*w2h, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
  //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window 
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   //  Request 500 x 500 pixel window
   glutInitWindowSize(500,500);
   //  Create the window
   glutCreateWindow("Visualizing the Lorenz Attractor");
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
  //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   //  Return code
   return 0;
}

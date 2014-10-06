/*
 *  hw4.c
 *  Author: Branden Olson
 *  Email: Branden.Olson@colorado.edu
 *
 *  Key bindings:
 *  left/right change the azimuth of view angle
 *  up/down change the elevation of the view angle
 *  m/M toggles the viewing mode
 *  0 resets the angle to the default
 *  ESC closes the window and terminates the program
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

#define PI 3.1415927
#define Cos(x) (cos((x)*PI/180))
#define Sin(x) (sin((x)*PI/180))

//  Globals
int theta = 0;       // Azimuth of view angle
int phi = 0;         // Elevation of view angle
int mode = 0;        // Projection mode (0 for orthogonal, 1 for perspective) 
double z = 0;        // Z variable
double w = 1;        // W variable
double dim = 60;     // Dimension of orthogonal box
int fov = 60;       //  Field of view (for perspective)
double asp = 1;     //  Aspect ratio
// Light values
int one       =   1;  // Unit value
int distance  =   60;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shinyvec[1];    // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light
const int light = 1; // Always have light
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

static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
      gluPerspective(fov, asp, dim/4, 4*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim, +asp*dim, -dim, +dim, -dim, +dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

static void drawTriangle(double x1, double y1, double z1,
                     double x2, double y2, double z2,
                     double x3, double y3, double z3)
{
   glPushMatrix();
   glBegin(GL_POLYGON);
   double nx = (y2 - y1)*(z3 - z1) - (z2 - z1)*(y3 - y1);
   double ny = (z2 - z1)*(x3 - x1) - (x2 - x1)*(z3 - z1);
   double nz = (x2 - x1)*(y3 - y1) - (y2 - y1)*(x3 - x1); 
   glNormal3f(nx, ny, nz);
   glVertex3d(x1, y1, z1);
   glVertex3d(x2, y2, z2);
   glVertex3d(x3, y3, z3);
   glEnd();
   glPopMatrix(); 
}
 
static void drawSquare(double x1, double z1, double x2, double z2, 
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
static void drawPyramid(double x, double z, double height, double angle,
                    double xscale, double yscale, double zscale)
{
   glPushMatrix();

   glTranslated(x, 0, z);
   glRotated(angle, 0, 1, 0);
   glScaled(xscale, 1, zscale);

   glColor3f(0, 0, 1);
   drawTriangle(1, 0, 0, /**/ 0, height, 0, /**/ 0, 0, 1);
   glColor3f(1, 1, 0);
   drawTriangle(0, 0, 1, /**/ 0, height, 0, /**/ -1, 0, 0);
   glColor3f(1, 0, 0);
   drawTriangle(-1, 0, 0, /**/ 0, height, 0, /**/ 0, 0, -1);
   glColor3f(0, 1, 0);
   drawTriangle(0, 0, -1, /**/ 0, height, 0, /**/ 1, 0, 0);
   glColor3f(1, 0, 1);
   drawSquare(1, 0, 0, 1, -1, 0, 0, -1);
   glPopMatrix();
}

// Grabbed this from example from class
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

static void ball(double x,double y,double z,double r)
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

// Grabbed this from example from class
static void drawSphere(double x, double y, double z, double r)
{
   const int d = 5;
   int th, ph = 0;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x, y, z);
   glScaled(r, r, r);

   //  Latitude bands
   for (ph = -90; ph < 90; ph += d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0; th<=360; th+=d)
      {
         Vertex(th, ph);
         Vertex(th, ph+d);
      }
      glEnd();
   }

   //  Undo transformations
   glPopMatrix();
}

// Thanks to mrmoo on opengl.org for help with this
static void drawCylinder(double r, double h, double x, double y, double z, 
                     double rotation)
{
   const int SIDE_COUNT = 100;
   glPushMatrix();

   glTranslated(x, y, z);
   if( rotation )
   {
      // Draw cylinder sideways for cactus branches
      glRotated(rotation, 1, 0, 0);
   } 

   glBegin(GL_QUAD_STRIP); 
   for (int i = 0; i <= SIDE_COUNT; i++) {     
       float angle = i*((1.0/SIDE_COUNT) * (2*PI));
       glNormal3d( cos(angle), 0, sin(angle) );
       glVertex3d( r*cos(angle), h, r*sin(angle) );
       glVertex3d( r*cos(angle), 0, r*sin(angle) );   }
   glEnd();

   glPopMatrix();
}

static void drawCactus(double r, double h, double x, double z, double th)
{
   glPushMatrix();

   glTranslated(x, 0, z);
   glRotated(th, 0, 1, 0);

   drawCylinder(r, h, 0, 0, 0, 0);
   drawSphere(0, h, 0, r);
   drawCylinder(r, h/3, 0, h/3, 0, 90);
   drawSphere(0, h/3, 0 + h/3, r);
   drawCylinder(r, h/3, 0, h/3, 0 + h/3, 0);
   drawSphere(0, h/3 + h/3, 0 + h/3, r);

   drawCylinder(r, h/3, 0, 3*h/5, 0, 270); 
   drawSphere(0, 3*h/5, 0 - h/3, r);
   drawCylinder(r, h/4, 0, 3*h/5, 0 -h/3, 0);
   drawSphere(0, h/4 + 3*h/5, 0 - h/3, r);

   glPopMatrix();
}

void display()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   // Reset previous transforms
   glLoadIdentity();
   if (mode)
   {
      double Ex = -2*dim*Sin(theta)*Cos(phi);
      double Ey = +2*dim*Sin(phi);
      double Ez = +2*dim*Cos(theta)*Cos(phi);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(phi),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(phi, 1, 0, 0);
      glRotatef(theta, 0, 1, 0);
   }

   //  Light switch
   if (light)
   {
        //  Translate intensity to color vectors
        float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
        float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
        float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
        //  Light position
        float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
        //  Draw light position as ball (still no lighting here)
        glColor3f(1,1,1);
        ball(Position[0],Position[1],Position[2] , 1.0);
        //  OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        //  Enable lighting
        glEnable(GL_LIGHTING);
        //  Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
        //  glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        //  Enable light 0
        glEnable(GL_LIGHT0);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else
     glDisable(GL_LIGHTING);

   // Draw stuff
   drawPyramid(0, 0, 30, 0, 20, 0, 20); 
   drawPyramid(30, 40, 20, 45, 15, 0, 15); 
   drawPyramid(-20, -30, 10, 60, 10, 1, 10);
   drawPyramid(-40, 0, 15, 80, 5, 1, 5);

   double cactusHeight = 20;
   double cactusRadius = 2; 

   glColor3d(0, 0.5, 0);
   
   drawCactus(cactusRadius/1.3, cactusHeight/1.3, -20, 35, 75);
   drawCactus(cactusRadius/2, cactusHeight/2, 40, -30, 120);
   drawCactus(cactusRadius/1.5, cactusHeight/1.5, 20, -35, 200);
   drawCactus(cactusRadius/2, cactusHeight/2, -40, -40, 120);
   drawCactus(cactusRadius/2, cactusHeight/2, 30, 0, 20);

   //  Draw axes in white
   glDisable(GL_LIGHTING);
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
   Print("View Angle: %d, %d; ", theta, phi);
   Print("Mode: %s", mode ? "Perspective" : "Orthogonal");
   glFlush();
   glutSwapBuffers();
}

void key(unsigned char ch, int x, int y)
{
   //  Exit on ESC
   if (27 == ch)
      exit(0);
   else if ('0' == ch)
   {
      //  Reset view angle and default paramter values for the attractor
      theta = phi = 0;
   }
   else if ('m' == ch || 'M' == ch)
   {
      mode = !mode;
   }
   Project();
   glutPostRedisplay();
}

void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   //  Tell GLUT it is necessary to redisplay the scene
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
   Project();
   glutPostRedisplay();
}

void reshape(int width, int height)
{
   //  Ratio of the width to the height of the window
   asp = (height > 0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0, 0, width, height);
   //  Tell OpenGL we want to manipulate the projection matrix
   Project();
}

int main(int argc,char* argv[])
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize(500,500);
   glutCreateWindow("Ancient Egypt in Technicolor, II (by Branden Olson)");
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   glutMainLoop();
   return 0;
}

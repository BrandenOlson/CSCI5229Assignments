/* HW7: Textures
   by Branden Olson
*/


#define PI 3.1415927
#include "CSCIx229.h"

int axes = 0;       //  Display axes
int th = 0;         //  Azimuth of view angle
int ph = 0;         //  Elevation of view angle
int zh= 0;         //  Azimuth of light
int fov = 55;       //  Field of view (for perspective)
double asp = 1;     //  Aspect ratio
double dim = 3.0;   //  Size of world
int light = 1;    //  Lighting
unsigned int canside, cantop, red;  //  Textures

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius r
 */
static void drawSphere(double x, double y, double z, double r)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1, 1, 1);
   glutSolidSphere(1.0, 16, 16);
   //  Undo transofrmations
   glPopMatrix();
}

static void drawCan(double r, double h, double x, double y, double z)
{
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   const int SIDE_COUNT = 100;
   glPushMatrix();

   glTranslated(x, y, z);
   glScaled(r, h, r);

   glBindTexture(GL_TEXTURE_2D, canside);
   glColor3f(1, 1, 1);
   glBegin(GL_QUAD_STRIP); 
   int i = 0;
   int k = 0;
   for (; i <= SIDE_COUNT; i++) {     
       float angle = i*((1.0/SIDE_COUNT) * (2*PI));
       glTexCoord2f(1 - 2*(float)i/SIDE_COUNT, 1);
       glNormal3d( cos(angle), 0, sin(angle) );
       glVertex3d( 1*cos(angle), 1, 1*sin(angle) );
       glTexCoord2f(1 - 2*(float)i/SIDE_COUNT, 0);
       glVertex3d( 1*cos(angle), 0, 1*sin(angle) );   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D, cantop);
   glColor3f(1, 1, 1);
   glBegin(GL_TRIANGLE_FAN);
   glTexCoord2f(0.5, 0.5);
   glNormal3f(0, 1, 0);
   glVertex3f(0, 1, 0);
   for (k = 0; k <= 360; k += 10)
   {
      glTexCoord2f(0.5*Cos(k) + 0.5, 0.5*Sin(k) + 0.5);
      glNormal3f(0, 1, 0);
      glVertex3f(Cos(k), 1, Sin(k));
   }
   glEnd();
   glDisable(GL_TEXTURE_2D);

   glColor3f(0.7, 0.7, 0.7);
   glNormal3f(0, -1, 0);
   glBegin(GL_TRIANGLE_FAN);
   glVertex3f(0, 0, 0);
   int q;
   for(q = 0; q <= 360; q += 10)
   {
      glVertex3f(Cos(q), 0, Sin(q));
   }
   glEnd();
   

   glPopMatrix();
}

/* Draw a torus */
void drawTorus(double xVal, double yVal, double zVal, double rbig,
   double rsmall)
{
   glPushMatrix();

   glTranslated(xVal, yVal/1.95, zVal);
   glScaled(rbig, 1, rbig);
   glRotated(90, 1, 0, 0);

   int i, j, k;
   double s, t, x, y, z, twopi;
   const int numc = 100;
   const int numt = 100;

   twopi = 2 * (double)M_PI;
   for (i = 0; i < numc; i++) {
      glBegin(GL_QUAD_STRIP);
      for (j = 0; j <= numt; j++) {
         for (k = 1; k >= 0; k--) {
            s = (i + k) % numc + 0.5;
            t = j % numt;

            x = (1+rsmall*cos(s*twopi/numc))*cos(t*twopi/numt);
            y = (1+rsmall*cos(s*twopi/numc))*sin(t*twopi/numt);
            z = rsmall * sin(s * twopi / numc);
            glNormal3f(x, y, z);
            glVertex3f(x, y, z);
         }
      }
      glEnd();
   }

   glPopMatrix();
}

// radius = radius of the top of cup. The bottom's radius is computed from the
// top's radius
static void drawCup(double radius, double height,  double x, double y,                     double z)
{
   glColor3f(1, 1, 1);
   drawTorus(x, height, z, radius, radius/15);
   glPushMatrix();

   glTranslated(x, y, z);
   glScaled(radius, height, radius);

   const double R1 = 1.0;
   const double R2 = 0.97; 
   const double R3 = 0.94; 
   const double R4 = 0.91;
   const double R5 = 0.88;
   const double R6 = 0.85;
   const double R7 = 0.82;
   const double R8 = 0.76;
   const double R9 = 0.73;
   const double R10 = 0.70;
   const double BOTTOM_RATIO = R10 + 0.005;
   double ratio_array[] = {R1, R2, R3, R4, R5, R6, R7, R8, R9, R10};
   double height_array[] = {1, 0.9, 0.895, 0.8, 0.795, 0.70, 0.695, 
                            0.30, 0.295, 0.0}; 
   const int RATIO_ARRAY_SIZE = 10;
   const int SIDE_COUNT = 100;
   const double RIM_RADIUS = 1 - radius/15;

   // Draw interior of cup
   int k = 0;
   for(; k < RATIO_ARRAY_SIZE - 1; k++) {
      glBegin(GL_QUAD_STRIP);
      int m = 0;
      for (; m <= SIDE_COUNT; m++) {     
          float angle = m*((1.0/SIDE_COUNT) * (2*PI));
          glNormal3d( -cos(angle), 0, -sin(angle) );
          glColor3f(1, 1, 1);
          glVertex3d( RIM_RADIUS*ratio_array[k]*cos(angle), 
                      height_array[k], 
                      RIM_RADIUS*ratio_array[k]*sin(angle) );
          double depth = (k == RATIO_ARRAY_SIZE - 2) ? 0.05 : 
                               height_array[k+1];
          glVertex3d( RIM_RADIUS*ratio_array[k+1]*cos(angle), 
                      depth, 
                      RIM_RADIUS*ratio_array[k+1]*sin(angle) );   
      }
      glEnd();
   }

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   // Draw exterior of cup
   int j = 0;
   for(; j < RATIO_ARRAY_SIZE - 1; j++) {
      glBindTexture(GL_TEXTURE_2D, red);
      glColor3f(1, 0, 0);
      glBegin(GL_QUAD_STRIP);
      int i = 0;
      for (; i <= SIDE_COUNT; i++) {     
          float angle = i*((1.0/SIDE_COUNT) * (2*PI));
          glTexCoord2f(1-2*(float)i/SIDE_COUNT, 1);
          glNormal3d( cos(angle), 0, sin(angle) );
          glVertex3d( ratio_array[j]*cos(angle), height_array[j], 
                      ratio_array[j]*sin(angle) );
          glTexCoord2f(1-2*(float)i/SIDE_COUNT, 0);
          glVertex3d( ratio_array[j+1]*cos(angle), height_array[j+1], 
                      ratio_array[j+1]*sin(angle) );   
      }
      glEnd();
   }
   glDisable(GL_TEXTURE_2D);

   // Draw bottom of cup
   int p;
   glColor3f(1, 1, 1);
   for(p = 1; p >= -1; p -= 2)
   {
      double depth = 0;
      if(p > 0)
      {
         glColor3f(1, 1, 1);
         depth = 0.05; 
      }
      else
      {
         glColor3f(1, 0, 0);
      }
      glNormal3f(0, p, 0);
      glBegin(GL_TRIANGLE_FAN);
      glVertex3f(0, 0, 0);
      int q;
      for(q = 0; q <= 360; q += 10)
      {
         glVertex3f(BOTTOM_RATIO*p*Cos(q), 
                    depth, BOTTOM_RATIO*p*Sin(q));
      }
      glEnd();
   }

   glPopMatrix();
}


/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len = 2.5;  //  Length of axes
   double Ex = -2*dim*Sin(th)*Cos(ph);
   double Ey = +2*dim        *Sin(ph);
   double Ez = +2*dim*Cos(th)*Cos(ph);
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //glEnable(GL_CULL_FACE);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(ph), 0);
   //  Light switch
   if (light)
   {
      //  Translate intensity to color vectors
      float Ambient[] = {0.3, 0.3, 0.3, 1.0};
      float Diffuse[] = {1, 1, 1, 1};
      float Specular[] = {1, 1, 0, 1};
      float white[] = {1, 1, 1, 1};
      //  Light direction
      float Position[] = {5*Cos(zh), 1, 5*Sin(zh), 1};
      //  Draw light position as ball (still no lighting here)
      drawSphere(Position[0], Position[1], Position[2], 0.1);
      //  Enable lighting with normalization
      glEnable(GL_LIGHTING);
      glEnable(GL_NORMALIZE);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
      glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
      glLightfv(GL_LIGHT0, GL_POSITION, Position);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
   }
   else
      glDisable(GL_LIGHTING);

   drawCan(0.5, 2, 0, -1, 1);
   drawCup(0.7, 2.1, 2, -1, -1);
   drawCup(0.7, 2.1, 0, -1, -2);
   drawCup(0.7, 2.1, -2, -1, -1);

   //  Draw axes
   glDisable(GL_LIGHTING);
   glColor3f(1, 1, 1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3d(len, 0.0, 0.0);
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3d(0.0, len, 0.0);
      glVertex3d(0.0, 0.0, 0.0);
      glVertex3d(0.0, 0.0, len);
      glEnd();
      //  Label axes
      glRasterPos3d(len, 0.0, 0.0);
      Print("X");
      glRasterPos3d(0.0, len, 0.0);
      Print("Y");
      glRasterPos3d(0.0, 0.0, len);
      Print("Z");
   }
   //  Display parameters
   glWindowPos2i(5, 5);
   Print("Angle=%d,%d  Dim=%.1f Light=%s", th, ph, dim, light? "On" :
         "Off");
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key, int x, int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
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
   //  Update projection
   Project(fov, asp, dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch, int x, int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Toggle axes
   else if (ch == 'a' || ch == 'A')
      axes = 1 - axes;
   //  Toggle light
   else if (ch == 'l' || ch == 'L')
      light = 1 - light;
   else if (ch == '-')
      dim += 0.1;
   else if (ch == '+')
      dim -= 0.1;
   //  Reproject
   Project(fov, asp, dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width, int height)
{
   //  Ratio of the width to the height of the window
   asp = (height > 0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0, 0, width, height);
   //  Set projection
   Project(fov, asp, dim);
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t, 360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc, char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc, argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600, 600);
   glutCreateWindow("Party Time! (By Branden Olson)");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   //  Load textures
   red = LoadTexBMP("red.bmp");
   canside = LoadTexBMP("can2.bmp");
   cantop = LoadTexBMP("beercan.bmp");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}

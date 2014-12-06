/* OpenPong 
   by Branden Olson
*/

#define PI 3.1415927
#include "CSCIx229.h"
#include "OlsonLib.h"
#include "stdio.h"

int axes = 0;       //  Display axes
int th = 0;         //  Azimuth of view angle
int ph = 25;         //  Elevation of view angle
int zh= 0;         //  Azimuth of light
double ylight  =  8;  // Elevation of light
#define LIGHT_RADIUS 10
int fov = 55;       //  Field of view (for perspective)
double asp = 1;     //  Aspect ratio
double dim = 10.0;   //  Size of world
int light = 1;    //  Lighting
int mode = 4;
// Texture allocation
unsigned int brick, canside, cantop, red, shingle, wood, door, grass, silver, sky;

// Light values
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 0;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shinyvec[1];    // Shininess (value)

const double Z0 = -6;

#define Y_GROUND -12
#define GROUND_WIDTH 5*dim
#define GROUND_LENGTH 5*dim
#define CUP_RADIUS 0.7 
#define BALL_RADIUS CUP_RADIUS/2
#define R  (CUP_RADIUS + CUP_RADIUS/15/2)
#define CUP_HEIGHT 2.1
#define GRAVITY -0.5
#define TABLE_WIDTH 7*CUP_RADIUS
#define TABLE_LENGTH abs(Z0 - 4.5*R*sqrt(3))
#define MODE 6
#define FENCE_X 5*dim
#define FENCE_Z 5*dim
#define FENCE_Y 10 
#define WALL_HEIGHT (Y_GROUND + 20)
#define WALL_WIDTH FENCE_X

char* text[]={"No shadows",
              "Draw flattened scene",
              "Flattened scene with lighting disabled",
              "Blended shadows with Z-buffer read-only",
              "Blended shadows with stencil buffer",
             };

float N[] = {0, -1, 0}; // Normal vector for the plane
float E[] = {0, Y_GROUND, 0 }; // Point of the ground plane
float E2[] = {0, -1, 0}; // Point of the table plane

typedef struct ball {
   float x; float y; float z; // Positions
   float vx; float vy; float vz; // Velocities for actual motion
   float vxp; float vyp; float vzp; // Potential velocities given by user
   int active;
} ball;

ball pBall;

materialStruct metal =
{
   {0.33, 0.22, 0.03, 1.0},
   {0.78, 0.57, 0.11, 1.0},
   {0.99, 0.91, 0.81, 1.0},
   27.8
};

void makeShadowProjection(float L[4], float E[4], float N[4])
{
   float mat[16];
   float e = E[0]*N[0] + E[1]*N[1] + E[2]*N[2];
   float l = L[0]*N[0] + L[1]*N[1] + L[2]*N[2];
   float c = e - l;
   //  Create the matrix.
   mat[0] = N[0]*L[0] + c;
   mat[4] = N[1]*L[0];
   mat[8] = N[2]*L[0];
   mat[12] = -e*L[0];
   mat[1] = N[0]*L[1];
   mat[5] = N[1]*L[1] + c;
   mat[9] = N[2]*L[1];
   mat[13] = -e*L[1];
   mat[2] = N[0]*L[2];
   mat[6] = N[1]*L[2];
   mat[10] = N[2]*L[2] + c; 
   mat[14] = -e*L[2];
   mat[3] = N[0];
   mat[7] = N[1];
   mat[11] = N[2];
   mat[15] = -l;
   //  Multiply modelview matrix
   glMultMatrixf(mat);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius r
 */

void Vertex(double th,double ph)
{
   glTexCoord2d(ph/360.0, th/180.0 + 0.5);
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}

void skyVertex(double th,double ph)
{
   glTexCoord2d(th/180.0, ph/190.0 + 0.5); // Divide ph by 190 instead of 180 
                                           // to compensate for a weird strip 
                                           // without texture
   glVertex3d(Sin(ph)*Cos(th), Sin(th) , Cos(ph)*Cos(th));
}

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

static void drawSky(double x, double y, double z, double r)
{
   const int d = 5;
   int th, ph = 0;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x, y, z);
   glScaled(r, r, r);

   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, sky);

   //  Latitude bands. Start at 0 for a hemisphere (no need for full sphere)
   for (ph = -90; ph < 90; ph += d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0; th<=360; th+=d)
      {
         skyVertex(th, ph);
         skyVertex(th, ph+d);
      }
      glEnd();
   }
   glDisable(GL_TEXTURE_2D);

   //  Undo transformations
   glPopMatrix();
}

static void drawCan(double r, double h, double x, double y, double z)
{
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   materials(&metal);
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

// Thanks to mrmoo on opengl.org for help with this
static void drawCylinder(double r, double h, double x, double y, double z, 
                         unsigned int texture, int rotation)
{
   const int SIDE_COUNT = 100;
   glPushMatrix();

   glTranslated(x, y, z);
   glRotated(rotation, 1, 1, 0); 
   glScaled(r, h, r);

   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, texture);
   glColor3f(1, 1, 1);
   glBegin(GL_QUAD_STRIP); 
   int i = 0;
   for (; i <= SIDE_COUNT; i++) {     
       float angle = i*((1.0/SIDE_COUNT) * (2*PI));
       glTexCoord2f(1 - 2*(float)i/SIDE_COUNT, 1);
       glNormal3d( cos(angle), 0, sin(angle) );
       glVertex3d( 1*cos(angle), 1, 1*sin(angle) );
       glTexCoord2f(1 - 2*(float)i/SIDE_COUNT, 0);
       glVertex3d( 1*cos(angle), 0, 1*sin(angle) );   }
   glEnd();
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

void drawTorus(double xCenter, double height, double zCenter,
                    double rSmall, double rBig)
{
   glPushMatrix();
   glTranslated(xCenter, height, zCenter);
   double x1, x2, y, z1, z2;
   int theta = 0;
   int phi;
   int delta = 5;
   for(; theta <= 360 - delta; theta += delta)
   {
      glBegin(GL_QUAD_STRIP);
      phi = 0;
      for(; phi <= 360; phi += 2*delta)
      {
         x1 = (rBig+ rSmall*Cos(phi))*Cos(theta);
         y = rSmall*Sin(phi);
         z1 = (rBig + rSmall*Cos(phi))*Sin(theta);
         glNormal3f(Cos(phi)*Cos(theta), Sin(phi), Cos(phi)*Sin(theta));
         glVertex3d(x1, y, z1);
         x2 = (rBig + rSmall*Cos(phi))*Cos(theta + delta);
         z2 = (rBig + rSmall*Cos(phi))*Sin(theta + delta);
         glNormal3f(Cos(phi)*Cos(theta + delta), Sin(phi), Cos(phi)*Sin(theta + delta));
         glVertex3d(x2, y, z2);
      }
      glEnd();
   }
   glPopMatrix();
}


// radius = radius of the top of cup 
// The bottom's radius is computed from the top's radius
static void drawCup(double radius, double height,  double x, double y,                     double z)
{
   glColor3f(1, 1, 1);
   drawTorus(x, y+height, z, radius/20, radius);
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

void drawCube(double x,double y,double z,
                 double dx,double dy,double dz, int texture)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture);
   //  Front
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
   glEnd();
   //  Back
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
   glBegin(GL_QUADS);
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   glEnd();
   //  Undo transformations and textures
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}

void drawCubeWithoutTexture(double x,double y,double z,
                 double dx,double dy,double dz)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);
   //  Front
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   glEnd();
   //  Back
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
   glBegin(GL_QUADS);
   glNormal3f( 0,-1, 0);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   glEnd();
   //  Undo transformations and textures
   glPopMatrix();
}
static void drawWall()
{

   glPushMatrix();
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1,1);
   glBindTexture(GL_TEXTURE_2D, brick);
   glNormal3f(0, 0, -1);
   float i, j;
   float delta = 0.2;
   float ydelta = 6;
   float texCoord = ydelta/10.0;
   for(i = -1; i <= 1 - delta; i += delta)
   {
      glBegin(GL_QUAD_STRIP);
      for(j = Y_GROUND; j <= WALL_HEIGHT; j += ydelta)
      {
         glTexCoord2f(0, 0);
         glVertex3f(i*WALL_WIDTH, j, FENCE_Z);
         glTexCoord2f(1, 0);
         glVertex3f((i + delta)*WALL_WIDTH, j, FENCE_Z);
         glTexCoord2f(0, texCoord);
         glVertex3f(i*WALL_WIDTH, (j + ydelta), FENCE_Z);
         glTexCoord2f(1, texCoord);
         glVertex3f((i + delta)*WALL_WIDTH, (j + ydelta), FENCE_Z);
      }
      glEnd();
   }
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_POLYGON_OFFSET_FILL);
   glPopMatrix();
   
}

void drawRoof(double ROOF_BASE, double BASE_Z)
{
   glPushMatrix();
   float ROOF_TOP = ROOF_BASE + 7;
   float OFFSET = 12.0;
   float Y_MID = (ROOF_BASE + ROOF_TOP)/2.0;
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, shingle);
   glBegin(GL_QUAD_STRIP);
   float i = 1.0;
   float delta = 0.2;
   for(; i >= -1 + delta; i -= delta)
   {
      glTexCoord2f(0, 0);
      glVertex3f(i*FENCE_X, ROOF_BASE, BASE_Z);
      glTexCoord2f(0, 1);
      glVertex3f(i*FENCE_X, Y_MID, BASE_Z + OFFSET/2);
      glTexCoord2f(1, 0);
      glVertex3f((i - delta)*FENCE_X, ROOF_BASE, BASE_Z);
      glTexCoord2f(1, 1);
      glVertex3f((i - delta)*FENCE_X, Y_MID, BASE_Z + OFFSET/2);
   }
   glEnd();
   i = 1.0;
   delta = 0.2;
   glBegin(GL_QUAD_STRIP);
   for(; i >= -1 + delta; i -= delta)
   {
      glTexCoord2f(0, 0);
      glVertex3f(i*FENCE_X, Y_MID, BASE_Z + OFFSET/2);
      glTexCoord2f(0, 1);
      glVertex3f(i*FENCE_X, ROOF_TOP, BASE_Z + OFFSET);
      glTexCoord2f(1, 0);
      glVertex3f((i - delta)*FENCE_X, Y_MID, BASE_Z + OFFSET/2);
      glTexCoord2f(1, 1);
      glVertex3f((i - delta)*FENCE_X, ROOF_TOP, BASE_Z + OFFSET);
   }
   glEnd();
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

static void drawHouse()
{
   drawWall();
   float ACCENT_WIDTH = 1;
   float ACCENT_HEIGHT = WALL_HEIGHT + 5;
   glColor3f(1, 1, 1);
   drawCubeWithoutTexture(0, ACCENT_HEIGHT, FENCE_Z, FENCE_X, ACCENT_WIDTH, 
            2);
   drawRoof(ACCENT_HEIGHT + ACCENT_WIDTH, FENCE_Z - ACCENT_WIDTH);

   // Draw door
   float DOOR_HEIGHT = ACCENT_HEIGHT - 4;
   float DOOR_WIDTH = 6;
   drawCubeWithoutTexture(0, DOOR_HEIGHT, FENCE_Z, DOOR_WIDTH, 0.5,
                          1); 
   drawCubeWithoutTexture(-DOOR_WIDTH + 0.5, (DOOR_HEIGHT + Y_GROUND)/2, 
                          FENCE_Z, 0.5, (DOOR_HEIGHT - Y_GROUND)/2, 1);
   drawCubeWithoutTexture(DOOR_WIDTH - 0.5, (DOOR_HEIGHT + Y_GROUND)/2, 
                          FENCE_Z, 0.5, (DOOR_HEIGHT - Y_GROUND)/2, 1);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, door);
   glBegin(GL_QUADS);
   // Converted bmp format was behaving strangely. Using 0.1 instead of 0 for
   // the LHS seems to work
   glTexCoord2f(0.1, 0);
   glVertex3f(DOOR_WIDTH - 1, Y_GROUND, FENCE_Z);
   glTexCoord2f(1, 0);
   glVertex3f(1 - DOOR_WIDTH, Y_GROUND, FENCE_Z);
   glTexCoord2f(1, 1);
   glVertex3f(1 - DOOR_WIDTH, DOOR_HEIGHT, FENCE_Z);
   glTexCoord2f(0.1, 1);
   glVertex3f(DOOR_WIDTH - 1, DOOR_HEIGHT, FENCE_Z);
   glEnd();
   glDisable(GL_TEXTURE_2D);
}

static void drawTable(double xCenter, double yCenter, double zCenter, 
                      double width, double height, double length)
{
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(0.5, 0.5);
   drawCube(xCenter, yCenter, zCenter, width, height, length, wood);
   glDisable(GL_POLYGON_OFFSET_FILL);
   double mid = 0.5*(Y_GROUND - 2);
   drawCube(width - 1, mid, length - 1, 0.5, mid - Y_GROUND, 0.5, wood);
   drawCube(-width + 1, mid, length - 1, 0.5, mid - Y_GROUND, 0.5, wood);
   drawCube(width - 1, mid, -length + 1, 0.5, mid - Y_GROUND, 0.5, wood);
   drawCube(-width + 1, mid, -length + 1, 0.5, mid - Y_GROUND, 0.5, wood);
}

void drawGround()
{
   glPushMatrix();
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(1,1);
   glBindTexture(GL_TEXTURE_2D, grass);
   glNormal3f(0, 1, 0);
   float i, j;
   float delta = 0.4;
   for(i = -2; i <= 2 - delta; i += delta)
   {
      glBegin(GL_QUAD_STRIP);
      for(j = -2; j <= 2 - delta; j += delta)
      {
         glTexCoord2f(0, 0);
         glVertex3f(i*GROUND_WIDTH, Y_GROUND, j*GROUND_LENGTH);
         glTexCoord2f(1, 0);
         glVertex3f((i + delta)*GROUND_WIDTH, Y_GROUND, j*GROUND_LENGTH);
         glTexCoord2f(0, 1);
         glVertex3f(i*GROUND_WIDTH, Y_GROUND, (j + delta)*GROUND_LENGTH);
         glTexCoord2f(1, 1);
         glVertex3f((i + delta)*GROUND_WIDTH, Y_GROUND, (j + delta)*GROUND_LENGTH);
      }
      glEnd();
   }
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_POLYGON_OFFSET_FILL);
   glPopMatrix();
}

void drawPost(double x, double y, double z, double dx, double dy, double dz)
{
   glPushMatrix();
   drawCube(x, y, z, dx, dy, dz, wood); 
   glPopMatrix();
}

void drawFence()
{
   glPushMatrix();

   float i;
   float Y_MID = Y_GROUND + FENCE_Y;
   float delta = 0.045;
   for(i = -1; i <= 1; i += delta)
   {
     // if(i < -0.5 || i > 0.5) 
     // {
     //    drawPost(i*FENCE_X, Y_MID, FENCE_Z, 1, FENCE_Y, 0.3);
     // }
      drawPost(i*FENCE_X, Y_MID, -FENCE_Z, 1, FENCE_Y, 0.3);
      drawPost(FENCE_X, Y_MID, i*FENCE_Z, 0.3, FENCE_Y, 1);
      drawPost(-FENCE_X, Y_MID, i*FENCE_Z, 0.3, FENCE_Y, 1);
   }
   glPopMatrix();
   drawPost(0, Y_MID + 5, -FENCE_Z + 0.5, FENCE_X, 0.5, 0.5);
   drawPost(0, Y_MID - 5, -FENCE_Z + 0.5, FENCE_X, 0.5, 0.5);
   drawPost(-FENCE_X, Y_MID + 5, 0, 0.5, 0.5, FENCE_Z);
   drawPost(-FENCE_X, Y_MID - 5, 0, 0.5, 0.5, FENCE_Z);
   drawPost(FENCE_X, Y_MID + 5, 0, 0.5, 0.5, FENCE_Z);
   drawPost(FENCE_X, Y_MID - 5, 0, 0.5, 0.5, FENCE_Z);
}

static void drawAnnulus(double rBig, double rSmall, double x, double y, 
                        double z, unsigned int texture)
{
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(rBig, 1, rBig);
   glBindTexture(GL_TEXTURE_2D, texture);
   glBegin(GL_QUAD_STRIP);
   double inner = (double)rSmall/rBig;
   glTexCoord2f(0.5, 0.5);
   int i = 0;
   for(; i <= 360; i += 10)
   {
      glNormal3d(0, 1, 0);
      glTexCoord2f(inner*Cos(i), inner*Sin(i));
      glVertex3d(inner*Cos(i), 0, inner*Sin(i));
      glNormal3d(0, 1, 0);
      glTexCoord2f(Cos(i), Sin(i));
      glVertex3d(Cos(i), 0, Sin(i));
   }
   glEnd();
   glDisable(GL_TEXTURE_2D);
   glPopMatrix(); 
}

static void drawKeg(double r, double h, double x, double y, double z)
{
   glPushMatrix();
   glColor3f(0.439, 0.439, 0.439);
   drawTorus(x, y + h/4, z, r/20, r);
   drawTorus(x, y + h/2, z, r/20, r);
   drawTorus(x, y + 3*h/4, z, r/20, r);
   glTranslated(x, y, z);
   glScaled(r, h, r);
   drawCylinder(1, 0.9, 0, 0, 0, silver, 0);
   drawCylinder(0.9, 0.9, 0, 0, 0, silver, 0);
   int theta = 90;
   int delta = 10;
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, silver);
   glBegin(GL_QUAD_STRIP);
   double step;
   const double HANDLE_RADIUS = 1.05;
   for(; theta <= 230; theta += delta)
   {
      glNormal3f(Cos(theta), 0, Sin(theta));
      step = (double)(theta - 90)/180;
      glTexCoord2f(step, 0);
      glVertex3d(HANDLE_RADIUS*Cos(theta), 0.9, HANDLE_RADIUS*Sin(theta));
      glTexCoord2f(step, 0.1);
      glVertex3d(HANDLE_RADIUS*Cos(theta), 0.97, HANDLE_RADIUS*Sin(theta));
   }
   glEnd();
   glBegin(GL_QUAD_STRIP);
   theta = 270;
   for(; theta <= 410; theta += delta)
   {
      glNormal3f(Cos(theta), 0, Sin(theta));
      step = (double)(theta - 270)/180;
      glTexCoord2f(step, 0);
      glVertex3d(HANDLE_RADIUS*Cos(theta), 0.9, HANDLE_RADIUS*Sin(theta));
      glTexCoord2f(step, 0.1);
      glVertex3d(HANDLE_RADIUS*Cos(theta), 0.97, HANDLE_RADIUS*Sin(theta));
   }
   glEnd();
   glBegin(GL_QUAD_STRIP);
   theta = 90;
   for(; theta <= 230; theta += delta)
   {
      glNormal3f(Cos(theta), 0, Sin(theta));
      step = (double)(theta - 90)/180;
      glTexCoord2f(step, 0);
      glVertex3d(0.9*Cos(theta), 0.9, 0.9*Sin(theta));
      glTexCoord2f(step, 0.1);
      glVertex3d(0.9*Cos(theta), 0.97, 0.9*Sin(theta));
   }
   glEnd();
   glBegin(GL_QUAD_STRIP);
   theta = 270;
   for(; theta <= 410; theta += delta)
   {
      glNormal3f(Cos(theta), 0, Sin(theta));
      step = (double)(theta - 270)/180;
      glTexCoord2f(step, 0);
      glVertex3d(0.9*Cos(theta), 0.9, 0.9*Sin(theta));
      glTexCoord2f(step, 0.1);
      glVertex3d(0.9*Cos(theta), 0.97, 0.9*Sin(theta));
   }
   glEnd();
   glBegin(GL_QUADS);
   glVertex3d(HANDLE_RADIUS*Cos(90), 0.9, HANDLE_RADIUS*Sin(90));
   glVertex3d(0.9*Cos(90), 0.9, 0.9*Sin(90));
   glVertex3d(0.9*Cos(90), 0.97, 0.9*Sin(90));
   glVertex3d(HANDLE_RADIUS*Cos(90), 0.97, HANDLE_RADIUS*Sin(90));
   glEnd();
   glBegin(GL_QUADS);
   glVertex3d(HANDLE_RADIUS*Cos(230), 0.9, HANDLE_RADIUS*Sin(230));
   glVertex3d(0.9*Cos(230), 0.9, 0.9*Sin(230));
   glVertex3d(0.9*Cos(230), 0.97, 0.9*Sin(230));
   glVertex3d(HANDLE_RADIUS*Cos(230), 0.97, HANDLE_RADIUS*Sin(230));
   glEnd();
   glBegin(GL_QUADS);
   glVertex3d(HANDLE_RADIUS*Cos(270), 0.9, HANDLE_RADIUS*Sin(270));
   glVertex3d(0.9*Cos(270), 0.9, 0.9*Sin(270));
   glVertex3d(0.9*Cos(270), 0.97, 0.9*Sin(270));
   glVertex3d(HANDLE_RADIUS*Cos(270), 0.97, HANDLE_RADIUS*Sin(270));
   glEnd();
   glBegin(GL_QUADS);
   glVertex3d(HANDLE_RADIUS*Cos(410), 0.9, HANDLE_RADIUS*Sin(410));
   glVertex3d(0.9*Cos(410), 0.9, 0.9*Sin(410));
   glVertex3d(0.9*Cos(410), 0.97, 0.9*Sin(410));
   glVertex3d(HANDLE_RADIUS*Cos(410), 0.97, HANDLE_RADIUS*Sin(410));
   glEnd();
   glDisable(GL_TEXTURE_2D);
   drawCylinder(1.05, 0.03, 0, 0.97, 0, silver, 0);
   drawCylinder(0.9, 0.03, 0, 0.97, 0, silver, 0);
   glColor3f(0.5, 0.5, 0.5);
   drawAnnulus(1.05, 1, 0, 0.90, 0, silver);
   drawAnnulus(1.05, 0.9, 0, 0.97, 0, silver);
   drawAnnulus(1.05, 0.9, 0, 1, 0, silver);
   drawAnnulus(1, 0.01, 0, 0.90, 0, silver);
   drawCylinder(0.15, 0.1, 0, 0.90, 0, silver, 0);
   glPopMatrix();
}

void drawSlicedCone(double rSmall, double rBig, double height, double x, 
                    double y, double z)
{
   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(rBig, height, rBig);

   double ratio = rSmall/rBig;   
   int theta = 0;
   int delta = 10;
   glBegin(GL_QUAD_STRIP);
   for(; theta <= 360; theta += delta)
   {
      glNormal3f(1, 1, Sin(theta));
      glVertex3d(ratio*Cos(theta), 1, ratio*Sin(theta));
      glNormal3f(1, 1, Sin(theta));
      glVertex3d(Cos(theta), 0, Sin(theta)); 
   }
   glEnd();

   glPopMatrix();
}

void drawLampHead(double x, double y, double z, int rotation, double rSmall, 
                  double rBig, double height)
{
   glPushMatrix();
   glTranslated(x, y, z);
   glRotated(rotation, 0, 0, -1);
   drawSlicedCone(rSmall, rBig, height, 0, 0, 0);

   glPopMatrix();
}

void drawLamp(double x, double y, double z)
{
   glPushMatrix();
   glTranslated(x, y, z);

   double CYLINDER_RADIUS = 0.1;
   double BASE_RADIUS = 1.5;
   double BASE_HEIGHT = 2;
   double CYLINDER_HEIGHT = ylight - Y_GROUND - BASE_HEIGHT + 1;
   glColor3f(0.3, 0.5, 1);
   drawSlicedCone(CYLINDER_RADIUS, BASE_RADIUS, BASE_HEIGHT, 0, 0, 0);
   drawCylinder(CYLINDER_RADIUS, CYLINDER_HEIGHT, 0, BASE_HEIGHT, 0, silver, 
                0); 
   glColor3f(0.3, 0.5, 1);
   drawLampHead(-1.44, CYLINDER_HEIGHT + 0.5, 0, 45, 0.2, 1, 2);
   glPopMatrix();
}

void drawTableScene()
{
   drawCan(0.5, 2, 6*CUP_RADIUS, -1, -Z0 - sqrt(3));
   drawCan(0.5, 2, -6*CUP_RADIUS, -1, -Z0 - sqrt(3));
   drawCan(0.5, 2, 6*CUP_RADIUS, -1, Z0 + sqrt(3));
   drawCan(0.5, 2, -6*CUP_RADIUS, -1, Z0 + sqrt(3));

   int i = 1;
   for(; i >= -1; i -= 2)
   {
      drawCup(CUP_RADIUS, CUP_HEIGHT, 0, -1, i*Z0);
      drawCup(CUP_RADIUS, CUP_HEIGHT, -R, -1, i*(Z0 - R*sqrt(3)));
      drawCup(CUP_RADIUS, CUP_HEIGHT, R, -1, i*(Z0 - R*sqrt(3)));
      drawCup(CUP_RADIUS, CUP_HEIGHT, 0, -1, i*(Z0 - 2*R*sqrt(3)));
      drawCup(CUP_RADIUS, CUP_HEIGHT, -2*R, -1, i*(Z0 - 2*R*sqrt(3)));
      drawCup(CUP_RADIUS, CUP_HEIGHT, +2*R, -1, i*(Z0 - 2*R*sqrt(3)));
      drawCup(CUP_RADIUS, CUP_HEIGHT, -R, -1, i*(Z0 - 3*R*sqrt(3)));
      drawCup(CUP_RADIUS, CUP_HEIGHT, +R, -1, i*(Z0 - 3*R*sqrt(3)));
      drawCup(CUP_RADIUS, CUP_HEIGHT, -3*R, -1, i*(Z0 - 3*R*sqrt(3)));
      drawCup(CUP_RADIUS, CUP_HEIGHT, +3*R, -1, i*(Z0 - 3*R*sqrt(3)));
   }
   if( pBall.active )
   {
      glColor3f(1, (float)153/255, (float)51/255);
      drawSphere(pBall.x, pBall.y, pBall.z, BALL_RADIUS);
   } 
}

void drawScene()
{
   drawFence();
   drawHouse();
   drawKeg(2.5, 10, -13, Y_GROUND, -5);
   drawKeg(2.5, 10, -13, Y_GROUND, 5);

   drawTable(0, -1.5, 0, TABLE_WIDTH, 0.5, TABLE_LENGTH);
   drawLamp(LIGHT_RADIUS + 1, Y_GROUND, 0);

}

void drawGroundShadows(float L[4], float E[4], float N[4])
{
   //  Blended with stencil buffer
   glDisable(GL_LIGHTING);
   //  Enable stencil operations
   glEnable(GL_STENCIL_TEST);

   /*
    *  Step 1:  Set stencil buffer to 1 where there are shadows
    */
   //  Existing value of stencil buffer doesn't matter
   glStencilFunc(GL_ALWAYS,1,0xFFFFFFFF);
   //  Set the value to 1 (REF=1 in StencilFunc)
   //  only if Z-buffer would allow write
   glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
   //  Make Z-buffer and color buffer read-only
   glDepthMask(0);
   glColorMask(0,0,0,0);
   //  Draw flattened scene
   glPushMatrix();
   makeShadowProjection(L,E,N);
   drawTableScene();
   drawScene();
   glPopMatrix();
   //  Make Z-buffer and color buffer read-write
   glDepthMask(1);
   glColorMask(1,1,1,1);

   /*
    *  Step 2:  Draw shadow masked by stencil buffer
    */
   //  Set the stencil test draw where stencil buffer is > 0
   glStencilFunc(GL_LESS,0,0xFFFFFFFF);
   //  Make the stencil buffer read-only
   glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
   //  Enable blending
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glColor4f(0,0,0,0.5);
   //  Draw the shadow over the entire floor
   glBegin(GL_QUADS);
   glVertex3f(-GROUND_WIDTH,Y_GROUND,-GROUND_LENGTH);
   glVertex3f(+GROUND_WIDTH,Y_GROUND,-GROUND_LENGTH);
   glVertex3f(+GROUND_WIDTH,Y_GROUND,+GROUND_LENGTH);
   glVertex3f(-GROUND_WIDTH,Y_GROUND,+GROUND_LENGTH);
   glEnd();

}

void drawTableShadows(float L[4], float E[4], float N[4])
{
   //  Draw table shadows
      //  Blended with stencil buffer
   glDisable(GL_LIGHTING);
   //  Enable stencil operations
   glEnable(GL_STENCIL_TEST);

   /*
    *  Step 1:  Set stencil buffer to 1 where there are shadows
    */
   //  Existing value of stencil buffer doesn't matter
   glStencilFunc(GL_ALWAYS,1,0xFFFFFFFF);
   //  Set the value to 1 (REF=1 in StencilFunc)
   //  only if Z-buffer would allow write
   glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
   //  Make Z-buffer and color buffer read-only
   glDepthMask(0);
   glColorMask(0,0,0,0);
   //  Draw flattened scene
   glPushMatrix();
   makeShadowProjection(L,E2,N);
   drawTableScene();
   glPopMatrix();
   //  Make Z-buffer and color buffer read-write
   glDepthMask(1);
   glColorMask(1,1,1,1);

   /*
    *  Step 2:  Draw shadow masked by stencil buffer
    */
   //  Set the stencil test draw where stencil buffer is > 0
   glStencilFunc(GL_LESS,0,0xFFFFFFFF);
   //  Make the stencil buffer read-only
   glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
   //  Enable blending
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glColor4f(0,0,0,0.5);
   //  Draw the shadow over the entire floor
   glBegin(GL_QUADS);
   glVertex3f(-TABLE_WIDTH,-1,-TABLE_LENGTH);
   glVertex3f(+TABLE_WIDTH,-1,-TABLE_LENGTH);
   glVertex3f(+TABLE_WIDTH,-1,+TABLE_LENGTH);
   glVertex3f(-TABLE_WIDTH,-1,+TABLE_LENGTH);
   glEnd();

}

void display()
{
   const double len = 2.5;  //  Length of axes
   double Ex = -2*dim*Sin(th)*Cos(ph);
   double Ey = +2*dim        *Sin(ph);
   double Ez = +2*dim*Cos(th)*Cos(ph);
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //glEnable(GL_CULL_FACE);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(ph), 0);
   //  Light switch
   //  Translate intensity to color vectors
   float Ambient[] = {0.3, 0.3, 0.3, 1.0};
   float Diffuse[] = {1, 1, 1, 1};
   float Specular[] = {1, 1, 0, 1};
   // float white[] = {1, 1, 1, 1};
   //  Light direction
   float Position[] = {LIGHT_RADIUS*Cos(zh), ylight, LIGHT_RADIUS*Sin(zh), 1};
   //  Draw light position as ball (still no lighting here)
   glColor3f(1, 1, 1);
   drawSphere(Position[0], Position[1], Position[2], 0.5);
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
   //glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
   //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);


   
   //  Draw scene
   drawSky(0, -10, 0, 2*GROUND_LENGTH);
   drawGround();
   drawScene();
   drawTableScene();

   //  Draw shadows
   glPushAttrib(GL_ENABLE_BIT);
   drawGroundShadows(Position, E, N);
   glClear(GL_STENCIL_BUFFER_BIT);
   drawTableShadows(Position, E2, N); 
   glPopAttrib();
 
   const double TOL = 0.1;
   if( pBall.y < -1 + TOL + BALL_RADIUS && pBall.vy < 0 
       && abs(pBall.x) < TABLE_WIDTH && abs(pBall.z) < abs(TABLE_LENGTH) )
   {
      pBall.vy *= 0.7;
      pBall.vy = -pBall.vy;
   } 
   if ( abs(pBall.y - CUP_HEIGHT + 1.5) < TOL && pBall.vy < 0
        && abs(pBall.z) < abs(TABLE_LENGTH) )
   {
      pBall.vy *= 0.7;
      pBall.vy = -pBall.vy;
   } 
   if ( abs(pBall.y + 5) < -10 )
   {
       pBall.active = 0;
   }


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
   Print(text[mode]);
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

void bounce(ball *b)
{
   ball bb = *b;
   bb.vy *= 0.7; // Reduce velocity
   bb.vy = -bb.vy; // Send the ball upward
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
   //  Reset view angle
   if (ch == '0')
   {
      th = 0;
      ph = 25;
   }
   else if ('j' == ch) { pBall.vxp -= 0.01; }
   else if ('l' == ch) { pBall.vxp += 0.01; }
   else if ('k' == ch) { pBall.vzp -= 0.01; }
   else if ('i' == ch) { pBall.vzp += 0.01; }
   else if ('-' == ch) { dim += 0.1; }
   else if ('+' == ch) { dim -= 0.1; }
   else if (27 == ch) { exit(0); } // Exit on ESC
   else if (ch == 'm' && mode<MODE) { mode++; };

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

void resetBall()
{
   pBall.active = 1;
   pBall.x = 0;
   pBall.vx = 0.0;
   pBall.y = 1;
   pBall.vy = 1;
   pBall.z = -Z0;
   pBall.vz = -1.5;
}

void mouse()
{
   resetBall();
}


void idle()
{
   //  Elapsed time in seconds
   // double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   // zh = fmod(90*t, 360.0);

   pBall.x += pBall.vx;
   pBall.y += pBall.vy;
   pBall.z += pBall.vz; 
   pBall.vy = pBall.vy + GRAVITY;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

// Thanks to Dr. Schreuder for the LoadTexBMP function
void loadTextures()
{
   red = LoadTexBMP("images/red.bmp");
   canside = LoadTexBMP("images/can2.bmp");
   cantop = LoadTexBMP("images/beercan.bmp");
   wood = LoadTexBMP("images/wood.bmp");
   brick = LoadTexBMP("images/brick.bmp");
   grass = LoadTexBMP("images/grass.bmp");
   silver = LoadTexBMP("images/scratch.bmp");
   sky = LoadTexBMP("images/skynight.bmp");
   shingle = LoadTexBMP("images/shingle.bmp");
   door = LoadTexBMP("images/door.bmp");
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc, char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc, argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL | GLUT_DOUBLE);
   int width = glutGet(GLUT_SCREEN_WIDTH);
   int height = glutGet(GLUT_SCREEN_HEIGHT);
   glutInitWindowSize(width, height);
   glutCreateWindow("Ready for the Weekend (By Branden Olson)");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   glutMouseFunc(mouse);
   loadTextures();
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}

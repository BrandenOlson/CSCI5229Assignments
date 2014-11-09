#ifndef OlsonLib
#define OlsonLib

#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
 
typedef struct materialStruct
{
   float ambient[4];
   float diffuse[4];
   float specular[4];
   float shininess;
} materialStruct;

void materials(materialStruct *materials);

#endif

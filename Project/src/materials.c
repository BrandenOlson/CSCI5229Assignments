#ifndef MATERIALS
#define MATERIALS 

#include "OlsonLib.h"

void materials(materialStruct *materials)
{
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materials->ambient);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materials->diffuse);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materials->specular);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materials->shininess);
}

#endif

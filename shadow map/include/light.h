#ifndef LIGHT_H	//use to check this .h ever been included
#define LIGHT_H

#include "GL/glut.h"
#include <string>
#include <vector>
class light_element
{
public:

	GLfloat position[4];
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	light_element()
	{
		for(int i = 0; i < 4; i++)
			position[i] = ambient[i] = diffuse[i] = specular[i] = 0.0f;
	}
};
class light
{
public:
	
	std::vector<light_element> lightList;
	int now;
	light(const char* obj_file);
	GLfloat total_ambient[3];
	void LoadLight(std::string obj_file);

private:
	
	FILE* light_file;
};
#endif // #ifndef LIGHT_H
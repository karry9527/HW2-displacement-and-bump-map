#ifndef SCENE_H
#define SCENE_H

#include<vector>
#include<string>

class scene_element
{
public:
	float Sx, Sy, Sz, Angle, Rx, Ry, Rz, Tx, Ty, Tz;
	int type;
	char objectName[30];
	scene_element()
	{
		Sx = Sy = Sz = Angle = Rx = Ry = Rz = Tx = Ty = Tz = 0;
		type = 0;
	}
};

class tex_element
{
public:
	char texName[30];
	char type[30];
	tex_element(){};
		
};

class scene
{
public:
	std::vector<scene_element> object_scene;
	int objectNum, objectNumMax, texObjectNum;
	//GLuint texObject;
	std::vector<tex_element> texObjectName;
	scene(const char* obj_file);

private:
	FILE* scene_file;
};

#endif
#include"scene.h"
#include<iostream>

using namespace std;

scene::scene(const char* obj_file)
{
	char	token[100], buf[30];
	float a, x, y, z;
	scene_file = fopen(obj_file, "r");
	objectNum = 0;
	texObjectNum = 0;
	tex_element t;
	int obType = 0;
	while(!feof(scene_file))
	{
		
		token[0] = NULL;
		fscanf(scene_file,"%s", token);
		if(!strcmp(token, "no-texture"))
		{
			texObjectName.push_back(t);
			strcpy(texObjectName[texObjectNum++].type, token);
			//fscanf(scene_file,"%s", buf);
			//strcpy(objectName[objectNum], buf);
			obType++;
		}
		else if(!strcmp(token, "single-texture"))
		{
			texObjectName.push_back(t);
			strcpy(texObjectName[texObjectNum].type, token);
			fscanf(scene_file,"%s", buf);
			strcpy(texObjectName[texObjectNum++].texName, buf);
			obType++;
		}
		else if(!strcmp(token, "multi-texture"))
		{
			texObjectName.push_back(t);
			strcpy(texObjectName[texObjectNum].type, token);
			fscanf(scene_file,"%s", buf);
			strcpy(texObjectName[texObjectNum++].texName, buf);
			texObjectName.push_back(t);
			fscanf(scene_file,"%s", buf);
			strcpy(texObjectName[texObjectNum++].texName, buf);
			obType++;
		}
		else if(!strcmp(token, "cube-map"))
		{
			texObjectName.push_back(t);
			strcpy(texObjectName[texObjectNum].type, token);
			fscanf(scene_file,"%s", buf);
			strcpy(texObjectName[texObjectNum++].texName, buf);
			for(int i = 1; i < 6; i++)
			{
				texObjectName.push_back(t);
				fscanf(scene_file,"%s", buf);
				strcpy(texObjectName[texObjectNum++].texName, buf);
			}
			obType++;
		}
		else if(!strcmp(token, "model"))
		{
			scene_element ele;
			object_scene.push_back(ele);
			fscanf(scene_file,"%s", buf);
			//if(!strcmp(buf, "*.obj"))
			//printf("%s\n", buf);
			//printf("objectName:%s\n", objectName[objectNum]);
			strcpy(object_scene[objectNum].objectName, buf);
			//printf("%s\n", buf);
			//printf("objectName:%s\n", objectName[objectNum]);
			fscanf(scene_file,"%f %f %f", &x, &y, &z);
			object_scene[objectNum].Sx = x;
			object_scene[objectNum].Sy = y;
			object_scene[objectNum].Sz = z;
			fscanf(scene_file,"%f %f %f %f", &a, &x, &y, &z);
			object_scene[objectNum].Angle = a;
			object_scene[objectNum].Rx = x;
			object_scene[objectNum].Ry = y;
			object_scene[objectNum].Rz = z;
			fscanf(scene_file,"%f %f %f", &x, &y, &z);
			object_scene[objectNum].Tx = x;
			object_scene[objectNum].Ty = y;
			object_scene[objectNum].Tz = z;

			object_scene[objectNum++].type = obType;

			
		}
	}

	objectNumMax = objectNum;
	objectNum = 0;
}
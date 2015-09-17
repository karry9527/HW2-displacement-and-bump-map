#include <iostream>
#include"light.h"

using namespace std;

light::light(const char* obj_file)
{
	LoadLight(obj_file);
}

void light::LoadLight(string obj_file)
{
	char s[80];
	now = 0;
	//string light_num[2] = {"GL_LIGHT0", "GL_LIGHT1"};
	light_file = fopen(obj_file.c_str(), "r");

	/*GLfloat light_specular[4] = {0, 0, 0, 1.0};
	GLfloat light_diffuse[4] = {0, 0, 0, 1.0};
	GLfloat light_ambient[4] = {0, 0, 0, 1.0};
	GLfloat light_position[4] = {0, 0, 0, 1.0};*/

	
	//GLfloat light_total_ambient[3];
	//GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
	//GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	//GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0};
	//GLfloat light_position[] = {150.0, 150.0, 150.0, 1.0};

	
	
	while (fgets(s, 80, light_file) != NULL) {
		printf(s);
		char *temp = s;
		char *test = strtok(temp, " ");
		while(test != NULL)
		{
			if(!strcmp(test, "light"))
			{
				//glEnable(GL_LIGHT0 + now);
				light_element ele;
				lightList.push_back(ele);
				for(int i = 0; i < 3; i++)
				{
					test = strtok(NULL, " ");
					cout << "light_test:" << test << endl;
					lightList[now].position[i] = atof(test);
				}
				//lightList[now].position[3] = 1;	//	make light to be a point
				//glLightfv(GL_LIGHT0 + now, GL_POSITION, position[now]);
				for(int i = 0; i < 3; i++)
				{
					test = strtok(NULL, " ");
					lightList[now].ambient[i] = atof(test);
				}
				//glLightfv(GL_LIGHT0 + now, GL_AMBIENT, ambient[now]);
				for(int i = 0; i < 3; i++)
				{
					test = strtok(NULL, " ");
					lightList[now].diffuse[i] = atof(test);
				}
				//glLightfv(GL_LIGHT0 + now, GL_DIFFUSE, diffuse[now]);
				for(int i = 0; i < 3; i++)
				{
					test = strtok(NULL, " ");
					printf("%s\n", test);
					lightList[now].specular[i] = atof(test);
				}
				//glLightfv(GL_LIGHT0 + now, GL_SPECULAR, specular[now]);
				test = strtok(NULL, " ");
				now++;
			}
			else if(!strcmp(test, "ambient"))
			{
				for(int i = 0; i < 3; i++)
				{
					test = strtok(NULL, " ");
					total_ambient[i] = atof(test);
				}
				//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, total_ambient);
				test = strtok(NULL, " ");
			}
		}
        
    }
}
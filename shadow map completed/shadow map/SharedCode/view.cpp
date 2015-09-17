#include <iostream>
#include "view.h"

using namespace std;

view::view(const char* obj_file)
{
	view_file = fopen(obj_file, "r");
	char s[80];
	while (fgets(s, 80, view_file) != NULL) {
		printf(s);
		char *temp = s;
		char *test = strtok(temp, " ");
		if(strcmp(test, "eye") == 0)
		{
			for(int i = 0; i < 3; i++)
			{
				test = strtok(NULL, " ");
				eye[i] = atof(test);
				cout << eye[i] << " ";
			}
		}
		else if(strcmp(test, "vat") == 0) 
		{
			for(int i = 0; i < 3; i++)
			{
				test = strtok(NULL, " ");
				center[i] = atof(test);
			}
		}
		else if(strcmp(test, "vup") == 0) 
		{
			for(int i = 0; i < 3; i++)
			{
				test = strtok(NULL, " ");
				up[i] = atof(test);
			}
		}
		else if(strcmp(test, "fovy") == 0) 
		{
			test = strtok(NULL, " ");
			fovy = atof(test);
		}
		else if(strcmp(test, "dnear") == 0) 
		{
			test = strtok(NULL, " ");
			dnear = atof(test);
		}
		else if(strcmp(test, "dfar") == 0) 
		{
			test = strtok(NULL, " ");
			dfar = atof(test);
		}
		else if(strcmp(test, "viewport") == 0) 
		{
			test = strtok(NULL, " ");
			x = atoi(test);
			test = strtok(NULL, " ");
			y = atoi(test);
			test = strtok(NULL, " ");
			w = atoi(test);
			test = strtok(NULL, " ");
			h = atoi(test);
		}
	}
}
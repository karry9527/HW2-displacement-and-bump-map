#ifndef VIEW_H
#define VIEW_H

class view
{
public:

	int x, y, w, h;
	float eye[3], center[3], up[3], fovy, dnear, dfar;
	view(const char* obj_file);

private:
	FILE *view_file;
};


#endif
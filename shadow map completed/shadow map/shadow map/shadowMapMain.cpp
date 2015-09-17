#include "../SharedCode/ShaderLoader.h"
#include "../SharedCode/mesh.h"


#include "FreeImage.h"
#include "view.h"
#include "scene.h"
#include "glmath.h"
#include "Matrix16.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include "light.h"
#include <GL/glut.h>

//#define		SHADOW_MAP_SIZE 1024
#define		TEX_NUM 2	  //the number of textures you use.
GLuint		texObject[TEX_NUM];	//texture object
GLuint		depthFBO, colorTextureId, ShadowMap, FBO, VBO, UniformLocations[2];
GLhandleARB	MyShader;
mesh			*object1, *object2, *object3;
light		*light1;
view			*view1;
scene		*scene1;
mat4x4		ModelMatrix, ViewMatrix, ViewMatrixInverse, ProjectionMatrix, ProjectionMatrixInverse;
mat4x4		LightViewMatrix, LightProjectionMatrix, ShadowMatrix;
int			WinW, WinH, scene_num;
bool		ShowShadowMap = false;

void RenderInit();
void SetFrameBufferObject(int fbowidth, int fboheight);
void LoadShaders();
void RenderObjects(bool pass1, mesh *object, int object_num);
void Display();
void keyboard(unsigned char key, int x, int y);
void Reshape(GLsizei , GLsizei );

int main(int argc, char** argv)
{
	//remember to change the path of mesh.cpp
	printf("Please choose the sample(1:TestScene1 2:TestScene2 3:TwoToys):");
	scanf("%d", &scene_num);


	switch(scene_num)
	{
	case 1:
		object1 = new mesh("HW3\\TestScene1\\Ground.obj");
		object2 = new mesh("HW3\\TestScene1\\SingleTriangle.obj");

		view1 = new view("HW3\\TestScene1\\TestScene1.view");
		light1 = new light("HW3\\TestScene1\\TestScene1.light");
		scene1 = new scene("HW3\\TestScene1\\TestScene1.scene");
		break;
	case 2:
		object1 = new mesh("HW3\\TestScene2\\Ground.obj");
		object2 = new mesh("HW3\\TestScene2\\Objects.obj");

		view1 = new view("HW3\\TestScene2\\TestScene2.view");
		light1 = new light("HW3\\TestScene2\\TestScene2.light");
		scene1 = new scene("HW3\\TestScene2\\TestScene2.scene");
		break;
	case 3:
		object1 = new mesh("HW3\\TwoToys\\Cornell_box.obj");
		object2 = new mesh("HW3\\TwoToys\\ToySit.obj");
		object3 = new mesh("HW3\\TwoToys\\ToyStand.obj");

		view1 = new view("HW3\\TwoToys\\TwoToys.view");
		light1 = new light("HW3\\TwoToys\\TwoToys.light");
		scene1 = new scene("HW3\\TwoToys\\TwoToys.scene");
		break;
	}

	glutInit(&argc, argv);
	glutInitWindowSize(view1->w, view1->h);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Shader Use Texture");

	GLenum glew_error;
	if((glew_error = glewInit()) != GLEW_OK)return -1;


	
	LoadShaders();

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
    glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}


void LoadShaders()
{
	MyShader = glCreateProgram();
	if(MyShader != 0)
	{	
		ShaderLoad(MyShader, "../shadowMap.vs", GL_VERTEX_SHADER);
		ShaderLoad(MyShader, "../shadowMap.fs", GL_FRAGMENT_SHADER);
	}
}

void RenderInit()
{	
	UniformLocations[0] = glGetUniformLocation(MyShader, "ShadowMatrix");
	// set constant uniforms --------------------------------------------------------------------------------------------------
	UniformLocations[1] = glGetUniformLocation(MyShader, "ShadowMap");
	glUseProgram(MyShader);
	glUniform1i(UniformLocations[1], 0);	///kkk 9.4 update bug(1 -> 0)
	//printf("ShadowMap: %d\n", UniformLocations[1]);
	glUseProgram(0);

	// generate shadow map texture --------------------------------------------------------------------------------------------

	glGenTextures(1, &ShadowMap);
	glBindTexture(GL_TEXTURE_2D, ShadowMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	// glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, view1->w, view1->h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// generate FBO -----------------------------------------------------------------------------------------------------------

	glGenFramebuffersEXT(1, &FBO);

	// set light --------------------------------------------------------------------------------------------------------------
	
	//LightView
	glEnable(GL_LIGHTING);
	for(int i = 0; i < light1->now; i++)
	{
		glEnable(GL_LIGHT0 + i);
		//some problem about GL_POSITION
		glLightfv(GL_LIGHT0 + i, GL_POSITION, light1->lightList[i].position);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light1->lightList[i].ambient);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light1->lightList[i].diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light1->lightList[i].specular);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light1->total_ambient);
		//cout << "i:" << i << "  " << light1->position[i] << light1->ambient[i] << light1->diffuse[i] << light1->specular[i] << endl;
	}
}

void RenderObjects(bool pass1, mesh *object, int object_num)
{
	//don't write in for loop
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glTranslatef(scene1->object_scene[object_num].Tx, scene1->object_scene[object_num].Ty, scene1->object_scene[object_num].Tz);
		glRotatef(scene1->object_scene[object_num].Angle, scene1->object_scene[object_num].Rx, scene1->object_scene[object_num].Ry, scene1->object_scene[object_num].Rz);
		glScalef(scene1->object_scene[object_num].Sx, scene1->object_scene[object_num].Sy, scene1->object_scene[object_num].Sz);
		
	int lastMaterial = -1;
	for (size_t i=0;i < object->fTotal;i++)
	{
			// set material property if this face used different material
			if(lastMaterial != object->faceList[i].m && !pass1)
			{
				lastMaterial = (int)object->faceList[i].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT  , object->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE  , object->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR , object->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &object->mList[lastMaterial].Ns);

				//you can obtain the texture name by object->mList[lastMaterial].map_Kd
				//load them once in the main function before mainloop
				//bind them in display function here
			}
			glBegin(GL_POLYGON);
			for (size_t j=0;j<3;j++)
			{
				//glTexCoord2fv(object->tList[object->faceList[i][j].t].ptr);
				glNormal3fv(object->nList[object->faceList[i][j].n].ptr);
				glVertex3fv(object->vList[object->faceList[i][j].v].ptr);	
			}
			glEnd();
	}
	glPopMatrix();
}



void Display()
{
	float buffer[16];
	RenderInit();
	//Pass1
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, view1->w, view1->h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	light1->lightList[0].position[0], light1->lightList[0].position[1], light1->lightList[0].position[2],// eye
				
				view1->center[0], view1->center[1], view1->center[2],  // center
				view1->up[0]+1, view1->up[1], view1->up[2]);    // up	//kkk 9.4 problem
	
	glGetFloatv(GL_MODELVIEW_MATRIX, buffer);
	for(int i = 0; i < 16; i++)
		LightViewMatrix.M[i] = buffer[i];
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(view1->fovy*1.4, (GLfloat)WinW/(GLfloat)WinH, view1->dnear*4, view1->dfar);//fovy aspect near far
	
	glGetFloatv(GL_PROJECTION_MATRIX, buffer);
	for(int i = 0; i < 16; i++)
		LightProjectionMatrix.M[i] = buffer[i];
	
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
	glDrawBuffers(0, NULL); glReadBuffer(GL_NONE);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, ShadowMap, 0);
	
	glClear(GL_DEPTH_BUFFER_BIT);
	
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//we don't use it because there are thin plane. If we cull front, shadowmap can't see thin plane.
	//glCullFace(GL_FRONT);	//kkk problem

	glShadeModel(GL_FLAT);
    glColorMask(0, 0, 0, 0);

	//kkk RenderObjects
	switch(scene_num)
	{
	case 1:
		RenderObjects(true, object1, 0);
		RenderObjects(true, object2, 1);
		break;
	case 2:
		RenderObjects(true, object1, 0);
		RenderObjects(true, object2, 1);
		break;
	case 3:
		RenderObjects(true, object1, 0);
		RenderObjects(true, object2, 1);
		RenderObjects(true, object3, 2);
		break;
	}
	
	//glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);
    glColorMask(1, 1, 1, 1);
	
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	//Pass2

	glViewport(view1->x, view1->y, view1->w, view1->h);						// Reset The Current Viewport
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	//glLoadMatrixf(&ViewMatrix);
	glLoadIdentity();
	gluLookAt(	view1->eye[0], view1->eye[1], view1->eye[2],// eye
				view1->center[0], view1->center[1], view1->center[2],  // center
				view1->up[0], view1->up[1], view1->up[2]);    // up
	glGetFloatv(GL_MODELVIEW_MATRIX, buffer);
	for(int i = 0; i < 16; i++)
		ViewMatrix.M[i] = buffer[i];
	ViewMatrixInverse = inverse(ViewMatrix);
	glMatrixMode(GL_PROJECTION);											// Select The Projection Matrix
	
	glLoadIdentity();
	gluPerspective(view1->fovy, (GLfloat)WinW/(GLfloat)WinH, view1->dnear, view1->dfar);//fovy aspect near far

	
	if(scene_num == 3)
		light1->lightList[0].position[3] = 1;
	glEnable(GL_LIGHTING);
	for(int i = 0; i < light1->now; i++)
	{
		glEnable(GL_LIGHT0 + i);
		//some problem about GL_POSITION
		glLightfv(GL_LIGHT0 + i, GL_POSITION, light1->lightList[i].position);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light1->lightList[i].ambient);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light1->lightList[i].diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light1->lightList[i].specular);
		//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light1->total_ambient);
		//cout << "i:" << i << "  " << light1->position[i] << light1->ambient[i] << light1->diffuse[i] << light1->specular[i] << endl;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glUseProgram(MyShader);
	ShadowMatrix = BiasMatrix * LightProjectionMatrix * LightViewMatrix * ViewMatrixInverse; //kkk
	
	glUniformMatrix4fv(UniformLocations[0], 1, GL_FALSE, &ShadowMatrix);

	glBindTexture(GL_TEXTURE_2D, ShadowMap);
	//kkk RenderObjects
	switch(scene_num)
	{
	case 1:
		RenderObjects(false, object1, 0);
		RenderObjects(false, object2, 1);
		break;
	case 2:
		RenderObjects(false, object1, 0);
		RenderObjects(false, object2, 1);
		break;
	case 3:
		RenderObjects(false, object1, 0);
		RenderObjects(false, object2, 1);
		RenderObjects(false, object3, 2);
		break;
	}
	glDisable(GL_LIGHTING);

	glUseProgram(0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
	
	if(ShowShadowMap)
	{
		glViewport(0, 0, 512, 512);
	
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ShadowMap);
		glColor3f(100.0f, 100.0f, 100.0f);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 1.0f);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}


	glutSwapBuffers();
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
		case 'w':	view1->eye[2] -= 10;break;//eye往前
        case 'a':	view1->eye[0] -= 10;break;//eye往左
        case 's':	view1->eye[2] += 10;break;//eye往後
        case 'd':	view1->eye[0] += 10;break;//eye往右
		case 'z':	ShowShadowMap = !ShowShadowMap;
		//case '1':	object_num = 1;break;
		//case '2':	object_num = 2;break;
    }
	glutPostRedisplay();
}

void Reshape(GLsizei w, GLsizei h)
{
	WinW = w;
	WinH = h;
	// viewport transformation
	glViewport(view1->x, view1->y, view1->w, view1->h);

}


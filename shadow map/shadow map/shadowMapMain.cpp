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


#define		TEX_NUM 2	  //the number of textures you use.
GLuint		texObject[TEX_NUM];	//texture object
GLuint		depthFBO, colorTextureId, shadowmapid;
GLhandleARB	MyShader;
mesh			*object1, *object2, *object3;
light*		light1;
view*		view1;
scene		*scene1;
float		ModelMatrix[16], ViewMatrix[16], ProjectionMatrix[16], ProjectionMatrixInverse[16];
CMatrix16		LightViewMatrix, LightProjectionMatrix, ShadowMatrix, ViewMatrixInverse;
int			WinW,WinH;

void GenShadowMap();
void SetFrameBufferObject(int fbowidth, int fboheight);
void LoadShaders();
void RenderObjects();
void Display();
void keyboard(unsigned char key, int x, int y);
void Reshape(GLsizei , GLsizei );

int main(int argc, char** argv)
{
	//remember to change the path of mesh.cpp

	/*object1 = new mesh("HW3\\TestScene1\\Ground.obj");
	object2 = new mesh("HW3\\TestScene1\\SingleTriangle.obj");

	view1 = new view("HW3\\TestScene1\\TestScene1.view");
	light1 = new light("HW3\\TestScene1\\TestScene1.light");
	scene1 = new scene("HW3\\TestScene1\\TestScene1.scene");*/

	/*object1 = new mesh("HW3\\TestScene2\\Ground.obj");
	object2 = new mesh("HW3\\TestScene2\\Objects.obj");

	view1 = new view("HW3\\TestScene2\\TestScene2.view");
	light1 = new light("HW3\\TestScene2\\TestScene2.light");
	scene1 = new scene("HW3\\TestScene2\\TestScene2.scene");*/

	object1 = new mesh("HW3\\TwoToys\\Cornell_box.obj");
	object2 = new mesh("HW3\\TwoToys\\ToySit.obj");
	object3 = new mesh("HW3\\TwoToys\\ToyStand.obj");

	view1 = new view("HW3\\TwoToys\\TwoToys.view");
	light1 = new light("HW3\\TwoToys\\TwoToys.light");
	scene1 = new scene("HW3\\TwoToys\\TwoToys.scene");

	glutInit(&argc, argv);
	glutInitWindowSize(view1->w, view1->h);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Shader Use Texture");

	GLenum glew_error;
	if((glew_error = glewInit()) != GLEW_OK)return -1;

	//unknown
	/*glShadeModel(GL_SMOOTH);												// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);									// Black Background
	glClearDepth(1.0f);														// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);												// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);													// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);						// Really Nice Perspective Calculations
	*/
	glEnable(GL_TEXTURE_2D);	
	LoadShaders();
	SetFrameBufferObject(view1->w, view1->h);

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
    glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}

void SetFrameBufferObject(int fbowidth, int fboheight)
{

   glGenFramebuffersEXT(1, &depthFBO);
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, depthFBO);

   	glGenRenderbuffersEXT(1, &colorTextureId);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, colorTextureId);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_RGBA, fbowidth, fboheight);
    

    glGenTextures(1, &shadowmapid);
    glBindTexture(GL_TEXTURE_2D, shadowmapid);
  	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, fbowidth, fboheight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);						
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, colorTextureId);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, shadowmapid, 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
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

void RenderObjects()
{
	int lastMaterial = -1;
	for (size_t i=0;i < object1->fTotal;i++)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glTranslatef(scene1->object_scene[0].Tx, scene1->object_scene[0].Ty, scene1->object_scene[0].Tz);
			glRotatef(scene1->object_scene[0].Angle, scene1->object_scene[0].Rx, scene1->object_scene[0].Ry, scene1->object_scene[0].Rz);
			glScalef(scene1->object_scene[0].Sx, scene1->object_scene[0].Sy, scene1->object_scene[0].Sz);
		
			
			// set material property if this face used different material
			if(lastMaterial != object1->faceList[i].m)
			{
				lastMaterial = (int)object1->faceList[i].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT  , object1->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE  , object1->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR , object1->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &object1->mList[lastMaterial].Ns);

				//you can obtain the texture name by object1->mList[lastMaterial].map_Kd
				//load them once in the main function before mainloop
				//bind them in display function here
			}
			glBegin(GL_POLYGON);
			for (size_t j=0;j<3;j++)
			{
				//glMultiTexCoord2fv(GL_TEXTURE0,	object1->tList[object1->faceList[i][j].t].ptr);
				glTexCoord2fv(object1->tList[object1->faceList[i][j].t].ptr);
				glNormal3fv(object1->nList[object1->faceList[i][j].n].ptr);
				glVertex3fv(object1->vList[object1->faceList[i][j].v].ptr);	
			}
			glEnd();
		glPopMatrix();
	}
	lastMaterial = -1;
	for (size_t i=0;i < object2->fTotal;i++)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glTranslatef(scene1->object_scene[1].Tx, scene1->object_scene[1].Ty, scene1->object_scene[1].Tz);
			glRotatef(scene1->object_scene[1].Angle, scene1->object_scene[1].Rx, scene1->object_scene[1].Ry, scene1->object_scene[1].Rz);
			glScalef(scene1->object_scene[1].Sx, scene1->object_scene[1].Sy, scene1->object_scene[1].Sz);
		
			
			// set material property if this face used different material
			if(lastMaterial != object2->faceList[i].m)
			{
				lastMaterial = (int)object2->faceList[i].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT  , object2->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE  , object2->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR , object2->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &object2->mList[lastMaterial].Ns);

				//you can obtain the texture name by object2->mList[lastMaterial].map_Kd
				//load them once in the main function before mainloop
				//bind them in display function here
			}
			glBegin(GL_POLYGON);
			for (size_t j=0;j<3;j++)
			{
				//glMultiTexCoord2fv(GL_TEXTURE0,	object2->tList[object2->faceList[i][j].t].ptr);
				glTexCoord2fv(object2->tList[object2->faceList[i][j].t].ptr);
				glNormal3fv(object2->nList[object2->faceList[i][j].n].ptr);
				glVertex3fv(object2->vList[object2->faceList[i][j].v].ptr);	
			}
			glEnd();
		glPopMatrix();
	}
	lastMaterial = -1;
	for (size_t i=0;i < object3->fTotal;i++)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glTranslatef(scene1->object_scene[2].Tx, scene1->object_scene[2].Ty, scene1->object_scene[2].Tz);
			glRotatef(scene1->object_scene[2].Angle, scene1->object_scene[2].Rx, scene1->object_scene[2].Ry, scene1->object_scene[2].Rz);
			glScalef(scene1->object_scene[2].Sx, scene1->object_scene[2].Sy, scene1->object_scene[2].Sz);
		
			
			// set material property if this face used different material
			if(lastMaterial != object3->faceList[i].m)
			{
				lastMaterial = (int)object3->faceList[i].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT  , object3->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE  , object3->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR , object3->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &object3->mList[lastMaterial].Ns);

				//you can obtain the texture name by object3->mList[lastMaterial].map_Kd
				//load them once in the main function before mainloop
				//bind them in display function here
			}
			glBegin(GL_POLYGON);
			for (size_t j=0;j<3;j++)
			{
				//glMultiTexCoord2fv(GL_TEXTURE0,	object3->tList[object3->faceList[i][j].t].ptr);
				glTexCoord2fv(object3->tList[object3->faceList[i][j].t].ptr);
				glNormal3fv(object3->nList[object3->faceList[i][j].n].ptr);
				glVertex3fv(object3->vList[object3->faceList[i][j].v].ptr);	
			}
			glEnd();
		glPopMatrix();
	}
}

void GenShadowMap()
{
	glPushMatrix();
  	    glViewport(view1->x, view1->y, view1->w, view1->h);
	    glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(LightProjectionMatrix.mt);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(LightViewMatrix.mt);


		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 	    
        glShadeModel(GL_FLAT);

		glPolygonOffset(8.0f, 4.0f);
		glEnable(GL_POLYGON_OFFSET_FILL);

	    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, depthFBO);
		glDrawBuffer(GL_DEPTH_ATTACHMENT_EXT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderObjects();		//kkk RenderObjects
		
	    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);


        glShadeModel(GL_SMOOTH);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 
		glDisable(GL_POLYGON_OFFSET_FILL);

	glPopMatrix();
}

void Display()
{
	float buffer[16];

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(view1->fovy, (GLfloat)WinW/(GLfloat)WinW, view1->dnear, view1->dfar);//fovy aspect near far

	glGetFloatv(GL_MODELVIEW_MATRIX, LightProjectionMatrix.mt);
	//LightProjectionMatrix = perspective(view1->fovy, (GLfloat)WinW/(GLfloat)WinW, view1->dnear, view1->dfar);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(	light1->lightList[0].position[0], light1->lightList[0].position[1], light1->lightList[0].position[2],// eye
				0, 20, 0,
				0, 1, 0);    // up

	glGetFloatv(GL_MODELVIEW_MATRIX, LightViewMatrix.mt);
	//LightViewMatrix = look(vec3(view1->eye[0], view1->eye[1], view1->eye[2]), vec3(view1->center[0], view1->center[1], view1->center[2]), vec3(view1->up[0], view1->up[1], view1->up[2]));

	GenShadowMap();

	glViewport(view1->x, view1->y, view1->w, view1->h);											// Reset The Current Viewport
																			
	glMatrixMode(GL_PROJECTION);											// Select The Projection Matrix
	glLoadIdentity();														// Reset The Projection Matrix

	gluPerspective(view1->fovy, (GLfloat)WinW/(GLfloat)WinW, view1->dnear, view1->dfar);		// Calculate The Aspect Ratio Of The Window

	glMatrixMode(GL_MODELVIEW);												// Select The Modelview Matrix
	glLoadIdentity();	

	//ViewMatrix
	gluLookAt(	view1->eye[0], view1->eye[1], view1->eye[2],// eye
				view1->center[0], view1->center[1], view1->center[2],  // center
				view1->up[0], view1->up[1], view1->up[2]);    // up
	

	glGetFloatv(GL_MODELVIEW_MATRIX, ViewMatrixInverse.mt);
	ViewMatrixInverse = ViewMatrixInverse.GetmvMatrixInverse();
/*
				*/
	// clear the buffer
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer

	glEnable(GL_LIGHTING);

	//LightView
	for(int i = 0; i < light1->now; i++)
	{
		glEnable(GL_LIGHT0 + i);
		glLightfv(GL_LIGHT0 + i, GL_POSITION, light1->lightList[i].position);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light1->lightList[i].ambient);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light1->lightList[i].diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light1->lightList[i].specular);
		//cout << "i:" << i << "  " << light1->position[i] << light1->ambient[i] << light1->diffuse[i] << light1->specular[i] << endl;
	}

	glUseProgram(MyShader);

	GLint location = glGetUniformLocation(MyShader, "shadowmap");
	if(location == -1)
		printf("Cant find texture name: shadowmap\n");
	else
		glUniform1i(location, 0);
	
	glEnable(GL_TEXTURE_2D); 
	glBindTexture(GL_TEXTURE_2D, shadowmapid);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

	    glMatrixMode(GL_TEXTURE);					//kkk need look		turn GL_texture and Light_Proj*Light_View*Camera_ViewIn
		glLoadMatrixf(LightProjectionMatrix.mt);
		glMultMatrixf(LightViewMatrix.mt);
		glMultMatrixf(ViewMatrixInverse.mt);	


	    glMatrixMode(GL_MODELVIEW);	
	    RenderObjects();				//kkk RenderObjects

		
    glDisable(GL_TEXTURE_2D);
	
	glViewport(0, 0, 512, 512);
	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, shadowmapid);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 1.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

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


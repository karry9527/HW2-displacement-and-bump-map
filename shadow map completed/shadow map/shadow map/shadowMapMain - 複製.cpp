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

#define		SHADOW_MAP_SIZE 1024
#define		TEX_NUM 2	  //the number of textures you use.
GLuint		texObject[TEX_NUM];	//texture object
GLuint		depthFBO, colorTextureId, ShadowMap, FBO, VBO, UniformLocations[2];
GLhandleARB	MyShader;
mesh			*object1, *object2, *object3;
light*		light1;
view*		view1;
scene		*scene1;
mat4x4		ModelMatrix, ViewMatrix, ViewMatrixInverse, ProjectionMatrix, ProjectionMatrixInverse;
mat4x4		LightViewMatrix, LightProjectionMatrix, ShadowMatrix;
int			WinW,WinH;

void RenderInit();
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
	glUniform1i(UniformLocations[1], 1);
	//printf("ShadowMap: %d\n", UniformLocations[1]);
	glUseProgram(0);
	// set light projection matrix --------------------------------------------------------------------------------------------
	
	LightProjectionMatrix = perspective(view1->fovy, (GLfloat)WinW/(GLfloat)WinH, view1->dnear, view1->dfar);
	/*
	// test
	LightProjectionMatrix = perspective(35.0f, 1.0f, 4.0f, 16.0f); //kkk*/
	
	// generate shadow map texture --------------------------------------------------------------------------------------------

	glGenTextures(1, &ShadowMap);
	glBindTexture(GL_TEXTURE_2D, ShadowMap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	// glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// generate FBO -----------------------------------------------------------------------------------------------------------

	glGenFramebuffersEXT(1, &FBO);

	// set light --------------------------------------------------------------------------------------------------------------
	
	//LightView
	for(int i = 0; i < light1->now; i++)
	{
		glEnable(GL_LIGHT0 + i);
		//some problem about GL_POSITION
		glLightfv(GL_LIGHT0 + i, GL_POSITION, light1->lightList[i].position);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light1->lightList[i].ambient);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light1->lightList[i].diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light1->lightList[i].specular);
		//cout << "i:" << i << "  " << light1->position[i] << light1->ambient[i] << light1->diffuse[i] << light1->specular[i] << endl;
	}
	// test
	/*
	vec3 LightColor = vec3(1.0f, 1.0f, 1.0f);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, &vec4(LightColor * 0.25f, 1.0f));
	glLightfv(GL_LIGHT0, GL_DIFFUSE, &vec4(LightColor * 0.75f, 1.0f));
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0f / 128.0f);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0f / 256.0f);
	*/
	// test end

	// set camera -------------------------------------------------------------------------------------------------------------
	
	vec3 X, Y, Z, Position;

	Position = vec3(view1->eye[0], view1->eye[1], view1->eye[2]);

	// test
	//Position = vec3(1.75f, 1.75f, 5.0f);
	
	Z = normalize(Position);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	//ViewMatrix = mat4x4(view1->eye[0], view1->eye[1], view1->eye[2], 0.0f, view1->center[0], view1->center[1], view1->center[2], 0.0f, view1->up[0], view1->up[1], view1->up[2], 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);

	ViewMatrixInverse = inverse(ViewMatrix);	//kkk

	// ------------------------------------------------------------------------------------------------------------------------
	// test
	float Data[] = 
	{	// s, t, nx, ny, nz, x, y, z
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -10.0f, -0.5f, 10.0f,
		20.0f, 0.0f, 0.0f, 1.0f, 0.0f, 10.0f, -0.5f, 10.0f,
		20.0f, 20.0f, 0.0f, 1.0f, 0.0f, 10.0f, -0.5f, -10.0f,
		0.0f, 20.0f, 0.0f, 1.0f, 0.0f, -10.0f, -0.5f, -10.0f,

		0.0f, 0.0f,  1.0f,  0.0f,  0.0f,  1.5f, -0.5f,  0.5f,
		1.0f, 0.0f,  1.0f,  0.0f,  0.0f,  1.5f, -0.5f, -0.5f,
		1.0f, 1.0f,  1.0f,  0.0f,  0.0f,  1.5f,  0.5f, -0.5f,
		0.0f, 1.0f,  1.0f,  0.0f,  0.0f,  1.5f,  0.5f,  0.5f,
		0.0f, 0.0f, -1.0f,  0.0f,  0.0f,  0.5f, -0.5f, -0.5f,
		1.0f, 0.0f, -1.0f,  0.0f,  0.0f,  0.5f, -0.5f,  0.5f,
		1.0f, 1.0f, -1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,
		0.0f, 1.0f, -1.0f,  0.0f,  0.0f,  0.5f,  0.5f, -0.5f,
		0.0f, 0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,
		1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  1.5f,  0.5f,  0.5f,
		1.0f, 1.0f,  0.0f,  1.0f,  0.0f,  1.5f,  0.5f, -0.5f,
		0.0f, 1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f, -0.5f,
		0.0f, 0.0f,  0.0f, -1.0f,  0.0f,  0.5f, -0.5f, -0.5f,
		1.0f, 0.0f,  0.0f, -1.0f,  0.0f,  1.5f, -0.5f, -0.5f,
		1.0f, 1.0f,  0.0f, -1.0f,  0.0f,  1.5f, -0.5f,  0.5f,
		0.0f, 1.0f,  0.0f, -1.0f,  0.0f,  0.5f, -0.5f,  0.5f,
		0.0f, 0.0f,  0.0f,  0.0f,  1.0f,  0.5f, -0.5f,  0.5f,
		1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  1.5f, -0.5f,  0.5f,
		1.0f, 1.0f,  0.0f,  0.0f,  1.0f,  1.5f,  0.5f,  0.5f,
		0.0f, 1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,
		0.0f, 0.0f,  0.0f,  0.0f, -1.0f,  1.5f, -0.5f, -0.5f,
		1.0f, 0.0f,  0.0f,  0.0f, -1.0f,  0.5f, -0.5f, -0.5f,
		1.0f, 1.0f,  0.0f,  0.0f, -1.0f,  0.5f,  0.5f, -0.5f,
		0.0f, 1.0f,  0.0f,  0.0f, -1.0f,  1.5f,  0.5f, -0.5f,

		0.0f, 0.0f,  1.0f,  0.0f,  0.0f, -0.5f, -0.5f,  0.5f,
		1.0f, 0.0f,  1.0f,  0.0f,  0.0f, -0.5f, -0.5f, -0.5f,
		1.0f, 1.0f,  1.0f,  0.0f,  0.0f, -0.5f,  0.5f, -0.5f,
		0.0f, 1.0f,  1.0f,  0.0f,  0.0f, -0.5f,  0.5f,  0.5f,
		0.0f, 0.0f, -1.0f,  0.0f,  0.0f, -1.5f, -0.5f, -0.5f,
		1.0f, 0.0f, -1.0f,  0.0f,  0.0f, -1.5f, -0.5f,  0.5f,
		1.0f, 1.0f, -1.0f,  0.0f,  0.0f, -1.5f,  0.5f,  0.5f,
		0.0f, 1.0f, -1.0f,  0.0f,  0.0f, -1.5f,  0.5f, -0.5f,
		0.0f, 0.0f,  0.0f,  1.0f,  0.0f, -1.5f,  0.5f,  0.5f,
		1.0f, 0.0f,  0.0f,  1.0f,  0.0f, -0.5f,  0.5f,  0.5f,
		1.0f, 1.0f,  0.0f,  1.0f,  0.0f, -0.5f,  0.5f, -0.5f,
		0.0f, 1.0f,  0.0f,  1.0f,  0.0f, -1.5f,  0.5f, -0.5f,
		0.0f, 0.0f,  0.0f, -1.0f,  0.0f, -1.5f, -0.5f, -0.5f,
		1.0f, 0.0f,  0.0f, -1.0f,  0.0f, -0.5f, -0.5f, -0.5f,
		1.0f, 1.0f,  0.0f, -1.0f,  0.0f, -0.5f, -0.5f,  0.5f,
		0.0f, 1.0f,  0.0f, -1.0f,  0.0f, -1.5f, -0.5f,  0.5f,
		0.0f, 0.0f,  0.0f,  0.0f,  1.0f, -1.5f, -0.5f,  0.5f,
		1.0f, 0.0f,  0.0f,  0.0f,  1.0f, -0.5f, -0.5f,  0.5f,
		1.0f, 1.0f,  0.0f,  0.0f,  1.0f, -0.5f,  0.5f,  0.5f,
		0.0f, 1.0f,  0.0f,  0.0f,  1.0f, -1.5f,  0.5f,  0.5f,
		0.0f, 0.0f,  0.0f,  0.0f, -1.0f, -0.5f, -0.5f, -0.5f,
		1.0f, 0.0f,  0.0f,  0.0f, -1.0f, -1.5f, -0.5f, -0.5f,
		1.0f, 1.0f,  0.0f,  0.0f, -1.0f, -1.5f,  0.5f, -0.5f,
		0.0f, 1.0f,  0.0f,  0.0f, -1.0f, -0.5f,  0.5f, -0.5f
	};
	//kkk look
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 1664, Data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//test end
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

/*void GenShadowMap()
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
}*/

void Display()
{
	// test
	
	//static vec3 LightPosition = vec3(0.0f, 2.5f, 5.0f);

	RenderInit();
	//Pass1
	glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&LightProjectionMatrix);
	
	// viewing and modeling transformation
	
	LightViewMatrix = look(vec3(light1->lightList[0].position[0], 0, light1->lightList[0].position[2]),
					 vec3(0.0f, 0.0f, 0.0f), 
					 vec3(0.0f, 1.0f, 0.0f));
	
	// test
	//LightViewMatrix = look(LightPosition, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f)); //kkk glmath.h
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&LightViewMatrix);
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
	glDrawBuffers(0, NULL); glReadBuffer(GL_NONE);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, ShadowMap, 0);
	
	glClear(GL_DEPTH_BUFFER_BIT);
	
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	RenderObjects();
	
	
	// test
	/*
	glCullFace(GL_FRONT);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 32, (void*)20);
	glDrawArrays(GL_QUADS, 0, 52);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glCullFace(GL_BACK);
	*/
	//test end
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	//Pass2

	glViewport(view1->x, view1->y, view1->w, view1->h);						// Reset The Current Viewport
	
	// test
	//glViewport(0, 0, 800, 600);

	glMatrixMode(GL_PROJECTION);											// Select The Projection Matrix
	
	ProjectionMatrix = perspective(view1->fovy, (GLfloat)WinW/(GLfloat)WinH, view1->dnear, view1->dfar); //kkk
	//ProjectionMatrix = perspective(45.0f, (GLfloat)WinW/(GLfloat)WinH, 0.125f, 512.0f);
	ProjectionMatrixInverse = inverse(ProjectionMatrix);
	
	// test
	/*
	ProjectionMatrix = perspective(45.0f, 800.0f / 600.0f, 0.125f, 512.0f);
	ProjectionMatrixInverse = inverse(ProjectionMatrix);
	*/
	
	//test end
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&ViewMatrix);

	for(int i = 0; i < light1->now; i++)
	{
		glEnable(GL_LIGHT0 + i);
		//some problem about GL_POSITION
		glLightfv(GL_LIGHT0 + i, GL_POSITION, light1->lightList[i].position);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light1->lightList[i].ambient);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light1->lightList[i].diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light1->lightList[i].specular);
		//cout << "i:" << i << "  " << light1->position[i] << light1->ambient[i] << light1->diffuse[i] << light1->specular[i] << endl;
	}
	// test
	
	//glLightfv(GL_LIGHT0, GL_POSITION, &vec4(LightPosition, 1.0f));
	

	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	
	glUseProgram(MyShader);
	ShadowMatrix = BiasMatrix * LightProjectionMatrix * LightViewMatrix * ViewMatrixInverse; //kkk
	

	//GLint location = glGetUniformLocation(MyShader, "ShadowMatrix");
	//if(location == -1)
	//	printf("Cant find texture name: ShadowMatrix\n");
	//else

		glUniformMatrix4fv(UniformLocations[0], 1, GL_FALSE, &ShadowMatrix);
	//printf("ShadowMapping:%d", UniformLocations[0]);

	glBindTexture(GL_TEXTURE_2D, ShadowMap);
	
	
	RenderObjects();				//kkk RenderObjects
	
	//test
	/*
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 32, (void*)0);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 32, (void*)8);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 32, (void*)20);
	//glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, Texture[0]);
	glColor3f(1.0f, 1.0f, 1.0f);
	glDrawArrays(GL_QUADS, 0, 4);
	//glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, Texture[1]);
	glColor3f(1.0f, 1.0f, 1.0f);
	glDrawArrays(GL_QUADS, 4, 48);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	

	glDisable(GL_CULL_FACE);

	*/
	//test end


	glUseProgram(0);

	glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
	
	
	glViewport(0, 0, 512, 512);
	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ShadowMap);
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


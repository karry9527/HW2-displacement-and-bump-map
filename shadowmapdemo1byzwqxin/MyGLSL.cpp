// MyGLSL.cpp: implementation of the MyGLSL class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyGLSL.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyGLSL::MyGLSL(): vertshader(0), fragshader(0)
{

}


MyGLSL::~MyGLSL()
{
  glDeleteProgram(shaderprogram);
}

char* MyGLSL::textFileRead(const GLchar *filename) 
{


	FILE *fp;
	char *content = NULL;

	int count=0;

	if (filename != NULL) {
		fp = fopen(filename,"rt");

		if (fp != NULL) {
      
      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      rewind(fp);

			if (count > 0) {
				content = new char[sizeof(char) * (count+1)];
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

void MyGLSL::setshader(const GLchar* vertfile, const GLchar* fragfile)
{
	char *vs = NULL,*fs = NULL;
    const char *use_vs, *use_fs;

    glewInit();	
    shaderprogram = glCreateProgram();

    vertshader = glCreateShader(GL_VERTEX_SHADER);
	fragshader = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead(vertfile);
	fs = textFileRead(fragfile);

	use_vs = vs;
	use_fs = fs;

	glShaderSource(vertshader, 1, &use_vs, NULL);
	glShaderSource(fragshader, 1, &use_fs, NULL);

	free(vs);
	free(fs);

	glCompileShader(vertshader);
	glCompileShader(fragshader);


	glAttachShader(shaderprogram, vertshader);
	glAttachShader(shaderprogram, fragshader);

	glLinkProgram(shaderprogram);

}

void MyGLSL::attachshader(GLuint id, GLenum type, const GLchar* shaderfile)
{
    CShaderX new_shader;
	char *file = NULL;
    const char *use_file;

    new_shader.handler = glCreateShader(type);

	file = textFileRead(shaderfile);

	use_file = file;

	glShaderSource(new_shader.handler, 1, &use_file, NULL);

	free(file);

	glCompileShader(new_shader.handler);

	glAttachShader(shaderprogram, new_shader.handler);

	new_shader.id = id;
	newshader.push_back(new_shader);
}

void MyGLSL::detachshader(GLuint id)
{
	for(vector<CShaderX>::iterator psh = newshader.begin();psh != newshader.end();psh++)
		if( (*psh).id == id)
	     glDetachShader(shaderprogram, (*psh).handler);
}

///////////////////////uniform (float)
void MyGLSL::SendUniform(const GLchar *UniformName, float x)							  
{
   //glUseProgram(shaderprogram);
   GLint location = glGetUniformLocation(shaderprogram,UniformName);
   glUniform1f(location, x);
   //glUseProgram(0);
}

void MyGLSL::SendUniform(const GLchar *UniformName, float x, float y)							  
{
   GLint location = glGetUniformLocation(shaderprogram,UniformName);
   glUniform2f(location, x, y);
}

void MyGLSL::SendUniform(const GLchar *UniformName, float x, float y, float z)							  
{
   GLint location = glGetUniformLocation(shaderprogram,UniformName);
   glUniform3f(location, x, y, z);
}

void MyGLSL::SendUniform(const GLchar *UniformName, float x, float y, float z,float w)							  
{
   GLint location = glGetUniformLocation(shaderprogram,UniformName);
   glUniform4f(location, x, y, z, w);
}

///////////////////////uniform (int)
void MyGLSL::SendUniform(const GLchar *UniformName, int x)							  
{
   GLint location = glGetUniformLocation(shaderprogram,UniformName);
   glUniform1i(location, x);
}

void MyGLSL::SendUniform(const GLchar *UniformName, int x, int y)							  
{
   GLint location = glGetUniformLocation(shaderprogram,UniformName);
   glUniform2i(location, x, y);
}

void MyGLSL::SendUniform(const GLchar *UniformName, int x, int y, int z)							  
{
   GLint location = glGetUniformLocation(shaderprogram,UniformName);
   glUniform3i(location, x, y, z);
}

void MyGLSL::SendUniform(const GLchar *UniformName, int x, int y, int z,int w)							  
{
   GLint location = glGetUniformLocation(shaderprogram,UniformName);
   glUniform4i(location, x, y, z, w);
}

/////////////////uniform matrix
void MyGLSL::SendUniform(const GLchar *UniformName, GLuint size, float *matrix, bool en_transpose)
{
   GLint location = glGetUniformLocation(shaderprogram,UniformName);
	switch(size)
	{
	case '2':
		glUniformMatrix2fv(location, 0, en_transpose, matrix);
		break;
	case '3':
		glUniformMatrix3fv(location, 0, en_transpose, matrix);
		break;
	case '4':
		glUniformMatrix4fv(location, 0, en_transpose, matrix);
		break;
	}
}
							  

//////////////attribute
void MyGLSL::setAttributeLocation(const GLchar *AttributeName, GLuint location)
{
	glBindAttribLocation(shaderprogram,location, AttributeName);
}

void MyGLSL::validate() const
{
	glValidateProgram(shaderprogram);
}

bool MyGLSL::IsValidProgram() const
{
	GLint status;
	glGetProgramiv(shaderprogram, GL_VALIDATE_STATUS, &status);
	if(!status)
		return false;
	else
		return true;
}

void MyGLSL::GetProgramLog(GLchar* &log) const
{
	GLchar *debug;
	GLint debugLength;
	glGetProgramiv(shaderprogram, GL_INFO_LOG_LENGTH, &debugLength);

	debug = new GLchar[debugLength];
	glGetProgramInfoLog(shaderprogram, debugLength, &debugLength, debug);

	//cout << debug;
	strcpy(log,debug);
	delete [] debug;
}

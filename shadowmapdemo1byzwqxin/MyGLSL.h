// MyGLSL.h: interface for the MyGLSL class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYGLSL_H__CCE00280_6E6A_4484_A2A4_7025F2FF557A__INCLUDED_)
#define AFX_MYGLSL_H__CCE00280_6E6A_4484_A2A4_7025F2FF557A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <vector>
using namespace std;

class MyGLSL  
{
public:
	MyGLSL();
	~MyGLSL();

	inline void enable(){ glUseProgram(shaderprogram); };
	inline void disable(){ glUseProgram(0); };
	inline GLuint getProgramHandler() const{return shaderprogram;};

	void setshader(const char* vertfile, const char* fragfile);
    void attachshader(GLuint id, GLenum type, const char* shaderfile);
    void detachshader(GLuint id);
    void validate() const;
    bool IsValidProgram() const;
    void GetProgramLog(GLchar* &log) const;

	///////////////////////uniform (float)
	void SendUniform(const GLchar *UniformName, float x);
	void SendUniform(const GLchar *UniformName, float x, float y);
	void SendUniform(const GLchar *UniformName, float x, float y, float z);
	void SendUniform(const GLchar *UniformName, float x, float y, float z,float w);
	///////////////////////uniform (int)
	void SendUniform(const GLchar *UniformName, int x);
	void SendUniform(const GLchar *UniformName, int x, int y);
	void SendUniform(const GLchar *UniformName, int x, int y, int z);
	void SendUniform(const GLchar *UniformName, int x, int y, int z,int w);
	/////////////////uniform matrix
	void SendUniform(const GLchar *UniformName, GLuint size, float *matrix, bool en_transpose);
	//////////////attribute
	void setAttributeLocation(const GLchar *AttributeName, GLuint location);
private:
	char* textFileRead(const char *filename) ;

	GLuint shaderprogram;
	GLuint vertshader;
	GLuint fragshader;

    typedef struct CShaderX{
        GLuint handler;
		GLuint id;
	} CShaderX;

	vector<CShaderX> newshader;

};

#endif // !defined(AFX_MYGLSL_H__CCE00280_6E6A_4484_A2A4_7025F2FF557A__INCLUDED_)

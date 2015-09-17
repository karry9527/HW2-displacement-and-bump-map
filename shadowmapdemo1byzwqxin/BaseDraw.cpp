#include "stdafx.h"
#include "BaseDraw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BaseDraw::BaseDraw()
{

}

BaseDraw::~BaseDraw()
{

}

void BaseDraw::drawpoint(float point_xs,float point_ys,float point_zs, float size)//画点函数
{   
	glPointSize(size);
    glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_POINTS);
        glVertex3f(point_xs, point_ys, point_zs);
	glEnd();
}

void BaseDraw::drawquad(int around_axis, GLfloat quad_xsize, GLfloat quad_ysize, GLfloat quad_zsize, 
						GLfloat s_repeat, GLfloat t_repeat, bool auto_normal)
{      

	switch(around_axis)
	{
case Z:	
	if(quad_zsize<=0.0)//矩形在后
	{
	 if(auto_normal)glNormal3f(0.0, 0.0,  1.0);
	 glBegin(GL_QUADS);       //逆时针作图
	    glTexCoord2f(0.0,      0.0);     glVertex3f(-quad_xsize/2, -quad_ysize/2,  quad_zsize);
		glTexCoord2f(s_repeat, 0.0);     glVertex3f( quad_xsize/2, -quad_ysize/2,  quad_zsize);
		glTexCoord2f(s_repeat, t_repeat);glVertex3f( quad_xsize/2,  quad_ysize/2,  quad_zsize);		
   	    glTexCoord2f(0.0,      t_repeat);glVertex3f(-quad_xsize/2,  quad_ysize/2,  quad_zsize);
	 glEnd();	
	}
	else              //矩形在前
	{
	if(auto_normal)glNormal3f(0.0, 0.0, -1.0);
	glBegin(GL_QUADS);      
	    glTexCoord2f(0.0,      0.0);     glVertex3f( quad_xsize/2, -quad_ysize/2,  quad_zsize);
		glTexCoord2f(s_repeat, 0.0);     glVertex3f(-quad_xsize/2, -quad_ysize/2,  quad_zsize);
		glTexCoord2f(s_repeat, t_repeat);glVertex3f(-quad_xsize/2,  quad_ysize/2,  quad_zsize);		
   	    glTexCoord2f(0.0,      t_repeat);glVertex3f( quad_xsize/2,  quad_ysize/2,  quad_zsize);
	glEnd();
	}	
break;
case X:
	if(quad_xsize<=0.0)//矩形在左
	{
	if(auto_normal)glNormal3f(1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	    glTexCoord2f(0.0,      0.0);     glVertex3f( quad_xsize, -quad_ysize/2,  quad_zsize/2);
		glTexCoord2f(s_repeat, 0.0);     glVertex3f( quad_xsize, -quad_ysize/2, -quad_zsize/2);    	
		glTexCoord2f(s_repeat, t_repeat);glVertex3f( quad_xsize,  quad_ysize/2, -quad_zsize/2);
		glTexCoord2f(0.0,      t_repeat);glVertex3f( quad_xsize,  quad_ysize/2,  quad_zsize/2);
	glEnd();
	}
	else              //矩形在右
	{
	if(auto_normal)glNormal3f(-1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	    glTexCoord2f(0.0,      0.0);     glVertex3f( quad_xsize, -quad_ysize/2, -quad_zsize/2);
		glTexCoord2f(s_repeat, 0.0);     glVertex3f( quad_xsize, -quad_ysize/2,  quad_zsize/2);    	
		glTexCoord2f(s_repeat, t_repeat);glVertex3f( quad_xsize,  quad_ysize/2,  quad_zsize/2);
		glTexCoord2f(0.0,      t_repeat);glVertex3f( quad_xsize,  quad_ysize/2, -quad_zsize/2);
	glEnd();
	}
break;
case Y:
	if(quad_ysize<=0.0)//矩形在下
	{
	if(auto_normal)glNormal3f(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0,      0.0);     glVertex3f(-quad_xsize/2,  quad_ysize,  quad_zsize/2);
		glTexCoord2f(s_repeat, 0.0);     glVertex3f( quad_xsize/2,  quad_ysize,  quad_zsize/2);   	
		glTexCoord2f(s_repeat, t_repeat);glVertex3f( quad_xsize/2,  quad_ysize, -quad_zsize/2);
		glTexCoord2f(0.0,      t_repeat);glVertex3f(-quad_xsize/2,  quad_ysize, -quad_zsize/2);
	glEnd();
	}
	else              //矩形在上
	{
	if(auto_normal)glNormal3f(0.0, -1.0, 0.0);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0,      0.0);     glVertex3f(-quad_xsize/2,  quad_ysize, -quad_zsize/2);
		glTexCoord2f(s_repeat, 0.0);     glVertex3f( quad_xsize/2,  quad_ysize, -quad_zsize/2);   	
		glTexCoord2f(s_repeat, t_repeat);glVertex3f( quad_xsize/2,  quad_ysize,  quad_zsize/2);
		glTexCoord2f(0.0,      t_repeat);glVertex3f(-quad_xsize/2,  quad_ysize,  quad_zsize/2);
	glEnd();
	}
break;	
	}
}



void BaseDraw::hemisphere(double R, int Slice ,int Stack, double FieldAngle)
{
	double width = R-R*cos(FieldAngle*PI/(2*180));   

		glBegin(GL_TRIANGLE_FAN);
		    glNormal3f(0, 0, 1);
		    glTexCoord2f(0.5,  0.5);
			glVertex3f(0,0,0);

			double t1= R-width/Stack;
			double FlatScale = sqrt(R*R-t1*t1);

			for(int j = 0; j <= Slice; j++)
			{
				double sita = double(j)*2.0*PI/Slice;

				glNormal3d(FlatScale*cos(sita), FlatScale*sin(sita), R-width/Stack);
	            glTexCoord2d(0.5 + 0.5*cos(sita)*FlatScale/R,  0.5 +  0.5*sin(sita)*FlatScale/R);
				glVertex3d(FlatScale*cos(sita), FlatScale*sin(sita), -width/Stack); 	
				
			}
		glEnd();

		glBegin(GL_QUAD_STRIP);
		for(int i = 1; i < Stack; i++)
		{
			double t1= R-i*width/Stack;
			double t2 =R-(i+1)*width/Stack;
			double r1 = sqrt(R*R-t1*t1);
			double r2 = sqrt(R*R-t2*t2);

			for(int j = 0; j <= Slice; j++)
			{
				double sita = double(j)*2.0*PI/Slice;
				double x = r1*cos(sita);
				double y = r1*sin(sita);
				glNormal3d(x, y, t1);
			    glTexCoord2d(0.5 + 0.5*r1*cos(sita)/R,  0.5 +  0.5*r1*sin(sita)/R);
                glVertex3d(x,y, -(R-t1));
				
				x = r2*cos(sita);
				y = r2*sin(sita);
				glNormal3d(x, y, t2);
			    glTexCoord2d(0.5 + 0.5*r2*cos(sita)/R,  0.5 +  0.5*r2*sin(sita)/R);	   
				glVertex3d(x,y,-(R-t2));

			}
		}
		glEnd();
}

void BaseDraw::drawcircle(double radius, double angvail)
{      //画圆函数，用线段连接拟合,参数为半径和可见弧度	 
	 double angle=0.0, n=365.0, x=0.0, z=0.0;

      glBegin(GL_LINE_STRIP);
         for(int i=0;i<=angvail;i++)
		 {
	     angle=2*i*PI/n;
         x=radius*cos(angle);
	     z=radius*sin(angle);
         glVertex3d(x,0,z);
		 }
       glEnd();
}


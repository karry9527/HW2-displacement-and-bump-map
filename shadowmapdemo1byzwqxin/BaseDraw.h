
// BaseDraw.h: interface for the BaseDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEDRAW_H__81F9BB59_7B67_4A7C_8B7D_1942A103F634__INCLUDED_)
#define AFX_BASEDRAW_H__81F9BB59_7B67_4A7C_8B7D_1942A103F634__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <math.h>

#ifndef  PI
#define  PI    3.1415926
#endif

	const int Z=1; //沿Z轴移动，Z分量表示位移量（下类似）
	const int X=2;
	const int Y=3;

class BaseDraw
{
private:
          
public:

	  BaseDraw();
	  virtual ~BaseDraw();

  	  static void drawpoint(float point_xs,float point_ys,float point_zs ,float size = 5);
	  static void drawquad(int around_axis, GLfloat quad_xsize, GLfloat quad_ysize, GLfloat quad_zsize, 
						GLfloat s_repeat, GLfloat t_repeat, bool auto_normal);
      static void hemisphere(double R, int Slice ,int Stack, double FieldAngle);
      static void drawcircle(double radius, double angvail);


protected: 


 }; 

#endif // !defined(AFX_BASEDRAW_H__81F9BB59_7B67_4A7C_8B7D_1942A103F634__INCLUDED_)
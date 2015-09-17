// Camera.h: interface for the CCamera class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CCAMERA_H
#define CCAMERA_H

#include "stdafx.h"  
#include <stdlib.h>
#include <windows.h> 
 
#include<math.h>                          // Math header that allows us to use cos() and sin().

#define UP              0.01f             // Forward speed.
#define DOWN            -0.01f            // Backward speed.
#define LEFT            0.03f             // Left speed.
#define RIGHT           -0.03f            // Right speed.
#define STRAFE_LEFT     -0.01f            // Left straft speed.
#define STRAFE_RIGHT    0.01f             // Right straft speed.


#define  MOVE_ALONG_X    1
#define  MOVE_ALONG_Y    2
#define  MOVE_ALONG_Z    3

#define  PLUS_DIRECTION    1
#define  MINUS_DIRECTION  -1

#ifndef  PI
#define  PI    3.1415926
#endif

class CCamera
{
   public:
	    
	  CCamera();
	  virtual ~CCamera();

	  void SetRotationSpeed(float rotate_speed);		    // 设置相机旋转速度
	  void SetTranslationSpeed(float translate_speed);	    // 设置相机移动速度

	  void Rotate(int oldx, int oldy, int x, int y);		// 旋转相机	
	  void Move(int direction);                             // 沿视线方向移动相机
	  void Translate(unsigned int axis, int direction);		//  沿坐标轴平移相机

	  void Look(); 

	  //	形参与gluLookAt形参相一致
	  void ReLocate(float eye_x, float eye_y, float eye_z,
		                         float dir_x, float dir_y, float dir_z,
								              float up_x, float up_y, float up_z);	//	把相机按新位置重置
 
	  double m_fEye[3];        //相机位置
	  double m_fDir[3];        //相机视线方向，单位向量
	  double m_fSightLength;   //相机位置到相机视点的距离


      double m_fAngleYaw;      //偏航
	  double m_fAnglePitch;    //俯仰
	  double m_fRotationSpeed;        //相机旋转速度
	  double m_fTranslationSpeed;     //相机移动速度	
	 
	  
	  double calookx,calooky,calookz;
   
   protected:    
       double m_fUp[3];         //单位向量
      


 }; 

#endif

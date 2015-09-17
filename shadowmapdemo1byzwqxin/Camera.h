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

	  void SetRotationSpeed(float rotate_speed);		    // ���������ת�ٶ�
	  void SetTranslationSpeed(float translate_speed);	    // ��������ƶ��ٶ�

	  void Rotate(int oldx, int oldy, int x, int y);		// ��ת���	
	  void Move(int direction);                             // �����߷����ƶ����
	  void Translate(unsigned int axis, int direction);		//  ��������ƽ�����

	  void Look(); 

	  //	�β���gluLookAt�β���һ��
	  void ReLocate(float eye_x, float eye_y, float eye_z,
		                         float dir_x, float dir_y, float dir_z,
								              float up_x, float up_y, float up_z);	//	���������λ������
 
	  double m_fEye[3];        //���λ��
	  double m_fDir[3];        //������߷��򣬵�λ����
	  double m_fSightLength;   //���λ�õ�����ӵ�ľ���


      double m_fAngleYaw;      //ƫ��
	  double m_fAnglePitch;    //����
	  double m_fRotationSpeed;        //�����ת�ٶ�
	  double m_fTranslationSpeed;     //����ƶ��ٶ�	
	 
	  
	  double calookx,calooky,calookz;
   
   protected:    
       double m_fUp[3];         //��λ����
      


 }; 

#endif

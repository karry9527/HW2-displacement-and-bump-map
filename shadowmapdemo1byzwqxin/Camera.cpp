// Camera.cpp: implementation of the CCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Camera.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCamera::CCamera()
{
	m_fEye[0]=0.0f; m_fEye[1]=1.6f; m_fEye[2]=1.0f;
	calookx=0.0f; calooky=1.6f; calookz=0.0f;
	m_fAngleYaw=0.0f, m_fAnglePitch=0.0f;
	m_fSightLength = 1.0f; 

	m_fRotationSpeed = 0.0f;
	m_fTranslationSpeed = 0.0f;

}

CCamera::~CCamera()
{

}

void CCamera::SetRotationSpeed(float rotate_speed)
{
	m_fRotationSpeed = rotate_speed;        
}

void CCamera::SetTranslationSpeed(float rotate_speed)
{
	m_fTranslationSpeed = rotate_speed;     
}

void CCamera::Rotate(int oldx, int oldy, int x, int y)
{
	 m_fAngleYaw += m_fRotationSpeed*(oldx-x)*PI/360;       //根据鼠标的移动改变旋转的偏航
	 m_fAnglePitch -= m_fRotationSpeed*(oldy-y)*PI/360;     //根据鼠标的移动改变旋转的俯仰 

	 m_fDir[0] = sin(m_fAngleYaw)*cos(m_fAnglePitch); 
	 m_fDir[1] = sin(m_fAnglePitch);
	 m_fDir[2] = -cos(m_fAngleYaw)*cos(m_fAnglePitch);

	 m_fUp[0] = -sin(m_fAngleYaw)*sin(m_fAnglePitch);
	 m_fUp[1] = cos(m_fAnglePitch);
	 m_fUp[2] = sin(m_fAnglePitch)*cos(m_fAngleYaw);
}

void CCamera::Move(int direction)
{
	 m_fEye[0] = m_fEye[0] + m_fTranslationSpeed*direction*(m_fDir[0]); 
	 m_fEye[1] = m_fEye[1] + m_fTranslationSpeed*direction*(m_fDir[1]); 
     m_fEye[2] = m_fEye[2] + m_fTranslationSpeed*direction*(m_fDir[2]); 
}

void CCamera::Translate(unsigned int axis, int direction)
{
	switch(axis)
	{
	case MOVE_ALONG_X:
	    m_fEye[0] = m_fEye[0] + m_fTranslationSpeed*direction; 
	 break;
	case  MOVE_ALONG_Y:
	    m_fEye[1] = m_fEye[1] + m_fTranslationSpeed*direction;
	 break;
	case  MOVE_ALONG_Z:
		m_fEye[2] = m_fEye[2] + m_fTranslationSpeed*direction; 
	 break;
	}
}

void CCamera::Look()
{   
		calookx=m_fEye[0]+m_fSightLength*m_fDir[0];
		calooky=m_fEye[1]+m_fSightLength*m_fDir[1];
		calookz=m_fEye[2]+m_fSightLength*m_fDir[2];

gluLookAt(m_fEye[0], m_fEye[1], m_fEye[2], calookx,calooky,calookz, m_fUp[0], m_fUp[1], m_fUp[2]); 
}

void CCamera::ReLocate(float eye_x, float eye_y, float eye_z, float dir_x, float dir_y, float dir_z, float up_x, float up_y, float up_z)
{
	m_fEye[0]=eye_x, m_fEye[1]=eye_y, m_fEye[2]=eye_z; 

	float dx = dir_x- eye_x;
	float dy = dir_y- eye_y;
	float dz = dir_z- eye_z;

	m_fSightLength = sqrt(dx*dx+dy*dy+dz*dz);
	m_fDir[0]=dx/m_fSightLength;
	m_fDir[1]=dy/m_fSightLength;
	m_fDir[2]=dz/m_fSightLength; 

	m_fUp[0]=up_x, m_fUp[1]=up_y, m_fUp[2]=up_z; 
	m_fAngleYaw = asin(m_fDir[0]);
    m_fAnglePitch = asin(m_fDir[1]);
}

// MainFrame.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRAME_H__D7629C65_B0FF_11D4_A3B0_0050DA731472__INCLUDED_)
#define AFX_MAINFRAME_H__D7629C65_B0FF_11D4_A3B0_0050DA731472__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Camera.h"
#include "3DS.h"
#include "Matrix16.h"
#include "MyGLSL.h"

class CMainFrame : public CFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

    void setlight();
	void Eyelimitation();

    void SetFrameBufferObject(int fbowidth, int fboheight);
	void clearFrameBufferObject();

	void RenderObjects();
    void GenShadowMap();
    void CastShadowMap();
//Attributes
protected:
	HDC m_hDC;								// Device Context
	HGLRC m_hRC;							// Rendering Context
	UINT m_cxClient;
	UINT m_cyClient;
	HPALETTE m_hPal;
	int VB_WIDTH;
	int VB_HEIGHT;
	int VB_DEPTH;
	DEVMODE	m_DMsaved;				        // Saves The Previous Screen Settings 
	BOOL m_bFullScreen;
	//frame calcs and time since last frame
	float m_fStartFrameTime;
	float m_fEndFrameTime;
	float m_fTime;
	int m_iFrames;
	int m_iFramesPerSec;

	
	bool EnRot;
	bool Select3DS;
	GLuint ShowLightDir;
	GLfloat jumpa,jumpb;
	GLfloat Lightpos[4];
	GLfloat Lightlookat[3];

	CCamera camera;
	CLoad3DS* shadowmodel;

	MyGLSL myshader;
	CMatrix16 Light_ProjectionMatrix;
	CMatrix16 Light_ViewMatrix;
	CMatrix16 Camera_ViewInverse;


	int SHADOWSCREEN_WIDTH; 
	int SHADOWSCREEN_HEIGHT;

	GLuint depthFBO;
	GLuint shadowmapid;
    GLuint colorTextureId;

	bool			m_LeftButtonDown;
	CPoint			m_LeftDownPos; 
// Operations
public:
	void RenderGLScene();
	BOOL m_bAppIsActive;
	GLvoid ReSizeGLScene(GLsizei width, GLsizei height);
	int InitGL(GLvoid);	
	GLvoid KillGLWindow(GLvoid);
	void calculateFramesPerSec ();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRAME_H__D7629C65_B0FF_11D4_A3B0_0050DA731472__INCLUDED_)

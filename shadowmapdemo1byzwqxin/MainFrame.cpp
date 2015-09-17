// MainFrame.cpp : implementation of the CMainFrame class
//
/*****************************
  # # ##  欢迎光临 zwqxin的3D图形编程记事簿  http://www.zwqxin.com  
  # # ##  Shadow Map Demo1  by zwqxin 2009.2.14
*////////////////////////////


#include "stdafx.h"
#include "shadowmap.h"

#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_ACTIVATEAPP()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	VB_WIDTH = 1024;
	VB_HEIGHT = 768;
	VB_DEPTH  = 32;
	m_bFullScreen = FALSE;
	m_hDC = NULL;
	m_hRC = NULL;
	m_cxClient = 0;
	m_cyClient = 0;
	m_hPal = NULL;
	m_bAppIsActive = FALSE;

	// calculateFramesPerSec initialization code

	m_fStartFrameTime = 0.0;
	m_fEndFrameTime = 0.0;
	m_fTime = 0.0;
	m_iFrames = 0;
	m_iFramesPerSec = 0;

    m_bFullScreen=FALSE;														// Windowed Mode

	if (MessageBox("加载汽车模型(YES)  加载几个一般几何模型(NO)", "Load Model",MB_YESNO|MB_ICONQUESTION)==IDYES)
	{
		Select3DS=FALSE;														// Windowed Mode
	}
	else Select3DS = TRUE;

	ShowLightDir = 0;
	EnRot = TRUE;
	jumpa = jumpb  = 0.0;

	Lightpos[0] = 0.5f;
	Lightpos[1] = 5.0f;
	Lightpos[2] = 23.0f;
    Lightpos[3] = 0.0f;

	Lightlookat[0] = Lightlookat[1] = Lightlookat[2] = 0.0f;

	SHADOWSCREEN_WIDTH = VB_WIDTH ;
	SHADOWSCREEN_HEIGHT= VB_HEIGHT;

	camera.ReLocate(0.0, 5.0, 25.0, 0.0, 0.0, 0.0, 0, 1, 0);

	m_LeftButtonDown = FALSE;
	m_LeftDownPos = (0,0);
}

CMainFrame::~CMainFrame()
{
	KillGLWindow();	// Shutdown

	
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame PreCreateWindow

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &m_DMsaved);					// save the current display state 


	if (m_bFullScreen)															// Attempt Fullscreen Mode?
	{																		
		DEVMODE dmScreenSettings;												// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));					// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);						// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= VB_WIDTH;								// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= VB_HEIGHT;							// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= VB_DEPTH;								// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.

		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.

			if (MessageBox("The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				m_bFullScreen=FALSE;											// Windowed Mode Selected.
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.

				MessageBox("Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;												
			}
		}		
	}


	// TODO: Modify the Window class or styles here by modifying
	// the CREATESTRUCT cs

	cs.cx = VB_WIDTH;
	cs.cy = VB_HEIGHT;

	if (m_bFullScreen)														// Are We Still In Fullscreen Mode?
	{																		
		cs.dwExStyle=WS_EX_APPWINDOW;										// Window Extended Style
		cs.style=WS_POPUP;													// Windows Style
		ShowCursor(FALSE);													// Hide Mouse Pointer
	}																		
	else																	
	{																		
		cs.dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;					// Window Extended Style
		cs.style=WS_OVERLAPPEDWINDOW;										// Windows Style
		cs.y = (int) GetSystemMetrics(SM_CYSCREEN) / 2 - cs.cy / 2; 
		cs.x = (int) GetSystemMetrics(SM_CXSCREEN) / 2 - cs.cx / 2;
	}

	cs.lpszClass = AfxRegisterWndClass(CS_OWNDC|CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,
		LoadCursor(NULL,IDC_ARROW), NULL, NULL);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


/////////////////////////////////////////////////////////////////////////////
// CMainFrame OnCreateClient

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	BOOL bRet = CFrameWnd::OnCreateClient(lpcs, pContext);
	if (bRet)
	{
		GLuint	PixelFormat;												// Holds The Results After Searching For A Match
		static	PIXELFORMATDESCRIPTOR pfd=									// pfd Tells Windows How We Want Things To Be
		{																		
			sizeof(PIXELFORMATDESCRIPTOR),									// Size Of This Pixel Format Descriptor
			1,																// Version Number
			PFD_DRAW_TO_WINDOW |											// Format Must Support Window
			PFD_SUPPORT_OPENGL |											// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,												// Must Support Double Buffering
			PFD_TYPE_RGBA,													// Request An RGBA Format
			VB_DEPTH,														// Select Our Color Depth
			0, 0, 0, 0, 0, 0,												// Color Bits Ignored
			0,																// No Alpha Buffer
			0,																// Shift Bit Ignored
			0,																// No Accumulation Buffer
			0, 0, 0, 0,														// Accumulation Bits Ignored
			16,																// 16Bit Z-Buffer (Depth Buffer)  
			0,																// No Stencil Buffer
			0,																// No Auxiliary Buffer
			PFD_MAIN_PLANE,													// Main Drawing Layer
			0,																// Reserved
			0, 0, 0															// Layer Masks Ignored
		};																		
	
		
		if ( !( m_hDC = ::GetDC ( m_hWnd ) ) )	{							// Did We Get A Device Context?																	
			KillGLWindow ();													// Reset The Display
			MessageBox ( "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
			return FALSE;												
		}
																			
		if ( !( PixelFormat = ChoosePixelFormat ( m_hDC, &pfd ) ) )	{		// Did Windows Find A Matching Pixel Format?																		
			KillGLWindow ();												// Reset The Display
			MessageBox ( "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
			return FALSE;												
		}
																			
		if ( !SetPixelFormat ( m_hDC, PixelFormat, &pfd ) ){				// Are We Able To Set The Pixel Format?																		
			KillGLWindow ();												// Reset The Display
			MessageBox ( "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
			return FALSE;												
		}
																			
		if ( !( m_hRC = wglCreateContext ( m_hDC ) ) ) {					// Are We Able To Get A Rendering Context?																	
			KillGLWindow ();												// Reset The Display
			MessageBox( " Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
			return FALSE;													
		}
																			
		if ( !wglMakeCurrent ( m_hDC, m_hRC ) )	{							// Try To Activate The Rendering Context																		
			KillGLWindow ();												// Reset The Display
			MessageBox ( "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
			return FALSE;													
		}

		if ( !InitGL () ) {													// Initialize Our Newly Created GL Window																		
			KillGLWindow ();												// Reset The Display
			MessageBox ( "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
			return FALSE;													
		}


		m_bAppIsActive = TRUE;
	}

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame OnPaint

void CMainFrame::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	// Do not call CMainFrame::OnPaint() for painting messages

	::ValidateRect ( m_hWnd, NULL );
	
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame OnSize

void CMainFrame::OnSize(UINT, int cx, int cy)
{
	m_cxClient = cx;
	m_cyClient = cy;
	ReSizeGLScene( cx, cy );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame OnSetFocus

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	OnQueryNewPalette();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame OnCreateClient

BOOL CMainFrame::OnQueryNewPalette()
{
	Invalidate();
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame OnPaletteChanged

void CMainFrame::OnPaletteChanged(CWnd* pFocusWnd)
{
	if ((pFocusWnd != this) && (!IsChild(pFocusWnd)))
		OnQueryNewPalette();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame OnActivateApp

void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask)
{
	CFrameWnd::OnActivateApp(bActive, hTask);
	m_bAppIsActive = bActive;
}



void CMainFrame::setlight()
{

	GLfloat  light_diff[] = {0.8f, 0.8f, 0.8f, 1.0f };

  glPushMatrix();
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lightpos);////光源坐标，初始Y=5
   glPopMatrix();

   glDisable(GL_LIGHTING);
   glPushMatrix();
	   glTranslatef(Lightpos[0],Lightpos[1],Lightpos[2]);
	   glColor3f(0,1,0);
	   auxSolidSphere(0.02f);
    glPopMatrix();

	if(ShowLightDir > 0){
    glPushMatrix();       
		 glColor4f(0.0f, 1.0f, 0.0f, 0.0f);
		 glBegin(GL_LINES);
		 glVertex3f(Lightpos[0], Lightpos[1], Lightpos[2]);
		 glVertex3f(Lightlookat[0], Lightlookat[1], Lightlookat[2]);
		 glEnd();
	glPopMatrix();
	}
   glEnable(GL_LIGHTING);

   	if(ShowLightDir == 0 || ShowLightDir == 1)
	{Lightlookat[0] = Lightlookat[1] = Lightlookat[2] = 0.0;}
	else if(ShowLightDir == 2){
		Lightlookat[0] = camera.calookx;
		Lightlookat[1] = camera.calooky;
		Lightlookat[2] = camera.calookz;}
}

void CMainFrame::SetFrameBufferObject(int fbowidth, int fboheight)
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

	glFramebufferRenderbufferEXT(
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, colorTextureId);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, shadowmapid, 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
		MessageBox ( "FBO Initialization Failed.", "Error", MB_OK | MB_ICONEXCLAMATION );


	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void CMainFrame::clearFrameBufferObject()
{
	glDeleteFramebuffersEXT(1, &depthFBO);
    glDeleteTextures(1, &shadowmapid);
    glDeleteTextures(1, &colorTextureId);	
}

void drawbox()
{
	GLfloat size = 20.0f;
   glPushMatrix();	
   glTranslatef(0.0, 5.0, -5.0);

   		glColor3f(1.0f, 0.0f, 1.0f);	
	   BaseDraw::drawquad(Z, size*2,   size*2, -size, 1, 1, true);

		glColor3f(0.9f, 0.0f, 0.0f);	
	    BaseDraw::drawquad(Y, size*2,   -size, size*2, 1, 1, true);

		glColor3f(0.0f, 0.0f, 0.9f);	
	    BaseDraw::drawquad(Y, size*2,   size, size*2, 1, 1, true);

 		glColor3f(1.0f, 0.0f, 0.0f);	 
	    BaseDraw::drawquad(X, -size,   size*2, size*2, 1, 1, true);		

		glColor3f(0.0f, 0.0f, 1.0f);
	    BaseDraw::drawquad(X, size,   size*2, size*2, 1, 1, true);		

  glPopMatrix();

       glPushMatrix();
		glColor4f(0.0f, 1.0f, 0.0f, 1.0);
	   glTranslatef(0.8f, -1.8f-10.0f, 0.0f);
       auxSolidSphere(0.2);
       glPopMatrix();
}


void CMainFrame::RenderObjects()
{
	glPushMatrix();
	   drawbox();	//draw object
       shadowmodel->RenderModel();		//transformation
	   
	glPushMatrix();
	   glColor4f(0.1f, 0.84f, 0.53f, 1.0f);
	   glTranslatef(-3.0f, 4.0f*sin(jumpa),2.0f*cos(jumpb)+15.0f);
	   auxSolidSphere(0.6);
	glPopMatrix();
	glPushMatrix();
	   glColor4f(0.56f, 0.84f, 0.13f, 1.0f);
	   glTranslatef(1.8f*sin(jumpa)+4.0f, 4.0f*cos(jumpb)-2.0f,2.0f*sin(jumpa)+10.0f);
	   auxSolidSphere(0.8);
	glPopMatrix();


	jumpa += 0.006f;
	jumpb += 0.008f;
	glPopMatrix();

}

void CMainFrame::GenShadowMap()
{
	glPushMatrix();
  	    glViewport(0, 0, SHADOWSCREEN_WIDTH, SHADOWSCREEN_HEIGHT);
	    glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(Light_ProjectionMatrix.mt);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(Light_ViewMatrix.mt);


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

void CMainFrame::CastShadowMap()		//kkk sent shadowmap
{	

	myshader.enable();
	myshader.SendUniform("shadowmap",0);


    glEnable(GL_TEXTURE_2D); 
	glBindTexture(GL_TEXTURE_2D, shadowmapid);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

	    glMatrixMode(GL_TEXTURE);					//kkk need look		turn GL_texture and Light_Proj*Light_View*Camera_ViewIn
		glLoadMatrixf(Light_ProjectionMatrix.mt);
		glMultMatrixf(Light_ViewMatrix.mt);
		glMultMatrixf(Camera_ViewInverse.mt);	


	    glMatrixMode(GL_MODELVIEW);	
	    RenderObjects();				//kkk RenderObjects

		
    glDisable(GL_TEXTURE_2D);
	myshader.disable();

}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame RenderGLScene

void CMainFrame::RenderGLScene()		//kkk start there
{
	if (!m_bAppIsActive)
		return;	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

/////////////////////////////////////
	glPushMatrix(); //纯粹保存&更新所需矩阵
		glLoadIdentity();
		gluPerspective(45.0f, (GLfloat)SHADOWSCREEN_WIDTH/(GLfloat)SHADOWSCREEN_HEIGHT, 2.0f, 60.0f);
		//gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
		glGetFloatv(GL_MODELVIEW_MATRIX, Light_ProjectionMatrix.mt);

		glLoadIdentity();		
		gluLookAt(Lightpos[0],Lightpos[1],Lightpos[2], Lightlookat[0], Lightlookat[1], Lightlookat[2],  0.0, 1.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, Light_ViewMatrix.mt);
	glPopMatrix();
//////////////////////////////////////
     GenShadowMap();		//kkk GenShadowMap

////////////////////////////////////////////////////	

    ReSizeGLScene(VB_WIDTH, VB_HEIGHT);		//kkk ReSizeGLScene


	static int xs;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setlight();
	shadowmodel->SetModelRotation(45.0+xs,45.0+xs,45.0+xs);
	
	   glEnable(GL_LIGHTING);
	   glEnable(GL_LIGHT0);
	   glEnable(GL_COLOR_MATERIAL);


    CastShadowMap();	//kkk CastShadowMap

     if(EnRot)xs++;	
/////////////////////////////////////
    calculateFramesPerSec();	//kkk calculateFramesPerSec

	SwapBuffers(m_hDC);	// Swap our scene to the front

	Invalidate(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame OnCreateClient

GLvoid CMainFrame::ReSizeGLScene(GLsizei width, GLsizei height)				// Resize And Initialize The GL Window
{																			
	if ( height==0) {														// Prevent A Divide By Zero By																	
		height=1;															// Making Height Equal One
	}																		
																			
	glViewport(0,0,width,height);											// Reset The Current Viewport
																			
	glMatrixMode(GL_PROJECTION);											// Select The Projection Matrix
	glLoadIdentity();														// Reset The Projection Matrix

	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);		// Calculate The Aspect Ratio Of The Window

	glMatrixMode(GL_MODELVIEW);												// Select The Modelview Matrix
	glLoadIdentity();	

	camera.Look();

	glGetFloatv(GL_MODELVIEW_MATRIX, Camera_ViewInverse.mt);
	Camera_ViewInverse = Camera_ViewInverse.GetmvMatrixInverse();//在这里获得逆矩阵
}


//////////////////////////////////////////////////////////////////////////////
// CMainFrame InitGL
//kkk
int CMainFrame::InitGL(GLvoid)												// All Setup For OpenGL Goes Here
{
	// EXAMPLE DIRECT SOUND CODE START ////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	// EXAMPLE DIRECT SOUND CODE END ////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////


	glShadeModel(GL_SMOOTH);												// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);									// Black Background
	glClearDepth(1.0f);														// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);												// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);													// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);						// Really Nice Perspective Calculations
	glEnable(GL_TEXTURE_2D);												// Enable Texture Mapping

	myshader.setshader("shadowmap1.vert","shadowmap1.frag");

    SetFrameBufferObject(SHADOWSCREEN_WIDTH, SHADOWSCREEN_HEIGHT);			//kkk SHADOWSCREEN_WIDTH is gluPerspective's

	shadowmodel = new CLoad3DS;
	if(Select3DS)shadowmodel->ImportModel(0,"3DS\\SCENE.3ds");
	else         shadowmodel->ImportModel(0,"3DS\\car.3ds");

    shadowmodel->SetModelPosition(0,0,0);
	shadowmodel->SetModelRotation(45,45,45);
	shadowmodel->SetModelSize(0.5);


	return TRUE;															// Initialization Went OK
}

//////////////////////////////////////////////////////////////////////////////
// CMainFrame KillGLWindow

GLvoid CMainFrame::KillGLWindow(GLvoid)										// Properly Kill The Window
{																			
	if (m_bFullScreen)															// Are We In Fullscreen Mode?
	{
		if (!ChangeDisplaySettings(NULL,CDS_TEST)) {						// if the shortcut doesn't work
			ChangeDisplaySettings(NULL,CDS_RESET);							// Do it anyway (to get the values out of the registry)
			ChangeDisplaySettings(&m_DMsaved,CDS_RESET);						// change it to the saved settings
		} else {
			ChangeDisplaySettings(NULL,CDS_RESET);
		}
			
		ShowCursor(TRUE);													// Show Mouse Pointer
	}																		
																			
	if ( m_hRC ) {															// Do We Have A Rendering Context?																		
		if ( !wglMakeCurrent ( NULL, NULL ) )	{							// Are We Able To Release The DC And RC Contexts?
			MessageBox ( "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		}

		if ( !wglDeleteContext ( m_hRC ) ) {								// Are We Able To Delete The RC?
			MessageBox ( "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		}
		m_hRC = NULL;														// Set RC To NULL
	}

	if ( m_hDC && !::ReleaseDC ( m_hWnd, m_hDC ) ) {						// Are We Able To Release The DC
		MessageBox ( "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		m_hDC = NULL;														// Set DC To NULL
	}

	if ( m_hWnd && !::DestroyWindow ( m_hWnd ) )	{							// Are We Able To Destroy The Window?
		MessageBox( "Could Not Release m_hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION );
		m_hWnd = NULL;														// Set m_hWnd To NULL
	}

clearFrameBufferObject();

}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame calculateFramesPerSec

void CMainFrame::calculateFramesPerSec() 
{
	// Get current time in seconds! Sytem time is in milliseconds since win98 started
	m_fTime = static_cast<float>(timeGetTime())*0.001f;
	m_fEndFrameTime = m_fTime;
	
	//Calculate frames/sec	
	if ( ( m_fEndFrameTime - m_fStartFrameTime ) >= 1.0f ) {
		m_iFramesPerSec  = m_iFrames;
		m_iFrames = 0;
		m_fStartFrameTime = m_fTime;
	} else {
		m_iFrames++;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame OnKeyDown

void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
		CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	// TODO: Add your message handler code here and/or call default

	switch ( nChar ) {
	case VK_ESCAPE:	
		PostMessage ( WM_CLOSE );
		break;
	case VK_SPACE:	
		ShowLightDir++;
		if(ShowLightDir >=3)ShowLightDir = 0;
		break;
	case VK_UP:	
		Lightpos[2]-=0.05f;
		break;
	case VK_DOWN:	
		Lightpos[2]+=0.05f;
		break;
	case VK_LEFT:	
		Lightpos[0]-=0.05f;
		break;
	case VK_RIGHT:	
		Lightpos[0]+=0.05f;
		break;
	case VK_PRIOR:	
		Lightpos[1]+=0.05f;
		break;
	case VK_NEXT:	
		Lightpos[1]-=0.05f;
		break;
	
	} // end switch

}

void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point) 
{
		m_LeftButtonDown = TRUE;
	    m_LeftDownPos = point;
	
	CFrameWnd::OnLButtonDown(nFlags, point);
}

void CMainFrame::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_LeftButtonDown = FALSE;
	
	CFrameWnd::OnLButtonUp(nFlags, point);
}

void CMainFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	CString lightlook;
	if(ShowLightDir==0 || ShowLightDir==1)lightlook.Format("原点");
	else if(ShowLightDir==2)lightlook.Format("相机焦点");

	CString str;
        str.Format("ZwqXin.com - 3D图形学 Eye:x:%.3f, y:%.3f, z:%.3f, Lookat:%.3f, %.3f, %.3f  光源目标绑定:%s   FPS:%d",
			camera.m_fEye[0], camera.m_fEye[1], camera.m_fEye[2],
			camera.calookx, camera.calooky, camera.calookz, lightlook, m_iFramesPerSec);
		    SetWindowText(str);   

		if(m_LeftButtonDown)
	{
		camera.SetRotationSpeed(0.16f);	
		camera.Rotate(point.x, point.y, m_LeftDownPos.x, m_LeftDownPos.y);
		m_LeftDownPos = point;
		Invalidate();
	}
	
	CFrameWnd::OnMouseMove(nFlags, point);
}

BOOL CMainFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
		if( zDelta>0 )          //鼠标滚轮
	{
		camera.SetTranslationSpeed(0.3f);
		camera.Move(PLUS_DIRECTION);
	}
	else if( zDelta<=0)
	{
		camera.SetTranslationSpeed(0.3f);
		camera.Move(MINUS_DIRECTION);
	}
	//Eyelimitation();
	return CFrameWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CMainFrame::Eyelimitation()
{
	if(camera.m_fEye[1]< -1.8f )
	camera.m_fEye[1]= -1.8f;
	if(camera.m_fEye[1]>7.8)camera.m_fEye[1]=7.8f;
	if(camera.m_fEye[0]>9.8)camera.m_fEye[0]=9.8f;
	if(camera.m_fEye[0]<-9.8)camera.m_fEye[0]=-9.8f;
	if(camera.m_fEye[2]>9.8)camera.m_fEye[2]=9.8f;
	if(camera.m_fEye[2]<-9.8)camera.m_fEye[2]=-9.8f;
}


void CMainFrame::OnRButtonDown(UINT nFlags, CPoint point) 
{
		EnRot = !EnRot;
	
	CFrameWnd::OnRButtonDown(nFlags, point);
}

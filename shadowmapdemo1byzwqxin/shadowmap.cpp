// shadowmap.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "shadowmap.h"

#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShadowmapApp

BEGIN_MESSAGE_MAP(CShadowmapApp, CWinApp)
	//{{AFX_MSG_MAP(CShadowmapApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	// DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShadowmapApp construction

CShadowmapApp::CShadowmapApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_lCurTime = 0;             
	m_dwTimeCount = 8;   
	m_lPerfCounter = 0;       
	m_bPerFlag = FALSE;    
	m_lNextTime = 0;    
	m_lLastTime = 0;	 
	m_dTimeElapsed = 0.0;	 
	m_dTimeScale = 0.0;		
	m_lFramesPerSecond = 120;

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CShadowmapApp object

CShadowmapApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CShadowmapApp initialization

BOOL CShadowmapApp::InitInstance()
{
	// Standard initialization

	// Change the registry key under which our settings are stored.

	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	m_pMainWnd = NULL;
	CMainFrame* pFrame = new CMainFrame;

	if (!pFrame->Create(NULL,"ZwqXin.com"))
		return FALSE;

	m_pMainWnd = pFrame;
	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CShadowmapApp  Run

int CShadowmapApp::Run()
{
	if (!m_pMainWnd)
		AfxPostQuitMessage(0);

	MSG msg;
	CMainFrame* pFrame = STATIC_DOWNCAST ( CMainFrame, m_pMainWnd );

	if ( QueryPerformanceFrequency ( ( LARGE_INTEGER *) &m_lPerfCounter ) ) { 

		// yes, set m_dwTimeCount and timer choice flag 
		m_bPerFlag = TRUE;
		m_dwTimeCount = unsigned long(m_lPerfCounter / m_lFramesPerSecond);
		QueryPerformanceCounter ( ( LARGE_INTEGER * ) &m_lNextTime ); 
		m_dTimeScale = 1.0 / m_lPerfCounter;
	} else  { 
		// no performance counter, read in using timeGetTime 

		m_lNextTime = timeGetTime (); 
		m_dTimeScale = 0.001;
	} 
	// save time of last frame

	m_lLastTime = m_lNextTime;

	while ( TRUE ) {
		//see if there is a message waiting

		if ( ::PeekMessage ( &msg, NULL, 0, 0, PM_NOREMOVE ) ) { 
			do //if there is pump all waiting
			{
				if ( !PumpMessage () )
					return ExitInstance ();
			} while ( ::PeekMessage ( &msg, NULL, 0, 0, PM_NOREMOVE ) );
		} else {

			// use the appropriate method to get time 
			// and calculate elapsed time since last frame

			if ( m_bPerFlag ) {
				QueryPerformanceCounter ( ( LARGE_INTEGER * ) &m_lCurTime );	
			} else {
				m_lCurTime=timeGetTime (); 
			}

			// is it time to render the frame? 

			if ( m_lCurTime > m_lNextTime ) { 

				// calculate elapsed time

				m_dTimeElapsed = ( m_lCurTime - m_lLastTime ) * m_dTimeScale;
				

				// save frame time

				m_lLastTime = m_lCurTime;
				
				// yes, render the frame 

				if ( !pFrame->m_bAppIsActive )
					WaitMessage();
				else
					pFrame->RenderGLScene ();

				// set time for next frame 

				m_lNextTime = m_lCurTime + m_dwTimeCount; 

			} // end if

		} // end if else

	} // end while
	return msg.wParam;

} 

/////////////////////////////////////////////////////////////////////////////
// CShadowmapApp message handlers

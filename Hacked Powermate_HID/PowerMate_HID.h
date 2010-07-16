// PowerMate_HID.h : main header file for the POWERMATE_HID application
//

#if !defined(AFX_POWERMATE_HID_H__BB946AC5_AA01_4E62_A399_21CCC008BAF9__INCLUDED_)
#define AFX_POWERMATE_HID_H__BB946AC5_AA01_4E62_A399_21CCC008BAF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPowerMateApp:
// See PowerMate_HID.cpp for the implementation of this class
//

class CPowerMateApp : public CWinApp
{
public:
	CPowerMateApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPowerMateApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPowerMateApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POWERMATE_HID_H__BB946AC5_AA01_4E62_A399_21CCC008BAF9__INCLUDED_)

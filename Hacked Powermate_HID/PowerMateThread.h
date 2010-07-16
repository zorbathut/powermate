#if !defined(AFX_POWERMATETHREAD_H__64752C2E_EC59_425F_8708_83AA46BDB856__INCLUDED_)
#define AFX_POWERMATETHREAD_H__64752C2E_EC59_425F_8708_83AA46BDB856__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PowerMateThread.h : header file
//

#include "PowerMateDlg.h"
#include "mmsystem.h"

/////////////////////////////////////////////////////////////////////////////
// CPowerMateThread thread

class CPowerMateThread : public CWinThread
{
	DECLARE_DYNAMIC(CPowerMateThread)
public:
	CPowerMateThread(CPowerMateDlg* pWnd);           // protected constructor used by dynamic creation
	virtual ~CPowerMateThread();

// Attributes
public:

// Operations
public:
	BOOLEAN m_Running;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPowerMateThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	HANDLE m_hPowerMate;
	char m_OldButtonState;

	//HMIXER m_mixer;

void PerformAction(HANDLE hPowerMate, char *pmReport);

	// Generated message map functions
	//{{AFX_MSG(CPowerMateThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POWERMATETHREAD_H__64752C2E_EC59_425F_8708_83AA46BDB856__INCLUDED_)

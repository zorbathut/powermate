// PowerMateDlg.h : header file
//

#if !defined(AFX_POWERMATEDLG_H__257588A0_AB5D_497C_94D0_B6A2174AACC6__INCLUDED_)
#define AFX_POWERMATEDLG_H__257588A0_AB5D_497C_94D0_B6A2174AACC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPowerMateThread;

/////////////////////////////////////////////////////////////////////////////
// CPowerMateDlg dialog

class CPowerMateDlg : public CDialog
{
// Construction
public:
	HANDLE m_hPowerMate;
	HANDLE m_hReadHandle;
	CPowerMateDlg(CWnd* pParent = NULL);	// standard constructor
	CPowerMateThread*	m_ActionThread;


// Dialog Data
	//{{AFX_DATA(CPowerMateDlg)
	enum { IDD = IDD_POWERMATE_HID_DIALOG };
	CEdit	m_PulseDuringSleepEditCtrl;
	CSliderCtrl	m_SliderPulseSpeedCtrl;
	CSliderCtrl	m_SliderBrightnessCtrl;
	CEdit	m_ButtonDataEditCtrl;
	CEdit	m_AxisDataEditCtrl;
	CEdit	m_PulsingSpeedEditCtrl;
	CEdit	m_PulseAlwaysEditCtrl;
	CEdit	m_GetBrightnessEditCtrl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPowerMateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	HANDLE GetPowerMateViaHID(void);
	HANDLE GetDeviceViaInterface( GUID* pGuid, DWORD instance);

	// Generated message map functions
	//{{AFX_MSG(CPowerMateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonPulseAlwaysOn();
	afx_msg void OnButtonPulseAlwaysOff();
	afx_msg void OnButtonPulseDuringSleepOn();
	afx_msg void OnButtonLowPowerMode();
	afx_msg void OnButtonGetBrightness();
	afx_msg void OnButtonGetPulseAlways();
	afx_msg void OnButtonGetPulseDuringSleep();
	afx_msg void OnButtonGetPulseSpeed();
	afx_msg void OnReleasedcaptureSliderBrightness(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSliderPulseSpeed(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POWERMATEDLG_H__257588A0_AB5D_497C_94D0_B6A2174AACC6__INCLUDED_)

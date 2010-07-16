// PowerMateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PowerMate_HID.h"
#include "PowerMateDlg.h"
#include "PowerMateThread.h"

#include "initguid.h"
#include "winioctl.h"

#include "objbase.h"
#include "dbt.h"

extern "C" {
// Declare the C libraries used
#include "setupapi.h" // Must link in setupapi.lib
#include "C:\WinDDK\7600.16385.1\inc\api\hidsdi.h" // Must link in hid.lib
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum {
	kVendorID		= 0x077d,
	kProductID		= 0x0410,
	kVersionNumber	= 0x0311
};


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPowerMateDlg dialog

CPowerMateDlg::CPowerMateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPowerMateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPowerMateDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPowerMateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPowerMateDlg)
	DDX_Control(pDX, IDC_EDIT_PULSE_DURING_SLEEP, m_PulseDuringSleepEditCtrl);
	DDX_Control(pDX, IDC_SLIDER_PULSE_SPEED, m_SliderPulseSpeedCtrl);
	DDX_Control(pDX, IDC_SLIDER_BRIGHTNESS, m_SliderBrightnessCtrl);
	DDX_Control(pDX, IDC_EDIT_BUTTON_DATA, m_ButtonDataEditCtrl);
	DDX_Control(pDX, IDC_EDIT_AXIS_DATA, m_AxisDataEditCtrl);
	DDX_Control(pDX, IDC_EDIT_PULSE_SPEED, m_PulsingSpeedEditCtrl);
	DDX_Control(pDX, IDC_EDIT_PULSE_ALWAYS, m_PulseAlwaysEditCtrl);
	DDX_Control(pDX, IDC_EDIT_BRIGHTNESS, m_GetBrightnessEditCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPowerMateDlg, CDialog)
	//{{AFX_MSG_MAP(CPowerMateDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_PULSE_ALWAYS_ON, OnButtonPulseAlwaysOn)
	ON_BN_CLICKED(IDC_BUTTON_PULSE_ALWAYS_OFF, OnButtonPulseAlwaysOff)
	ON_BN_CLICKED(IDC_BUTTON_PULSE_DURING_SLEEP_ON, OnButtonPulseDuringSleepOn)
	ON_BN_CLICKED(IDC_BUTTON_LOW_POWER_MODE, OnButtonLowPowerMode)
	ON_BN_CLICKED(IDC_BUTTON_GET_BRIGHTNESS, OnButtonGetBrightness)
	ON_BN_CLICKED(IDC_BUTTON_GET_PULSE_ALWAYS, OnButtonGetPulseAlways)
	ON_BN_CLICKED(IDC_BUTTON_GET_PULSE_DURING_SLEEP, OnButtonGetPulseDuringSleep)
	ON_BN_CLICKED(IDC_BUTTON_GET_PULSE_SPEED, OnButtonGetPulseSpeed)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_BRIGHTNESS, OnReleasedcaptureSliderBrightness)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_PULSE_SPEED, OnReleasedcaptureSliderPulseSpeed)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPowerMateDlg message handlers

BOOL CPowerMateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_SliderBrightnessCtrl.SetRange(0, 255, TRUE);
	m_SliderPulseSpeedCtrl.SetRange(0, 24, TRUE);
	m_hPowerMate = GetPowerMateViaHID();
	m_hReadHandle = GetPowerMateViaHID();

	// there is a PowerMate plugged in
	// create a new thread
	m_ActionThread = new CPowerMateThread(this);
	if (m_ActionThread == NULL)
		return false;

	ASSERT_VALID(m_ActionThread);
	// Create Thread in a suspended state so we can set the Priority
	// before it starts getting away from us
	if (!m_ActionThread->CreateThread(CREATE_SUSPENDED))
	{
		delete m_ActionThread;
		return false;
	}

	// set the priority
	VERIFY(m_ActionThread->SetThreadPriority(THREAD_PRIORITY_NORMAL));

	// Now the thread can run wild
	m_ActionThread->ResumeThread();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

HANDLE CPowerMateDlg::GetPowerMateViaHID()
{
	HIDD_ATTRIBUTES HidAttr;
	GUID HidGuid;
	bool found = false;
	DWORD Instance = 0;
	HANDLE hHidPM = NULL;
	USHORT InputReportLen = 0, OutputReportLen = 0;

	HidD_GetHidGuid(&HidGuid);

	while( !found)
	{
		hHidPM = GetDeviceViaInterface( &HidGuid, Instance++);
		OutputDebugString("infound");
		if( hHidPM==NULL)
			break;

		// Get attributes. ie find vendor and product ids
		if( !HidD_GetAttributes( hHidPM, &HidAttr))
		{
			OutputDebugString("no attribs");
			return false;
		}
		else
		{
			char bf[1024];
			sprintf(bf, "%x %x", HidAttr.VendorID, HidAttr.ProductID);
			OutputDebugString(bf);

			if ((HidAttr.VendorID == kVendorID) && (HidAttr.ProductID == kProductID))
			{
				OutputDebugString("found");
//				if (HidAttr.VersionNumber == kVersionNumber)
					return hHidPM;
//				else
//					return false;
			}
			else
			{
				OutputDebugString("fail");
			}
		}
		CloseHandle(hHidPM);
	}
	OutputDebugString("nulling out");
	if( !found || InputReportLen==0 || OutputReportLen==0)
	{
		return NULL;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
//	GetDeviceViaInterface:	Open a handle via a device interface
//
HANDLE CPowerMateDlg::GetDeviceViaInterface( GUID* pGuid, DWORD instance)
{
	// Get handle to relevant device information set
	HDEVINFO info = SetupDiGetClassDevs(pGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if(info==INVALID_HANDLE_VALUE)
		return NULL;

	// Get interface data for the requested instance
	SP_INTERFACE_DEVICE_DATA ifdata;
	ifdata.cbSize = sizeof(ifdata);
	if(!SetupDiEnumDeviceInterfaces(info, NULL, pGuid, instance, &ifdata))
	{
		SetupDiDestroyDeviceInfoList(info);
		return NULL;
	}

	// Get size of symbolic link name
	DWORD ReqLen;
	SetupDiGetDeviceInterfaceDetail(info, &ifdata, NULL, 0, &ReqLen, NULL);
	PSP_INTERFACE_DEVICE_DETAIL_DATA ifDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)(new char[ReqLen]);
	if( ifDetail==NULL)
	{
		SetupDiDestroyDeviceInfoList(info);
		return NULL;
	}

	// Get symbolic link name
	ifDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
	if( !SetupDiGetDeviceInterfaceDetail(info, &ifdata, ifDetail, ReqLen, NULL, NULL))
	{
		SetupDiDestroyDeviceInfoList(info);
		delete ifDetail;
		return NULL;
	}

	// Open file
	HANDLE rv = CreateFile( ifDetail->DevicePath, 
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);  //  FILE_FLAG_OVERLAPPED  FILE_ATTRIBUTE_NORMAL
	if( rv==INVALID_HANDLE_VALUE) rv = NULL;

	delete ifDetail;
	SetupDiDestroyDeviceInfoList(info);
	return rv;
}


void CPowerMateDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPowerMateDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPowerMateDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPowerMateDlg::OnButtonPulseAlwaysOn() 
{
	BOOLEAN	error;
	UCHAR reportBuffer[9];

	memset(reportBuffer, 0, sizeof(reportBuffer));
	reportBuffer[0] = 0x00;
	reportBuffer[1] = 0x41;
	reportBuffer[2] = 0x01;
	reportBuffer[3] = 0x03;	// command type
	reportBuffer[4] = 0x00;
	reportBuffer[5] = 0x01;	// command value
	error = HidD_SetFeature(m_hPowerMate, reportBuffer, sizeof(reportBuffer));
}

void CPowerMateDlg::OnButtonPulseAlwaysOff() 
{
	BOOLEAN	error;
	UCHAR reportBuffer[9];

	memset(reportBuffer, 0, sizeof(reportBuffer));
	reportBuffer[0] = 0x00;
	reportBuffer[1] = 0x41;
	reportBuffer[2] = 0x01;
	reportBuffer[3] = 0x03;	// command type
	reportBuffer[4] = 0x00;
	reportBuffer[5] = 0x00;	// command value
	error = HidD_SetFeature(m_hPowerMate, reportBuffer, sizeof(reportBuffer));
}

void CPowerMateDlg::OnButtonPulseDuringSleepOn() 
{
	BOOLEAN	error;
	UCHAR reportBuffer[9];

	memset(reportBuffer, 0, sizeof(reportBuffer));
	reportBuffer[0] = 0x00;
	reportBuffer[1] = 0x41;
	reportBuffer[2] = 0x01;
	reportBuffer[3] = 0x02; // command type
	reportBuffer[4] = 0x00;
	reportBuffer[5] = 0x01;	// command value
	error = HidD_SetFeature(m_hPowerMate, reportBuffer, sizeof(reportBuffer));
}

void CPowerMateDlg::OnButtonLowPowerMode() 
{
	BOOLEAN	error;
	UCHAR reportBuffer[9];

	memset(reportBuffer, 0, sizeof(reportBuffer));
	reportBuffer[0] = 0x00;
	reportBuffer[1] = 0x41;
	reportBuffer[2] = 0x01;
	reportBuffer[3] = 0x02;	// command type
	reportBuffer[4] = 0x00;
	reportBuffer[5] = 0x00;	// command value
	error = HidD_SetFeature(m_hPowerMate, reportBuffer, sizeof(reportBuffer));
}

void CPowerMateDlg::OnButtonGetBrightness() 
{
	BOOLEAN	error;
	UCHAR reportBuffer[7];
	char	num[6];

	memset(reportBuffer, 0, sizeof(reportBuffer));
	error = HidD_GetInputReport(m_hPowerMate, reportBuffer, sizeof(reportBuffer));

	sprintf(num, "%d", (unsigned char)reportBuffer[4]);	
	m_GetBrightnessEditCtrl.SetWindowText(num);
}

void CPowerMateDlg::OnButtonGetPulseAlways() 
{
	BOOLEAN	error;
	UCHAR reportBuffer[7];
	CString editText;

	memset(reportBuffer, 0, sizeof(reportBuffer));
	error = HidD_GetInputReport(m_hPowerMate, reportBuffer, sizeof(reportBuffer));

	if (reportBuffer[5] & 0x01)
		m_PulseAlwaysEditCtrl.SetWindowText("ON");
	else
		m_PulseAlwaysEditCtrl.SetWindowText("OFF");
}

void CPowerMateDlg::OnButtonGetPulseDuringSleep() 
{
	BOOLEAN	error;
	UCHAR reportBuffer[7];
	CString editText;

	memset(reportBuffer, 0, sizeof(reportBuffer));
	error = HidD_GetInputReport(m_hPowerMate, reportBuffer, sizeof(reportBuffer));

	if (reportBuffer[5] & 0x04)
		m_PulseDuringSleepEditCtrl.SetWindowText("Pulse During Sleep");
	else
		m_PulseDuringSleepEditCtrl.SetWindowText("Low Power Mode");
}

void CPowerMateDlg::OnButtonGetPulseSpeed() 
{
	BOOLEAN	error;
	UCHAR reportBuffer[7];
	char	num[6];

	memset(reportBuffer, 0, sizeof(reportBuffer));
	error = HidD_GetInputReport(m_hPowerMate, reportBuffer, sizeof(reportBuffer));

	int pulseSpeed = (reportBuffer[5] & 0x30) << 4;
	int pulseTable = (reportBuffer[5] & 0xC0) << 6;
	sprintf(num, "%d", (unsigned char)reportBuffer[6]);	
	m_PulsingSpeedEditCtrl.SetWindowText(num);
}

void CPowerMateDlg::OnReleasedcaptureSliderBrightness(NMHDR* pNMHDR, LRESULT* pResult) 
{
	BOOLEAN	error;
	UCHAR reportBuffer[2];
	DWORD TxdBytes;

	int pos = m_SliderBrightnessCtrl.GetPos();

	reportBuffer[0] = 0;
	reportBuffer[1] = pos;
	error = WriteFile(m_hPowerMate, reportBuffer, sizeof(reportBuffer), &TxdBytes, NULL);
	
	*pResult = 0;
}

void CPowerMateDlg::OnReleasedcaptureSliderPulseSpeed(NMHDR* pNMHDR, LRESULT* pResult) 
{
	BOOLEAN	error;
	UCHAR reportBuffer[9];

	int pos = m_SliderPulseSpeedCtrl.GetPos();

	memset(reportBuffer, 0, sizeof(reportBuffer));
	reportBuffer[0] = 0x00;
	reportBuffer[1] = 0x41;
	reportBuffer[2] = 0x01;
	reportBuffer[3] = 0x04;	// command type
	reportBuffer[4] = 0x00;	// select table
	// set pulse rate
	// byte 5; 0 = divide, 1 = normal speed, 2 = multiply
	// byte 6; value
	if (pos < 8)
	{
		reportBuffer[5] = 0;
		reportBuffer[6] = (7-pos) * 2;
	}
	else if (pos > 8)
	{
		reportBuffer[5] = 2;
		reportBuffer[6] = (pos-8) * 2;
	}
	else
	{
		reportBuffer[5] = 1;
		reportBuffer[6] = 0;
	}

	error = HidD_SetFeature(m_hPowerMate, reportBuffer, sizeof(reportBuffer));

	*pResult = 0;
}

void CPowerMateDlg::OnDestroy() 
{
	CDialog::OnDestroy();
}

// PowerMateThread.cpp : implementation file
//

#include "stdafx.h"
#include "PowerMate_HID.h"
#include "PowerMateThread.h"
#include <mmsystem.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPowerMateThread

IMPLEMENT_DYNAMIC(CPowerMateThread, CWinThread)

CPowerMateThread::CPowerMateThread(CPowerMateDlg *pWnd)
{
	m_hPowerMate = pWnd->m_hReadHandle;
}

CPowerMateThread::~CPowerMateThread()
{
}

BOOL CPowerMateThread::InitInstance()
{
	//mixerOpen(&m_mixer, 0, NULL, 0, MIXER_OBJECTF_WAVEOUT);

	char reportBuffer[8];
	DWORD txdBytes;
	BOOL result;

	m_OldButtonState = 0;
	//m_pDlgWnd->m_AxisDataEditCtrl.SetWindowText("0");
	//m_pDlgWnd->m_ButtonDataEditCtrl.SetWindowText("Up");
	m_Running = TRUE;
	while (m_Running == TRUE)
	{
		result = ReadFile(m_hPowerMate, reportBuffer, sizeof(reportBuffer), &txdBytes, NULL);

		if (result)
		{
			if( txdBytes == 7)
				PerformAction(m_hPowerMate, reportBuffer);
		}
		else
			break;
	}

	AfxEndThread(0);
	
	// avoid entering standard message loop by returning FALSE
	return FALSE;
}

int CPowerMateThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CPowerMateThread, CWinThread)
	//{{AFX_MSG_MAP(CPowerMateThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPowerMateThread message handlers


void CPowerMateThread::PerformAction(HANDLE hPowerMate, char *reportBuffer)
{
	CString editText;
	char	num[6];


	MMRESULT rc;              // Return code.
	HMIXER hMixer;            // Mixer handle used in mixer API calls.
	MIXERCONTROL mxc;         // Holds the mixer control data.
	MIXERLINE mxl;            // Holds the mixer line data.
	MIXERLINECONTROLS mxlc;   // Obtains the mixer control.

	// Open the mixer. This opens the mixer with a deviceID of 0. If you
	// have a single sound card/mixer, then this will open it. If you have
	// multiple sound cards/mixers, the deviceIDs will be 0, 1, 2, and
	// so on.
	rc = mixerOpen(&hMixer, 0,0,0,0);
	if (MMSYSERR_NOERROR != rc) {
		// Couldn't open the mixer.
	}

	// Initialize MIXERLINE structure.
	ZeroMemory(&mxl,sizeof(mxl));
	mxl.cbStruct = sizeof(mxl);

	// Specify the line you want to get. You are getting the input line
	// here. If you want to get the output line, you need to use
	// MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT.
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;

	rc = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl,
						   MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (MMSYSERR_NOERROR == rc) {
		// Couldn't get the mixer line.
	}

	// Get the control.
	ZeroMemory(&mxlc, sizeof(mxlc));
	mxlc.cbStruct = sizeof(mxlc);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(mxc);
	mxlc.pamxctrl = &mxc;
	ZeroMemory(&mxc, sizeof(mxc));
	mxc.cbStruct = sizeof(mxc);
	rc = mixerGetLineControls((HMIXEROBJ)hMixer,&mxlc,
							   MIXER_GETLINECONTROLSF_ONEBYTYPE);
	if (MMSYSERR_NOERROR != rc) {
		// Couldn't get the control.
	}

	// After successfully getting the peakmeter control, the volume range
	// will be specified by mxc.Bounds.lMinimum to mxc.Bounds.lMaximum.

	MIXERCONTROLDETAILS mxcd;             // Gets the control values.
	MIXERCONTROLDETAILS_SIGNED volStruct; // Gets the control values.
	long volume;                          // Holds the final volume value.

	// Initialize the MIXERCONTROLDETAILS structure
	ZeroMemory(&mxcd, sizeof(mxcd));
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.cbDetails = sizeof(volStruct);
	mxcd.dwControlID = mxc.dwControlID;
	mxcd.paDetails = &volStruct;
	mxcd.cChannels = 1;

	// Get the current value of the peakmeter control. Typically, you
	// would set a timer in your program to query the volume every 10th
	// of a second or so.
	rc = mixerGetControlDetails((HMIXEROBJ)hMixer, &mxcd,
								 MIXER_GETCONTROLDETAILSF_VALUE);
	if (MMSYSERR_NOERROR == rc) {
		// Couldn't get the current volume.
	}
	volume = volStruct.lValue;

	// Get the absolute value of the volume.
	if (volume < 0)
		volume = -volume;


	int ovolume = volume;

	//char brf[1024];
	//sprintf(brf, "wopa %08x", volume);
	//OutputDebugString(brf);

	/*
	// get button state
	if (reportBuffer[1] == 1 && m_OldButtonState == 0)
	{
		// detected a button down
		m_pDlgWnd->m_ButtonDataEditCtrl.SetWindowText("Down");
		m_OldButtonState = 1;
	}
	else if (reportBuffer[1] == 0 && m_OldButtonState == 1)
	{
		// detected a button up
		m_pDlgWnd->m_ButtonDataEditCtrl.SetWindowText("Up");
		m_OldButtonState = 0;
	}*/

	volume = volume + 200 * reportBuffer[2];

	// get knob displacement
	if (reportBuffer[2] > 0)
	{	
		//editText.Format("%d", reportBuffer[2]);
		//m_pDlgWnd->m_AxisDataEditCtrl.SetWindowText(editText);
	}
	else if (reportBuffer[2] < 0)
	{
		//editText.Format("%d", reportBuffer[2]);
		//m_pDlgWnd->m_AxisDataEditCtrl.SetWindowText(editText);
	}

	if(volume > 0x60000 || volume < 0) {
		volume = 0;
	}
	if(volume > 0xffff) {
		volume = 0xffff;
	}

		rc = mixerGetControlDetails((HMIXEROBJ)hMixer, &mxcd,
								 MIXER_GETCONTROLDETAILSF_VALUE);
	if (MMSYSERR_NOERROR == rc) {
		// Couldn't get the current volume.
	}


	if(volume != ovolume) {
		//char bf[1024];
		//sprintf(bf, "booga %08x %08x", volume, ovolume);
		//OutputDebugString(bf);

		volStruct.lValue = volume;
		mixerSetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE);
		//waveOutSetVolume(0, curvol);
	}

	/*
	sprintf(num, "%d", (unsigned char)reportBuffer[4]);	
	m_pDlgWnd->m_GetBrightnessEditCtrl.SetWindowText(num);
	
	if (reportBuffer[5] & 0x01)
		m_pDlgWnd->m_PulseAlwaysEditCtrl.SetWindowText("ON");
	else
		m_pDlgWnd->m_PulseAlwaysEditCtrl.SetWindowText("OFF");
		
	if (reportBuffer[5] & 0x04)
		m_pDlgWnd->m_PulseDuringSleepEditCtrl.SetWindowText("Pulse During Sleep");
	else
		m_pDlgWnd->m_PulseDuringSleepEditCtrl.SetWindowText("Low Power Mode");

	int pulseSpeed = (reportBuffer[5] & 0x30) << 4;
	int pulseTable = (reportBuffer[5] & 0xC0) << 6;
	sprintf(num, "%d", (unsigned char)reportBuffer[6]);	
	m_pDlgWnd->m_PulsingSpeedEditCtrl.SetWindowText(num);*/
}


/*
void CPowerMateThread::PerformAction(HANDLE hPowerMate, char *reportBuffer)
{
	CString editText;
	char	num[6];

	DWORD curvol;
	waveOutGetVolume(0, &curvol);

	DWORD ocv = curvol;

	curvol = curvol & 0xffff;

	char brf[1024];
	sprintf(brf, "%08x", curvol);
	OutputDebugString(brf);


	// get button state
	if (reportBuffer[1] == 1 && m_OldButtonState == 0)
	{
		// detected a button down
		m_pDlgWnd->m_ButtonDataEditCtrl.SetWindowText("Down");
		m_OldButtonState = 1;
	}
	else if (reportBuffer[1] == 0 && m_OldButtonState == 1)
	{
		// detected a button up
		m_pDlgWnd->m_ButtonDataEditCtrl.SetWindowText("Up");
		m_OldButtonState = 0;
	}
	// get knob displacement
	if (reportBuffer[2] > 0)
	{
		curvol = curvol + 100;
		editText.Format("%d", reportBuffer[2]);
		m_pDlgWnd->m_AxisDataEditCtrl.SetWindowText(editText);
	}
	else if (reportBuffer[2] < 0)
	{
		curvol = curvol - 100;
		editText.Format("%d", reportBuffer[2]);
		m_pDlgWnd->m_AxisDataEditCtrl.SetWindowText(editText);
	}

	if(curvol > 0x60000 || curvol < 0) {
		curvol = 0;
	}
	if(curvol > 0xffff) {
		curvol = 0xffff;
	}

	curvol = curvol | (curvol << 16);

	if(curvol != ocv) {
		char bf[1024];
		sprintf(bf, "booga %08x %08x", curvol, ocv);
		OutputDebugString(bf);
		waveOutSetVolume(0, curvol);
	}

	sprintf(num, "%d", (unsigned char)reportBuffer[4]);	
	m_pDlgWnd->m_GetBrightnessEditCtrl.SetWindowText(num);

	
	if (reportBuffer[5] & 0x01)
		m_pDlgWnd->m_PulseAlwaysEditCtrl.SetWindowText("ON");
	else
		m_pDlgWnd->m_PulseAlwaysEditCtrl.SetWindowText("OFF");
		
	if (reportBuffer[5] & 0x04)
		m_pDlgWnd->m_PulseDuringSleepEditCtrl.SetWindowText("Pulse During Sleep");
	else
		m_pDlgWnd->m_PulseDuringSleepEditCtrl.SetWindowText("Low Power Mode");

	int pulseSpeed = (reportBuffer[5] & 0x30) << 4;
	int pulseTable = (reportBuffer[5] & 0xC0) << 6;
	sprintf(num, "%d", (unsigned char)reportBuffer[6]);	
	m_pDlgWnd->m_PulsingSpeedEditCtrl.SetWindowText(num);
}*/

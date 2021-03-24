#include <Windows.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <mmsystem.h>
#include <mmeapi.h>
#include <iostream>
#include "Test.h"
#include <dshow.h>
#include <strmif.h>

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Strmiids.lib")


#if 1
//参数:
//	-2 恢复静音
//	-1 静音
//	0~100:音量比例
bool setSysVolume(int level)
{
	HRESULT hr;
	IMMDeviceEnumerator* pDeviceEnumerator = 0;
	IMMDevice* pDevice = 0;
	IAudioEndpointVolume* pAudioEndpointVolume = 0;
	IAudioClient* pAudioClient = 0;

	try {
		hr = CoInitialize(0);
		if (FAILED(hr)) throw "CoInitialize";
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pDeviceEnumerator);
		if (FAILED(hr)) throw "CoCreateInstance";
		hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
		if (FAILED(hr)) throw "GetDefaultAudioEndpoint";
		hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pAudioEndpointVolume);
		if (FAILED(hr)) throw "pDevice->Active";
		hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
		if (FAILED(hr)) throw "pDevice->Active";

		if (level == -2) {
			hr = pAudioEndpointVolume->SetMute(FALSE, NULL);
			if (FAILED(hr)) throw "SetMute";
		}
		else if (level == -1) {
			hr = pAudioEndpointVolume->SetMute(TRUE, NULL);
			if (FAILED(hr)) throw "SetMute";
		}
		else {
			if (level < 0 || level>100) {
				hr = E_INVALIDARG;
				throw "Invalid Arg";
			}

			float fVolume;
			fVolume = level / 100.0f;
			hr = pAudioEndpointVolume->SetMasterVolumeLevelScalar(fVolume, &GUID_NULL);
			if (FAILED(hr)) throw "SetMasterVolumeLevelScalar";

			pAudioClient->Release();
			pAudioEndpointVolume->Release();
			pDevice->Release();
			pDeviceEnumerator->Release();
			return true;
		}
		CoUninitialize();
	}
	catch (...) {
		if (pAudioClient) pAudioClient->Release();
		if (pAudioEndpointVolume) pAudioEndpointVolume->Release();
		if (pDevice) pDevice->Release();
		if (pDeviceEnumerator) pDeviceEnumerator->Release();
		throw;
	}
	return false;
}

int sysVolume()
{
	HRESULT hr;
	IMMDeviceEnumerator* pDeviceEnumerator = 0;
	IMMDevice* pDevice = 0;
	IAudioEndpointVolume* pAudioEndpointVolume = 0;
	IAudioClient* pAudioClient = 0;

	try {
		hr = CoInitialize(0);
		if (FAILED(hr)) throw "CoInitialize";
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pDeviceEnumerator);
		if (FAILED(hr)) throw "CoCreateInstance";
		hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
		if (FAILED(hr)) throw "GetDefaultAudioEndpoint";
		hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pAudioEndpointVolume);
		if (FAILED(hr)) throw "pDevice->Active";
		hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
		if (FAILED(hr)) throw "pDevice->Active";

		float fVolume;

		hr = pAudioEndpointVolume->GetMasterVolumeLevelScalar(&fVolume);

		if (FAILED(hr)) throw "SetMasterVolumeLevelScalar";

		pAudioClient->Release();
		pAudioEndpointVolume->Release();
		pDevice->Release();
		pDeviceEnumerator->Release();

		int  intVolume = fVolume * 100;
		if (fVolume > 100)
		{
			fVolume = 100;
		}
		return intVolume;
		CoUninitialize();
	}

	catch (...) {
		if (pAudioClient) pAudioClient->Release();
		if (pAudioEndpointVolume) pAudioEndpointVolume->Release();
		if (pDevice) pDevice->Release();
		if (pDeviceEnumerator) pDeviceEnumerator->Release();
		throw;
	}
}

bool SetMicrophoneVolumeMute(bool bMute)
{
	HMIXER hMixer;
	HMIXEROBJ aaa;
	MIXERLINECONTROLS mxlc;
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED vol;
	MIXERCONTROL mxc;
	MIXERLINE mxl;
	MIXERCONTROLDETAILS_BOOLEAN mcdb;

	// Check if Mixer is available
	int nMixerDevs = mixerGetNumDevs();
	if (nMixerDevs < 1)
		return false;
	bool bRet = false;
	// open the mixer
	// MIXER_OBJECTF_HWAVEIN
	// MIXER_OBJECTF_MIDIIN
	// MIXER_OBJECTF_HMIDIIN
	// MIXER_OBJECTF_AUX
	// MIXER_OBJECTF_WAVEIN
	UINT uRet = mixerOpen(&hMixer, 0, 0, 0, MIXER_OBJECTF_WAVEIN);
	if (uRet == MMSYSERR_NOERROR)
	{
		// also in use
		//mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
		mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
		mxl.cbStruct = sizeof(mxl);
		// mixerline info
		uRet = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
		if (uRet == MMSYSERR_NOERROR)
		{
			ZeroMemory(&mxlc, sizeof(mxlc));
			mxlc.cbStruct = sizeof(mxlc);
			mxlc.dwLineID = mxl.dwLineID;
			mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
			mxlc.cControls = 1;
			mxlc.cbmxctrl = sizeof(mxc);
			mxlc.pamxctrl = &mxc;
			// Get the mute control
			uRet = mixerGetLineControls((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
			if (uRet == MMSYSERR_NOERROR)
			{
				ZeroMemory(&mxcd, sizeof(mxcd));
				mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
				mxcd.dwControlID = mxc.dwControlID;
				mxcd.cChannels = 1;
				mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
				mxcd.paDetails = &mcdb;
				mcdb.fValue = bMute;
				// set, unset mute
				uRet = mixerSetControlDetails(
					(HMIXEROBJ)hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE);
				// mixerGetControlDetails(hMixer, &mxcd,
				// MIXER_GETCONTROLDETAILSF_VALUE);
				// Result = Boolean(mcdMute.fValue);
				bRet = uRet == MMSYSERR_NOERROR;
			}
		}
		uRet = mixerClose(hMixer);
	}
	return bRet;
}

int main() {

	/*try
	{
		setSysVolume(-1);
		std::cout << sysVolume() << '\n';
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
		return 1;
	}*/

	/*if (SetMicrophoneVolumeMute(true)) {
		std::cout << "operating success\n";
	}
	else {
		std::cout << "operating failed!!!\n";
	}*/

	CGTVolumeMute mute;
	//mute.SetVolume(4, 60);
	//mute.SetMute(4, false);
	std::cout << mute.GetVolume(1) << '\n';

	//mute.SetVolume(4, 30);


	//char sName[256] = { 0 };
	//std::string capture = "";
	//bool bRet = false;
	//::CoInitialize(NULL);

	//ICreateDevEnum* pCreateDevEnum;//enumrate all audio capture devices
	//HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum,
	//	NULL,
	//	CLSCTX_INPROC_SERVER,
	//	IID_ICreateDevEnum,
	//	(void**)&pCreateDevEnum);

	//IEnumMoniker* pEm;
	//hr = pCreateDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &pEm, 0);
	//if (hr != NOERROR)
	//{
	//	::CoUninitialize();
	//	return 1;
	//}

	//pEm->Reset();
	//ULONG cFetched;
	//IMoniker* pM;
	//while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK)
	//{

	//	IPropertyBag* pBag = NULL;
	//	hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pBag);
	//	if (SUCCEEDED(hr))
	//	{
	//		VARIANT var;
	//		var.vt = VT_BSTR;
	//		hr = pBag->Read(L"FriendlyName", &var, NULL);//还有其他属性，像描述信息等等
	//		if (hr == NOERROR)
	//		{
	//			//获取设备名称
	//			WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, sName, 256, "", NULL);
	//			printf("%s", sName);
	//			SysFreeString(var.bstrVal);
	//		}
	//		pBag->Release();
	//	}
	//	pM->Release();
	//	bRet = true;
	//}
	//pCreateDevEnum = NULL;
	//pEm = NULL;
	//::CoUninitialize();

	return 0;
}
#endif

#if 0
DWORD m_dwControlMasterVolume = 0;
DWORD m_dwControlMasterMute = 0;
DWORD mx = 0; // mixer ID

MMRESULT GetControls();
BOOL GetLineInfo(UINT mx, MIXERLINE& mxline, DWORD source, DWORD
	destination, DWORD& dwVolume, LONG& bMute);
BOOL GetLineControls(UINT mx, MIXERLINE& mxline, DWORD source, DWORD
	destination, DWORD& dwVolumeControl, DWORD& bMuteControl);
BOOL GetVolume(UINT Mixer, DWORD dwControlID, DWORD& dwVolume);
BOOL GetMute(UINT Mixer, DWORD dwControlID, LONG& bMute);


int main()
{
	DWORD dwMasterVolume = 0;
	LONG bMasterMute = 0;

	GetControls();

	GetVolume(mx, m_dwControlMasterVolume, dwMasterVolume);
	GetMute(mx, m_dwControlMasterMute, bMasterMute);
}

MMRESULT GetControls()
{
	UINT mx, ln, devs;
	MMRESULT mmr = MMSYSERR_NOERROR;
	MIXERCAPS mxcaps;
	MIXERLINE mxline;
	MIXERCONTROLDETAILS* mxdets = NULL;
	DWORD dwVolume, dwMasterVolume;
	LONG bMute, bMasterMute;


	devs = mixerGetNumDevs();

	mx = 0;
	//for(mx = 0; mx < devs; mx++) // loop optional to check all devices
	{
		mmr = mixerGetDevCaps(mx, &mxcaps, sizeof(MIXERCAPS));
		if (mmr != MMSYSERR_NOERROR) return 1;;

		ln = 0;
		// for(ln = 0; ln < mxcaps.cDestinations; ln++)
		{

			GetLineInfo(mx, mxline, -1, ln, dwMasterVolume, bMasterMute);
			GetLineControls(0, mxline, -1, ln, m_dwControlMasterVolume,
				m_dwControlMasterMute);
		}
	}

}

BOOL GetLineInfo(UINT mx, MIXERLINE& mxline, DWORD source, DWORD
	destination, DWORD& dwVolume, LONG& bMute)
{
	MMRESULT mmr = MMSYSERR_NOERROR;
	UINT ct;
	MIXERLINECONTROLS mxctls;

	memset(&mxline, 0, sizeof(MIXERLINE));
	mxline.cbStruct = sizeof(MIXERLINE);
	mxline.dwDestination = destination;
	mxline.dwSource = source;

	if (source == -1)
	{
		mxline.dwSource = 0;
		mmr = mixerGetLineInfo((HMIXEROBJ)mx, &mxline,
			MIXER_OBJECTF_MIXER | MIXER_GETLINEINFOF_DESTINATION);
	}
	else
		mmr = mixerGetLineInfo((HMIXEROBJ)mx, &mxline,
			MIXER_OBJECTF_MIXER | MIXER_GETLINEINFOF_SOURCE);

	mxctls.cbStruct = sizeof(MIXERLINECONTROLS);
	mxctls.dwLineID = mxline.dwLineID;
	mxctls.cControls = mxline.cControls;
	mxctls.cbmxctrl = sizeof(MIXERCONTROL);
	mxctls.pamxctrl = (LPMIXERCONTROL)malloc(mxctls.cbmxctrl *
		mxctls.cControls);


	mmr =
		mixerGetLineControls((HMIXEROBJ)mx, &mxctls, MIXER_OBJECTF_MIXER | MIXER_GETLINECONTROLSF_ALL);
	if (mmr == MMSYSERR_NOERROR)
	{
		for (ct = 0; ct < mxctls.cControls; ct++)
		{
			int temptype = MIXERCONTROL_CONTROLTYPE_VOLUME;

			if (mxctls.pamxctrl[ct].dwControlType ==
				MIXERCONTROL_CONTROLTYPE_VOLUME)
			{
				GetVolume(mx, mxctls.pamxctrl[ct].dwControlID, dwVolume);
			}
			else if (mxctls.pamxctrl[ct].dwControlType ==
				MIXERCONTROL_CONTROLTYPE_MUTE)
			{
				GetMute(mx, mxctls.pamxctrl[ct].dwControlID, bMute);
			}
			else if (mxctls.pamxctrl[ct].dwControlType ==
				MIXERCONTROL_CONTROLTYPE_SINGLESELECT)
			{
				//TODO:
			}
		}
	}

	free(mxctls.pamxctrl);
	mxctls.pamxctrl = NULL;

	return (mmr == 0);
}


BOOL GetLineControls(UINT mx, MIXERLINE& mxline, DWORD source, DWORD
	destination, DWORD& dwVolumeControl, DWORD& bMuteControl)
{
	MMRESULT mmr = MMSYSERR_NOERROR;
	UINT ct;
	MIXERLINECONTROLS mxctls;
	// MIXERLINE mxline;

	memset(&mxline, 0, sizeof(MIXERLINE));
	mxline.cbStruct = sizeof(MIXERLINE);
	mxline.dwDestination = destination;
	mxline.dwSource = source;

	if (source == -1)
	{
		mxline.dwSource = 0;
		mmr = mixerGetLineInfo((HMIXEROBJ)mx, &mxline,
			MIXER_OBJECTF_MIXER | MIXER_GETLINEINFOF_DESTINATION);
	}
	else
		mmr = mixerGetLineInfo((HMIXEROBJ)mx, &mxline,
			MIXER_OBJECTF_MIXER | MIXER_GETLINEINFOF_SOURCE);

	mxctls.cbStruct = sizeof(MIXERLINECONTROLS);
	mxctls.dwLineID = mxline.dwLineID;
	mxctls.cControls = mxline.cControls;
	mxctls.cbmxctrl = sizeof(MIXERCONTROL);
	mxctls.pamxctrl = (LPMIXERCONTROL)malloc(mxctls.cbmxctrl *
		mxctls.cControls);


	mmr =
		mixerGetLineControls((HMIXEROBJ)mx, &mxctls, MIXER_OBJECTF_MIXER | MIXER_GETLINECONTROLSF_ALL);
	if (mmr == MMSYSERR_NOERROR)
	{
		for (ct = 0; ct < mxctls.cControls; ct++)
		{
			int temptype = MIXERCONTROL_CONTROLTYPE_VOLUME;

			if (mxctls.pamxctrl[ct].dwControlType ==
				MIXERCONTROL_CONTROLTYPE_VOLUME)
			{
				dwVolumeControl = mxctls.pamxctrl[ct].dwControlID;
			}
			else if (mxctls.pamxctrl[ct].dwControlType ==
				MIXERCONTROL_CONTROLTYPE_MUTE)
			{
				bMuteControl = mxctls.pamxctrl[ct].dwControlID;
			}
		}
	}

	free(mxctls.pamxctrl);
	mxctls.pamxctrl = NULL;

	return (mmr == 0);
}

BOOL GetVolume(UINT Mixer, DWORD dwControlID, DWORD& dwVolume)
{
	MMRESULT mmr = MMSYSERR_NOERROR;
	MIXERCONTROLDETAILS* mxdets =
		(LPMIXERCONTROLDETAILS)calloc(1, sizeof(MIXERCONTROLDETAILS));

	mxdets->cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxdets->dwControlID = dwControlID;
	mxdets->cMultipleItems = 0;
	mxdets->cChannels = 1;
	mxdets->cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxdets->paDetails = &dwVolume;

	mmr = mixerGetControlDetails((HMIXEROBJ)Mixer, mxdets,
		MIXER_GETCONTROLDETAILSF_VALUE | MIXER_OBJECTF_MIXER);

	free(mxdets);

	return (mmr == 0);
}


BOOL GetMute(UINT Mixer, DWORD dwControlID, LONG& bMute)
{
	MMRESULT mmr = MMSYSERR_NOERROR;
	MIXERCONTROLDETAILS* mxdets =
		(LPMIXERCONTROLDETAILS)calloc(1, sizeof(MIXERCONTROLDETAILS));

	mxdets->cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxdets->dwControlID = dwControlID;
	mxdets->cMultipleItems = 0;
	mxdets->cChannels = 1;
	mxdets->cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
	mxdets->paDetails = &bMute;

	mmr = mixerGetControlDetails((HMIXEROBJ)Mixer, mxdets,
		MIXER_GETCONTROLDETAILSF_VALUE | MIXER_OBJECTF_MIXER);

	free(mxdets);

	return (mmr == 0);
}
#endif
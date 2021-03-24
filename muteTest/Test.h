#if !defined(AFX_GTVOLUMEMUTE_H__AFD7D44E_54A3_48B6_A87E_7CFD0A17D148__INCLUDED_)
#define AFX_GTVOLUMEMUTE_H__AFD7D44E_54A3_48B6_A87E_7CFD0A17D148__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include   <mmsystem.h>
#include   <mmreg.h>


class CGTVolumeMute
{
public:
	CGTVolumeMute();
	virtual ~CGTVolumeMute();

public:
	unsigned   WINAPI   GetVolume(int   dev);
	bool   WINAPI   SetVolume(long   dev, long   vol);
	bool   WINAPI   SetMute(long   dev, bool   vol);
	bool   WINAPI   GetMute(long   dev);

private:
	bool   GetVolumeControl(HMIXER  hmixer, long  componentType, long ctrlType, MIXERCONTROL* mxc);
	long   GetMuteValue(HMIXER   hmixer, MIXERCONTROL* mxc);
	unsigned   GetVolumeValue(HMIXER   hmixer, MIXERCONTROL* mxc);
	bool   SetMuteValue(HMIXER   hmixer, MIXERCONTROL* mxc, bool   mute);
	bool   SetVolumeValue(HMIXER   hmixer, MIXERCONTROL* mxc, long   volume);

};

#endif



#include "test.h"

//#include "stdafx.h"
//#include "Dlg.h"
//#include "GTVolumeMute.h"
#include <mmsystem.h>
#include <mmeapi.h>

#pragma comment(lib, "Winmm.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGTVolumeMute::CGTVolumeMute()
{

}

CGTVolumeMute::~CGTVolumeMute()
{

}

bool  CGTVolumeMute::GetVolumeControl(HMIXER   hmixer, long   componentType, long   ctrlType, MIXERCONTROL* mxc)
{
    MIXERLINECONTROLS   mxlc;
    MIXERLINE   mxl;
    mxl.cbStruct = sizeof(mxl);
    mxl.dwComponentType = componentType;
    if (!mixerGetLineInfo((HMIXEROBJ)hmixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE))
    {
        mxlc.cbStruct = sizeof(mxlc);
        mxlc.dwLineID = mxl.dwLineID;
        mxlc.dwControlType = ctrlType;
        mxlc.cControls = 1;
        mxlc.cbmxctrl = sizeof(MIXERCONTROL);
        mxlc.pamxctrl = mxc;
        if (mixerGetLineControls((HMIXEROBJ)hmixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE))
            return   0;
        else
            return   1;
    }
    return   0;
}
//---------------------------------------------------------------------------
long   CGTVolumeMute::GetMuteValue(HMIXER   hmixer, MIXERCONTROL* mxc)
{
    MIXERCONTROLDETAILS   mxcd;
    MIXERCONTROLDETAILS_BOOLEAN   mxcdMute;
    mxcd.hwndOwner = 0;
    mxcd.cbStruct = sizeof(mxcd);
    mxcd.dwControlID = mxc->dwControlID;
    mxcd.cbDetails = sizeof(mxcdMute);
    mxcd.paDetails = &mxcdMute;
    mxcd.cChannels = 1;
    mxcd.cMultipleItems = 0;
    if (mixerGetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE))
        return   -1;
    return   mxcdMute.fValue;
}

//---------------------------------------------------------------------------
unsigned   CGTVolumeMute::GetVolumeValue(HMIXER   hmixer, MIXERCONTROL* mxc)
{
    MIXERCONTROLDETAILS   mxcd;
    MIXERCONTROLDETAILS_UNSIGNED   vol;   vol.dwValue = 0;
    mxcd.hwndOwner = 0;
    mxcd.cbStruct = sizeof(mxcd);
    mxcd.dwControlID = mxc->dwControlID;
    mxcd.cbDetails = sizeof(vol);
    mxcd.paDetails = &vol;
    mxcd.cChannels = 1;
    if (mixerGetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE))
        return   -1;
    return   vol.dwValue;
}

//---------------------------------------------------------------------------
bool   CGTVolumeMute::SetMuteValue(HMIXER   hmixer, MIXERCONTROL* mxc, bool   mute)
{
    MIXERCONTROLDETAILS   mxcd;
    MIXERCONTROLDETAILS_BOOLEAN   mxcdMute; mxcdMute.fValue = mute;
    mxcd.hwndOwner = 0;
    mxcd.dwControlID = mxc->dwControlID;
    mxcd.cbStruct = sizeof(mxcd);
    mxcd.cbDetails = sizeof(mxcdMute);
    mxcd.paDetails = &mxcdMute;
    mxcd.cChannels = 1;
    mxcd.cMultipleItems = 0;
    if (mixerSetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE))
        return   0;
    return   1;
}

//---------------------------------------------------------------------------

bool   CGTVolumeMute::SetVolumeValue(HMIXER   hmixer, MIXERCONTROL* mxc, long   volume)
{
    MIXERCONTROLDETAILS   mxcd;
    MIXERCONTROLDETAILS_UNSIGNED   vol; vol.dwValue = volume;
    mxcd.hwndOwner = 0;
    mxcd.dwControlID = mxc->dwControlID;
    mxcd.cbStruct = sizeof(mxcd);
    mxcd.cbDetails = sizeof(vol);
    mxcd.paDetails = &vol;
    mxcd.cChannels = 1;
    if (mixerSetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE))
        return   0;
    return   1;
}


//---------------------------------------------------------------------------
unsigned   WINAPI   CGTVolumeMute::GetVolume(int   dev)//
{

    long   device; unsigned   rt = 0;
    MIXERCONTROL   volCtrl;
    HMIXER   hmixer;
    switch (dev)
    {
    case   1:
        device = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
        if (mixerOpen(&hmixer, 0, 0, 0, MIXER_OBJECTF_WAVEOUT))	return   0;
        break;
    case   2:
        device = MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER;
        if (mixerOpen(&hmixer, 0, 0, 0, 0))	return   0;
        break;
    case   3:
        device = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;
        if (mixerOpen(&hmixer, 0, 0, 0, 0))	return   0;
        break;
    case   4:
        device = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE;
        if (mixerOpen(&hmixer, 0, 0, 0, MIXER_OBJECTF_WAVEIN))	return   0;
        break;
    default:
        device = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
        if (mixerOpen(&hmixer, 0, 0, 0, 0))	return   0;
    }

    if (!GetVolumeControl(hmixer, device, MIXERCONTROL_CONTROLTYPE_VOLUME, &volCtrl))
        return   0;
    rt = GetVolumeValue(hmixer, &volCtrl) * 100 / volCtrl.Bounds.lMaximum + 1;
    mixerClose(hmixer);
    return   rt;

}

//---------------------------------------------------------------------------
bool   WINAPI   CGTVolumeMute::SetVolume(long   dev, long   vol)
{
    //   dev   =0,1,2   ·&Ouml;±&eth;±í&Ecirc;&frac34;&Ouml;÷&Ograve;&ocirc;&Aacute;&iquest;,&sup2;¨&ETH;&Icirc;,MIDI   ,LINE   IN
    //   vol=0-100   ±í&Ecirc;&frac34;&Ograve;&ocirc;&Aacute;&iquest;&micro;&Auml;&acute;ó&ETH;&iexcl;   ,   &Eacute;è&Ouml;&Atilde;&Oacute;&euml;·&micro;&raquo;&Oslash;&Ograve;&ocirc;&Aacute;&iquest;&micro;&Auml;&Ouml;&micro;&Oacute;&Atilde;&micro;&Auml;&Ecirc;&Ccedil;°&Ugrave;·&Ouml;±&Egrave;&pound;&not;&frac14;&acute;&Ograve;&ocirc;&Aacute;&iquest;&acute;&Oacute;0   -   100&pound;&not;&para;&oslash;&sup2;&raquo;&Ecirc;&Ccedil;&Eacute;è±&cedil;&micro;&Auml;&frac34;&oslash;&para;&Ocirc;&Ouml;&micro;
    //   retrun   false   ±í&Ecirc;&frac34;&Eacute;è&Ouml;&Atilde;&Ograve;&ocirc;&Aacute;&iquest;&micro;&Auml;&acute;ó&ETH;&iexcl;&micro;&Auml;&sup2;&Ugrave;×÷&sup2;&raquo;&sup3;&Eacute;&sup1;&brvbar;
    //   retrun   true   ±í&Ecirc;&frac34;&Eacute;è&Ouml;&Atilde;&Ograve;&ocirc;&Aacute;&iquest;&micro;&Auml;&acute;ó&ETH;&iexcl;&micro;&Auml;&sup2;&Ugrave;×÷&sup3;&Eacute;&sup1;&brvbar;

    long   device;
    bool   rc = false;
    MIXERCONTROL   volCtrl;
    HMIXER   hmixer;
    switch (dev)
    {
    case   1:
        device = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;   break;
    case   2:
        device = MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER;   break;
    case   3:
        device = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;   break;
    case   4:
        device = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE; break;
    default:
        device = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
    }

    if (mixerOpen(&hmixer, 0, 0, 0, MIXER_OBJECTF_WAVEIN))   return   0;

    bool res = GetVolumeControl(hmixer, device, MIXERCONTROL_CONTROLTYPE_VOLUME, &volCtrl);
    if (res)
    {
        vol = vol * volCtrl.Bounds.lMaximum / 100;
        if (SetVolumeValue(hmixer, &volCtrl, vol))
            rc = true;
    }
    mixerClose(hmixer);
    return   rc;
}

//---------------------------------------------------------------------------

bool   WINAPI   CGTVolumeMute::SetMute(long   dev, bool   vol)//&Eacute;è&Ouml;&Atilde;&Eacute;è±&cedil;&frac34;&sup2;&Ograve;&ocirc;
{
    //   dev   =0,1,2   分别表示主音量,波形,MIDI ,LINE IN
    //   vol=0,1   分别表示取消静音,设置静音
    //   retrun   false   表示取消或设置静音操作不成功
    //   retrun   true   表示取消或设置静音操作成功

    long   device;
    bool   rc = false;
    MIXERCONTROL   volCtrl;
    HMIXER   hmixer;
    switch (dev)
    {
    case   1:
        device = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;   break;
    case   2:
        device = MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER;   break;
    case   3:
        device = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;   break;
    case   4:
        device = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE; break;
    default:
        device = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
    }

    // with MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE
    if (mixerOpen(&hmixer, 0, 0, 0, MIXER_OBJECTF_WAVEIN))   return   0;
    if (GetVolumeControl(hmixer, device, MIXERCONTROL_CONTROLTYPE_MUTE, &volCtrl))
        if (SetMuteValue(hmixer, &volCtrl, (bool)vol))
            rc = true;
    mixerClose(hmixer);
    return   rc;
}

//---------------------------------------------------------------------------

bool   WINAPI   CGTVolumeMute::GetMute(long   dev)
{
    //dev   =0,1,2   ·&Ouml;±&eth;±í&Ecirc;&frac34;&Ouml;÷&Ograve;&ocirc;&Aacute;&iquest;&pound;&not;&sup2;¨&ETH;&Icirc;,MIDI   ,LINE   IN
    //   retrun   false   ±í&Ecirc;&frac34;&Atilde;&raquo;&Oacute;&ETH;&frac34;&sup2;&Ograve;&ocirc;
    //   retrun   true   ±í&Ecirc;&frac34;&frac34;&sup2;&Ograve;&ocirc;
    long   device;
    bool   rc = false;
    MIXERCONTROL   volCtrl;
    HMIXER   hmixer;
    switch (dev)
    {
    case   1:
        device = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;   break;
    case   2:
        device = MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER;   break;
    case   3:
        device = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC;   break;
    case   4:
        device = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE; break;
    default:
        device = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
    }

    if (mixerOpen(&hmixer, 0, 0, 0, MIXER_OBJECTF_WAVEIN))   
        return   0;

    if (GetVolumeControl(hmixer, device, MIXERCONTROL_CONTROLTYPE_MUTE, &volCtrl))
        rc = GetMuteValue(hmixer, &volCtrl) == 0 ? false : true;

    mixerClose(hmixer);    
    
    return   rc;
}

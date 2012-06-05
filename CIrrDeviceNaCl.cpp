// Copyright (C) 2002-2008 Nikolaus Gebhardt
// Copyright (C) 2008 Redshift Software, Inc.
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CIrrDeviceNaCl.h"

#ifdef _IRR_COMPILE_WITH_NACL_DEVICE_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "IEventReceiver.h"
#include "os.h"
#include "CTimer.h"
#include "irrString.h"
#include "Keycodes.h"
#include "COSOperator.h"
#include "CColorConverter.h"
#include "SIrrCreationParameters.h"
#include "SExposedVideoData.h"


namespace irr
{
	namespace video
	{
		#ifdef _IRR_COMPILE_WITH_OGLES2_
		IVideoDriver* createOGLES2Driver(const SIrrlichtCreationParameters& params, video::SExposedVideoData& data, io::IFileSystem* io);
		#endif
	}
}

namespace irr
{

//! constructor
CIrrDeviceNaCl::CIrrDeviceNaCl(const SIrrlichtCreationParameters& params)
	: CIrrDeviceStub(params)
{
	createDriver();

	if (VideoDriver)
		createGUIAndScene();
	
}

//! destructor
CIrrDeviceNaCl::~CIrrDeviceNaCl()
{
}

//! create the driver
void CIrrDeviceNaCl::createDriver()
{
	switch(CreationParams.DriverType)
	{
    case video::EDT_OGLES2:
        #ifdef _IRR_COMPILE_WITH_OGLES2_
            {
                video::SExposedVideoData data;
                VideoDriver = video::createOGLES2Driver(
                    CreationParams, data, FileSystem);
            }
		#endif
			break;
	case video::EDT_NULL:
		// create null driver
		VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
		break;

	default:
		os::Printer::log("Unable to create video driver of unknown type.", ELL_ERROR);
		break;
	}
}


bool CIrrDeviceNaCl::run()
{
    os::Timer::tick();
	return true;
}

//! Pause the current process for the minimum time allowed only to allow other processes to execute
void CIrrDeviceNaCl::yield()
{
}

//! Pause execution and let other processes to run for a specified amount of time.
void CIrrDeviceNaCl::sleep(u32 timeMs, bool pauseTimer=false)
{
}

//! sets the caption of the window
void CIrrDeviceNaCl::setWindowCaption(const wchar_t* text)
{
}

//! presents a surface in the client area
bool CIrrDeviceNaCl::present(video::IImage* surface, void* windowId, core::rect<s32>* srcClip)
{
	return true;
}

//! notifies the device that it should close itself
void CIrrDeviceNaCl::closeDevice()
{
}

//! \return Pointer to a list with all video modes supported
video::IVideoModeList* CIrrDeviceNaCl::getVideoModeList()
{
	return NULL;
}

//! returns if window is active. if not, nothing need to be drawn
bool CIrrDeviceNaCl::isWindowActive() const
{
	return true;
}


bool CIrrDeviceNaCl::isWindowFocused() const
{
	return isWindowActive();
}


bool CIrrDeviceNaCl::isWindowMinimized() const
{
	return !isWindowActive();
}

//! Set the current Gamma Value for the Display
bool CIrrDeviceNaCl::setGammaRamp( f32 red, f32 green, f32 blue, f32 brightness, f32 contrast )
{
	return false;
}

//! Get the current Gamma Value for the Display
bool CIrrDeviceNaCl::getGammaRamp( f32 &red, f32 &green, f32 &blue, f32 &brightness, f32 &contrast )
{
	return false;
}

//! returns color format of the window.
video::ECOLOR_FORMAT CIrrDeviceNaCl::getColorFormat() const
{
		return CIrrDeviceStub::getColorFormat();
}

//! Sets if the window should be resizeable in windowed mode.
void CIrrDeviceNaCl::setResizable(bool resize)
{
}


//! Minimizes the window
void CIrrDeviceNaCl::minimizeWindow()
{
}


//! Maximizes the window if possible.
void CIrrDeviceNaCl::maximizeWindow()
{
}


//! Restore the window to normal size if possible.
void CIrrDeviceNaCl::restoreWindow()
{
}

//! Activate any joysticks, and generate events for them.
bool CIrrDeviceNaCl::activateJoysticks(core::array<SJoystickInfo> & joystickInfo)
{
	return false;
}

}

#endif // _IRR_COMPILE_WITH_NACL_DEVICE_

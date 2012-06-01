// Copyright (C) 2002-2008 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#include "CIrrDeviceNaCl.h"

#ifdef _IRR_NACL_PLATFORM_

#include "IEventReceiver.h"
#include "irrList.h"
#include "os.h"
#include "CTimer.h"
#include "irrString.h"
#include "Keycodes.h"
#include "COSOperator.h"
#include <stdio.h>
#include <stdlib.h>
#include "SIrrCreationParameters.h"

namespace irr
{
	namespace video
	{
		IVideoDriver* createOGLES2Driver(const SIrrlichtCreationParameters& params,
				video::SExposedVideoData& data,
				io::IFileSystem* io);
	}
} // end namespace irr


namespace irr
{


//! constructor
CIrrDeviceNaCl::CIrrDeviceNaCl(const SIrrlichtCreationParameters& param)
    : CIrrDeviceStub(param)
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
			VideoDriver = video::createOGLES2Driver(CreationParams, data, FileSystem);
		}
	#else
		os::Printer::log("No OpenGL-ES2 support compiled in.", ELL_ERROR);
	#endif
		break;

	case video::EDT_NULL:
		VideoDriver = video::createNullDriver(FileSystem, CreationParams.WindowSize);
		break;

    case video::EDT_SOFTWARE:
        #ifdef _IRR_COMPILE_WITH_SOFTWARE_
        VideoDriver = video::createSoftwareDriver(CreationParams.WindowSize, CreationParams.Fullscreen, FileSystem, this);
        #else
        os::Printer::log("No Software driver support compiled in.", ELL_ERROR);
        #endif
        break;

    case video::EDT_BURNINGSVIDEO:
        #ifdef _IRR_COMPILE_WITH_BURNINGSVIDEO_
        VideoDriver = video::createBurningVideoDriver(CreationParams, FileSystem, this);
        #else
        os::Printer::log("Burning's video driver was not compiled in.", ELL_ERROR);
        #endif
        break;

    default:
        os::Printer::log("Unable to create video driver of unknown type.", ELL_ERROR);
        break;
    }
}


//! runs the device. Returns false if device wants to be deleted
bool CIrrDeviceNaCl::run()
{
    os::Timer::tick();
    return true;
}

//! Activate any joysticks, and generate events for them.
bool CIrrDeviceNaCl::activateJoysticks(core::array<SJoystickInfo> & joystickInfo)
{
    return false;
}



//! pause execution temporarily
void CIrrDeviceNaCl::yield()
{
}


//! pause execution for a specified time
void CIrrDeviceNaCl::sleep(u32 timeMs, bool pauseTimer)
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


//! Sets if the window should be resizeable in windowed mode.
void CIrrDeviceNaCl::setResizable(bool resize)
{
}


//! returns if window is active. if not, nothing need to be drawn
bool CIrrDeviceNaCl::isWindowActive() const
{
    return (WindowHasFocus && !WindowMinimized);
}


//! returns if window has focus.
bool CIrrDeviceNaCl::isWindowFocused() const
{
    return WindowHasFocus;
}


//! returns if window is minimized.
bool CIrrDeviceNaCl::isWindowMinimized() const
{
    return WindowMinimized;
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

void CIrrDeviceNaCl::createKeyMap()
{
}

IRRLICHT_API IrrlichtDevice* IRRCALLCONV createDeviceEx(const SIrrlichtCreationParameters& param)
{
    CIrrDeviceNaCl* dev = new CIrrDeviceNaCl(param);

    if (dev && !dev->getVideoDriver() && param.DriverType != video::EDT_NULL)
    {
        dev->drop();
        dev = 0;
    }

    return dev;
}


} // end namespace irr

#endif // _IRR_NACL_PLATFORM_


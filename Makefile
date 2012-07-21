VERSION_MAJOR = 1
VERSION_MINOR = 8
VERSION_RELEASE = 0-SVN

# Irrlicht Engine 1.8.0-SVN
# Makefile for Linux
#
# To use, just run:
#
# make
#
# This will compile Irrlicht, create a static lib (libIrrlicht.a), and copy it
# into the subdirectory lib/Linux. That's all.
#
# If you want Irrlicht to be compiled as shared lib (libIrrlicht.so.versionnumber), then run:
#
# make sharedlib
# make install
#
# If you want to compile in release mode run:
#
# make NDEBUG=1
#
# For cross-compilation for Win32 under Linux, just use the win32 targets. You have to set
# at least CXX, CC, and AR to the proper binaries.
#

#List of object files, separated based on engine architecture


IRRMESHLOADER = CB3DMeshFileLoader.o CMD2MeshFileLoader.o CXMeshFileLoader.o
IRRMESHOBJ = $(IRRMESHLOADER) \
	CSkinnedMesh.o CBoneSceneNode.o CMeshSceneNode.o \
	CAnimatedMeshSceneNode.o CAnimatedMeshMD2.o CAnimatedMeshMD3.o \
	CQ3LevelMesh.o CQuake3ShaderSceneNode.o CAnimatedMeshHalfLife.o
IRROBJ = CBillboardSceneNode.o CCameraSceneNode.o CDummyTransformationSceneNode.o CEmptySceneNode.o CGeometryCreator.o CLightSceneNode.o CMeshManipulator.o CMetaTriangleSelector.o COctreeSceneNode.o COctreeTriangleSelector.o CSceneCollisionManager.o CSceneManager.o CShadowVolumeSceneNode.o CSkyBoxSceneNode.o CSkyDomeSceneNode.o CTerrainSceneNode.o CTerrainTriangleSelector.o CVolumeLightSceneNode.o CCubeSceneNode.o CSphereSceneNode.o CTextSceneNode.o CTriangleBBSelector.o CTriangleSelector.o CWaterSurfaceSceneNode.o CMeshCache.o CDefaultSceneNodeAnimatorFactory.o CDefaultSceneNodeFactory.o CSceneLoaderIrr.o
IRRPARTICLEOBJ = CParticleAnimatedMeshSceneNodeEmitter.o CParticleBoxEmitter.o CParticleCylinderEmitter.o CParticleMeshEmitter.o CParticlePointEmitter.o CParticleRingEmitter.o CParticleSphereEmitter.o CParticleAttractionAffector.o CParticleFadeOutAffector.o CParticleGravityAffector.o CParticleRotationAffector.o CParticleSystemSceneNode.o CParticleScaleAffector.o
IRRANIMOBJ = CSceneNodeAnimatorCameraFPS.o CSceneNodeAnimatorCameraMaya.o CSceneNodeAnimatorCollisionResponse.o CSceneNodeAnimatorDelete.o CSceneNodeAnimatorFlyCircle.o CSceneNodeAnimatorFlyStraight.o CSceneNodeAnimatorFollowSpline.o CSceneNodeAnimatorRotation.o CSceneNodeAnimatorTexture.o
IRRDRVROBJ = CNullDriver.o COGLES2Driver.o COGLES2FixedPipelineShader.o COGLES2ParallaxMapRenderer.o COGLES2SLMaterialRenderer.o COGLES2ExtensionHandler.o COGLES2NormalMapRenderer.o COGLES2Renderer2D.o COGLES2Texture.o
IRRIMAGEOBJ = CColorConverter.o CImage.o CImageLoaderBMP.o CImageLoaderPNG.o CImageWriterPNG.o
IRRVIDEOOBJ = CVideoModeList.o CFPSCounter.o $(IRRDRVROBJ) $(IRRIMAGEOBJ)
IRRIOOBJ = CFileList.o CFileSystem.o CLimitReadFile.o CMemoryFile.o CReadFile.o CWriteFile.o CXMLReader.o CXMLWriter.o CWADReader.o CZipReader.o CPakReader.o CNPKReader.o CTarReader.o CMountPointReader.o irrXML.o CAttributes.o lzma/LzmaDec.o
IRROTHEROBJ = CIrrDeviceConsole.o CIrrDeviceStub.o CIrrDeviceNaCl.o CLogger.o COSOperator.o Irrlicht.o os.o
IRRGUIOBJ = CGUIButton.o CGUICheckBox.o CGUIComboBox.o CGUIContextMenu.o CGUIEditBox.o CGUIEnvironment.o CGUIFileOpenDialog.o CGUIFont.o CGUIImage.o CGUIInOutFader.o CGUIListBox.o CGUIMenu.o CGUIMeshViewer.o CGUIMessageBox.o CGUIModalScreen.o CGUIScrollBar.o CGUISpinBox.o CGUISkin.o CGUIStaticText.o CGUITabControl.o CGUITable.o CGUIToolBar.o CGUIWindow.o CGUIColorSelectDialog.o CDefaultGUIElementFactory.o CGUISpriteBank.o CGUIImageList.o CGUITreeView.o
ZLIBOBJ = zlib/adler32.o zlib/compress.o zlib/crc32.o zlib/deflate.o zlib/inffast.o zlib/inflate.o zlib/inftrees.o zlib/trees.o zlib/uncompr.o zlib/zutil.o
LIBPNGOBJ = libpng/png.o libpng/pngerror.o libpng/pngget.o libpng/pngmem.o libpng/pngpread.o libpng/pngread.o libpng/pngrio.o libpng/pngrtran.o libpng/pngrutil.o libpng/pngset.o libpng/pngtrans.o libpng/pngwio.o libpng/pngwrite.o libpng/pngwtran.o libpng/pngwutil.o
BZIP2OBJ = bzip2/blocksort.o bzip2/huffman.o bzip2/crctable.o bzip2/randtable.o bzip2/bzcompress.o bzip2/decompress.o bzip2/bzlib.o

# Next variable is for additional scene nodes etc. of customized Irrlicht versions
EXTRAOBJ =
LINKOBJ = $(IRRMESHOBJ) $(IRROBJ) $(IRRPARTICLEOBJ) $(IRRANIMOBJ) \
	$(IRRVIDEOOBJ) $(IRRSWRENDEROBJ) $(IRRIOOBJ) $(IRROTHEROBJ) \
	$(IRRGUIOBJ) $(ZLIBOBJ) $(JPEGLIBOBJ) $(LIBPNGOBJ) $(LIBAESGM) \
	$(BZIP2OBJ) $(EXTRAOBJ)


###############
#Compiler flags

THIS_MAKEFILE:=$(abspath $(lastword $(MAKEFILE_LIST)))
NACL_SDK_ROOT?=$(abspath $(dir $(THIS_MAKEFILE))../..)

OSNAME:=win
TC_PATH:=$(abspath $(NACL_SDK_ROOT)/toolchain/$(OSNAME)_x86_newlib)
CXX:=$(TC_PATH)/bin/i686-nacl-g++
CC:=$(TC_PATH)/bin/i686-nacl-gcc
AR:=$(TC_PATH)/bin/i686-nacl-ar


CXXINCS = -Iinclude -Izlib -Ilibpng
CPPFLAGS += $(CXXINCS) -DIRRLICHT_EXPORTS=1
CXXFLAGS += -Wall -pipe -fno-exceptions -fno-rtti -fstrict-aliasing
ifndef NDEBUG
CXXFLAGS += -g -D_DEBUG
else
CXXFLAGS += -fexpensive-optimizations -O3
endif
ifdef PROFILE
CXXFLAGS += -pg
endif
CFLAGS := -O3 -fexpensive-optimizations -DPNG_THREAD_UNSAFE_OK -DPNG_NO_MMX_CODE -DPNG_NO_MNG_FEATURES

sharedlib sharedlib_osx: CXXFLAGS += -fPIC
sharedlib sharedlib_osx: CFLAGS += -fPIC

#Linux specific options
staticlib sharedlib install: SYSTEM = Linux
STATIC_LIB = libIrrlicht.a
LIB_PATH = lib/$(SYSTEM)
INSTALL_DIR = /usr/local/lib
sharedlib install: SHARED_LIB = libIrrlicht.so
staticlib sharedlib: LDFLAGS += --no-export-all-symbols --add-stdcall-alias

VERSION = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_RELEASE)
SHARED_FULLNAME = $(SHARED_LIB).$(VERSION)
SONAME = $(SHARED_LIB).$(VERSION_MAJOR).$(VERSION_MINOR)

####################
# All target, builds Irrlicht as static lib (libIrrlicht.a) and copies it into lib/Linux
all linux: staticlib

# Builds Irrlicht as shared lib (libIrrlicht.so.versionNumber) and copies it into lib/Linux
sharedlib: $(LINKOBJ)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -shared -Wl,-soname,$(SONAME) -o $(SHARED_FULLNAME) $^ $(LDFLAGS)
	mkdir -p $(LIB_PATH)
	cp $(SHARED_FULLNAME) $(LIB_PATH)
	cp $(SHARED_FULLNAME) ../testIrrNaCl/libs/libIrrlicht.so

# Builds Irrlicht as static lib (libIrrlicht.a)
$(STATIC_LIB): $(LINKOBJ)
	$(AR) rs $@ $^

# Copies static lib into lib/Linux
staticlib staticlib_osx: $(STATIC_LIB)
	mkdir -p $(LIB_PATH)
	cp $^ $(LIB_PATH)
	cp $^ ../testIrrNaCl/Irrlicht/libIrrlicht.a

TAGS:
	ctags *.cpp ../../include/*.h *.h

# Create dependency files for automatic recompilation
%.d:%.cpp
	$(CXX) $(CPPFLAGS) -MM -MF $@ $<

# Create dependency files for automatic recompilation
%.d:%.c
	$(CC) $(CPPFLAGS) -MM -MF $@ $<

# Create object files from objective-c code
%.o:%.mm
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

ifneq ($(MAKECMDGOALS),clean)
-include $(LINKOBJ:.o=.d)
endif

help:
	@echo "Available targets for Irrlicht"
	@echo " sharedlib: Build shared library Irrlicht.so for Linux"
	@echo " staticlib: Build static library Irrlicht.a for Linux"
	@echo " install: Copy shared library to /usr/local/lib"
	@echo ""
	@echo " sharedlib_win32: Build shared library Irrlicht.dll for Windows"
	@echo " staticlib_win32: Build static library Irrlicht.a for Windows"
	@echo ""
	@echo " clean: Clean up directory"

# Cleans all temporary files and compilation results.
clean:
	$(RM) $(LINKOBJ) $(SHARED_FULLNAME) $(STATIC_LIB) $(LINKOBJ:.o=.d)

.PHONY: all sharedlib staticlib sharedlib_win32 staticlib_win32 help install clean

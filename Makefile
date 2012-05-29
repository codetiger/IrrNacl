
#
# Project information
#
# These variables store project specific settings for the project name
# build flags, files to copy or install.  In the examples it is typically
# only the list of sources and project name that will actually change and
# the rest of the makefile is boilerplate for defining build rules.
#

PROJECT:=irrnacl
LDFLAGS:=-lppapi_gles2 -lppapi_cpp -lppapi

INCLUDES_DIRS := \
				Irrlicht \
				Irrlicht/include \
				Irrlicht/aesGladman \
				Irrlicht/bzip2 \
				Irrlicht/jpeglib \
				Irrlicht/libpng \
				Irrlicht/lzma \
				Irrlicht/zlib

IRRMESHLOADER := \
				Irrlicht/CBSPMeshFileLoader.cpp \
				Irrlicht/CMD2MeshFileLoader.cpp \
				Irrlicht/CMD3MeshFileLoader.cpp \
				Irrlicht/CMS3DMeshFileLoader.cpp \
				Irrlicht/CB3DMeshFileLoader.cpp \
				Irrlicht/C3DSMeshFileLoader.cpp \
				Irrlicht/COgreMeshFileLoader.cpp \
				Irrlicht/COBJMeshFileLoader.cpp \
				Irrlicht/CColladaFileLoader.cpp \
				Irrlicht/CCSMLoader.cpp \
				Irrlicht/CDMFLoader.cpp \
				Irrlicht/CLMTSMeshFileLoader.cpp \
				Irrlicht/CMY3DMeshFileLoader.cpp \
				Irrlicht/COCTLoader.cpp \
				Irrlicht/CXMeshFileLoader.cpp \
				Irrlicht/CIrrMeshFileLoader.cpp \
				Irrlicht/CSTLMeshFileLoader.cpp \
				Irrlicht/CLWOMeshFileLoader.cpp \
				Irrlicht/CPLYMeshFileLoader.cpp

IRRMESHWRITER := \
				Irrlicht/CColladaMeshWriter.cpp \
				Irrlicht/CIrrMeshWriter.cpp \
				Irrlicht/CSTLMeshWriter.cpp \
				Irrlicht/COBJMeshWriter.cpp \
				Irrlicht/CPLYMeshWriter.cpp

IRRMESHOBJ := \
				$(IRRMESHLOADER) \
				$(IRRMESHWRITER) \
				Irrlicht/CSkinnedMesh.cpp \
				Irrlicht/CBoneSceneNode.cpp \
				Irrlicht/CMeshSceneNode.cpp \
				Irrlicht/CAnimatedMeshSceneNode.cpp \
				Irrlicht/CAnimatedMeshMD2.cpp \
				Irrlicht/CAnimatedMeshMD3.cpp \
				Irrlicht/CQ3LevelMesh.cpp \
				Irrlicht/CQuake3ShaderSceneNode.cpp

IRROBJ := \
				Irrlicht/CBillboardSceneNode.cpp \
				Irrlicht/CCameraSceneNode.cpp \
				Irrlicht/CDummyTransformationSceneNode.cpp \
				Irrlicht/CEmptySceneNode.cpp \
				Irrlicht/CGeometryCreator.cpp \
				Irrlicht/CLightSceneNode.cpp \
				Irrlicht/CMeshManipulator.cpp \
				Irrlicht/CMetaTriangleSelector.cpp \
				Irrlicht/COctreeSceneNode.cpp \
				Irrlicht/COctreeTriangleSelector.cpp \
				Irrlicht/CSceneCollisionManager.cpp \
				Irrlicht/CSceneManager.cpp \
				Irrlicht/CShadowVolumeSceneNode.cpp \
				Irrlicht/CSkyBoxSceneNode.cpp \
				Irrlicht/CSkyDomeSceneNode.cpp \
				Irrlicht/CTerrainSceneNode.cpp \
				Irrlicht/CTerrainTriangleSelector.cpp \
				Irrlicht/CVolumeLightSceneNode.cpp \
				Irrlicht/CCubeSceneNode.cpp \
				Irrlicht/CSphereSceneNode.cpp \
				Irrlicht/CTextSceneNode.cpp \
				Irrlicht/CTriangleBBSelector.cpp \
				Irrlicht/CTriangleSelector.cpp \
				Irrlicht/CWaterSurfaceSceneNode.cpp \
				Irrlicht/CMeshCache.cpp \
				Irrlicht/CDefaultSceneNodeAnimatorFactory.cpp \
				Irrlicht/CDefaultSceneNodeFactory.cpp

IRRPARTICLEOBJ := \
				Irrlicht/CParticleAnimatedMeshSceneNodeEmitter.cpp \
				Irrlicht/CParticleBoxEmitter.cpp \
				Irrlicht/CParticleCylinderEmitter.cpp \
				Irrlicht/CParticleMeshEmitter.cpp \
				Irrlicht/CParticlePointEmitter.cpp \
				Irrlicht/CParticleRingEmitter.cpp \
				Irrlicht/CParticleSphereEmitter.cpp \
				Irrlicht/CParticleAttractionAffector.cpp \
				Irrlicht/CParticleFadeOutAffector.cpp \
				Irrlicht/CParticleGravityAffector.cpp \
				Irrlicht/CParticleRotationAffector.cpp \
				Irrlicht/CParticleSystemSceneNode.cpp \
				Irrlicht/CParticleScaleAffector.cpp

IRRANIMOBJ := \
				Irrlicht/CSceneNodeAnimatorCameraFPS.cpp \
				Irrlicht/CSceneNodeAnimatorCameraMaya.cpp \
				Irrlicht/CSceneNodeAnimatorCollisionResponse.cpp \
				Irrlicht/CSceneNodeAnimatorDelete.cpp \
				Irrlicht/CSceneNodeAnimatorFlyCircle.cpp \
				Irrlicht/CSceneNodeAnimatorFlyStraight.cpp \
				Irrlicht/CSceneNodeAnimatorFollowSpline.cpp \
				Irrlicht/CSceneNodeAnimatorRotation.cpp \
				Irrlicht/CSceneNodeAnimatorTexture.cpp

IRRDRVROBJ := \
				Irrlicht/CNullDriver.cpp \
				Irrlicht/COGLESDriver.cpp \
				Irrlicht/COGLESTexture.cpp \
				Irrlicht/COGLESExtensionHandler.cpp

IRRDRVOBJ2 := \
				Irrlicht/COGLES2Driver.cpp \
				Irrlicht/COGLES2FixedPipelineShader.cpp \
				Irrlicht/COGLES2ParallaxMapRenderer.cpp \
				Irrlicht/COGLES2SLMaterialRenderer.cpp \
				Irrlicht/COGLES2ExtensionHandler.cpp \
				Irrlicht/COGLES2NormalMapRenderer.cpp \
				Irrlicht/COGLES2Renderer2D.cpp \
				Irrlicht/COGLES2Texture.cpp

IRRIMAGEOBJ := \
				Irrlicht/CColorConverter.cpp \
				Irrlicht/CImage.cpp \
				Irrlicht/CImageLoaderBMP.cpp \
				Irrlicht/CImageLoaderJPG.cpp \
				Irrlicht/CImageLoaderPCX.cpp \
				Irrlicht/CImageLoaderPNG.cpp \
				Irrlicht/CImageLoaderPSD.cpp \
				Irrlicht/CImageLoaderTGA.cpp \
				Irrlicht/CImageLoaderPPM.cpp \
				Irrlicht/CImageLoaderWAL.cpp \
				Irrlicht/CImageLoaderRGB.cpp \
				Irrlicht/CImageWriterBMP.cpp \
				Irrlicht/CImageWriterJPG.cpp \
				Irrlicht/CImageWriterPCX.cpp \
				Irrlicht/CImageWriterPNG.cpp \
				Irrlicht/CImageWriterPPM.cpp \
				Irrlicht/CImageWriterPSD.cpp \
				Irrlicht/CImageWriterTGA.cpp

IRRVIDEOOBJ := \
				Irrlicht/CVideoModeList.cpp \
				Irrlicht/CFPSCounter.cpp \
				$(IRRDRVROBJ) \
				$(IRRIMAGEOBJ)

IRRSWRENDEROBJ := \
				Irrlicht/CSoftwareDriver.cpp \
				Irrlicht/CSoftwareTexture.cpp \
				Irrlicht/CTRFlat.cpp \
				Irrlicht/CTRFlatWire.cpp \
				Irrlicht/CTRGouraud.cpp \
				Irrlicht/CTRGouraudWire.cpp \
				Irrlicht/CTRTextureFlat.cpp \
				Irrlicht/CTRTextureFlatWire.cpp \
				Irrlicht/CTRTextureGouraud.cpp \
				Irrlicht/CTRTextureGouraudAdd.cpp \
				Irrlicht/CTRTextureGouraudNoZ.cpp \
				Irrlicht/CTRTextureGouraudWire.cpp \
				Irrlicht/CZBuffer.cpp \
				Irrlicht/CTRTextureGouraudVertexAlpha2.cpp \
				Irrlicht/CTRTextureGouraudNoZ2.cpp \
				Irrlicht/CTRTextureLightMap2_M2.cpp \
				Irrlicht/CTRTextureLightMap2_M4.cpp \
				Irrlicht/CTRTextureLightMap2_M1.cpp \
				Irrlicht/CSoftwareDriver2.cpp \
				Irrlicht/CSoftwareTexture2.cpp \
				Irrlicht/CTRTextureGouraud2.cpp \
				Irrlicht/CTRGouraud2.cpp \
				Irrlicht/CTRGouraudAlpha2.cpp \
				Irrlicht/CTRGouraudAlphaNoZ2.cpp \
				Irrlicht/CTRTextureDetailMap2.cpp \
				Irrlicht/CTRTextureGouraudAdd2.cpp \
				Irrlicht/CTRTextureGouraudAddNoZ2.cpp \
				Irrlicht/CTRTextureWire2.cpp \
				Irrlicht/CTRTextureLightMap2_Add.cpp \
				Irrlicht/CTRTextureLightMapGouraud2_M4.cpp \
				Irrlicht/IBurningShader.cpp \
				Irrlicht/CTRTextureBlend.cpp \
				Irrlicht/CTRTextureGouraudAlpha.cpp \
				Irrlicht/CTRTextureGouraudAlphaNoZ.cpp \
				Irrlicht/CDepthBuffer.cpp \
				Irrlicht/CBurningShader_Raster_Reference.cpp

IRRIOOBJ := \
				Irrlicht/CFileList.cpp \
				Irrlicht/CFileSystem.cpp \
				Irrlicht/CLimitReadFile.cpp \
				Irrlicht/CMemoryFile.cpp \
				Irrlicht/CReadFile.cpp \
				Irrlicht/CWriteFile.cpp \
				Irrlicht/CXMLReader.cpp \
				Irrlicht/CXMLWriter.cpp \
				Irrlicht/CZipReader.cpp \
				Irrlicht/CPakReader.cpp \
				Irrlicht/CNPKReader.cpp \
				Irrlicht/CTarReader.cpp \
				Irrlicht/CMountPointReader.cpp \
				Irrlicht/irrXML.cpp \
				Irrlicht/CAttributes.cpp

IRROTHEROBJ := \
				Irrlicht/CIrrDeviceConsole.cpp \
				Irrlicht/CIrrDeviceStub.cpp \
				Irrlicht/CLogger.cpp \
				Irrlicht/Irrlicht.cpp \
				Irrlicht/os.cpp

IRRGUIOBJ := \
				Irrlicht/CGUIButton.cpp \
				Irrlicht/CGUICheckBox.cpp \
				Irrlicht/CGUIComboBox.cpp \
				Irrlicht/CGUIContextMenu.cpp \
				Irrlicht/CGUIEditBox.cpp \
				Irrlicht/CGUIEnvironment.cpp \
				Irrlicht/CGUIFileOpenDialog.cpp \
				Irrlicht/CGUIFont.cpp \
				Irrlicht/CGUIImage.cpp \
				Irrlicht/CGUIInOutFader.cpp \
				Irrlicht/CGUIListBox.cpp \
				Irrlicht/CGUIMenu.cpp \
				Irrlicht/CGUIMeshViewer.cpp \
				Irrlicht/CGUIMessageBox.cpp \
				Irrlicht/CGUIModalScreen.cpp \
				Irrlicht/CGUIScrollBar.cpp \
				Irrlicht/CGUISpinBox.cpp \
				Irrlicht/CGUISkin.cpp \
				Irrlicht/CGUIStaticText.cpp \
				Irrlicht/CGUITabControl.cpp \
				Irrlicht/CGUITable.cpp \
				Irrlicht/CGUIToolBar.cpp \
				Irrlicht/CGUIWindow.cpp \
				Irrlicht/CGUIColorSelectDialog.cpp \
				Irrlicht/CDefaultGUIElementFactory.cpp \
				Irrlicht/CGUISpriteBank.cpp \
				Irrlicht/CGUIImageList.cpp \
				Irrlicht/CGUITreeView.cpp

ZLIBOBJ := \
				Irrlicht/zlib/adler32.c \
				Irrlicht/zlib/compress.c \
				Irrlicht/zlib/crc32.c \
				Irrlicht/zlib/deflate.c \
				Irrlicht/zlib/inffast.c \
				Irrlicht/zlib/inflate.c \
				Irrlicht/zlib/inftrees.c \
				Irrlicht/zlib/trees.c \
				Irrlicht/zlib/uncompr.c \
				Irrlicht/zlib/zutil.c

JPEGLIBOBJ := \
				Irrlicht/jpeglib/jcapimin.c \
				Irrlicht/jpeglib/jcapistd.c \
				Irrlicht/jpeglib/jccoefct.c \
				Irrlicht/jpeglib/jccolor.c \
				Irrlicht/jpeglib/jcdctmgr.c \
				Irrlicht/jpeglib/jchuff.c \
				Irrlicht/jpeglib/jcinit.c \
				Irrlicht/jpeglib/jcmainct.c \
				Irrlicht/jpeglib/jcmarker.c \
				Irrlicht/jpeglib/jcmaster.c \
				Irrlicht/jpeglib/jcomapi.c \
				Irrlicht/jpeglib/jcparam.c \
				Irrlicht/jpeglib/jcprepct.c \
				Irrlicht/jpeglib/jcsample.c \
				Irrlicht/jpeglib/jctrans.c \
				Irrlicht/jpeglib/jdapimin.c \
				Irrlicht/jpeglib/jdapistd.c \
				Irrlicht/jpeglib/jdatadst.c \
				Irrlicht/jpeglib/jdatasrc.c \
				Irrlicht/jpeglib/jdcoefct.c \
				Irrlicht/jpeglib/jdcolor.c \
				Irrlicht/jpeglib/jddctmgr.c \
				Irrlicht/jpeglib/jdhuff.c \
				Irrlicht/jpeglib/jdinput.c \
				Irrlicht/jpeglib/jdmainct.c \
				Irrlicht/jpeglib/jdmarker.c \
				Irrlicht/jpeglib/jdmaster.c \
				Irrlicht/jpeglib/jdmerge.c \
				Irrlicht/jpeglib/jdpostct.c \
				Irrlicht/jpeglib/jdsample.c \
				Irrlicht/jpeglib/jdtrans.c \
				Irrlicht/jpeglib/jerror.c \
				Irrlicht/jpeglib/jfdctflt.c \
				Irrlicht/jpeglib/jfdctfst.c \
				Irrlicht/jpeglib/jfdctint.c \
				Irrlicht/jpeglib/jidctflt.c \
				Irrlicht/jpeglib/jidctfst.c \
				Irrlicht/jpeglib/jidctint.c \
				Irrlicht/jpeglib/jmemmgr.c \
				Irrlicht/jpeglib/jmemnobs.c \
				Irrlicht/jpeglib/jquant1.c \
				Irrlicht/jpeglib/jquant2.c \
				Irrlicht/jpeglib/jutils.c \
				Irrlicht/jpeglib/jcarith.c \
				Irrlicht/jpeglib/jdarith.c \
				Irrlicht/jpeglib/jaricom.c

LIBPNGOBJ := \
				Irrlicht/libpng/png.c \
				Irrlicht/libpng/pngerror.c \
				Irrlicht/libpng/pngget.c \
				Irrlicht/libpng/pngmem.c \
				Irrlicht/libpng/pngpread.c \
				Irrlicht/libpng/pngread.c \
				Irrlicht/libpng/pngrio.c \
				Irrlicht/libpng/pngrtran.c \
				Irrlicht/libpng/pngrutil.c \
				Irrlicht/libpng/pngset.c \
				Irrlicht/libpng/pngtrans.c \
				Irrlicht/libpng/pngwio.c \
				Irrlicht/libpng/pngwrite.c \
				Irrlicht/libpng/pngwtran.c \
				Irrlicht/libpng/pngwutil.c

LIBAESGM := \
				Irrlicht/aesGladman/aescrypt.cpp \
				Irrlicht/aesGladman/aeskey.cpp \
				Irrlicht/aesGladman/aestab.cpp \
				Irrlicht/aesGladman/fileenc.cpp \
				Irrlicht/aesGladman/hmac.cpp \
				Irrlicht/aesGladman/prng.cpp \
				Irrlicht/aesGladman/pwd2key.cpp \
				Irrlicht/aesGladman/sha1.cpp \
				Irrlicht/aesGladman/sha2.cpp

BZIP2OBJ := \
	`			Irrlicht/bzip2/blocksort.c \
				Irrlicht/bzip2/huffman.c \
				Irrlicht/bzip2/crctable.c \
				Irrlicht/bzip2/randtable.c \
				Irrlicht/bzip2/bzcompress.c \
				Irrlicht/bzip2/decompress.c \
				Irrlicht/bzip2/bzlib.c \
				Irrlicht/lzma/LzmaDec.c

NACL := \
				Irrlicht/CIrrDeviceNaCl.cpp

C_SOURCES := \
				$(ZLIBOBJ) \
				$(JPEGLIBOBJ) \
				$(LIBPNGOBJ) \
				$(BZIP2OBJ)

CXX_SOURCES := \
				$(LIBAESGM) \
				$(IRRVIDEOOBJ) \
				$(IRRDRVOBJ2) \
				$(IRRMESHOBJ) \
				$(IRROBJ) \
				$(IRRPARTICLEOBJ) \
				$(IRRANIMOBJ) \
				$(IRRSWRENDEROBJ) \
				$(IRRIOOBJ) \
				$(IRROTHEROBJ) \
				$(IRRGUIOBJ)

#
# Get pepper directory for toolchain and includes.
#
# If PEPPER_ROOT is not set, then assume it can be found a two directories up,
# from the default example directory location.
#
THIS_MAKEFILE:=$(abspath $(lastword $(MAKEFILE_LIST)))
NACL_SDK_ROOT?=$(abspath $(dir $(THIS_MAKEFILE))../..)

# Project Build flags
WARNINGS:=-Wno-long-long
CXXFLAGS:=$(INCLUDES_DIRS:%=-I%) -pthread -std=gnu++98 $(WARNINGS)

#
# Compute tool paths
#
#
##### I had to replace this OSNAME:=$(shell python $(NACL_SDK_ROOT)/tools/getos.py) with "win" otherwise, #####
OSNAME:=win
TC_PATH:=$(abspath $(NACL_SDK_ROOT)/toolchain/$(OSNAME)_x86_glibc)
CXX:=$(TC_PATH)/bin/i686-nacl-g++
CC:=$(TC_PATH)/bin/i686-nacl-gcc

#
# Disable DOS PATH warning when using Cygwin based tools Windows
#
CYGWIN ?= nodosfilewarning
export CYGWIN


# Declare the ALL target first, to make the 'all' target the default build
all: $(PROJECT)_x86_32.nexe $(PROJECT)_x86_64.nexe

# Define 32 bit compile and link rules for C++ sources
x86_32_OBJS:=$(addsuffix _32.o,$(basename $(CXX_SOURCES)))

$(patsubst %.c,%_32.o,$(C_SOURCES)) : %_32.o : %.c $(THIS_MAKE)
	$(C) -o $@ -c $< -m32 -O0 -g $(CXXFLAGS)
$(patsubst %.cpp,%_32.o,$(CXX_SOURCES)) : %_32.o : %.cpp $(THIS_MAKE)
	$(CXX) -o $@ -c $< -m32 -O0 -g $(CXXFLAGS)

$(PROJECT)_x86_32.nexe : $(x86_32_OBJS)
	$(CXX) -o $@ $^ -m32 -O0 -g $(CXXFLAGS) $(LDFLAGS)

# Define 64 bit compile and link rules for C++ sources
x86_64_OBJS:=$(addsuffix _64.o,$(basename $(CXX_SOURCES)))

$(patsubst %.c,%_64.o,$(C_SOURCES)) : %_64.o : %.c $(THIS_MAKE)
	$(C) -o $@ -c $< -m64 -O0 -g $(CXXFLAGS)
$(patsubst %.cpp,%_64.o,$(CXX_SOURCES)) : %_64.o : %.cpp $(THIS_MAKE)
	$(CXX) -o $@ -c $< -m64 -O0 -g $(CXXFLAGS)

$(PROJECT)_x86_64.nexe : $(x86_64_OBJS)
	$(CXX) -o $@ $^ -m64 -O0 -g $(CXXFLAGS) $(LDFLAGS)
	

# Define a phony rule so it always runs, to build nexe and start up server.
.PHONY: RUN 
RUN: all
	python ../httpd.py



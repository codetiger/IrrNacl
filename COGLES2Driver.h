// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h
#ifndef __C_OGLES2_DRIVER_H_INCLUDED__
#define __C_OGLES2_DRIVER_H_INCLUDED__

#include "IrrCompileConfig.h"

#if defined(_IRR_WINDOWS_API_)
// include windows headers for HWND
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#elif defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
#include "MacOSX/CIrrDeviceMacOSX.h"
#elif defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
#include "CIrrDeviceIPhone.h"
#endif

#include "SIrrCreationParameters.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#include <EGL/eglplatform.h>

#include "CNullDriver.h"
#include "IMaterialRendererServices.h"
#include "EDriverFeatures.h"
#include "fast_atof.h"

#ifdef _MSC_VER
#pragma comment(lib, "libEGL.lib")
#pragma comment(lib, "libGLESv2.lib")
#endif
#include "COGLES2ExtensionHandler.h"

namespace irr
{
namespace video
{
	class COGLES2Texture;
	class COGLES2FixedPipelineShader;
	class COGLES2Renderer2d;

	class COGLES2Driver : public CNullDriver, public IMaterialRendererServices, public COGLES2ExtensionHandler
	{
	public:
#if defined(_IRR_COMPILE_WITH_X11_DEVICE_) || defined(_IRR_COMPILE_WITH_SDL_DEVICE_) || defined(_IRR_WINDOWS_API_) || defined(_IRR_COMPILE_WITH_CONSOLE_DEVICE_)
		COGLES2Driver(const SIrrlichtCreationParameters& params,
					const SExposedVideoData& data,
					io::IFileSystem* io);
#endif

#ifdef _IRR_COMPILE_WITH_OSX_DEVICE_
		COGLES2Driver(const SIrrlichtCreationParameters& params,
					io::IFileSystem* io, CIrrDeviceMacOSX *device);
#endif

#if defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
		COGLES2Driver(const SIrrlichtCreationParameters& params,
					const SExposedVideoData& data,
					io::IFileSystem* io, MIrrIPhoneDevice const & device);
#endif

		//! destructor
		virtual ~COGLES2Driver();

		//! clears the zbuffer
		virtual bool beginScene(bool backBuffer=true, bool zBuffer=true,
					SColor color=SColor(255, 0, 0, 0),
					const SExposedVideoData& videoData=SExposedVideoData(),
					core::rect<s32>* sourceRect=0);

		//! presents the rendered scene on the screen, returns false if failed
		virtual bool endScene();

		//! sets transformation
		virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat);

		struct SHWBufferLink_opengl : public SHWBufferLink
		{
			SHWBufferLink_opengl(const scene::IMeshBuffer *meshBuffer): SHWBufferLink(meshBuffer), vbo_verticesID(0), vbo_indicesID(0) {}

			u32 vbo_verticesID; //tmp
			u32 vbo_indicesID; //tmp

			u32 vbo_verticesSize; //tmp
			u32 vbo_indicesSize; //tmp
		};

		bool updateVertexHardwareBuffer(SHWBufferLink_opengl *HWBuffer);
		bool updateIndexHardwareBuffer(SHWBufferLink_opengl *HWBuffer);

		//! updates hardware buffer if needed
		virtual bool updateHardwareBuffer(SHWBufferLink *HWBuffer);

		//! Create hardware buffer from mesh
		virtual SHWBufferLink *createHardwareBuffer(const scene::IMeshBuffer* mb);

		//! Delete hardware buffer (only some drivers can)
		virtual void deleteHardwareBuffer(SHWBufferLink *HWBuffer);

		//! Draw hardware buffer
		virtual void drawHardwareBuffer(SHWBufferLink *HWBuffer);

		//! draws a vertex primitive list
		virtual void drawVertexPrimitiveList(const void* vertices, u32 vertexCount,
				const void* indexList, u32 primitiveCount,
				E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType);

		void drawVertexPrimitiveList2d3d(const void* vertices, u32 vertexCount,
				const void* indexList, u32 primitiveCount,
				E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType,
				E_INDEX_TYPE iType = EIT_16BIT, bool threed = true);

		//! queries the features of the driver, returns true if feature is available
		virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
		{
			return FeatureEnabled[feature] && COGLES2ExtensionHandler::queryFeature(feature);
		}

		//! Sets a material.
		virtual void setMaterial(const SMaterial& material);

		//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
		virtual void draw2DImage(const video::ITexture* texture,
				const core::position2d<s32>& destPos,
				const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
				SColor color = SColor(255, 255, 255, 255), bool useAlphaChannelOfTexture = false);

		//! draws a set of 2d images
		virtual void draw2DImageBatch(const video::ITexture* texture,
				const core::position2d<s32>& pos,
				const core::array<core::rect<s32> >& sourceRects,
				const core::array<s32>& indices, s32 kerningWidth = 0,
				const core::rect<s32>* clipRect = 0,
				SColor color = SColor(255, 255, 255, 255),
				bool useAlphaChannelOfTexture = false);

		//! Draws a part of the texture into the rectangle.
		virtual void draw2DImage(const video::ITexture* texture, const core::rect<s32>& destRect,
				const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
				const video::SColor* const colors = 0, bool useAlphaChannelOfTexture = false);

		void draw2DImageBatch(const video::ITexture* texture,
				const core::array<core::position2d<s32> >& positions,
				const core::array<core::rect<s32> >& sourceRects,
				const core::rect<s32>* clipRect,
				SColor color,
				bool useAlphaChannelOfTexture);

		//! draw an 2d rectangle
		virtual void draw2DRectangle(SColor color, const core::rect<s32>& pos,
				const core::rect<s32>* clip = 0);

		//!Draws an 2d rectangle with a gradient.
		virtual void draw2DRectangle(const core::rect<s32>& pos,
				SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
				const core::rect<s32>* clip = 0);

		//! Draws a 2d line.
		virtual void draw2DLine(const core::position2d<s32>& start,
				const core::position2d<s32>& end,
				SColor color = SColor(255, 255, 255, 255));

		//! Draws a single pixel
		virtual void drawPixel(u32 x, u32 y, const SColor & color);

		//! Draws a 3d line.
		virtual void draw3DLine(const core::vector3df& start,
				const core::vector3df& end,
				SColor color = SColor(255, 255, 255, 255));

		//! Draws a pixel
//			virtual void drawPixel(u32 x, u32 y, const SColor & color);

		//! Returns the name of the video driver.
		virtual const wchar_t* getName() const;

		//! deletes all dynamic lights there are
		virtual void deleteAllDynamicLights();

		//! adds a dynamic light
		virtual s32 addDynamicLight(const SLight& light);

		//! Turns a dynamic light on or off
		/** \param lightIndex: the index returned by addDynamicLight
		\param turnOn: true to turn the light on, false to turn it off */
		virtual void turnLightOn(s32 lightIndex, bool turnOn);

		//! returns the maximal amount of dynamic lights the device can handle
		virtual u32 getMaximalDynamicLightAmount() const;

		//! Sets the dynamic ambient light color.
		virtual void setAmbientLight(const SColorf& color);

		//! return the dynamic ambient light color.
		const SColorf& getAmbientLight() const;

		//! Returns the maximum texture size supported.
		virtual core::dimension2du getMaxTextureSize() const;

		//! Draws a shadow volume into the stencil buffer.
		virtual void drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail);

		//! Fills the stencil shadow with color.
		virtual void drawStencilShadow(bool clearStencilBuffer = false,
				video::SColor leftUpEdge = video::SColor(0, 0, 0, 0),
				video::SColor rightUpEdge = video::SColor(0, 0, 0, 0),
				video::SColor leftDownEdge = video::SColor(0, 0, 0, 0),
				video::SColor rightDownEdge = video::SColor(0, 0, 0, 0));

		//! sets a viewport
		virtual void setViewPort(const core::rect<s32>& area);

		//! Only used internally by the engine
		virtual void OnResize(const core::dimension2d<u32>& size);

		//! Returns type of video driver
		virtual E_DRIVER_TYPE getDriverType() const;

		//! get color format of the current color buffer
		virtual ECOLOR_FORMAT getColorFormat() const;

		//! Returns the transformation set by setTransform
		virtual const core::matrix4& getTransform(E_TRANSFORMATION_STATE state) const;

		//! Can be called by an IMaterialRenderer to make its work easier.
		virtual void setBasicRenderStates(const SMaterial& material,
				const SMaterial& lastmaterial,
				bool resetAllRenderstates);

		//! Sets a vertex shader constant.
		virtual void setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount = 1);

		//! Sets a pixel shader constant.
		virtual void setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount = 1);

		//! Sets a constant for the vertex shader based on a name.
		virtual bool setVertexShaderConstant(const c8* name, const f32* floats, int count);

		//! Sets a constant for the pixel shader based on a name.
		virtual bool setPixelShaderConstant(const c8* name, const f32* floats, int count);

		//! Sets a vertex pointer the vertex shader based on a name.
		virtual bool setVertexShaderPointer(const c8* name, const void* pointer, s32 size = 3, bool normalized = false, u16 stride = 0);

		//! sets the current Texture
		bool setActiveTexture(u32 stage, const video::ITexture* texture);

		//! disables all textures beginning with fromStage.
		bool disableTextures(u32 fromStage = 0);

		//! Adds a new material renderer to the VideoDriver
		virtual s32 addShaderMaterial(const c8* vertexShaderProgram, const c8* pixelShaderProgram,
				IShaderConstantSetCallBack* callback, E_MATERIAL_TYPE baseMaterial, s32 userData);

		//! Adds a new material renderer to the VideoDriver
		virtual s32 addHighLevelShaderMaterial(
				const c8* vertexShaderProgram,
				const c8* vertexShaderEntryPointName = 0,
				E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_1_1,
				const c8* pixelShaderProgram = 0,
				const c8* pixelShaderEntryPointName = 0,
				E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_1_1,
				const c8* geometryShaderProgram = 0,
				const c8* geometryShaderEntryPointName = "main",
				E_GEOMETRY_SHADER_TYPE gsCompileTarget = EGST_GS_4_0,
				scene::E_PRIMITIVE_TYPE inType = scene::EPT_TRIANGLES,
				scene::E_PRIMITIVE_TYPE outType = scene::EPT_TRIANGLE_STRIP,
				u32 verticesOut = 0,
				IShaderConstantSetCallBack* callback = 0,
				E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
				s32 userData=0);

		//! Returns pointer to the IGPUProgrammingServices interface.
		virtual IGPUProgrammingServices* getGPUProgrammingServices();

		//! Returns a pointer to the IVideoDriver interface.
		virtual IVideoDriver* getVideoDriver();

		//! Returns the maximum amount of primitives
		virtual u32 getMaximalPrimitiveCount() const;

		virtual ITexture* addRenderTargetTexture(const core::dimension2d<u32>& size,
				const io::path& name, const ECOLOR_FORMAT format = ECF_UNKNOWN);

		virtual bool setRenderTarget(video::ITexture* texture, bool clearBackBuffer,
				bool clearZBuffer, SColor color);

		//! set or reset special render targets
//			virtual bool setRenderTarget(video::E_RENDER_TARGET target, bool clearTarget,
//					bool clearZBuffer, SColor color);

		//! Sets multiple render targets
//			virtual bool setRenderTarget(const core::array<video::IRenderTarget>& texture,
//					bool clearBackBuffer=true, bool clearZBuffer=true, SColor color=SColor(0,0,0,0));

		//! Clears the ZBuffer.
		virtual void clearZBuffer();

		//! Returns an image created from the last rendered frame.
		virtual IImage* createScreenShot(video::ECOLOR_FORMAT format=video::ECF_UNKNOWN, video::E_RENDER_TARGET target=video::ERT_FRAME_BUFFER);

		//! checks if an OpenGL error has happend and prints it
		bool testGLError();

		//! checks if an OGLES1 error has happend and prints it
		bool testEGLError();

		//! Set/unset a clipping plane.
		virtual bool setClipPlane(u32 index, const core::plane3df& plane, bool enable = false);

		//! returns the current amount of user clip planes set.
		u32 getClipPlaneCount() const;

		//! returns the 0 indexed Plane
		const core::plane3df& getClipPlane(u32 index) const;

		//! Enable/disable a clipping plane.
		virtual void enableClipPlane(u32 index, bool enable);

		//! Returns the graphics card vendor name.
		virtual core::stringc getVendorInfo()
		{
			return vendorName;
		};

		ITexture* createDepthTexture(ITexture* texture, bool shared = true);
		void removeDepthTexture(ITexture* texture);

		void renderHighLevelShader(bool state = true)
		{
			NoHighLevelShader = !state;
		}

		COGLES2FixedPipelineShader* fixedPipeline()
		{
			return FixedPipeline;
		}

		virtual void reloadShaders();

		//native ogles2 which was ogles1 extensions.

		void deleteFramebuffers(s32 n, const u32 *framebuffers);
		void deleteRenderbuffers(s32 n, const u32 *renderbuffers);

		void enableBlend();
		void disableBlend();
		u32 getGLBlend(E_BLEND_FACTOR factor);
		void blendFunc(E_BLEND_FACTOR sFactor, E_BLEND_FACTOR dFactor);

	private:

		void uploadClipPlane(u32 index);

		//! inits the opengl-es driver
		bool genericDriverInit(const core::dimension2d<u32>& screenSize, bool stencilBuffer);

		//! returns a device dependent texture from a software surface (IImage)
		virtual video::ITexture* createDeviceDependentTexture(IImage* surface, const io::path& name, void* mipmapData);

		//! creates a transposed matrix in supplied GLfloat array to pass to OGLES1
		inline void createGLMatrix(float gl_matrix[16], const core::matrix4& m);

		inline void createGLTextureMatrix(float gl_matrix[16], const core::matrix4& m);

		//! Map Irrlicht wrap mode to OpenGL enum
		GLint getTextureWrapMode(u8 clamp) const;

		//! Set GL pipeline to desired texture wrap modes of the material
		void setWrapMode(const SMaterial& material);

		//! sets the needed renderstates
		void setRenderStates3DMode();

		//! sets the needed renderstates
		void setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel);

		// returns the current size of the screen or rendertarget
		virtual const core::dimension2d<u32>& getCurrentRenderTargetSize() const;

		void createMaterialRenderers();

		core::stringw Name;
		core::matrix4 Matrices[ETS_COUNT];

		//! enumeration for rendering modes such as 2d and 3d for minizing the switching of renderStates.
		enum E_RENDER_MODE
		{
			ERM_NONE = 0, // no render state has been set yet.
			ERM_2D, // 2d drawing rendermode
			ERM_3D // 3d rendering mode
		};

		E_RENDER_MODE CurrentRenderMode;
		//! bool to make all renderstates reset if set to true.
		bool ResetRenderStates;
		bool Transformation3DChanged;
		u8 AntiAlias;

		SMaterial Material, LastMaterial;
		COGLES2Texture* RenderTargetTexture;
		const ITexture* CurrentTexture[MATERIAL_MAX_TEXTURES];
		core::array<ITexture*> DepthTextures;

		struct SUserClipPlane
		{
			core::plane3df Plane;
			bool Enabled;
		};

		core::array<SUserClipPlane> UserClipPlane;

		core::dimension2d<u32> CurrentRendertargetSize;

		core::stringc vendorName;

		core::matrix4 TextureFlipMatrix;

		//! Color buffer format
		ECOLOR_FORMAT ColorFormat;

		//! All the lights that have been requested; a hardware limited
		//! number of them will be used at once.
		struct RequestedLight
		{
			RequestedLight(SLight const & lightData)
					: LightData(lightData), DesireToBeOn(true) { }

			SLight LightData;
			bool DesireToBeOn;
		};

		core::array<RequestedLight> RequestedLights;
		SColorf AmbientLight;

#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
		HDC HDc;
#endif
		NativeWindowType EglWindow;
		void* EglDisplay;
		void* EglSurface;
		void* EglContext;

		COGLES2FixedPipelineShader* FixedPipeline;
		COGLES2Renderer2d* TwoDRenderer;
		bool NoHighLevelShader;

		bool BlendEnabled;
		E_BLEND_FACTOR SourceFactor;
		E_BLEND_FACTOR DestFactor;
	};

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_

#endif


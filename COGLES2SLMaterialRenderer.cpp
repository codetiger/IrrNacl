// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "COGLES2SLMaterialRenderer.h"
#include "IGPUProgrammingServices.h"
#include "IShaderConstantSetCallBack.h"
#include "IMaterialRendererServices.h"
#include "IVideoDriver.h"
#include "os.h"
#include "COGLES2Driver.h"
#include "EVertexAttributes.h"
#include "COGLES2Texture.h"

#include <GLES2/gl2.h>

namespace irr
{
namespace video
{

	//! Constructor
	COGLES2SLMaterialRenderer::COGLES2SLMaterialRenderer( video::COGLES2Driver* driver,
			s32& outMaterialTypeNr, const c8* vertexShaderProgram,
			const c8* pixelShaderProgram,
			IShaderConstantSetCallBack* callback,
			video::IMaterialRenderer* baseMaterial, s32 userData )
			: Driver( driver ), CallBack( callback ), BaseMaterial( baseMaterial ), FileSystem( 0 ),
			Program( 0 ), UserData( userData ), UniformStringTable( 0 ), UniformCount( 0 )
	{
#ifdef _DEBUG
		setDebugName( "COGLES2SLMaterialRenderer" );
#endif

		//entry points must always be main, and the compile target isn't selectable
		//it is fine to ignore what has been asked for, as the compiler should spot anything wrong
		//just check that GLSL is available

		if ( BaseMaterial )
			BaseMaterial->grab();

		if ( CallBack )
			CallBack->grab();

		init( outMaterialTypeNr, vertexShaderProgram, pixelShaderProgram );
	}


	//! constructor only for use by derived classes who want to
	//! create a fall back material for example.
	COGLES2SLMaterialRenderer::COGLES2SLMaterialRenderer( COGLES2Driver* driver,
			io::IFileSystem* fs, IShaderConstantSetCallBack* callback,
			IMaterialRenderer* baseMaterial, const char* const * uniformStringTable,
			const u32& uniformCount, s32 userData )
			: Driver( driver ), CallBack( callback ), BaseMaterial( baseMaterial ), FileSystem( fs ),
			Program( 0 ), UserData( userData ), UniformStringTable( uniformStringTable ),
			UniformCount( uniformCount )
	{
		if ( BaseMaterial )
			BaseMaterial->grab();

		if ( CallBack )
			CallBack->grab();
	}


	//! Destructor
	COGLES2SLMaterialRenderer::~COGLES2SLMaterialRenderer()
	{
		if ( CallBack )
			CallBack->drop();

		if ( Program )
		{
			glDeleteProgram( Program );
			Program = 0;
		}

		UniformInfo.clear();

		AttributeInfo.clear();

		if ( BaseMaterial )
			BaseMaterial->drop();
	}

	bool COGLES2SLMaterialRenderer::init(s32& outMaterialTypeNr,
			const c8* vertexShaderProgram,
			const c8* pixelShaderProgram,
			bool registerMaterial )
	{
		outMaterialTypeNr = -1;

		if ( Program == 0 && !createProgram() )
			return false;

		if ( vertexShaderProgram )
			if ( !createShader( GL_VERTEX_SHADER, vertexShaderProgram, "" ) )
				return false;

		if ( pixelShaderProgram )
			if ( !createShader( GL_FRAGMENT_SHADER, pixelShaderProgram, "" ) )
				return false;

		if ( !linkProgram() )
			return false;

		// register myself as new material
		if ( registerMaterial )
			outMaterialTypeNr = Driver->addMaterialRenderer( this );
		return true;
	}

	bool COGLES2SLMaterialRenderer::initFromFiles( s32 &outMaterialTypeNr,
			const c8 *vertexShaderFile,
			const c8 *pixelShaderFile,
			bool registerMaterial )
	{
		outMaterialTypeNr = -1;
		if ( !createProgram() )
		{
			os::Printer::log( "Could not create shader program.", ELL_ERROR );
			return false;
		}

		if ( !readVertexShader( vertexShaderFile ) )
		{
			os::Printer::log( "Error reading fixed pipeline vertex shader.", ELL_ERROR );
		}

		if ( !readFragmentShader( pixelShaderFile ) )
		{
			os::Printer::log( "Error reading fixed pipeline fragment shader.", ELL_ERROR );
		}

		for ( size_t i = 0; i < EVA_COUNT; ++i )
			glBindAttribLocation( Program, i, sBuiltInVertexAttributeNames[i] );

		if ( !linkProgram() )
		{
			os::Printer::log( "Error linking fixed pipeline shader program.", ELL_ERROR );
			return false;
		}

		if ( registerMaterial )
			outMaterialTypeNr = Driver->addMaterialRenderer( this );
		return true;
	}

	bool COGLES2SLMaterialRenderer::reloadFromFiles( const c8 *vertexShaderFile,
			const c8 *pixelShaderFile )
	{
		GLsizei shaderCount;
		GLuint shaderHandles[2];
		glGetAttachedShaders( Program, 2, &shaderCount, shaderHandles );
		glDetachShader( Program, shaderHandles[0] );
		glDeleteShader( shaderHandles[0] );
		glDetachShader( Program, shaderHandles[1] );
		glDeleteShader( shaderHandles[1] );
		if ( !readVertexShader( vertexShaderFile ) )
		{
			os::Printer::log( "Error reading fixed pipeline vertex shader.", ELL_ERROR );
		}

		if ( !readFragmentShader( pixelShaderFile ) )
		{
			os::Printer::log( "Error reading fixed pipeline fragment shader.", ELL_ERROR );
		}

		if ( !linkProgram() )
		{
			os::Printer::log( "Error linking fixed pipeline shader program.", ELL_ERROR );
			return false;
		}
		else
			return true;
	}


	bool COGLES2SLMaterialRenderer::readShader( GLenum shaderType, const c8* shaderFile )
	{
#ifndef _IRR_NACL_PLATFORM_
		io::IReadFile* file = FileSystem->createAndOpenFile(shaderFile);
		if (!file)
		{
			os::Printer::log("Could not open shader file", shaderFile, ELL_ERROR);
			return false;
		}
		const long size = file->getSize();
		if (!size)
		{
			os::Printer::log( "Shader file is empty", shaderFile, ELL_ERROR );
			file->drop();
			return false;
		}

		c8* shader = new c8[size+1];
		file->read(shader, size);
		shader[size] = 0;
		file->drop();
#else
		c8* shader = strdup(getShader(shaderFile));
		const long size = strlen(shader);
		if(shader == NULL || size)
			os::Printer::log( "Shader file is empty", shaderFile, ELL_ERROR );
#endif

		bool success = createShader(shaderType, shader, shaderFile);
		//free(shader);
		return success;
	}

	bool COGLES2SLMaterialRenderer::readVertexShader( const c8 *vertexShaderFile )
	{
		return readShader( GL_VERTEX_SHADER, vertexShaderFile );
	}

	bool COGLES2SLMaterialRenderer::readFragmentShader( const c8 *fragmentShaderFile )
	{
		return readShader( GL_FRAGMENT_SHADER, fragmentShaderFile );
	}

	bool COGLES2SLMaterialRenderer::OnRender( IMaterialRendererServices* service,
			E_VERTEX_TYPE vtxtype )
	{
		// call callback to set shader constants and attributes
		if ( CallBack && Program )
		{
			CallBack->OnSetAttribute( this, UserData );
			CallBack->OnSetConstants( this, UserData );
		}

		return true;
	}


	bool COGLES2SLMaterialRenderer::PostRender( IMaterialRendererServices* service,
			E_VERTEX_TYPE vtxtype )
	{
		// call callback to unset shader attributes
		if ( CallBack && Program )
			CallBack->OnUnSetAttribute( this, UserData );

		return true;
	}

	void COGLES2SLMaterialRenderer::OnSetMaterial( const video::SMaterial& material,
			const video::SMaterial& lastMaterial,
			bool resetAllRenderstates,
			video::IMaterialRendererServices* services )
	{
		if ( material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates )
		{
			if ( Program )
				glUseProgram( Program );

			//let callback know used material
			if ( CallBack )
				CallBack->OnSetMaterial( material );
		}
		//if (BaseMaterial)
		//	BaseMaterial->OnSetMaterial(material, material, true, this);

		//for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		//	Driver->setActiveTexture(i, material.getTexture(i));
		Driver->setBasicRenderStates( material, lastMaterial, resetAllRenderstates );
	}


	void COGLES2SLMaterialRenderer::OnUnsetMaterial()
	{
		glUseProgram( 0 );

		//if (BaseMaterial)
		//	BaseMaterial->OnUnsetMaterial();
	}

	//! Returns if the material is transparent.
	bool COGLES2SLMaterialRenderer::isTransparent() const
	{
		return BaseMaterial ? BaseMaterial->isTransparent() : false;
	}

	bool COGLES2SLMaterialRenderer::createProgram()
	{
		Program = glCreateProgram();
		return true;
	}

	bool COGLES2SLMaterialRenderer::createShader( GLenum shaderType, const char* shader, const char* shaderFile )
	{
		GLuint shaderHandle = glCreateShader( shaderType );

		glShaderSource( shaderHandle, 1, &shader, NULL );
		glCompileShader( shaderHandle );

		int status = 0;

		glGetShaderiv( shaderHandle, GL_COMPILE_STATUS, &status );

		if ( !status )
		{
			os::Printer::log("GLSL shader failed to compile", shaderFile, ELL_ERROR);

			// check error message and log it
			int maxLength = 0;
			GLsizei length;

			glGetShaderiv( shaderHandle, GL_INFO_LOG_LENGTH, &maxLength );

			char *pInfoLog = new char[maxLength];
			glGetShaderInfoLog( shaderHandle, maxLength, &length, pInfoLog );
			os::Printer::log(pInfoLog, ELL_ERROR);
			delete [] pInfoLog;

			return false;
		}

		glAttachShader(Program, shaderHandle);

		return true;
	}

	bool COGLES2SLMaterialRenderer::linkProgram()
	{
		glLinkProgram( Program );

		int status = 0;

		glGetProgramiv( Program, GL_LINK_STATUS, &status );

		if ( !status )
		{
			os::Printer::log( "GLSL shader program failed to link", ELL_ERROR );
			// check error message and log it
			int maxLength = 0;
			GLsizei length;

			glGetProgramiv( Program, GL_INFO_LOG_LENGTH, &maxLength );

			char *pInfoLog = new char[maxLength];
			glGetProgramInfoLog( Program, maxLength, &length, pInfoLog );
			os::Printer::log(pInfoLog, ELL_ERROR);
			delete [] pInfoLog;

			return false;
		}

		// get uniforms information

		int num = 0;
		glGetProgramiv( Program, GL_ACTIVE_UNIFORMS, &num );

		int maxlen = 0;
		glGetProgramiv( Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlen );

		if ( maxlen == 0 && num != 0 )
		{
			os::Printer::log( "GLSL: failed to retrieve uniform information", ELL_ERROR );
			return false;
		}

		c8 *buf = new c8[maxlen];

		UniformInfo.clear();
		UniformInfo.reallocate( num );

		core::array<core::stringc> names( num );
		core::array<SUniformInfo> uni( num );

		for ( int i = 0; i < num; ++i )
		{
			memset( buf, 0, maxlen );
			GLint size;
			SUniformInfo ui;
			glGetActiveUniform( Program, i, maxlen, 0, &size, &ui.type, reinterpret_cast<char*>( buf ) );
			ui.location = glGetUniformLocation( Program, buf );
			uni.push_back( ui );
			names.push_back( buf );
		}

		delete [] buf;

		for ( int i = 0; i < UniformCount; ++i )
		{
			int j;
			for ( j = 0; j < num; ++j )
			{
				if ( names[j] == UniformStringTable[i] )
					break;
			}
			if ( j < num )
			{
				UniformInfo.push_back( uni[j] );
			}
			else
			{
				wchar_t buf[512];
				swprintf( buf, 512, L"Unable to find uniform : %S", UniformStringTable[i] );
				os::Printer::log( buf, ELL_WARNING );
				SUniformInfo blank;
				blank.location = -1;
				blank.type = GL_INVALID_ENUM;
				UniformInfo.push_back( blank );
			}
		}

		return true;
	}


	void COGLES2SLMaterialRenderer::setBasicRenderStates( const SMaterial& material,
			const SMaterial& lastMaterial,
			bool resetAllRenderstates )
	{
		// forward
		Driver->setBasicRenderStates( material, lastMaterial, resetAllRenderstates );
	}


	bool COGLES2SLMaterialRenderer::setVertexShaderConstant( const c8* name, const f32* floats, int count )
	{
		return setPixelShaderConstant( name, floats, count );
	}


	void COGLES2SLMaterialRenderer::setVertexShaderConstant( const f32* data, s32 startRegister, s32 constantAmount )
	{
		os::Printer::log( "Cannot set constant, please use high level shader call instead.", ELL_WARNING );
	}

	bool COGLES2SLMaterialRenderer::setVertexShaderPointer( const c8* name, const void* pointer,
			s32 size, bool normalized, u16 stride )
	{
		os::Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );

		return false;
	}

	void COGLES2SLMaterialRenderer::enableVertexShaderPointer( const c8* name )
	{
		os::Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );
	}

	void COGLES2SLMaterialRenderer::disableVertexShaderPointer( const c8* name )
	{
		os::Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );
	}

	bool COGLES2SLMaterialRenderer::setPixelShaderConstant( const c8* name, const f32* floats, int count )
	{
		os::Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );
		return false;
	}

	bool COGLES2SLMaterialRenderer::setUniform( int index, const void* data, int count )
	{
		if ((u32)index>=UniformInfo.size())
			return false;
		SUniformInfo& ui = UniformInfo[index];
		if ( ui.location == -1 )
			return false;
		switch ( ui.type )
		{
			case GL_FLOAT:
				glUniform1fv( ui.location, count, static_cast<const GLfloat*>( data ) );
				break;
			case GL_FLOAT_VEC2:
				glUniform2fv( ui.location, count, static_cast<const GLfloat*>( data ) );
				break;
			case GL_FLOAT_VEC3:
				glUniform3fv( ui.location, count, static_cast<const GLfloat*>( data ) );
				break;
			case GL_FLOAT_VEC4:
				glUniform4fv( ui.location, count, static_cast<const GLfloat*>( data ) );
				break;
			case GL_INT:
			case GL_BOOL:
				glUniform1iv( ui.location, count, static_cast<const GLint*>( data ) );
				break;
			case GL_INT_VEC2:
			case GL_BOOL_VEC2:
				glUniform2iv( ui.location, count, static_cast<const GLint*>( data ) );
				break;
			case GL_INT_VEC3:
			case GL_BOOL_VEC3:
				glUniform3iv( ui.location, count, static_cast<const GLint*>( data ) );
				break;
			case GL_INT_VEC4:
			case GL_BOOL_VEC4:
				glUniform4iv( ui.location, count, static_cast<const GLint*>( data ) );
				break;
			case GL_FLOAT_MAT2:
				glUniformMatrix2fv( ui.location, count, false, static_cast<const GLfloat*>( data ) );
				break;
			case GL_FLOAT_MAT3:
				glUniformMatrix3fv( ui.location, count, false, static_cast<const GLfloat*>( data ) );
				break;
			case GL_FLOAT_MAT4:
				glUniformMatrix4fv( ui.location, count, false, static_cast<const GLfloat*>( data ) );
				break;
			default: // sampler
				glUniform1iv( ui.location, count, static_cast<const GLint*>( data ) );
				break;
		}
		return !Driver->testGLError();
	}

	bool COGLES2SLMaterialRenderer::setTextureUnit( const c8* name, int unit )
	{
		os::Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );
		return false;
	}

	bool COGLES2SLMaterialRenderer::enableMaterialTexture( const c8* name, int materialId )
	{
		ITexture * t = Driver->getTextureByIndex( materialId );
		COGLES2Texture * tex = reinterpret_cast<COGLES2Texture*>( t );
		if ( !tex )
			return false;
		const GLuint unit = tex->getOGLES2TextureName();

		glActiveTexture( GL_TEXTURE0 + unit );
		glBindTexture( GL_TEXTURE_2D, unit );

		return setTextureUnit( name, unit );
	}

	bool COGLES2SLMaterialRenderer::disableMaterialTexture( int materialId )
	{
		COGLES2Texture * tex = reinterpret_cast<COGLES2Texture*>( Driver->getTextureByIndex( materialId ) );
		if ( !tex )
			return false;

		const GLuint unit = tex->getOGLES2TextureName();

		glActiveTexture( GL_TEXTURE0 + unit );
		glBindTexture( GL_TEXTURE_2D, 0 );

		return true;
	}

	void COGLES2SLMaterialRenderer::setPixelShaderConstant( const f32* data, s32 startRegister, s32 constantAmount )
	{
		os::Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );
	}

	IVideoDriver* COGLES2SLMaterialRenderer::getVideoDriver()
	{
		return Driver;
	}

	void COGLES2SLMaterialRenderer::useProgram()
	{
		glUseProgram( Program );
	}

	char* COGLES2SLMaterialRenderer::getShader(const char* fileName) {
		if(!strcmp(fileName, "COGLES2FixedPipeline.fsh")) {
			return "// Copyright (C) 2009-2010 Amundis\n"
					"// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt\n"
					"// and OpenGL ES driver implemented by Christian Stehno\n"
					"// This file is part of the \"Irrlicht Engine\".\n"
					"// For conditions of distribution and use, see copyright notice in Irrlicht.h\n"
					"#define MAX_TEXTURE_UNITS 4\n"
					"#define FOG_EXP 1\n"
					"#define FOG_LINEAR 2\n"
					"\n"
					"#define TwoD             24\n"
					"#define Solid            0\n"
					"#define Solid2Layer      1\n"
					"#define LightMap         2\n"
					"#define DetailMap		 9\n"
					"#define SphereMap		 10\n"
					"#define Reflection2Layer 11\n"
					"#define TransparentAlphaChannel 13\n"
					"#define TransparentVertexAlpha 15\n"
					"\n"
					"precision mediump float;\n"
					"\n"
					"vec4 red   = vec4(1.0, 0.0, 0.0, 1.0); "
					"vec4 green = vec4(0.0, 1.0, 0.0, 1.0); "
					"vec4 blue  = vec4(0.0, 0.0, 1.0, 1.0); "
					" "
					" "
					"/* Varyings */\n"
					"\n"
					"varying vec4 varTexCoord[MAX_TEXTURE_UNITS];\n"
					"varying vec4 varVertexColor;\n"
					"varying float eyeDist;\n"
					"varying float varClipDist;\n"
					"\n"
					"/* Uniforms */\n"
					"\n"
					"uniform lowp int uRenderMode;\n"
					"\n"
					"uniform bool  uAlphaTest;\n"
					"uniform float uAlphaValue;\n"
					"\n"
					"/* Fog Uniforms */\n"
					"\n"
					"uniform bool  uFog;\n"
					"uniform int   uFogType;\n"
					"uniform vec4  uFogColor;\n"
					"uniform float uFogStart;\n"
					"uniform float uFogEnd;\n"
					"uniform float uFogDensity;\n"
					"\n"
					"/* Texture Uniforms */\n"
					"\n"
					"uniform sampler2D uTextureUnit0;\n"
					"uniform sampler2D uTextureUnit1;\n"
					"uniform bool      uUseTexture [MAX_TEXTURE_UNITS];\n"
					"\n"
					"vec4 render2D(void)\n"
					"{\n"
					"	vec4 color = varVertexColor;\n"
					"	vec4 texel = texture2D(uTextureUnit0, varTexCoord[0].xy);\n"
					"	//if(uUseTexture[0])\n"
					"	{\n"
					"		color *= texel;\n"
					"	}\n"
					"	return color;\n"
					"}\n"
					"\n"
					"vec4 renderSolid(void)\n"
					"{\n"
					"	vec4 color = varVertexColor;\n"
					"	vec4 texel = texture2D(uTextureUnit0, varTexCoord[0].xy);\n"
					"	//if(uUseTexture[0])\n"
					"		color *= texel;\n"
					"	return color;\n"
					"}\n"
					"\n"
					"vec4 renderTransparentVertexAlpha(void)\n"
					"{\n"
					"	vec4 color = renderSolid();\n"
					"	color.a = varVertexColor.a;\n"
					"	return color;\n"
					"}\n"
					"\n"
					"vec4 renderTransparentAlphaChannel(void)\n"
					"{\n"
					"	vec4 texel = texture2D(uTextureUnit0, varTexCoord[0].xy);\n"
					"	vec4 color = texel * varVertexColor;\n"
					"	color.a = texel.a;\n"
					"	return color;\n"
					"}\n"
					"\n"
					"vec4 render2LayerSolid(void)\n"
					"{\n"
					"	float blendFactor = varVertexColor.a;\n"
					"	vec4 texel0 = texture2D(uTextureUnit0, varTexCoord[0].xy);\n"
					"	vec4 texel1 = texture2D(uTextureUnit1, varTexCoord[1].xy);\n"
					"	vec4 color = texel0 * blendFactor + texel1 * ( 1.0 - blendFactor );\n"
					"	return color;\n"
					"}\n"
					"\n"
					"vec4 renderLightMap(void)\n"
					"{\n"
					"	vec4 texel0  = texture2D(uTextureUnit0, varTexCoord[0].xy);\n"
					"	vec4 texel1  = texture2D(uTextureUnit1, varTexCoord[1].xy);\n"
					"	vec4 color   = texel0 * texel1 * 4.0;\n"
					"	color.a = texel0.a * texel0.a;\n"
					"	return color;\n"
					"}\n"
					"\n"
					"vec4 renderDetailMap(void)\n"
					"{\n"
					"	vec4 texel0  = texture2D(uTextureUnit0, varTexCoord[0].xy);\n"
					"	vec4 texel1  = texture2D(uTextureUnit1, varTexCoord[1].xy);\n"
					"	vec4 color   = texel0;\n"
					"	color += texel1 - 0.5;\n"
					"	return color;\n"
					"}\n"
					"\n"
					"vec4 renderReflection2Layer(void)\n"
					"{\n"
					"	vec4 color = varVertexColor;\n"
					"	vec4 texel0 = texture2D(uTextureUnit0, varTexCoord[0].xy);\n"
					"	vec4 texel1 = texture2D(uTextureUnit1, varTexCoord[1].xy);\n"
					"	color *=  texel0 * texel1;\n"
					"	return color;\n"
					"}\n"
					"\n"
					"float ComputeFog()\n"
					"{\n"
					"	float factor = 1.0;\n"
					"	if(uFogType == FOG_LINEAR)\n"
					"	{\n"
					"		factor = (uFogEnd - eyeDist) / (uFogEnd - uFogStart);\n"
					"	}\n"
					"	else if(uFogType == FOG_EXP)\n"
					"	{\n"
					"		 factor = exp(-(eyeDist * uFogDensity));\n"
					"	}\n"
					"	else //uFogType == FOG_EXP2\n"
					"	{\n"
					"		factor = eyeDist * uFogDensity;\n"
					"		factor = exp(-(factor * factor)) ;\n"
					"	}\n"
					"	factor = clamp(factor, 0.0, 1.0);\n"
					"	return factor;\n"
					"}\n"
					"\n"
					"void main (void)\n"
					"{\n"
					"	if(varClipDist < 0.0)\n"
					"		discard;\n"
					"	\n"
					"	if( uRenderMode == TwoD)\n"
					"		gl_FragColor = render2D();\n"
					"	else if( uRenderMode == Solid)\n"
					"		gl_FragColor = renderSolid();\n"
					"	else if(uRenderMode == LightMap)\n"
					"		gl_FragColor = renderLightMap();\n"
					"	else if(uRenderMode == DetailMap)\n"
					"		gl_FragColor = renderDetailMap();\n"
					"	else if(uRenderMode == SphereMap)\n"
					"		gl_FragColor = renderSolid();\n"
					"	else if(uRenderMode == Reflection2Layer)\n"
					"		gl_FragColor = renderReflection2Layer();\n"
					"	else if(uRenderMode == TransparentVertexAlpha)\n"
					"		gl_FragColor = renderTransparentVertexAlpha();\n"
					"	else if(uRenderMode == TransparentAlphaChannel)\n"
					"		gl_FragColor = renderTransparentAlphaChannel();\n"
					"	else\n"
					"		gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0);\n"
					"		\n"
					"	//gl_FragColor = varVertexColor;\n"
					"\n"
					"	if(uFog)\n"
					"	{\n"
					"		float fogFactor = ComputeFog();\n"
					"		gl_FragColor = gl_FragColor * fogFactor + uFogColor * (1.0 - fogFactor);\n"
					"	}\n"
					"			\n"
					"	if(uAlphaTest && uAlphaValue > gl_FragColor.a)\n"
					"		discard;\n"
					"	\n"
					"} \n";
		} else if(!strcmp(fileName, "COGLES2FixedPipeline.vsh")) {
			return "// Copyright (C) 2009-2010 Amundis\n"
					"// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt\n"
					"// and OpenGL ES driver implemented by Christian Stehno\n"
					"// This file is part of the \"Irrlicht Engine\".\n"
					"// For conditions of distribution and use, see copyright notice in Irrlicht.h\n"
					"#define MAX_TEXTURE_UNITS 4\n"
					"#define MAX_LIGHTS 8\n"
					"\n"
					"#define SphereMap        10\n"
					"#define Reflection2Layer 11\n"
					"\n"
					"const vec4 red   = vec4(1.0, 0.0, 0.0, 1.0);\n"
					"const vec4 green = vec4(0.0, 1.0, 0.0, 1.0);\n"
					"const vec4 blue  = vec4(0.0, 0.0, 1.0, 1.0);\n"
					"const vec4 white = vec4(1.0);\n"
					"const vec4 black = vec4(0.0);\n"
					"\n"
					"/* Vertex Attributes */\n"
					" \n"
					"attribute vec4 inVertexPosition;\n"
					"attribute vec4 inVertexColor;\n"
					"attribute vec4 inTexCoord0;\n"
					"attribute vec4 inTexCoord1;\n"
					"attribute vec3 inVertexNormal;\n"
					"\n"
					"uniform lowp int uRenderMode;\n"
					"\n"
					"/* Matrix Uniforms */\n"
					"\n"
					"uniform mat4 uMvpMatrix;\n"
					"uniform mat4 uWorldMatrix;\n"
					"\n"
					"uniform bool uNormalize;\n"
					"uniform vec3 uEyePos;\n"
					"\n"
					"/* Light Uniforms */\n"
					"\n"
					"uniform bool  uUseLight         [MAX_LIGHTS];\n"
					"uniform vec4  uLightPosition    [MAX_LIGHTS];\n"
					"uniform vec4  uLightAmbient     [MAX_LIGHTS];\n"
					"uniform vec4  uLightDiffuse     [MAX_LIGHTS];\n"
					"#ifdef USE_SPECULAR\n"
					"uniform vec4  uLightSpecular    [MAX_LIGHTS];\n"
					"#endif\n"
					"#ifdef USE_LIGHT_CUTOFF\n"
					"uniform vec3  uLightDirection   [MAX_LIGHTS];\n"
					"uniform float uLightCutoff      [MAX_LIGHTS];\n"
					"uniform float uLightExponent    [MAX_LIGHTS];\n"
					"#endif\n"
					"uniform vec3  uLightAttenuation [MAX_LIGHTS];\n"
					"uniform vec4  uAmbientColor;\n"
					"uniform bool  uLighting;\n"
					"\n"
					"/* Material Uniforms */\n"
					"uniform vec4  uMaterialAmbient;\n"
					"uniform vec4  uMaterialEmission;\n"
					"uniform vec4  uMaterialDiffuse;\n"
					"uniform vec4  uMaterialSpecular;\n"
					"uniform float uMaterialShininess;\n"
					"uniform int   uColorMaterial;\n"
					"\n"
					"#define ECM_NONE     0\n"
					"#define ECM_DIFFUSE  1\n"
					"#define ECM_AMBIENT  2\n"
					"#define ECM_EMISSIVE 3\n"
					"#define ECM_SPECULAR 4\n"
					"#define ECM_DIFFUSE_AND_AMBIENT 5\n"
					"\n"
					"/* Texture Uniforms */\n"
					"uniform bool uUseTexture    [MAX_TEXTURE_UNITS];\n"
					"uniform mat4 uTextureMatrix [MAX_TEXTURE_UNITS];\n"
					"uniform bool uUseTexMatrix  [MAX_TEXTURE_UNITS];\n"
					"\n"
					"/* Clip Plane Uniforms */\n"
					"uniform bool uClip;\n"
					"uniform vec4 uClipPlane;\n"
					"varying float varClipDist;\n"
					"\n"
					"/* Varyings */\n"
					"\n"
					"varying vec4 varTexCoord[MAX_TEXTURE_UNITS];\n"
					"varying vec4 varVertexColor;\n"
					"varying float eyeDist;\n"
					"\n"
					"/* shader variables */\n"
					"\n"
					"vec3 gNormal;\n"
					"vec3 gWorldPos;\n"
					"vec4 gColor;\n"
					"\n"
					"struct material {\n"
					"   vec4    Ambient;\n"
					"   vec4    Diffuse;\n"
					"   vec4    Specular;\n"
					"   vec4    Emission;\n"
					"   float   Shininess;\n"
					"} ;\n"
					"\n"
					"material gMaterial;\n"
					"\n"
					"vec4 lightEquation(int lidx)\n"
					"{		\n"
					"	vec4 color = vec4(0.0);\n"
					"	\n"
					"	float att = 1.0;\n"
					"	vec3 lightDir;\n"
					"	\n"
					"	\n"
					"	if( uLightPosition[lidx].w == 0.0) // Directional light\n"
					"	{\n"
					"		lightDir = -uLightPosition[lidx].xyz;\n"
					"	}\n"
					"	else\n"
					"	{\n"
					"		lightDir = uLightPosition[lidx].xyz - inVertexPosition.xyz;\n"
					"		att = 1.0 / (uLightAttenuation[lidx].y * length(lightDir));\n"
					"		lightDir = normalize(lightDir);\n"
					"		\n"
					"#ifdef USE_LIGHT_CUTOFF\n"
					"		if(uLightCutoff[lidx] < 180.0)\n"
					"		{\n"
					"			// compute spot factor\n"
					"			float spotEffect = dot(-lightDir, uLightDirection[lidx]);\n"
					"			if( spotEffect >= cos( radians( uLightCutoff[lidx])))\n"
					"				spotEffect = pow( spotEffect, uLightExponent[lidx]);\n"
					"			else\n"
					"				spotEffect = 0.0;\n"
					"			att *= spotEffect;\n"
					"		}\n"
					"#endif\n"
					"	}\n"
					"	\n"
					"	if(att >= 0.0 )\n"
					"	{\n"
					"		color += uLightAmbient[lidx] * gMaterial.Ambient;\n"
					"		\n"
					"		//Compute cos(Light, Normal)\n"
					"		float NdotL = max(dot(normalize(gNormal), lightDir), 0.0);\n"
					"		color += NdotL * uLightDiffuse[lidx] * gMaterial.Diffuse;\n"
					"		\n"
					"		//Compute cos(hvec, Normal)\n"
					"#ifdef USE_SPECULAR\n"
					"		vec3 hvec = normalize(lightDir + vec3(0.0, 0.0, 1.0));\n"
					"		float NdotH = dot(gNormal, hvec); "
					"		if(NdotH > 0.0) "
					"		{\n"
					"			color += pow(NdotH, uMaterialShininess) * uLightSpecular[lidx] * gMaterial.Specular;\n"
					"		}\n"
					"#endif\n"
					"		color *= att;\n"
					"	}\n"
					"	return color;\n"
					"}\n"
					"\n"
					"vec4 computeLighting(void)\n"
					"{\n"
					"	vec4 color = gMaterial.Emission + gMaterial.Ambient * uAmbientColor;\n"
					"\n"
					"	for ( int i = 0 ; i < MAX_LIGHTS; ++i)\n"
					"	{\n"
					"		if ( uUseLight[i] )\n"
					"		{\n"
					"			color += lightEquation(i) ;\n"
					"		}\n"
					"	}\n"
					"	color.a = gMaterial.Diffuse.a;\n"
					"	return color;\n"
					"}\n"
					"\n"
					"void ReplaceColorMaterial(void)\n"
					"{\n"
					"	gMaterial.Ambient = uMaterialAmbient;\n"
					"	gMaterial.Diffuse = uMaterialDiffuse;\n"
					"	gMaterial.Emission = uMaterialEmission;\n"
					"	gMaterial.Specular = uMaterialSpecular;\n"
					"	gMaterial.Shininess = uMaterialShininess;\n"
					"\n"
					"	if(uColorMaterial == ECM_DIFFUSE)\n"
					"		gMaterial.Diffuse = gColor;\n"
					"	else if(uColorMaterial == ECM_AMBIENT)\n"
					"		gMaterial.Ambient = gColor;\n"
					"	else if(uColorMaterial == ECM_DIFFUSE_AND_AMBIENT)\n"
					"	{\n"
					"		gMaterial.Diffuse = gColor;\n"
					"		gMaterial.Ambient = gColor;\n"
					"	}	\n"
					"	else if(uColorMaterial == ECM_EMISSIVE)\n"
					"		gMaterial.Emission = gColor;\n"
					"	else if(uColorMaterial == ECM_SPECULAR)\n"
					"		gMaterial.Specular = gColor;\n"
					"}\n"
					"\n"
					"void main(void)\n"
					"{\n"
					"	gl_Position = uMvpMatrix * inVertexPosition;\n"
					"	\n"
					"	gWorldPos = (uWorldMatrix * inVertexPosition).xyz;\n"
					"	\n"
					"	gColor = inVertexColor.bgra;\n"
					"	\n"
					"	gNormal = inVertexNormal.xyz;\n"
					"	if(uNormalize)\n"
					"		gNormal = normalize(gNormal);\n"
					"	\n"
					"	ReplaceColorMaterial();\n"
					"	if(uLighting)\n"
					"		varVertexColor = computeLighting();\n"
					"	else\n"
					"		varVertexColor = gColor;\n"
					"	\n"
					"	for(int i = 0; i < MAX_TEXTURE_UNITS; ++i)\n"
					"		varTexCoord[i] = vec4(0.0);\n"
					"	\n"
					"	if( uUseTexture[0])\n"
					"	{\n"
					"		if(uRenderMode == SphereMap || uRenderMode == Reflection2Layer)\n"
					"		{\n"
					"			vec3 eyeDir = normalize(inVertexPosition.xyz - uEyePos);\n"
					"			vec3 reflection = reflect(eyeDir, gNormal);\n"
					"			float m = 2.0 * sqrt(reflection.x * reflection.x +\n"
					"								 reflection.y * reflection.y +\n"
					"								 (reflection.z + 1.0) * (reflection.z + 1.0));\n"
					"			varTexCoord[0] = vec4((reflection.x / m + 0.5), (reflection.y / m + 0.5), 0.0, 0.0); \n"
					"		}\n"
					"		else\n"
					"		{\n"
					"			varTexCoord[0] = inTexCoord0;\n"
					"			if(uUseTexMatrix[0])\n"
					"				varTexCoord[0] *= uTextureMatrix[0];\n"
					"		}\n"
					"	}\n"
					"	if(uUseTexture[1])\n"
					"	{\n"
					"		varTexCoord[1] = inTexCoord1;\n"
					"		if(uUseTexMatrix[1])\n"
					"			varTexCoord[1] *= uTextureMatrix[1];\n"
					"	}\n"
					"	\n"
					"	eyeDist = length(uEyePos);\n"
					"	\n"
					"	varClipDist = uClip ? dot(gWorldPos, uClipPlane.xyz)-uClipPlane.w : 0.0;\n"
					"	\n"
					"	varVertexColor.rgb  = clamp(varVertexColor.rgb, 0.0, 1.0);\n"
					"	varVertexColor.a = gColor.a;\n"
					"} \n";
		} else if(!strcmp(fileName, "COGLES2NormalMap.fsh")) {
			return  "// Copyright (C) 2009-2010 Amundis\n"
					"// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt\n"
					"// and OpenGL ES driver implemented by Christian Stehno\n"
					"// This file is part of the \"Irrlicht Engine\".\n"
					"// For conditions of distribution and use, see copyright notice in Irrlicht.h\n"
					"#define MAX_LIGHTS 2\n"
					"\n"
					"precision mediump float;\n"
					"\n"
					"uniform sampler2D texture0;\n"
					"uniform sampler2D texture1;\n"
					"\n"
					"varying vec4 varTexCoord;\n"
					"varying vec3 varLightVector[MAX_LIGHTS];\n"
					"varying vec4 varLightColor[MAX_LIGHTS];\n"
					"\n"
					"varying vec4 debug;\n"
					"\n"
					"void main(void)\n"
					"{\n"
					"	// fetch color and normal map\n"
					"	vec4 normalMap = texture2D(texture1, varTexCoord.xy) *  2.0 - 1.0;\n"
					"	vec4 colorMap  = texture2D(texture0, varTexCoord.xy);\n"
					"	\n"
					"	// calculate color of light 0\n"
					"	vec4 color = clamp(varLightColor[0], 0.0, 1.0) * dot(normalMap.xyz, normalize(varLightVector[0].xyz));\n"
					"	\n"
					"	// calculate color of light 1\n"
					"	color += clamp(varLightColor[1], 0.0, 1.0) * dot(normalMap.xyz, normalize(varLightVector[1].xyz));\n"
					"\n"
					"	//luminance * base color\n"
					"	color *= colorMap;\n"
					"	color.a = varLightColor[0].a;\n"
					"	\n"
					"	gl_FragColor = color;\n"
					"}\n";
		} else if(!strcmp(fileName, "COGLES2NormalMap.vsh")) {
			return "// Copyright (C) 2009-2010 Amundis\n"
					"// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt\n"
					"// and OpenGL ES driver implemented by Christian Stehno\n"
					"// This file is part of the \"Irrlicht Engine\".\n"
					"// For conditions of distribution and use, see copyright notice in Irrlicht.h\n"
					"#define MAX_LIGHTS 2\n"
					"\n"
					"attribute vec4 inVertexPosition;\n"
					"attribute vec4 inVertexColor;\n"
					"attribute vec4 inTexCoord0;\n"
					"attribute vec3 inVertexNormal;\n"
					"attribute vec3 inVertexTangent;\n"
					"attribute vec3 inVertexBinormal;\n"
					"\n"
					"uniform mat4 uMvpMatrix;\n"
					"uniform vec4 uLightPos[MAX_LIGHTS];\n"
					"uniform vec4 uLightColor[MAX_LIGHTS];\n"
					"\n"
					"varying vec4 varTexCoord;\n"
					"varying vec3 varLightVector[MAX_LIGHTS];\n"
					"varying vec4 varLightColor[MAX_LIGHTS];\n"
					"\n"
					"varying vec4 debug;\n"
					"\n"
					"void main(void)\n"
					"{\n"
					"	debug = vec4(inVertexNormal, 1.0);\n"
					"	// transform position to clip space\n"
					"	gl_Position = uMvpMatrix * inVertexPosition;\n"
					"	\n"
					"	// vertex - lightpositions\n"
					"	vec4 tempLightVector0 = uLightPos[0] - inVertexPosition;\n"
					"	vec4 tempLightVector1 = uLightPos[1] - inVertexPosition;\n"
					"	\n"
					"	// transform the light vector 1 with U, V, W\n"
					"	varLightVector[0].x = dot(inVertexTangent,  tempLightVector0.xyz);\n"
					"	varLightVector[0].y = dot(inVertexBinormal, tempLightVector0.xyz);\n"
					"	varLightVector[0].z = dot(inVertexNormal,   tempLightVector0.xyz);\n"
					"\n"
					"	\n"
					"	// transform the light vector 2 with U, V, W\n"
					"	varLightVector[1].x = dot(inVertexTangent,  tempLightVector1.xyz);\n"
					"	varLightVector[1].y = dot(inVertexBinormal, tempLightVector1.xyz);\n"
					"	varLightVector[1].z = dot(inVertexNormal,   tempLightVector1.xyz);\n"
					"\n"
					"	// calculate attenuation of light 0\n"
					"	varLightColor[0].w = 0.0;\n"
					"	varLightColor[0].x = dot(tempLightVector0, tempLightVector0);\n"
					"	varLightColor[0].x *= uLightColor[0].w;\n"
					"	varLightColor[0] = vec4(inversesqrt(varLightColor[0].x));\n"
					"	varLightColor[0] *= uLightColor[0];\n"
					"	\n"
					"	// normalize light vector 0\n"
					"	varLightVector[0] = normalize(varLightVector[0]);\n"
					"	\n"
					"	// calculate attenuation of light 1\n"
					"	varLightColor[1].w = 0.0;\n"
					"	varLightColor[1].x = dot(tempLightVector1, tempLightVector1);\n"
					"	varLightColor[1].x *= uLightColor[1].w;\n"
					"	varLightColor[1] = vec4(inversesqrt(varLightColor[1].x));\n"
					"	varLightColor[1] *= uLightColor[1];\n"
					"	\n"
					"	// normalize light vector 1\n"
					"	varLightVector[1] = normalize(varLightVector[1]);\n"
					"	\n"
					"	// move out texture coordinates and original alpha value\n"
					"	varTexCoord = inTexCoord0;\n"
					"	varLightColor[0].a = inVertexColor.a;\n"
					"}\n";
		} else if(!strcmp(fileName, "COGLES2ParallaxMap.fsh")) {
			return "// Copyright (C) 2009-2010 Amundis\n"
					"// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt\n"
					"// and OpenGL ES driver implemented by Christian Stehno\n"
					"// This file is part of the \"Irrlicht Engine\".\n"
					"// For conditions of distribution and use, see copyright notice in Irrlicht.h\n"
					"#define MAX_LIGHTS 2\n"
					"\n"
					"precision mediump float;\n"
					"\n"
					"uniform sampler2D texture0;\n"
					"uniform sampler2D texture1;\n"
					"\n"
					"//uniform vec4 uLightDiffuse[MAX_LIGHTS];\n"
					"uniform float uHeightScale;\n"
					"\n"
					"varying vec4 varTexCoord;\n"
					"varying vec3 varLightVector[MAX_LIGHTS];\n"
					"varying vec4 varLightColor[MAX_LIGHTS];\n"
					"varying vec3 varEyeVector;\n"
					"\n"
					"varying vec4 debug;\n"
					"\n"
					"void main(void)\n"
					"{\n"
					"	// fetch color and normal map\n"
					"	vec4 normalMap = texture2D(texture1, varTexCoord.xy) *  2.0 - 1.0;\n"
					"\n"
					"	// height = height * scale\n"
					"	normalMap *= uHeightScale;\n"
					"	\n"
					"	// calculate new texture coord: height * eye + oldTexCoord\n"
					"	vec2 offset = varEyeVector.xy * normalMap.w + varTexCoord.xy;\n"
					"\n"
					"	// fetch new textures\n"
					"	vec4 colorMap  = texture2D(texture0, offset);\n"
					"	normalMap = normalize(texture2D(texture1, offset) * 2.0 - 1.0); \n"
					"	\n"
					"	// calculate color of light 0\n"
					"	vec4 color = clamp(varLightColor[0], 0.0, 1.0) * dot(normalMap.xyz, normalize(varLightVector[0].xyz));\n"
					"	\n"
					"	// calculate color of light 1\n"
					"	color += clamp(varLightColor[1], 0.0, 1.0) * dot(normalMap.xyz, normalize(varLightVector[1].xyz));\n"
					"\n"
					"	//luminance * base color\n"
					"	color *= colorMap;\n"
					"	color.a = varLightColor[0].a;\n"
					"	\n"
					"	gl_FragColor = color;\n"
					"} \n";
		} else if(!strcmp(fileName, "COGLES2ParallaxMap.vsh")) {
			return "// Copyright (C) 2009-2010 Amundis\n"
					"// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt\n"
					"// and OpenGL ES driver implemented by Christian Stehno\n"
					"// This file is part of the \"Irrlicht Engine\".\n"
					"// For conditions of distribution and use, see copyright notice in Irrlicht.h\n"
					"#define MAX_TEXTURE_UNITS 4\n"
					"#define MAX_LIGHTS 8\n"
					"\n"
					"#define SphereMap        10\n"
					"#define Reflection2Layer 11\n"
					"\n"
					"const vec4 red   = vec4(1.0, 0.0, 0.0, 1.0);\n"
					"const vec4 green = vec4(0.0, 1.0, 0.0, 1.0);\n"
					"const vec4 blue  = vec4(0.0, 0.0, 1.0, 1.0);\n"
					"const vec4 white = vec4(1.0);\n"
					"const vec4 black = vec4(0.0);\n"
					"\n"
					"/* Vertex Attributes */\n"
					" \n"
					"attribute vec4 inVertexPosition;\n"
					"attribute vec4 inVertexColor;\n"
					"attribute vec4 inTexCoord0;\n"
					"attribute vec4 inTexCoord1;\n"
					"attribute vec3 inVertexNormal;\n"
					"\n"
					"uniform lowp int uRenderMode;\n"
					"\n"
					"/* Matrix Uniforms */\n"
					"\n"
					"uniform mat4 uMvpMatrix;\n"
					"uniform mat4 uWorldMatrix;\n"
					"\n"
					"uniform bool uNormalize;\n"
					"uniform vec3 uEyePos;\n"
					"\n"
					"/* Light Uniforms */\n"
					"\n"
					"uniform bool  uUseLight         [MAX_LIGHTS];\n"
					"uniform vec4  uLightPosition    [MAX_LIGHTS];\n"
					"uniform vec4  uLightAmbient     [MAX_LIGHTS];\n"
					"uniform vec4  uLightDiffuse     [MAX_LIGHTS];\n"
					"#ifdef USE_SPECULAR\n"
					"uniform vec4  uLightSpecular    [MAX_LIGHTS];\n"
					"#endif\n"
					"#ifdef USE_LIGHT_CUTOFF\n"
					"uniform vec3  uLightDirection   [MAX_LIGHTS];\n"
					"uniform float uLightCutoff      [MAX_LIGHTS];\n"
					"uniform float uLightExponent    [MAX_LIGHTS];\n"
					"#endif\n"
					"uniform vec3  uLightAttenuation [MAX_LIGHTS];\n"
					"uniform vec4  uAmbientColor;\n"
					"uniform bool  uLighting;\n"
					"\n"
					"/* Material Uniforms */\n"
					"uniform vec4  uMaterialAmbient;\n"
					"uniform vec4  uMaterialEmission;\n"
					"uniform vec4  uMaterialDiffuse;\n"
					"uniform vec4  uMaterialSpecular;\n"
					"uniform float uMaterialShininess;\n"
					"uniform int   uColorMaterial;\n"
					"\n"
					"#define ECM_NONE     0\n"
					"#define ECM_DIFFUSE  1\n"
					"#define ECM_AMBIENT  2\n"
					"#define ECM_EMISSIVE 3\n"
					"#define ECM_SPECULAR 4\n"
					"#define ECM_DIFFUSE_AND_AMBIENT 5\n"
					"\n"
					"/* Texture Uniforms */\n"
					"uniform bool uUseTexture    [MAX_TEXTURE_UNITS];\n"
					"uniform mat4 uTextureMatrix [MAX_TEXTURE_UNITS];\n"
					"uniform bool uUseTexMatrix  [MAX_TEXTURE_UNITS];\n"
					"\n"
					"/* Clip Plane Uniforms */\n"
					"uniform bool uClip;\n"
					"uniform vec4 uClipPlane;\n"
					"varying float varClipDist;\n"
					"\n"
					"/* Varyings */\n"
					"\n"
					"varying vec4 varTexCoord[MAX_TEXTURE_UNITS];\n"
					"varying vec4 varVertexColor;\n"
					"varying float eyeDist;\n"
					"\n"
					"/* shader variables */\n"
					"\n"
					"vec3 gNormal;\n"
					"vec3 gWorldPos;\n"
					"vec4 gColor;\n"
					"\n"
					"struct material {\n"
					"   vec4    Ambient;\n"
					"   vec4    Diffuse;\n"
					"   vec4    Specular;\n"
					"   vec4    Emission;\n"
					"   float   Shininess;\n"
					"} ;\n"
					"\n"
					"material gMaterial;\n"
					"\n"
					"vec4 lightEquation(int lidx)\n"
					"{		\n"
					"	vec4 color = vec4(0.0);\n"
					"	\n"
					"	float att = 1.0;\n"
					"	vec3 lightDir;\n"
					"	\n"
					"	\n"
					"	if( uLightPosition[lidx].w == 0.0) // Directional light\n"
					"	{\n"
					"		lightDir = -uLightPosition[lidx].xyz;\n"
					"	}\n"
					"	else\n"
					"	{\n"
					"		lightDir = uLightPosition[lidx].xyz - inVertexPosition.xyz;\n"
					"		att = 1.0 / (uLightAttenuation[lidx].y * length(lightDir));\n"
					"		lightDir = normalize(lightDir);\n"
					"		\n"
					"#ifdef USE_LIGHT_CUTOFF\n"
					"		if(uLightCutoff[lidx] < 180.0)\n"
					"		{\n"
					"			// compute spot factor\n"
					"			float spotEffect = dot(-lightDir, uLightDirection[lidx]);\n"
					"			if( spotEffect >= cos( radians( uLightCutoff[lidx])))\n"
					"				spotEffect = pow( spotEffect, uLightExponent[lidx]);\n"
					"			else\n"
					"				spotEffect = 0.0;\n"
					"			att *= spotEffect;\n"
					"		}\n"
					"#endif\n"
					"	}\n"
					"	\n"
					"	if(att >= 0.0 )\n"
					"	{\n"
					"		color += uLightAmbient[lidx] * gMaterial.Ambient;\n"
					"		\n"
					"		//Compute cos(Light, Normal)\n"
					"		float NdotL = max(dot(normalize(gNormal), lightDir), 0.0);\n"
					"		color += NdotL * uLightDiffuse[lidx] * gMaterial.Diffuse;\n"
					"		\n"
					"		//Compute cos(hvec, Normal)\n"
					"#ifdef USE_SPECULAR\n"
					"		vec3 hvec = normalize(lightDir + vec3(0.0, 0.0, 1.0));\n"
					"		float NdotH = dot(gNormal, hvec);\n"
					"		if(NdotH > 0.0)\n"
					"		{\n"
					"			color += pow(NdotH, uMaterialShininess) * uLightSpecular[lidx] * gMaterial.Specular;\n"
					"		}\n"
					"#endif\n"
					"		color *= att;\n"
					"	}\n"
					"	return color;\n"
					"}\n"
					"\n"
					"vec4 computeLighting(void)\n"
					"{\n"
					"	vec4 color = gMaterial.Emission + gMaterial.Ambient * uAmbientColor;\n"
					"\n"
					"	for ( int i = 0 ; i < MAX_LIGHTS; ++i)\n"
					"	{\n"
					"		if ( uUseLight[i] )\n"
					"		{\n"
					"			color += lightEquation(i) ;\n"
					"		}\n"
					"	}\n"
					"	color.a = gMaterial.Diffuse.a;\n"
					"	return color;\n"
					"}\n"
					"\n"
					"void ReplaceColorMaterial(void)\n"
					"{\n"
					"	gMaterial.Ambient = uMaterialAmbient;\n"
					"	gMaterial.Diffuse = uMaterialDiffuse;\n"
					"	gMaterial.Emission = uMaterialEmission;\n"
					"	gMaterial.Specular = uMaterialSpecular;\n"
					"	gMaterial.Shininess = uMaterialShininess;\n"
					"\n"
					"	if(uColorMaterial == ECM_DIFFUSE)\n"
					"		gMaterial.Diffuse = gColor;\n"
					"	else if(uColorMaterial == ECM_AMBIENT)\n"
					"		gMaterial.Ambient = gColor;\n"
					"	else if(uColorMaterial == ECM_DIFFUSE_AND_AMBIENT)\n"
					"	{\n"
					"		gMaterial.Diffuse = gColor;\n"
					"		gMaterial.Ambient = gColor;\n"
					"	}	\n"
					"	else if(uColorMaterial == ECM_EMISSIVE)\n"
					"		gMaterial.Emission = gColor;\n"
					"	else if(uColorMaterial == ECM_SPECULAR)\n"
					"		gMaterial.Specular = gColor;\n"
					"}\n"
					"\n"
					"void main(void)\n"
					"{\n"
					"	gl_Position = uMvpMatrix * inVertexPosition;\n"
					"	\n"
					"	gWorldPos = (uWorldMatrix * inVertexPosition).xyz;\n"
					"	\n"
					"	gColor = inVertexColor.bgra;\n"
					"	\n"
					"	gNormal = inVertexNormal.xyz;\n"
					"	if(uNormalize)\n"
					"		gNormal = normalize(gNormal);\n"
					"	\n"
					"	ReplaceColorMaterial();\n"
					"	if(uLighting)\n"
					"		varVertexColor = computeLighting();\n"
					"	else\n"
					"		varVertexColor = gColor;\n"
					"	\n"
					"	for(int i = 0; i < MAX_TEXTURE_UNITS; ++i)\n"
					"		varTexCoord[i] = vec4(0.0);\n"
					"	\n"
					"	if( uUseTexture[0])\n"
					"	{\n"
					"		if(uRenderMode == SphereMap || uRenderMode == Reflection2Layer)\n"
					"		{\n"
					"			vec3 eyeDir = normalize(inVertexPosition.xyz - uEyePos);\n"
					"			vec3 reflection = reflect(eyeDir, gNormal);\n"
					"			float m = 2.0 * sqrt(reflection.x * reflection.x +\n"
					"								 reflection.y * reflection.y +\n"
					"								 (reflection.z + 1.0) * (reflection.z + 1.0));\n"
					"			varTexCoord[0] = vec4((reflection.x / m + 0.5), (reflection.y / m + 0.5), 0.0, 0.0); \n"
					"		}\n"
					"		else\n"
					"		{\n"
					"			varTexCoord[0] = inTexCoord0;\n"
					"			if(uUseTexMatrix[0])\n"
					"				varTexCoord[0] *= uTextureMatrix[0];\n"
					"		}\n"
					"	}\n"
					"	if(uUseTexture[1])\n"
					"	{\n"
					"		varTexCoord[1] = inTexCoord1;\n"
					"		if(uUseTexMatrix[1])\n"
					"			varTexCoord[1] *= uTextureMatrix[1];\n"
					"	}\n"
					"	\n"
					"	eyeDist = length(uEyePos);\n"
					"	\n"
					"	varClipDist = uClip ? dot(gWorldPos, uClipPlane.xyz)-uClipPlane.w : 0.0;\n"
					"	\n"
					"	varVertexColor.rgb  = clamp(varVertexColor.rgb, 0.0, 1.0);\n"
					"	varVertexColor.a = gColor.a;\n"
					"} \n";
		} else if(!strcmp(fileName, "COGLES2Renderer2D.fsh")) {
			return "// Copyright (C) 2009-2010 Amundis\n"
					"// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt\n"
					"// and OpenGL ES driver implemented by Christian Stehno\n"
					"// This file is part of the \"Irrlicht Engine\".\n"
					"// For conditions of distribution and use, see copyright notice in Irrlicht.h\n"
					"precision mediump float;\n"
					"\n"
					"uniform bool uUseTexture;\n"
					"uniform sampler2D uTextureUnit;\n"
					"uniform bool uAlphaTest;\n"
					"uniform float uAlphaValue;\n"
					"\n"
					"varying vec4 varVertexColor;\n"
					"varying vec4 varTexCoord;\n"
					"\n"
					"void main(void)\n"
					"{\n"
					"	vec4 color = varVertexColor;\n"
					"	vec4 texel = texture2D(uTextureUnit, varTexCoord.xy);\n"
					"	if(uUseTexture)\n"
					"	{\n"
					"		color *= texel;\n"
					"	}\n"
					"	\n"
					"	if(uAlphaTest && !(color.a > uAlphaValue))\n"
					"		discard;\n"
					"	\n"
					"	gl_FragColor = color;\n"
					"} \n";
		} else if(!strcmp(fileName, "COGLES2Renderer2D.vsh")) {
			return "// Copyright (C) 2009-2010 Amundis\n"
					"// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt\n"
					"// and OpenGL ES driver implemented by Christian Stehno\n"
					"// This file is part of the \"Irrlicht Engine\".\n"
					"// For conditions of distribution and use, see copyright notice in Irrlicht.h\n"
					"attribute vec4 inVertexPosition;\n"
					"attribute vec4 inVertexColor;\n"
					"attribute vec4 inTexCoord0;\n"
					"\n"
					"uniform mat4 uOrthoMatrix;\n"
					"\n"
					"varying vec4 varVertexColor;\n"
					"varying vec4 varTexCoord;\n"
					"\n"
					"void main(void)\n"
					"{\n"
					"	gl_Position    = uOrthoMatrix * inVertexPosition;\n"
					"	varVertexColor = inVertexColor.bgra;\n"
					"	varTexCoord    = inTexCoord0;\n"
					"} \n";
		}
		return "";
	}
} // end namespace video
} // end namespace irr

#endif //_IRR_COMPILE_WITH_OPENGLES2_


#include "OpenGLFactory.h"
#include "OpenGLBuffer.h"
#include "OpenGLTexture.h"
#include "OpenGLView.h"
#include "OpenGLSamplerState.h"
#include "OpenGLShader.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLVertexDeclaration.h"
#include <Core/Exception.h>
#include <Graphics/RenderState.h>
#include "pfm.h"

namespace RcEngine {

shared_ptr<RasterizerState> OpenGLFactory::CreateRasterizerStateImpl( const RasterizerStateDesc& desc )
{
	return shared_ptr<RasterizerState>( new RasterizerState(desc) );
}

shared_ptr<SamplerState> OpenGLFactory::CreateSamplerStateImpl( const SamplerStateDesc& desc )
{
	return shared_ptr<OpenGLSamplerState>( new OpenGLSamplerState(desc) );
}
 
shared_ptr<DepthStencilState> OpenGLFactory::CreateDepthStencilStateImpl( const DepthStencilStateDesc& desc )
{
	return shared_ptr<DepthStencilState>( new DepthStencilState(desc) );
}

shared_ptr<BlendState> OpenGLFactory::CreateBlendStateImpl( const BlendStateDesc& desc )
{
	return shared_ptr<BlendState>( new BlendState(desc) );
}

shared_ptr<GraphicsBuffer> OpenGLFactory::CreateVertexBuffer( uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	return std::shared_ptr<OpenGLBuffer>(
		new OpenGLBuffer(buffreSize, accessHint, createFlags, GL_ARRAY_BUFFER, initData) );
}

shared_ptr<GraphicsBuffer> OpenGLFactory::CreateIndexBuffer( uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags,ElementInitData* initData )
{
	return std::shared_ptr<OpenGLBuffer>(
		new OpenGLBuffer(buffreSize, accessHint, createFlags, GL_ELEMENT_ARRAY_BUFFER, initData) );
}

shared_ptr<GraphicsBuffer> OpenGLFactory::CreateConstantBuffer( uint32_t buffreSize, uint32_t accessHint, uint32_t createFlags,ElementInitData* initData )
{
	return std::shared_ptr<OpenGLBuffer>(
		new OpenGLBuffer(buffreSize, accessHint, createFlags, GL_UNIFORM_BUFFER, initData) );
}

shared_ptr<GraphicsBuffer> OpenGLFactory::CreateTextureBuffer( PixelFormat format, uint32_t elementCount, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	uint32_t texelSize = PixelFormatUtils::GetNumElemBytes(format);
	return std::shared_ptr<OpenGLBuffer>(
		new OpenGLBuffer(texelSize * elementCount, accessHint, createFlags, GL_TEXTURE_BUFFER, initData) );
}

shared_ptr<GraphicsBuffer> OpenGLFactory::CreateStructuredBuffer(uint32_t strutureStride, uint32_t elementCount, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData)
{
	return std::shared_ptr<OpenGLBuffer>(
		new OpenGLBuffer(strutureStride * elementCount, accessHint, createFlags, GL_SHADER_STORAGE_BUFFER, initData) );
}

shared_ptr<Texture> OpenGLFactory::CreateTexture1D( uint32_t width, PixelFormat format, uint32_t arrSize, uint32_t numMipMaps, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	return shared_ptr<OpenGLTexture1D>( 
		new OpenGLTexture1D(format, arrSize, numMipMaps, width, accessHint, createFlags, initData) );
}

shared_ptr<Texture> OpenGLFactory::CreateTexture2D( uint32_t width, uint32_t height, PixelFormat format, uint32_t arrSize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	return shared_ptr<OpenGLTexture2D>(
		new OpenGLTexture2D(format, arrSize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, createFlags, initData));

	// VS2012 Bug, only support at most 6 parameters
	//return std::make_shared<OpenGLTexture1D>(format, arrSize, numMipMaps, width, sampleCount, sampleQuality, accessHint, initData);
}

shared_ptr<Texture> OpenGLFactory::CreateTexture3D( uint32_t width, uint32_t height, uint32_t depth, PixelFormat format, uint32_t numMipMaps, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	return shared_ptr<OpenGLTexture3D>(
		new OpenGLTexture3D(format, numMipMaps, width, height, depth, accessHint, createFlags, initData) );
}

shared_ptr<Texture> OpenGLFactory::CreateTextureCube( uint32_t width, uint32_t height, PixelFormat format, uint32_t arraySize, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint, uint32_t createFlags, ElementInitData* initData )
{
	return shared_ptr<OpenGLTextureCube>(
		new OpenGLTextureCube(format, arraySize, numMipMaps, width, height, sampleCount, sampleQuality, accessHint, createFlags, initData) );
}

shared_ptr<ShaderResourceView> OpenGLFactory::CreateTextureBufferSRV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, PixelFormat format )
{
	return shared_ptr<ShaderResourceView>( new OpenGLTextureBufferSRV(buffer, elementOffset, elementCount, format) );
}

shared_ptr<ShaderResourceView> OpenGLFactory::CreateStructuredBufferSRV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, uint32_t strutureStride )
{
	return shared_ptr<ShaderResourceView>( new OpenGLStructuredBufferSRV(buffer, elementOffset, elementCount, strutureStride) );
}

shared_ptr<UnorderedAccessView> OpenGLFactory::CreateStructuredBufferUAV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, uint32_t strutureStride )
{
	return shared_ptr<UnorderedAccessView>( new OpenGLStructuredBufferUAV(buffer, elementOffset, elementCount, strutureStride) );
}

shared_ptr<UnorderedAccessView> OpenGLFactory::CreateTextureBufferUAV( const shared_ptr<GraphicsBuffer>& buffer, uint32_t elementOffset, uint32_t elementCount, PixelFormat format )
{
	return shared_ptr<UnorderedAccessView>( new OpenGLTextureBufferUAV(buffer, elementOffset, elementCount, format) );
}

shared_ptr<RenderView> OpenGLFactory::CreateRenderTargetView2D( const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level )
{
	return shared_ptr<RenderView>( new OpenGLRenderTargetView2D(texture, arrayIndex, level) );
}

shared_ptr<RenderView> OpenGLFactory::CreateDepthStencilView( const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level, uint32_t flags /*= 0*/ )
{
	return shared_ptr<RenderView>( new OpenGLDepthStencilView(texture, arrayIndex, level) );
}

shared_ptr<RenderView> OpenGLFactory::CreateRenderTargetViewArray( const shared_ptr<Texture>& texture, uint32_t level )
{
	return shared_ptr<RenderView>( new OpenGLRenderTargetArrayView(texture, level) );
}

shared_ptr<Shader> OpenGLFactory::CreateShader( ShaderType type )
{
	return shared_ptr<Shader>( new OpenGLShader(type) );
}

shared_ptr<ShaderResourceView> OpenGLFactory::CreateTexture3DSRV( const shared_ptr<Texture>& texture )
{
	return shared_ptr<ShaderResourceView>( new OpenGLTextureSRV(texture, 0, texture->GetMipLevels(), 0, texture->GetTextureArraySize()) );
}

shared_ptr<ShaderResourceView> OpenGLFactory::CreateTextureCubeSRV( const shared_ptr<Texture>& texture )
{
	return shared_ptr<ShaderResourceView>( new OpenGLTextureSRV(texture, 0, texture->GetMipLevels(), 0, texture->GetTextureArraySize()) );
}

shared_ptr<ShaderResourceView> OpenGLFactory::CreateTexture1DSRV( const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
{
	return shared_ptr<ShaderResourceView>( new OpenGLTextureSRV(texture, mostDetailedMip, mipLevels, firstArraySlice, arraySize) );
}

shared_ptr<ShaderResourceView> OpenGLFactory::CreateTexture2DSRV( const shared_ptr<Texture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
{
	return shared_ptr<ShaderResourceView>( new OpenGLTextureSRV(texture, mostDetailedMip, mipLevels, firstArraySlice, arraySize) );
}

shared_ptr<UnorderedAccessView> OpenGLFactory::CreateTexture1DUAV( const shared_ptr<Texture>& texture, uint32_t level, uint32_t firstArraySlice, uint32_t arraySize )
{
	return shared_ptr<UnorderedAccessView>( new OpenGLTextureUAV(texture, level, firstArraySlice, arraySize ) );
}

shared_ptr<UnorderedAccessView> OpenGLFactory::CreateTexture2DUAV( const shared_ptr<Texture>& texture, uint32_t level, uint32_t firstArraySlice, uint32_t arraySize )
{
	return shared_ptr<UnorderedAccessView>( new OpenGLTextureUAV(texture, level, firstArraySlice, arraySize ) );
}

shared_ptr<UnorderedAccessView> OpenGLFactory::CreateTexture3DUAV( const shared_ptr<Texture>& texture )
{
	ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "3D UAV not implemented!", "OpenGLFactory::CreateTexture3DUAV");
}

shared_ptr<UnorderedAccessView> OpenGLFactory::CreateTextureCubeUAV( const shared_ptr<Texture>& texture )
{
	ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "CubeMap UAV not implemented!", "OpenGLFactory::CreateTextureCubeUAV");
}

shared_ptr<VertexDeclaration> OpenGLFactory::CreateVertexDeclaration( VertexElement* elems, uint32_t count )
{
	return shared_ptr<VertexDeclaration>( new OpenGLVertexDeclaration(elems, count) );
}

shared_ptr<ShaderPipeline> OpenGLFactory::CreateShaderPipeline( Effect& effect )
{
	return shared_ptr<ShaderPipeline>( new OpenGLShaderPipeline(effect) );
}

shared_ptr<FrameBuffer> OpenGLFactory::CreateFrameBuffer( uint32_t width, uint32_t height )
{
	return std::make_shared<OpenGLFrameBuffer>(width, height);
}

//shared_ptr<Texture> OpenGLRenderFactory::CreateTextureFromFile( const std::string& texFileName, uint32_t accessHint )
//{
//	TextureType type;
//
//	nv::Image image;
//
//	if (!image.loadImageFromFile(texFileName.c_str()))
//	{
//		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, "Load Texture Error", "OpenGLRenderFactory::CreateTextureFromFile");
//	}
//
//	int32_t numMipmaps = image.getMipLevels();
//	int32_t imageWidth = image.getWidth();
//	int32_t imageHeight = image.getHeight();
//	int32_t imageDepth = image.getDepth();
//	int32_t numCubeFaces = image.getFaces();
//
//	bool isCompressed = image.isCompressed();
//	bool isCubeMap = image.isCubeMap();
//	bool isVolume = image.isVolume();
//
//
//	if( isCubeMap )
//	{
//		type = TT_TextureCube;
//	}
//	else if (isVolume && imageDepth>0)
//	{
//		type = TT_Texture3D;
//	}
//	else if(imageHeight>0 && imageWidth>0)
//	{
//		type = TT_Texture2D;
//	}else if(imageWidth>0 && imageHeight==0)
//	{
//		type = TT_Texture1D;
//	}else
//	{
//		assert(false);
//	}
//
//
//	PixelFormat format;
//	format = OpenGLMapping::UnMapping(image.getInternalFormat(), image.getFormat(), image.getType());
//
//	uint32_t elementSize;
//	elementSize = isCubeMap ? (6*numMipmaps) : numMipmaps;
//	std::vector<ElementInitData> imageData(elementSize);
//
//	if(!isCubeMap)
//	{
//		for (int32_t level = 0; level < numMipmaps; ++level)
//		{
//			imageData[level].pData = image.getLevel(level);
//			imageData[level].rowPitch = image.getImageSize(level);
//			imageData[level].slicePitch = 0;
//		}
//	}
//	else
//	{
//		// A texture cube is a 2D texture array that contains 6 textures, one for each face of the cube
//		for(uint32_t face = 0; face < 6; ++face)
//		{
//			for (int32_t level = 0; level < numMipmaps; ++level)
//			{
//				imageData[face*numMipmaps+level].pData = image.getLevel(level, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face);
//				imageData[face*numMipmaps+level].rowPitch = image.getImageSize(level);
//				imageData[face*numMipmaps+level].slicePitch = 0;
//			}
//		}
//	}
//
//
//	switch(type)
//	{
//	case TT_Texture1D:
//		return CreateTexture1D(imageWidth, format, 1, numMipmaps, 1, 0, accessHint, &imageData[0]);
//	case TT_Texture2D:
//		return CreateTexture2D(imageWidth, imageHeight, format, 1, numMipmaps, 1, 0, accessHint, &imageData[0]);
//	case TT_Texture3D:
//		return CreateTexture3D(imageWidth, imageHeight, imageDepth, format, 1, numMipmaps, 1, 0, accessHint, &imageData[0]);
//	case TT_TextureCube:
//		return CreateTextureCube(imageWidth, imageHeight, format, 1, numMipmaps, 1, 0, accessHint, &imageData[0]);
//	};
//
//	ENGINE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED, "Unsupported Texture Format", "OpenGLRenderFactory::CreateTextureFromFile");
//}
//
//shared_ptr<Texture> OpenGLRenderFactory::CreateTextureArrayFromFile( const vector<String>& textures, uint32_t accessHint /*= 0*/ )
//{
//	TextureType textureArrayType;
//	uint32_t textureWidth;
//	uint32_t textureHeight;
//	uint32_t textureMipMaps;
//	PixelFormat textureFormat;
//
//	nv::Image image;
//
//	vector<ElementInitData> imageData;
//
//	size_t arraySize = textures.size();
//	for (size_t arrIndex = 0; arrIndex < textures.size(); ++arrIndex)
//	{
//		if (!image.loadImageFromFile(textures[arrIndex].c_str()))
//		{
//			ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, "Load texture error: " + textures[arrIndex] + "doesn't exit.",
//				"OpenGLRenderFactory::CreateTextureFromFile");
//		}
//
//		int32_t numMipmaps = image.getMipLevels();
//		int32_t imageWidth = image.getWidth();
//		int32_t imageHeight = image.getHeight();
//		int32_t imageDepth = image.getDepth();
//		int32_t numCubeFaces = image.getFaces();
//
//		bool isCompressed = image.isCompressed();
//		bool isCubeMap = image.isCubeMap();
//		bool isVolume = image.isVolume();
//
//		TextureType type;
//		if( isCubeMap )
//		{
//			type = TT_TextureCube;
//		}
//		else if (isVolume && imageDepth>0)
//		{
//			type = TT_Texture3D;
//		}
//		else if(imageHeight>0 && imageWidth>0)
//		{
//			type = TT_Texture2D;
//		}else if(imageWidth>0 && imageHeight==0)
//		{
//			type = TT_Texture1D;
//		}else
//		{
//			assert(false);
//		}
//
//		PixelFormat format;
//		format = OpenGLMapping::UnMapping(image.getInternalFormat(), image.getFormat(), image.getType());
//
//		// all texture in texture array must have the same data format and dimensions (including mipmap levels)
//		if ( arrIndex == 0)
//		{
//			textureArrayType = type;
//			textureWidth = imageWidth;
//			textureHeight = imageHeight;
//			textureMipMaps = numMipmaps;
//			textureFormat = format;
//
//			uint32_t elementSize = isCubeMap ? (6*numMipmaps*arraySize) : numMipmaps*arraySize;
//			imageData.resize(elementSize);
//		}
//		else
//		{
//			if (textureArrayType != type || textureWidth != imageWidth ||
//				textureHeight != imageHeight || textureMipMaps != numMipmaps || textureFormat != format)
//			{
//				ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Texture array must created with the same texture type",
//					"OpenGLRenderFactory::CreateTextureFromFile");
//			}
//		}
//
//		if(!isCubeMap)
//		{
//			for (int32_t level = 0; level < numMipmaps; ++level)
//			{
//				uint32_t imageIndex = arrIndex * textureMipMaps + level;
//				imageData[imageIndex].pData = image.getLevel(level);
//				imageData[imageIndex].rowPitch = image.getImageSize(level);
//				imageData[imageIndex].slicePitch = 0;
//			}
//		}
//		else
//		{
//			// A texture cube is a 2D texture array that contains 6 textures, one for each face of the cube
//			for(uint32_t face = 0; face < 6; ++face)
//			{
//				for (int32_t level = 0; level < numMipmaps; ++level)
//				{
//					uint32_t imageIndex =  arrIndex*textureMipMaps*6 + face*textureMipMaps + level;
//					imageData[imageIndex].pData = image.getLevel(level, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face);
//					imageData[imageIndex].rowPitch = image.getImageSize(level);
//					imageData[imageIndex].slicePitch = 0;
//				}
//			}
//		}
//	}
//
//	switch(textureArrayType)
//	{
//	case TT_Texture1D:
//		return CreateTexture1D(textureWidth, textureFormat, arraySize, textureMipMaps, 0, 1, accessHint, &imageData[0]);
//	case TT_Texture2D:
//		return CreateTexture2D(textureWidth, textureHeight, textureFormat, arraySize, textureMipMaps, 0, 1, accessHint, &imageData[0]);
//	case TT_TextureCube:
//		return CreateTextureCube(textureWidth, textureHeight, textureFormat, arraySize, textureMipMaps, 0, 1, accessHint, &imageData[0]);
//	default:
//		ENGINE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED, "Unsupported Texture Format", "OpenGLRenderFactory::CreateTextureFromFile");
//	};
//}
//
//void ExportToPfm(const String& filename, uint32_t width, uint32_t height, PixelFormat format, void* data)
//{
//	vector<float> temp;
//	temp.resize(width * height * 3);
//	float* temPixel = &temp[0];
//
//	uint8_t* pixel = (uint8_t*)data;
//	
//	if (format == PF_A8R8G8B8)
//	{
//		for (uint32_t j = 0; j < height; j++)
//			for(uint32_t i = 0; i < width; i ++)
//		{	
//			uint8_t b = pixel[(j * width + i)*4 + 0];
//			uint8_t g = pixel[(j * width + i)*4 +1];
//			uint8_t r = pixel[(j * width + i)*4 +2];
//			uint8_t a = pixel[(j * width + i)*4 +3];
//
//			assert(a == 255);
//			temPixel[0] = r / 255.0f;
//			temPixel[1] = g / 255.0f;
//			temPixel[2] = b / 255.0f;
//			temPixel += 3;
//		}
//
//			FILE* file = fopen(filename.c_str(), "wb");
//			assert(file != NULL);
//			fprintf(file, "PF\n%d %d\n-1.0\n", width, height);
//			fwrite(&temp[0],sizeof(float) * temp.size(), 1,file);
//			fclose(file);
//	}
//	else if (format == PF_Luminance8)
//	{
//		for (uint32_t j = 0; j < height; j++)
//			for(uint32_t i = 0; i < width; i ++)
//			{	
//				uint8_t b = pixel[(j * width + i)];
//				uint8_t g = pixel[(j * width + i)];
//				uint8_t r = pixel[(j * width + i)];
//				uint8_t a = pixel[(j * width + i)];
//
//				temPixel[0] = r / 255.0f;
//				temPixel[1] = g / 255.0f;
//				temPixel[2] = b / 255.0f;
//				temPixel += 3;
//			}
//
//			FILE* file = fopen(filename.c_str(), "wb");
//			assert(file != NULL);
//			fprintf(file, "PF\n%d %d\n-1.0\n", width, height);
//			fwrite(&temp[0],sizeof(float) * temp.size(), 1,file);
//			fclose(file);
//	}
//}
//
//void OpenGLRenderFactory::SaveTexture2D( const String& texFile, const shared_ptr<Texture>& texture, uint32_t arrayIndex, uint32_t level)
//{
//	assert(texture->GetTextureType() == TT_Texture2D);
//
//	uint32_t w = texture->GetWidth(level);
//	uint32_t h = texture->GetHeight(level);
//
//	if (texture->GetTextureFormat() == PF_A8R8G8B8)
//	{
//		void* pData;
//		uint32_t rowPitch;
//		texture->Map2D(arrayIndex, level, TMA_Read_Only, 0, 0, 0, 0, pData, rowPitch);
//
//		uint8_t* pixel = (uint8_t*)pData;
//		vector<Pixel32> imageData(w*h);
//		for (uint32_t j = 0; j < h; j++)
//			for(uint32_t i = 0; i < w; i ++)
//			{
//				uint8_t b = pixel[((h-j -1) * w + i)*4 + 0];
//				uint8_t g = pixel[((h-j-1) * w + i)*4 +1];
//				uint8_t r = pixel[((h-j-1) * w + i)*4 +2];
//				uint8_t a = pixel[((h-j-1) * w + i)*4 +3];
//
//				imageData[j*w+i].r = r;
//				imageData[j*w+i].g = g;
//				imageData[j*w+i].b = b;
//				imageData[j*w+i].a = a;
//			}
//
//			WriteTGA(texFile.c_str(), &imageData[0], w, h);
//			texture->Unmap2D(arrayIndex, level);
//	}
//	else if (texture->GetTextureFormat() == PF_A8B8G8R8)
//	{
//		void* pData;
//		uint32_t rowPitch;
//		texture->Map2D(arrayIndex, level, TMA_Read_Only, 0, 0, 0, 0, pData, rowPitch);
//
//		uint8_t* pixel = (uint8_t*)pData;
//		vector<Pixel32> imageData(w*h);
//		for (uint32_t j = 0; j < h; j++)
//			for(uint32_t i = 0; i < w; i ++)
//			{
//				uint8_t r = pixel[((h-j -1) * w + i)*4 + 0];
//				uint8_t g = pixel[((h-j-1) * w + i)*4 +1];
//				uint8_t b = pixel[((h-j-1) * w + i)*4 +2];
//				uint8_t a = pixel[((h-j-1) * w + i)*4 +3];
//
//				imageData[j*w+i].r = r;
//				imageData[j*w+i].g = g;
//				imageData[j*w+i].b = b;
//				imageData[j*w+i].a = a;
//			}
//
//			WriteTGA(texFile.c_str(), &imageData[0], w, h);
//			texture->Unmap2D(arrayIndex, level);
//	}
//	else if (texture->GetTextureFormat() == PF_R8G8B8)
//	{
//		void* pData;
//		uint32_t rowPitch;
//		texture->Map2D(arrayIndex, level, TMA_Read_Only, 0, 0, 0, 0, pData, rowPitch);
//
//		uint8_t* pixel = (uint8_t*)pData;
//		vector<Pixel32> imageData(w*h);
//		for (uint32_t j = 0; j < h; j++)
//			for(uint32_t i = 0; i < w; i ++)
//			{
//				uint8_t b = pixel[((h-j -1) * w + i)*3 + 0];
//				uint8_t g = pixel[((h-j-1) * w + i)*3 +1];
//				uint8_t r = pixel[((h-j-1) * w + i)*3 +2];
//
//				imageData[j*w+i].r = r;
//				imageData[j*w+i].g = g;
//				imageData[j*w+i].b = b;
//				imageData[j*w+i].a = 255;
//			}
//
//			WriteTGA(texFile.c_str(), &imageData[0], w, h);
//			texture->Unmap2D(arrayIndex, level);
//	}
//	else if (texture->GetTextureFormat() == PF_Alpha8)
//	{
//		void* pData;
//		uint32_t rowPitch;
//		texture->Map2D(arrayIndex, level, TMA_Read_Only, 0, 0, 0, 0, pData, rowPitch);
//
//		uint8_t* pixel = (uint8_t*)pData;
//		vector<Pixel32> imageData(w*h);
//		for (uint32_t j = 0; j < h; j++)
//			for(uint32_t i = 0; i < w; i ++)
//			{
//				uint8_t b = pixel[((j) * w + i)];
//				uint8_t g = pixel[((j) * w + i)];
//				uint8_t r = pixel[((j) * w + i)];
//				uint8_t a = 255;
//
//				imageData[j*w+i].r = r;
//				imageData[j*w+i].g = g;
//				imageData[j*w+i].b = b;
//				imageData[j*w+i].a = a;
//			}
//
//			WriteTGA(texFile.c_str(), &imageData[0], w, h);
//			texture->Unmap2D(arrayIndex, level);
//	}
//	else if (texture->GetTextureFormat() == PF_A32B32G32R32F)
//	{
//		void* pData;
//		uint32_t rowPitch;
//		texture->Map2D(arrayIndex, level, TMA_Read_Only, 0, 0, 0, 0, pData, rowPitch);
//
//		float* pixel = (float*)pData;
//
//		vector<float> temp;
//		temp.resize(w * h * 3);
//		float* imageData = &temp[0];
//		for (uint32_t j = 0; j < h; j++)
//		for(uint32_t i = 0; i < w; i ++)
//		{
//			float r = pixel[(j * w + i)*4 + 0];
//			float g = pixel[(j * w + i)*4 +1];
//			float b = pixel[(j * w + i)*4 +2];
//			float a = pixel[(j * w + i)*4 +3];
//
//			*imageData++ = r;
//			*imageData++ = g;
//			*imageData++ = b;
//		}
//
//		WritePfm(texFile.c_str(), w, h, 3, &temp[0]);
//		texture->Unmap2D(arrayIndex, level);
//		
//	}
//	else if (texture->GetTextureFormat() == PF_Depth32 || texture->GetTextureFormat() == PF_R32F)
//	{
//		void* pData;
//		uint32_t rowPitch;
//		texture->Map2D(arrayIndex, level, TMA_Read_Only, 0, 0, 0, 0, pData, rowPitch);
//
//		float* pixel = (float*)pData;
//
//		vector<float> temp;
//		temp.resize(w * h);
//		float* imageData = &temp[0];
//		for (uint32_t j = 0; j < h; j++)
//		for(uint32_t i = 0; i < w; i ++)
//		{
//			float r = pixel[(j * w + i)];
//			*imageData++ = r;
//		}
//
//		WritePfm(texFile.c_str(), w, h, 1, &temp[0]);
//		texture->Unmap2D(arrayIndex, level);
//	}
//	else if (texture->GetTextureFormat() == PF_G32R32F)
//	{
//		void* pData;
//		uint32_t rowPitch;
//		texture->Map2D(arrayIndex, level, TMA_Read_Only, 0, 0, 0, 0, pData, rowPitch);
//
//		float* pixel = (float*)pData;
//
//		vector<float> temp;
//		temp.resize(w * h);
//		float* imageData = &temp[0];
//
//		for (uint32_t j = 0; j < h; j++)
//			for(uint32_t i = 0; i < w; i ++)
//			{
//				float r = pixel[(j * w + i)*2+0];
//				float g = pixel[(j * w + i)*2+1];
//
//				*imageData++ = r;
//			}
//			
//		WritePfm(texFile.c_str(), w, h, 1, &temp[0]);
//		texture->Unmap2D(arrayIndex, level);
//	}
//}
//





}



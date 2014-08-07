#include <Graphics/Image.h>
#include <Graphics/GraphicsResource.h>
#include <MainApp/Application.h>
#include <Core/Exception.h>
#include <fstream>

namespace RcEngine {

Image::Image()
	: mValid(false)
{

}

Image::~Image()
{
	if (mValid)
		Clear();
}


void Image::Clear()
{
	if (mValid)
	{
		uint8_t* pImageDataBase = (uint8_t*) mSurfaces.front().pData;

		delete[] pImageDataBase;
		mSurfaces.clear();
	}

	mValid = false;
}

uint32_t Image::GetRowPitch( uint32_t level )
{
	uint32_t index = (mType == TT_TextureCube) ? (level*6) : level;
	return mSurfaces[index].RowPitch;
}

uint32_t Image::GetSlicePitch( uint32_t level )
{
	assert(mType == TT_Texture3D);
	return mSurfaces[level].SlicePitch;
}

uint32_t Image::GetSurfaceSize( uint32_t level )
{
	return 0;
}

const void* Image::GetLevel( uint32_t level, uint32_t layer /*= 0*/, CubeMapFace face /*= CMF_PositiveX*/ ) const
{
	uint32_t index;

	if (mType == TT_TextureCube)
		index = layer * mLevels * CMF_Count + face * mLevels + level;
	else
		index = layer * mLevels + level;

	return mSurfaces[index].pData;
}

void* Image::GetLevel( uint32_t level, uint32_t layer /*= 0*/, CubeMapFace face /*= CMF_PositiveX*/ )
{
	uint32_t index;

	if (mType == TT_TextureCube)
		index = layer * mLevels * CMF_Count + face * mLevels + level;
	else
		index = layer * mLevels + level;

	return mSurfaces[index].pData;
}

bool Image::CopyImageFromTexture( const shared_ptr<Texture>& texture )
{
	Clear();

	mType = texture->GetTextureType();
	mFormat = texture->GetTextureFormat();
	mWidth = texture->GetWidth();
	mHeight = texture->GetHeight();
	mDepth = texture->GetDepth();
	mLevels= texture->GetMipLevels();
	mLayers = texture->GetTextureArraySize();

	uint32_t totalSize;
	uint8_t* pImageData;

	// Compute total size
	if (PixelFormatUtils::IsCompressed(mFormat))
	{
		ENGINE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Unimplement to save Compressed texture", "Image::CopyImageFromTexture");
	}
	else
	{
		totalSize = mWidth * mHeight * mLevels * mLayers * PixelFormatUtils::GetNumElemBytes(mFormat);

		if (mType == TT_TextureCube)
			totalSize *= 6;
		else if (mType == TT_TextureCube)
			totalSize *= mDepth;

		pImageData = new uint8_t[totalSize];
	}

	switch (mType)
	{
	case TT_Texture2D:
		{
			uint8_t* pSrcBits = pImageData;
			uint8_t* ppEndBits = pSrcBits + totalSize;

			uint32_t rowPitch, rowPitchPad;
			uint8_t* pLevelData;

			for (uint32_t layer = 0; layer < mLayers; ++layer)
			{
				for (uint32_t level = 0; level < mLevels; ++level)
				{
					uint32_t levelWidth = (std::max)(1U, mWidth>>level) ;
					uint32_t levelHeight = (std::max)(1U, mHeight>>level);
					
					rowPitch = levelWidth * PixelFormatUtils::GetNumElemBytes(mFormat);
					pLevelData = (uint8_t*)texture->Map2D(layer, level, RMA_Read_Only, rowPitchPad);
					
					SurfaceInfo surface = { pSrcBits, rowPitch, 0 };
					mSurfaces.push_back(surface);

					for (uint32_t i = 0; i < levelHeight; ++i)
					{
						memcpy(pSrcBits, pLevelData, rowPitch);

						pSrcBits += rowPitch;
						pLevelData += rowPitchPad;
					}

					texture->Unmap2D(layer, level);
				}
			}

			assert(pSrcBits == ppEndBits);
		}
		break;
	case TT_Texture1D:
	case TT_Texture3D:
	case TT_TextureCube:
	default:
		break;
	}

	mValid = true;
	return true;
}

namespace {

union Pixel32
{
	Pixel32() : integer(0) { }
	Pixel32(uint8_t bi, uint8_t gi, uint8_t ri, uint8_t ai = 255)
	{
		b = bi;
		g = gi;
		r = ri;
		a = ai;
	}

	uint8_t integer;

	struct
	{
#ifdef BIG_ENDIAN
		uint8_t a, r, g, b;
#else // BIG_ENDIAN
		uint8_t b, g, r, a;
#endif // BIG_ENDIAN
	};
};

// TGA Header struct to make it simple to dump a TGA to disc.
#if defined(_MSC_VER) || defined(__GNUC__)
	#pragma pack(push, 1)
	#pragma pack(1)               // Dont pad the following struct.
#endif

struct TGAHeader
{
	uint8_t   idLength,           // Length of optional identification sequence.
		paletteType,        // Is a palette present? (1=yes)
		imageType;          // Image data type (0=none, 1=indexed, 2=rgb,
	// 3=grey, +8=rle packed).
	uint16_t  firstPaletteEntry,  // First palette index, if present.
		numPaletteEntries;  // Number of palette entries, if present.
	uint8_t   paletteBits;        // Number of bits per palette entry.
	uint16_t  x,                  // Horiz. pixel coord. of lower left of image.
		y,                  // Vert. pixel coord. of lower left of image.
		width,              // Image width in pixels.
		height;             // Image height in pixels.
	uint8_t   depth,              // Image color depth (bits per pixel).
		descriptor;         // Image attribute flags.
};

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(pop)
#endif

bool WriteTGA(const std::string &filename, const Pixel32 *pxl, uint16_t width, uint16_t height)
{
	std::ofstream file(filename.c_str(), std::ios::binary);
	if (file)
	{
		TGAHeader header;
		memset(&header, 0, sizeof(TGAHeader));
		header.imageType  = 2;
		header.width = width;
		header.height = height;
		header.depth = 32;
		header.descriptor = 0x20;

		file.write((const char *)&header, sizeof(TGAHeader));
		file.write((const char *)pxl, sizeof(Pixel32) * width * height);

		return true;
	}
	return false;
}

int WritePfm(const char *fn, int resX, int resY, int channels, const float* data)
{
	if(channels!=1&&channels!=3&&channels!=4)
	{
		return -2;
	}
	FILE* f;
	errno_t err=fopen_s(&f,fn,"wb");
	if(err!=0) return -1;
	const char* indicator;	
	switch(channels)
	{
	case 1:
		indicator="Pf";
		break;
	case 3:
		indicator="PF";
		break;
	case 4:
		indicator="P4";
		break;
	default:
		break;
	}
	fprintf_s(f,"%s\n%d %d\n%f\n",indicator,resX,resY,-1.f);		
	int written=fwrite(data,sizeof(float)*channels,resX*resY,f);
	if(written!=resX*resY)
	{
		fclose(f);
		return -3;
	}
	fclose(f);
	return 0;
}

float Float16ToFloat( uint16_t fltInt16 )
{
	uint32_t fltInt32    =  ((fltInt16 & 0x8000) << 16);
	fltInt32        |= ((fltInt16 & 0x7fff) << 13) + 0x38000000;

	float fRet;
	memcpy( &fRet, &fltInt32, sizeof( float ) );
	return fRet;
}

}

void Image::SaveImageToFile( const String& filename )
{
	if (mFormat == PF_RGBA8_UNORM)
	{
		uint8_t* pixel = (uint8_t*)mSurfaces.front().pData;
		uint32_t w = mWidth, h = mHeight;

		vector<Pixel32> imageData(w*h);
		
		if (Application::msApp->GetAppSettings().RHDeviceType != RD_Direct3D11)
		{
			for (uint32_t j = 0; j < h; j++)
				for(uint32_t i = 0; i < w; i ++)
				{
					uint8_t r = pixel[((h-j-1) * w + i)*4 + 0];
					uint8_t g = pixel[((h-j-1) * w + i)*4 + 1];
					uint8_t b = pixel[((h-j-1) * w + i)*4 + 2];
					uint8_t a = pixel[((h-j-1) * w + i)*4 + 3];

					imageData[j*w+i].r = r;
					imageData[j*w+i].g = g;
					imageData[j*w+i].b = b;
					imageData[j*w+i].a = a;
				}
		}
		else
		{
			for (uint32_t j = 0; j < h; j++)
				for(uint32_t i = 0; i < w; i ++)
				{
					uint8_t r = pixel[(j * w + i)*4 + 0];
					uint8_t g = pixel[(j * w + i)*4 + 1];
					uint8_t b = pixel[(j * w + i)*4 + 2];
					uint8_t a = pixel[(j * w + i)*4 + 3];

					imageData[j*w+i].r = r;
					imageData[j*w+i].g = g;
					imageData[j*w+i].b = b;
					imageData[j*w+i].a = a;
				}
		}
		
		WriteTGA(filename.c_str(), &imageData[0], w, h);
	}
	else if (mFormat == PF_RGBA32F)
	{		
		float* pixel = (float*)mSurfaces.front().pData;

		uint32_t w = mWidth, h = mHeight;

		vector<float> temp;
		temp.resize(w * h * 3);
		float* imageData = &temp[0];

		if (Application::msApp->GetAppSettings().RHDeviceType == RD_Direct3D11)
		{
			for (uint32_t j = 0; j < h; j++)
				for(uint32_t i = 0; i < w; i ++)
				{
					float r = pixel[((h-j-1) * w + i)*4 + 0];
					float g = pixel[((h-j-1) * w + i)*4 + 1];
					float b = pixel[((h-j-1) * w + i)*4 + 2];
					float a = pixel[((h-j-1) * w + i)*4 + 3];

					*imageData++ = r;
					*imageData++ = g;
					*imageData++ = b;
				}
		}
		else
		{
			for (uint32_t j = 0; j < h; j++)
				for(uint32_t i = 0; i < w; i ++)
				{
					float r = pixel[(j * w + i)*4 + 0];
					float g = pixel[(j * w + i)*4 +1];
					float b = pixel[(j * w + i)*4 +2];
					float a = pixel[(j * w + i)*4 +3];

					*imageData++ = r;
					*imageData++ = g;
					*imageData++ = b;
				}
		}

		WritePfm(filename.c_str(), w, h, 3, &temp[0]);
	}
	else if (mFormat == PF_RGBA16F)
	{
		uint16_t* pixel = (uint16_t*)mSurfaces.front().pData;

		uint32_t w = mWidth, h = mHeight;

		vector<float> temp;
		temp.resize(w * h * 3);
		float* imageData = &temp[0];

		if (Application::msApp->GetAppSettings().RHDeviceType == RD_Direct3D11)
		{
			for (uint32_t j = 0; j < h; j++)
				for(uint32_t i = 0; i < w; i ++)
				{
					uint16_t r = pixel[((h-j-1) * w + i)*4 + 0];
					uint16_t g = pixel[((h-j-1) * w + i)*4 + 1];
					uint16_t b = pixel[((h-j-1) * w + i)*4 + 2];
					uint16_t a = pixel[((h-j-1) * w + i)*4 + 3];

					*imageData++ = Float16ToFloat(r);
					*imageData++ = Float16ToFloat(g);
					*imageData++ = Float16ToFloat(b);
				}
		}
		else
		{
			for (uint32_t j = 0; j < h; j++)
				for(uint32_t i = 0; i < w; i ++)
				{
					uint16_t r = pixel[(j * w + i)*4 + 0];
					uint16_t g = pixel[(j * w + i)*4 +1];
					uint16_t b = pixel[(j * w + i)*4 +2];
					uint16_t a = pixel[(j * w + i)*4 +3];

					*imageData++ = Float16ToFloat(r);
					*imageData++ = Float16ToFloat(g);
					*imageData++ = Float16ToFloat(b);
				}
		}

		WritePfm(filename.c_str(), w, h, 3, &temp[0]);
	}
	else if (mFormat == PF_D24S8)
	{
		uint32_t* pixel = (uint32_t*)mSurfaces.front().pData;

		uint32_t w = mWidth, h = mHeight;

		vector<float> temp;
		temp.resize(w * h);
		float* imageData = &temp[0];

		if (Application::msApp->GetAppSettings().RHDeviceType == RD_Direct3D11)
		{
			for (uint32_t j = 0; j < h; j++)
				for(uint32_t i = 0; i < w; i ++)
				{
					uint32_t color = pixel[(h-j-1) * w + i];

					// Extract 24 depth bits
					float depth = static_cast<float>(color & 0x00FFFFFF); 
					depth /= 16777216.0f; // divide bY 2^24

					uint32_t stencil = color >> 24;

					*imageData++ = depth;
				}
		}
		else
		{
			for (uint32_t j = 0; j < h; j++)
				for(uint32_t i = 0; i < w; i ++)
				{
					uint32_t color = pixel[j * w + i];

					/**
					 * Unlike D3D11, OpenGL use LBS 8 bit as stencil.
					 * Need more check
					 */
					uint32_t stencil = (color & 255);

					// Extract 24 depth bits
					float depth = static_cast<float>(color >> 8); 
					depth /= 16777216.0f; // divide bY 2^24

					*imageData++ = depth;
				}
		}

		WritePfm(filename.c_str(), w, h, 1, &temp[0]);
	}
}

void Image::SaveLinearDepthToFile( const String& filename, float projM33, float projM43 )
{
	if (mFormat == PF_D24S8)
	{
		uint32_t* pixel = (uint32_t*)mSurfaces.front().pData;

		uint32_t w = mWidth, h = mHeight;

		vector<float> temp;
		temp.resize(w * h * 2); // for depth and stencil
		float* pDepth = &temp[0];
		float* pStencil = &temp[w*h];

		if (Application::msApp->GetAppSettings().RHDeviceType == RD_Direct3D11)
		{
			for (uint32_t j = 0; j < h; j++)
				for(uint32_t i = 0; i < w; i ++)
				{
					uint32_t color = pixel[(h-j-1) * w + i];

					// Extract 24 depth bits
					float depth = static_cast<float>(color & 0x00FFFFFF); 
					depth /= 16777216.0f; // divide bY 2^24

					uint32_t stencil = color >> 24;

					*pDepth++ = (1.0f - projM33) / (depth - projM33);
					*pStencil++ = float(stencil);
				}
		}
		else
		{
			for (uint32_t j = 0; j < h; j++)
				for(uint32_t i = 0; i < w; i ++)
				{
					uint32_t color = pixel[j * w + i];

					/**
					 * Unlike D3D11, OpenGL use LBS 8 bit as stencil.
					 * Need more check
					 */
					uint32_t stencil = (color & 255);

					// Extract 24 depth bits
					float depth = static_cast<float>(color >> 8); 
					depth /= 16777216.0f; // divide bY 2^24

					*pDepth++ = (1.0f - projM33) / (depth - projM33);
					*pStencil++ = float(stencil);
				}
		}

		WritePfm(filename.c_str(), w, h, 1, &temp[0]);
		WritePfm("E:/Stencil.pfm", w, h, 1, &temp[w*h]);
	}
}


}
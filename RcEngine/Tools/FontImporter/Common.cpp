#include "Common.h"

using namespace std;

namespace {

	union Pixel32
	{
		Pixel32()
			: integer(0) { }
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

	bool
		WriteTGA(const std::string &filename,
		const Pixel32 *pxl,
		uint16_t width,
		uint16_t height)
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

}
	void SaveTGA(int32_t w, int32_t h, uint8_t* pData)
	{
		uint8_t* pixel = pData;
		vector<Pixel32> imageData(w*h);


		for (int32_t j = 0; j < h; j++)
			for(int32_t i = 0; i < w; i ++)
			{
				uint8_t b = pixel[((j) * w + i)];
				uint8_t g = pixel[((j) * w + i)];
				uint8_t r = pixel[((j) * w + i)];
				uint8_t a = 255;

				imageData[j*w+i].r = r;
				imageData[j*w+i].g = g;
				imageData[j*w+i].b = b;
				imageData[j*w+i].a = a;
			}

			WriteTGA("char.tga", &imageData[0], w, h);
	}

	void SaveTexture2D(const char* file, GLuint texID, int32_t width, int32_t height)
	{
		vector<uint8_t> TextureData(width * height);

		glBindTexture(GL_TEXTURE_2D, texID);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_ALPHA, GL_UNSIGNED_BYTE, &TextureData[0]);

		uint8_t* pixel = (uint8_t*)&TextureData[0];
		vector<Pixel32> imageData(width*height);

		auto h = height;
		auto w = width;

		for (int32_t j = 0; j < h; j++)
			for(int32_t i = 0; i < w; i ++)
			{
				uint8_t b = pixel[((j) * w + i)];
				uint8_t g = pixel[((j) * w + i)];
				uint8_t r = pixel[((j) * w + i)];
				uint8_t a = 255;

				imageData[j*w+i].r = r;
				imageData[j*w+i].g = g;
				imageData[j*w+i].b = b;
				imageData[j*w+i].a = a;
			}

			WriteTGA(file, &imageData[0], w, h);
	}

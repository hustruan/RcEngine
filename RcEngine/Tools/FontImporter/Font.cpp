#include "Font.h"
#include <algorithm>
#include <cmath>
#include <bitset>

static const int FONT_TEXTURE_SIZE = 1024;
static const int FONT_DPI = 96;
static const int CHAR_GLYPH_SIZE = 4096;

namespace DistanceField {

void ComputeDistanceField( uint8_t* srcImage, int32_t srcWidth, int32_t srcHeight, int32_t pitch, int32_t size, int32_t upScale, int32_t spread, vector<uint8_t>& outImage, int32_t* outWidth, int32_t* outHeight )
{
	const int32_t width = size / upScale;
	const int32_t height = size / upScale;

	outImage.resize(width*height);

	// copy image from freetype to buffer

	//  ÂÖÀªÖÜÎ§×î´ó·¶Î§¾àÀë
	const int32_t maxDist = upScale * spread;
	
	for (int32_t h = 0; h < height; ++h)
	{
		for (int32_t w = 0; w < width; ++w)
		{
			int32_t srcPtX = w * upScale + upScale / 2;
			int32_t srcPtY = h * upScale + upScale / 2;	

			srcPtX = (std::min)(srcWidth-1, srcPtX); 
			srcPtY = (std::min)(srcHeight-1, srcPtY); 

			uint8_t baseValue = srcImage[srcPtY * pitch +  srcPtX];

			int32_t startX = (std::max)(0, srcPtX - maxDist); 
			int32_t endX = (std::min)(srcWidth-1, srcPtX + maxDist); 
			int32_t startY = (std::max)(0, srcPtY - maxDist); 
			int32_t endY = (std::min)(srcHeight-1, srcPtY + maxDist); 

			int32_t closestDistSquared = maxDist * maxDist;

			for (int32_t y = startY; y <= endY; ++y)
			{
				for (int32_t x = startX; x <= endX; ++x)
				{		
					uint8_t testValue = srcImage[y * pitch +  x];

					if( baseValue != testValue )
					{
						int32_t squaredDist = (srcPtX - x) * (srcPtX - x) +  (srcPtY - y) * (srcPtY - y);
						if (squaredDist < closestDistSquared)
						{
							closestDistSquared = squaredDist;
						}
					}
				}
			}

			float closestDist = (float) sqrtf((float)closestDistSquared);
			closestDist = (baseValue ? 1 : -1) * closestDist / upScale;
		
			uint8_t destAlpha = uint8_t(255U * 
				(std::max)(0.0f, (std::min)(1.0f, (0.5f + 0.5f * closestDist / spread)))); // clamp to [0, 1] first

			outImage[h * width + w] = destAlpha;

		}

		printf("%d finished\n", h);
	}

	if (outWidth) *outWidth = width;
	if (outHeight) *outHeight = height;
}

Font::Font( const string&  fontname, int32_t fontSize, const wstring& text)
	: CharSize(fontSize), TextureSize(TextureSize)
{
	CreateFontFace(fontname, fontSize, text);
}

Font::~Font( void )
{
	FT_Done_Face(Face);
	FT_Done_FreeType(Library);
}

void Font::UpdateTexture( const wstring& text )
{
	static int64_t tick = 0;
	tick++;

	FT_Error error;

	const uint32_t numCharPerRow = TextureSize / CharSize;
	const uint32_t totalNumChars = numCharPerRow * numCharPerRow;

	glBindTexture(GL_TEXTURE_2D, mTextureID);

	for (auto charIter = text.begin(); charIter != text.end(); ++charIter)
	{
		wchar_t ch = *charIter;

		auto found = mCharacterCached.find(ch);

		if (found != mCharacterCached.end())
		{
			// already exits in texture, add use tick
			found->second.Tick = tick;
		}
		else
		{
			error = FT_Load_Char(Face, ch, FT_LOAD_DEFAULT);

			FT_GlyphSlot slot = Face->glyph;

			if (!error)
			{
				int32_t charPosX, charPosY;
				CharInfo charInfo;

				if (mCharacterCached.size() < totalNumChars)
				{
					std::pair<int32_t, int32_t>& firstFreeSlots = mFreeCharacterSlots.front();

					const int32_t slot = firstFreeSlots.first;

					charPosY = slot / numCharPerRow;
					charPosX = slot - charPosY * numCharPerRow;

					charPosX *= CharSize;
					charPosY *= CharSize;

					charInfo.u1 = (static_cast<float>(charPosX) / TextureSize);
					charInfo.v1 = (static_cast<float>(charPosY) / TextureSize);

					++ firstFreeSlots.first;
					if (firstFreeSlots.first == firstFreeSlots.second)
					{
						mFreeCharacterSlots.pop_front();
					}
				}
				else
				{
					auto minIter = mCharacterCached.begin();
					uint64_t minTick = minIter->second.Tick;	
					for (auto iter = mCharacterCached.begin(); iter != mCharacterCached.end(); ++iter)
					{
						if (iter->second.Tick < minTick)
						{
							minTick = iter->second.Tick;
							minIter = iter;
						}
					}

					charPosX =  static_cast<int32_t>(minIter->second.u1 * TextureSize);
					charPosY = static_cast<int32_t>(minIter->second.v1 * TextureSize);

					charInfo.u1 = (minIter->second.u1);
					charInfo.v1 = (minIter->second.v1);

					//delete min one
					mCharacterCached.erase(minIter);

					// remove those character
					auto delIter = mCharacterCached.begin();
					while(delIter != mCharacterCached.end())
					{
						if (delIter->second.Tick == minTick)
						{
							// push this character slot to free list
							const int32_t x = static_cast<int32_t>(delIter->second.u1 * numCharPerRow);
							const int32_t y = static_cast<int32_t>(delIter->second.v1 * numCharPerRow);
							const int32_t slotIndex = y * numCharPerRow + x;

							auto slotIter = mFreeCharacterSlots.begin();
							while ((slotIter != mFreeCharacterSlots.end()) && (slotIter->second <= slotIndex))
							{
								++ slotIter;
							}
							mFreeCharacterSlots.insert(slotIter, std::make_pair(slotIndex, slotIndex + 1));	// only one character is freed


							wchar_t decCh = delIter->first;
							mCharacterCached.erase(delIter++);
						}
						else
						{
							++delIter;
						}
					}

					// connect all slot if they are adjacent
					for (auto iter = mFreeCharacterSlots.begin(); iter != mFreeCharacterSlots.end(); )
					{
						auto next = iter; ++next;

						if ( (next != mFreeCharacterSlots.end()) && iter->second == next->first )
						{
							iter->second = next->second;
							mFreeCharacterSlots.erase(next);
						}
						else
						{
							++iter;
						}
					}
				}

				charInfo.u2 = (charInfo.u1 + static_cast<float>(slot->metrics.width >> 6) / TextureSize);
				charInfo.v2 = (charInfo.v1 + static_cast<float>(slot->metrics.height >> 6) / TextureSize);
				charInfo.Tick = tick;

				// add new GlyphMapping if doesn't exit
				if (GlyphMapping.find(ch) == GlyphMapping.end())
				{
					FontGlyph newGlyph;
					newGlyph.Advance = slot->advance.x >> 6;
					newGlyph.Width = slot->metrics.width >> 6;
					newGlyph.Height = slot->metrics.height >> 6;
					newGlyph.OffsetX = slot->metrics.horiBearingX >> 6;
					newGlyph.OffsetY = slot->metrics.horiBearingY >> 6;

					GlyphMapping.insert(make_pair((unsigned)ch, Glyphs.size()));
					Glyphs.push_back(newGlyph);
				}

				// cache this character
				mCharacterCached.insert(std::make_pair(ch, charInfo));

				// convert into bitmap
				error = FT_Render_Glyph( slot,  FT_RENDER_MODE_NORMAL );
				if (!error)
				{
					memset(&FontCharData[0], 0, FontCharData.size());
					for (int32_t y = 0; y < slot->bitmap.rows; ++y)
					{
						unsigned char* src = slot->bitmap.buffer + slot->bitmap.pitch * y;
						unsigned char* dest = &FontCharData[0] + CharSize * y;

						for (int32_t x = 0; x < slot->bitmap.width; ++x)
						{
							dest[x] = src[x];
						}
					}

					SaveTGA(CharSize, CharSize, &FontCharData[0]);
					glTexSubImage2D(GL_TEXTURE_2D, 0, charPosX, charPosY, CharSize, CharSize, 
						GL_ALPHA, GL_UNSIGNED_BYTE, &FontCharData[0]);					
				}	
			}
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Font::CreateFontFace( const string& fontName, int32_t fontSize, const wstring& text )
{
	FT_Error error;

	if (FT_Init_FreeType( &Library )) 
	{
		throw std::runtime_error("FT_Init_FreeType failed");
	}

	error = FT_New_Face( Library, fontName.c_str(), 0, &Face );
	if (error)
	{
		throw std::runtime_error("Could not create font face");
	}

	// build high resolution image used to generate distance field
	error = FT_Set_Pixel_Sizes(Face, 0, CHAR_GLYPH_SIZE);

	if (error)
	{
		FT_Done_Face(Face);
		throw std::runtime_error("Could not set font point size");
	}

	// Set character encoding
	FT_Select_Charmap(Face,FT_ENCODING_UNICODE);

	RowHeight = static_cast<int>(float(Face->height >> 6) / CHAR_GLYPH_SIZE * CharSize);
	TextureSize = FONT_TEXTURE_SIZE / CharSize * CharSize;

	// Create font texture
	glGenTextures(1,&mTextureID);
	glBindTexture(GL_TEXTURE_2D,mTextureID);

	glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, TextureSize, TextureSize, 0,
		GL_ALPHA,GL_UNSIGNED_BYTE,NULL);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	/*glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );*/

	FontCharData.resize(CharSize * CharSize);

	mFreeCharacterSlots.push_back(std::make_pair(0, TextureSize * TextureSize / CharSize / CharSize));

	BuildFontTexture(text);

	/*SaveTexture2D("Distance Field Dat")*/

	//UpdateTexture(text);
}

const FontGlyph& Font::GetGlyph( unsigned c ) const
{
	auto found = GlyphMapping.find(c);
	if (found != GlyphMapping.end())
	{
		return Glyphs[found->second];
	}
	else
		throw std::exception("Glyph doesn't exit");
}

void Font::DrawText( const std::wstring& text, int sx, int sy, int32_t fontSize)
{
	//UpdateTexture(text);

	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glActiveTexture(GL_TEXTURE0);

	float x = sx;
	float y = sy;

	const float scale = float(fontSize) / float(CharSize);

	glColor3f(1, 0, 0);
	glBegin ( GL_QUADS );

	for (auto iter = text.begin(); iter != text.end(); ++iter)
	{
		wchar_t ch = *iter;

		if (ch == L'\n')
		{
			y += RowHeight * scale;
			x = sx * scale;
		}
		else
		{
			const FontGlyph& glyph = Glyphs[GlyphMapping[ch]];

			const CharInfo& charInfo = mCharacterCached[ch];

			int ch_x = x + glyph.OffsetX * scale;
			int ch_y = y - glyph.OffsetY * scale;

			// top left
			glTexCoord2f(charInfo.u1, charInfo.v1); glVertex3f(ch_x, ch_y,  1.0f);

			// top right
			glTexCoord2f(charInfo.u2, charInfo.v1); glVertex3f(ch_x + glyph.Width * scale, ch_y    ,  1.0f);

			// bottom right
			glTexCoord2f(charInfo.u2, charInfo.v2);  glVertex3f(ch_x + glyph.Width* scale , ch_y + glyph.Height* scale,  1.0f);

			// bottom left
			glTexCoord2f(charInfo.u1, charInfo.v2); glVertex3f(ch_x, ch_y + glyph.Height* scale,  1.0f);

			x += glyph.Advance * scale;
		}
	}

	glEnd();
}

void Font::BuildFontTexture( const wstring& text )
{
	FT_Error error;

	const uint32_t numCharPerRow = TextureSize / CharSize;
	const uint32_t totalNumChars = numCharPerRow * numCharPerRow;

	glBindTexture(GL_TEXTURE_2D, mTextureID);

	for (auto charIter = text.begin(); charIter != text.end(); ++charIter)
	{
		wchar_t ch = *charIter;

		auto found = mCharacterCached.find(ch);

		if (found == mCharacterCached.end())
		{
			error = FT_Load_Char(Face, ch, FT_LOAD_DEFAULT);

			FT_GlyphSlot slot = Face->glyph;

			if (!error)
			{
				int32_t charPosX, charPosY;
				CharInfo charInfo;

				if (mCharacterCached.size() < totalNumChars)
				{
					std::pair<int32_t, int32_t>& firstFreeSlots = mFreeCharacterSlots.front();

					const int32_t slot = firstFreeSlots.first;

					charPosY = slot / numCharPerRow;
					charPosX = slot - charPosY * numCharPerRow;

					charPosX *= CharSize;
					charPosY *= CharSize;

					charInfo.u1 = (static_cast<float>(charPosX) / TextureSize);
					charInfo.v1 = (static_cast<float>(charPosY) / TextureSize);

					++ firstFreeSlots.first;
					if (firstFreeSlots.first == firstFreeSlots.second)
					{
						mFreeCharacterSlots.pop_front();
					}
				}
				else
				{
					throw std::exception("Char overflow");
				}

				charInfo.u2 = (charInfo.u1 + static_cast<float>(slot->metrics.width >> 6) / CHAR_GLYPH_SIZE * CharSize / TextureSize);
				charInfo.v2 = (charInfo.v1 + static_cast<float>(slot->metrics.height >> 6) / CHAR_GLYPH_SIZE * CharSize / TextureSize);

				// add new GlyphMapping if doesn't exit
				if (GlyphMapping.find(ch) == GlyphMapping.end())
				{
					FontGlyph newGlyph;
					newGlyph.Advance = int32_t(float(slot->advance.x >> 6) / CHAR_GLYPH_SIZE * CharSize);
					newGlyph.Width = int32_t(float(slot->metrics.width >> 6) / CHAR_GLYPH_SIZE * CharSize);
					newGlyph.Height = int32_t(float(slot->metrics.height >> 6) / CHAR_GLYPH_SIZE * CharSize);
					newGlyph.OffsetX = int32_t(float(slot->metrics.horiBearingX >> 6) / CHAR_GLYPH_SIZE * CharSize);
					newGlyph.OffsetY = int32_t(float(slot->metrics.horiBearingY >> 6) / CHAR_GLYPH_SIZE * CharSize);

					GlyphMapping.insert(make_pair((unsigned)ch, Glyphs.size()));
					Glyphs.push_back(newGlyph);
				}

				// cache this character
				mCharacterCached.insert(std::make_pair(ch, charInfo));

				// convert into bitmap
				error = FT_Render_Glyph( slot,  FT_RENDER_MODE_NORMAL );
				FontCharData.resize(CHAR_GLYPH_SIZE*CHAR_GLYPH_SIZE);
				if (!error)
				{
					memset(&FontCharData[0], 0, FontCharData.size());
					int32_t width, height;
					//ComputeDistanceField(slot->bitmap.buffer, slot->bitmap.width, slot->bitmap.rows, slot->bitmap.pitch,
					//	CHAR_GLYPH_SIZE, 128, 4, FontCharData, &width, &height);

					for (int32_t y = 0; y < slot->bitmap.rows; ++y)
					{
						unsigned char* src = slot->bitmap.buffer + slot->bitmap.pitch * y;
						unsigned char* dest = &FontCharData[0] + CHAR_GLYPH_SIZE * y;

						for (int32_t x = 0; x < slot->bitmap.width; ++x)
						{
							dest[x] = src[x];
						}
					}

					SaveTGA(CHAR_GLYPH_SIZE, CHAR_GLYPH_SIZE, &FontCharData[0]);
					glTexSubImage2D(GL_TEXTURE_2D, 0, charPosX, charPosY, CharSize, CharSize, 
						GL_ALPHA, GL_UNSIGNED_BYTE, &FontCharData[0]);					
				}	
			}
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}


}



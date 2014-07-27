#pragma once
#include "Common.h"

#include <ft2build.h>
#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "freetype/ftoutln.h"
#include "freetype/fttrigon.h"
#include FT_FREETYPE_H

#pragma comment(lib, "freetype.lib")

namespace DistanceField {

using namespace std;

struct FontGlyph
{
	int32_t Width, Height;
	// Glyph X, Y offset from origin.
	int32_t OffsetX, OffsetY;
	// the horizontal advance value for the glyph. 
	int32_t Advance;
};

struct CharInfo
{
	float u1, v1, u2, v2;
	uint64_t Tick;
};

class Font
{
public:
	/// Construct.
	Font(const string& fontname, int32_t fontsize, const wstring& text);

	/// Destruct.
	~Font();

	void CreateFontFace(const string& fontName, int32_t fontSize, const wstring& text);

	void BuildFontTexture(const wstring& text);

	void UpdateTexture(const wstring& text);

	void DrawText(const std::wstring& text, int x, int y, int32_t fontSize);

	/// Return the glyph structure corresponding to a character.
	const FontGlyph& GetGlyph(unsigned c) const;

	/// Return the kerning for a character and the next character.
	short GetKerning(unsigned c, unsigned d) const;

	
	GLuint mTextureID;


private:

	vector<uint8_t> FontCharData;

	/// Glyphs.
	vector<FontGlyph> Glyphs;

	map<unsigned, unsigned> GlyphMapping;

	/// Point size.
	int CharSize;

	/// Row height.
	int RowHeight;

	/// Glyph index mapping.
	map<pair<int32_t, int32_t>, int32_t> Kerning;

	// cached glyph in current texture
	unordered_map<wchar_t, CharInfo> mCharacterCached;

	// free slots in font texture,  [first, second]
	list<std::pair<int32_t, int32_t> > mFreeCharacterSlots;

	FT_Face Face;

	FT_Library Library;

	int32_t TextureSize;

	GLuint mVertexBufferID, mIndexBufferID;
};

}


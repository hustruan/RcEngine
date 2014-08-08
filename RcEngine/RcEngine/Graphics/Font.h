#ifndef Font_h__
#define Font_h__

#include <Core/Prerequisites.h>
#include <Resource/Resource.h>
#include <Math/Rectangle.h>
#include <Math/ColorRGBA.h>
#include <Math/Vector.h>

namespace RcEngine {

class SpriteBatch;
class FontLoader;

enum Alignment
{
	AlignLeft     = 1UL << 0,
	AlignRight    = 1UL << 1,
	AlignHCenter  = 1UL << 2,

	AlignTop      = 1UL << 3,
	AlignBottom   = 1UL << 4,
	AlignVCenter  = 1UL << 5,

	AlignCenter   = AlignHCenter | AlignVCenter
};

class _ApiExport Font : public Resource
{
public:

	struct Glyph
	{
		// Start Position in Texture
		int32_t SrcX, SrcY;

		int32_t Width, Height;

		// Glyph X, Y offset from origin.
		float OffsetX, OffsetY;

		// the horizontal advance value for the glyph. 
		float Advance;	
	};

	typedef std::unordered_map<wchar_t, Glyph> FontMetrics;

public:
	Font(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	~Font();

	void DrawString(SpriteBatch& spriteBatch, const std::wstring& text, float fontSize, const float2& position, const ColorRGBA& color, float layerDepth = 0.0f);
	
	void DrawString(SpriteBatch& spriteBatch, const std::wstring& text, float fontSize, uint32_t alignment, const Rectanglef& region, const ColorRGBA& color, float layerDepth = 0.0f);

	void MeasureString(const std::wstring& text, float fontSize, float* widthOut, float* heightOut);

	inline float GetRowHeight(float yScale = 1.0) const  	{ return mRowHeight * yScale; }
	inline float GetBaseLine(float yScale = 1.0) const	    { return mAscent * yScale; }

	inline float GetFontSize() const								{ return mFontSize; }	
	inline float GetSpaceAdvance() const							{ return mSpaceAdvance; }
	
	inline const shared_ptr<Texture>& GetFontTexture() const		{ return mFontTexture; }

	const FontMetrics& GetFontMetrics() const						{ return mFontMetrics; }
	const Glyph& GetGlyphInfo(wchar_t ch) const;

protected:
	void LoadImpl();
	void UnloadImpl();

	void LoadTXT(const String& fileName);
	void LoadBinary(const String& fileName);

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

protected:

	String mFontName;

	FontMetrics mFontMetrics;

	float mFontSize;

	// maximal font ascender (pixels above the baseline)
	float mAscent;
	// maximal font descender (negative pixels below the baseline)
	float mDescent;
	// Row height. (ascender - descender) 
	float mRowHeight;

	float mSpaceAdvance;

	shared_ptr<Texture> mFontTexture;
};


}


#endif // Font_h__

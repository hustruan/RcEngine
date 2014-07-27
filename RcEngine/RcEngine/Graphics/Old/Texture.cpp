#include <Graphics/Texture.h>
#include <Graphics/RenderFactory.h>
#include <Resource/ResourceManager.h>
#include <IO/FileSystem.h>
#include <IO/FileStream.h>
#include <Core/Context.h>

namespace RcEngine {

Texture::Texture( TextureType type, PixelFormat format, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint )
	: mType(type),
	  mFormat(format),
	  mWidth(0),
	  mHeight(0),
	  mDepth(0),
	  mSampleCount(sampleCount),
	  mSampleQuality(sampleQuality),
	  mAccessHint(accessHint)
{

}

Texture::~Texture()
{

}

uint32_t Texture::CalculateMipmapLevels( uint32_t n )
{
	//return int(ceil( log( float(_n) ) / log( 2.f ) )) + 1;
	//return uint32_t(1.0 + floor(log(float(n)/log(2.f))));

	uint32_t levels = 1;
	while (n >> levels) levels++;
	return levels;
}

TextureResource::TextureResource(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(RT_Texture, creator, handle, name, group), mTexture(nullptr)
{

}

TextureResource::~TextureResource()
{

}

void TextureResource::LoadImpl()
{
	FileSystem& fileSystem = FileSystem::GetSingleton();
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	String fullPath = fileSystem.Locate(mResourceName, mGroup);

	mTexture = factory.CreateTextureFromFile(fullPath);
}

void TextureResource::UnloadImpl()
{

}

shared_ptr<Resource> TextureResource::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	return std::make_shared<TextureResource>(creator, handle, name, group);
}

void _ApiExport SaveTexture( const shared_ptr<Texture> texture, const String& texName )
{

}


} // Namespace RcEngine
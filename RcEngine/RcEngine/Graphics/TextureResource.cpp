#include <Graphics/TextureResource.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/RenderFactory.h>
#include <Core/Environment.h>
#include <IO/FileSystem.h>

namespace RcEngine {

TextureResource::TextureResource( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(RT_Texture, creator, handle, name, group)
{

}

void TextureResource::LoadImpl()
{
	String fullPath = FileSystem::GetSingleton().Locate(mResourceName, mGroup);

	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();
	mTexture = factory->LoadTextureFromFile(fullPath);
}

void TextureResource::UnloadImpl()
{

}

shared_ptr<Resource> RcEngine::TextureResource::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	return std::make_shared<TextureResource>(creator, handle, name, group);
}

}


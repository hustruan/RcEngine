#ifndef TextureResource_h__
#define TextureResource_h__

#include <Resource/Resource.h>

namespace RcEngine {

class _ApiExport TextureResource : public Resource
{
public:
	TextureResource(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

	inline const shared_ptr<Texture>& GetTexture() const { return mTexture; }

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

protected:
	void LoadImpl();
	void UnloadImpl();

private:
	shared_ptr<Texture> mTexture; 
};




}


#endif // TextureResource_h__

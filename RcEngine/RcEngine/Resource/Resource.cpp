#include <Resource/Resource.h>
#include <Core/Exception.h>


namespace RcEngine {

Resource::Resource( ResourceTypes resType, ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: mResourceType(resType), mCreator(creator), mResourceName(name), mResourceHandle(handle), mGroup(group), mBackground(false), mSize(0),
		mLoadState(Unloaded)
{

}

Resource::~Resource()
{

}

void Resource::Load( bool background /*= false*/ )
{
	LoadSync();
}

void Resource::Unload()
{

}

void Resource::Reload()
{

}

void Resource::Touch()
{

}

void Resource::LoadSync()
{
	if (mLoadState == Unloaded)
	{
		SetLoadState(Loading);
		LoadImpl();
		SetLoadState(Loaded);
	}
}

void Resource::LoadAsync()
{

}

Resource::LoadState Resource::GetLoadState()
{
	LoadState state;
	
	mMutex.lock();
	state = mLoadState;
	mMutex.unlock();

	return state;
}

void Resource::SetLoadState( LoadState state )
{
	mMutex.lock();
	mLoadState = state;
	mMutex.unlock();
}

shared_ptr<Resource> Resource::Clone()
{
	String err = "Resource type doesn't allow to clone";
	ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, err , "Resource::Clone");
}

}
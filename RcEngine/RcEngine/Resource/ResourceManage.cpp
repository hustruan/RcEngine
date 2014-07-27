#include <Resource/ResourceManager.h>
#include <IO/FileSystem.h>
#include <Core/Exception.h>

namespace RcEngine {

ResourceManager::ResourceManager()
: mNextHandle(1)
{

}

ResourceManager::~ResourceManager()
{
	mResourcesByHandle.clear();
}


ResourceHandle ResourceManager::GetNextHandle()
{
	return mNextHandle ++;
}

void ResourceManager::RegisterType( uint32_t type, const String& typeString, ResTypeFactoryFunc factoryFunc )
{
	ResourceRegEntry entry;
	entry.TypeString = typeString;
	entry.FactoryFunc = factoryFunc;

	auto found = mRegistry.find(type);
	if (found != mRegistry.end() && found->second.TypeString != typeString)
	{
		ENGINE_EXCEPT(Exception::ERR_DUPLICATE_ITEM, "Resorce type exits", "ResourceManager::RegisterType");
	}

	mRegistry[type] = entry;
}

void ResourceManager::AddResourceGroup( const String& groupName )
{
	if (mResourcesWithGroup.find(groupName) == mResourcesWithGroup.end())
	{
		mResourcesWithGroup.insert(std::make_pair(groupName, ResourceGroup()));
	}
}

ResourceHandle ResourceManager::AddResource( uint32_t type, const String& name, const String& group )
{
	ResourceHandle retVal = 0;

	unordered_map<String, ResourceGroup>::iterator groupIter;
	groupIter = mResourcesWithGroup.find(group);

	if (groupIter != mResourcesWithGroup.end())
	{
		ResourceGroup& resourceGroup = groupIter->second;

		auto resIter = resourceGroup.Resources.find(name);

		if (resIter != resourceGroup.Resources.end())
			retVal = resIter->second->GetResourceHandle();
		else
			retVal = AddNonExitingResource(type, name, group);
	}
	else
	{
		retVal = AddNonExitingResource(type, name, group);
	}


	return retVal;
}


ResourceHandle ResourceManager::AddNonExitingResource( uint32_t type, const String& name, const String& group )
{
	ResourceHandle retVal;

	auto factoryIter = mRegistry.find(type);
	if (factoryIter == mRegistry.end())
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Resource type factory not found", "ResourceManager::AddNonExitingResouce");
	}

	retVal = GetNextHandle();

	shared_ptr<Resource> resource = (factoryIter->second.FactoryFunc)(this, retVal, name, group);

	mResourcesWithGroup[group].Resources.insert( std::make_pair(name, resource) );
	mResourcesByHandle.insert( std::make_pair(retVal, resource) );

	return retVal;
}

	
shared_ptr<Resource> ResourceManager::GetResourceByName( uint32_t type, const String& name, const String& group )
{
	shared_ptr<Resource> retVal = nullptr;

	auto groupIter = mResourcesWithGroup.find(group);

	if (groupIter == mResourcesWithGroup.end())
	{
		String err = "Resource Group: " + group + " doesn't exit";
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, err , "ResourceManager::GetResourceByName");
	}


	ResourceGroup& resourceGroup = groupIter->second;

	auto resIter = resourceGroup.Resources.find(name);

	if (resIter != resourceGroup.Resources.end())
	{
		if (resIter->second->GetResourceType() == type)
		{
			retVal = resIter->second;
		}
	}
	else
	{
		ResourceHandle newResHandle = AddNonExitingResource(type, name, group);
		retVal = GetResourceByHandle(newResHandle);
	}

	if (retVal && retVal->IsLoaded() == false)
		retVal->Load();

	return retVal;
}


shared_ptr<Resource> ResourceManager::GetResourceByHandle( ResourceHandle handle )
{
	shared_ptr<Resource> retVal = nullptr;

	auto found = mResourcesByHandle.find(handle);
	if (found != mResourcesByHandle.end())
	{
		retVal =  found->second;
	}

	if (retVal && retVal->IsLoaded() == false)
		retVal->Load();

	return retVal;
}


void ResourceManager::LoadAllFromDisk()
{
	for (auto iter = mResourcesByHandle.begin(); iter != mResourcesByHandle.end(); ++iter)
	{
		iter->second->Load();
	}
}

void ResourceManager::ReleaseResource( ResourceHandle handle )
{
	auto it = mResourcesByHandle.find(handle);
	if (it != mResourcesByHandle.end())
	{
		//mResourcesByHandle[handle] = NULL;
		mResourcesByHandle.erase(it);
	}
}

void ResourceManager::UnLoadAll()
{
	mResourcesByHandle.clear();
}

}
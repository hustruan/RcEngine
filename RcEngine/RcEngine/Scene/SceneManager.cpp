#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/SceneObject.h>
#include <Scene/Entity.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/Material.h>
#include <Graphics/Mesh.h>
#include <Graphics/Camera.h>
#include <Graphics/Sky.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/AnimationController.h>
#include <Core/Exception.h>
#include <Core/Environment.h>
#include <IO/FileStream.h>
#include <IO/FileSystem.h>
#include <Resource/ResourceManager.h>
#include <Scene/SubEntity.h>
#include <Scene/Light.h>
#include <Graphics/Effect.h>

namespace RcEngine {

SceneManager::SceneManager()
	: mSkyBox(nullptr) 
{
	Environment::GetSingleton().mSceneManager = this;

	// Register all known scene object types
	RegisterType(SOT_Entity, "Entity Type", nullptr, nullptr, Entity::FactoryFunc);
	RegisterType(SOT_Light, "Light Type", nullptr, nullptr, Light::FactoryFunc);

	mAnimationController = new AnimationController;
}

SceneManager::~SceneManager()
{
	ClearScene();
	SAFE_DELETE(mAnimationController);
}

void SceneManager::ClearScene()
{
	// clear all scene node
	for (SceneNode* node : mAllSceneNodes) 
		delete node;

	mAllSceneNodes.clear();

	// clear all sprite
	for (SpriteBatch* batch : mSpriteBatchs)
		delete batch;

	// clear all scene object
	for (auto& kv : mSceneObjectCollections)
	{
		for (SceneObject* object : kv.second)
			delete object;
	}
	mSceneObjectCollections.clear();

	// lights has delete by mSceneObjectCollections
	mAllSceneLights.clear();
}

void SceneManager::RegisterType( uint32_t type, const String& typeString, ResTypeInitializationFunc inf, ResTypeReleaseFunc rf, ResTypeFactoryFunc ff )
{
	SceneObjectRegEntry entry;
	entry.typeString = typeString;
	entry.initializationFunc = inf;
	entry.releaseFunc = rf;
	entry.factoryFunc = ff;
	mRegistry[type] = entry;

	// Initialize resource type
	if( inf != 0 ) (*inf)();
}

SceneNode* SceneManager::CreateSceneNode( const String& name )
{
	SceneNode* node = CreateSceneNodeImpl(name);
	mAllSceneNodes.push_back(node);
	return node;
}

SceneNode* SceneManager::CreateSceneNodeImpl( const String& name )
{
	return new SceneNode(this, name);
}

SceneNode* SceneManager::GetRootSceneNode()
{
	SceneNode* root;

	if (mAllSceneNodes.empty())
	{
		root = CreateSceneNodeImpl("SceneRoot");
		mAllSceneNodes.push_back(root);
	}
	else
	{
		root = mAllSceneNodes[0];
	}

	return root;
}

AnimationController* SceneManager::GetAnimationController() const
{
	return mAnimationController;
}

void SceneManager::DestroySceneNode( SceneNode* node )
{
	auto found = std::find(mAllSceneNodes.begin(), mAllSceneNodes.end(), node);

	if (found == mAllSceneNodes.end())
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "SceneNode '" + node->GetName() + "' not found.",
			"SceneManager::DestroySceneNode");
	}

	// detach from parent (don't do this in destructor since bulk destruction behaves differently)
	Node* parentNode = (*found)->GetParent();
	if (parentNode)
	{
		parentNode->DetachChild((*found));
	}

	delete (*found);
	mAllSceneNodes.erase(found);
}

Light* SceneManager::CreateLight( const String& name, uint32_t type )
{
	if (mRegistry.find(SOT_Light) == mRegistry.end())
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Light type haven't Registed", "SceneManager::CreateEntity");

	static String lightTypeStr[LT_Count] = { "DirectionalLight", "PointLight", "SpotLight", "AreaLight" }; 
	
	NameValuePairList params;
	params["LightType"] = lightTypeStr[type];

	Light* light = static_cast<Light*>((mRegistry[SOT_Light].factoryFunc)(name, &params));
	mSceneObjectCollections[SOT_Light].push_back(light);

	// keep track of light
	mAllSceneLights.push_back(light);

	return light;
}

Entity* SceneManager::CreateEntity( const String& entityName, const String& meshName, const String& groupName )
{
	auto entityFactoryIter = mRegistry.find(SOT_Entity);
	if (entityFactoryIter == mRegistry.end())
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Entity type haven't Registed", "SceneManager::CreateEntity");
	}

	NameValuePairList params;
	params["ResourceGroup"] = groupName;
	params["Mesh"] = meshName;

	Entity* entity = static_cast<Entity*>((entityFactoryIter->second.factoryFunc)(entityName, &params));
	mSceneObjectCollections[SOT_Entity].push_back(entity);
	
	return entity;
}

SceneNode* SceneManager::FindSceneNode( const String& name ) const
{
	for (SceneNode* sceneNode : mAllSceneNodes)
	{
		if (sceneNode->GetName() == name)
			return sceneNode;
	}

	return nullptr;
}

void SceneManager::CreateSkyBox( const shared_ptr<Texture>& texture )
{
	SAFE_DELETE(mSkyBox);
	mSkyBox = new SkyBox;
	mSkyBox->GetMaterial()->SetTexture("EnvTex", texture->GetShaderResourceView());
}

void SceneManager::UpdateSceneGraph( float delta )
{
	// update anination controller first 
	mAnimationController->Update(delta);

	// update scene node transform
	GetRootSceneNode()->Update();
}

void SceneManager::UpdateRenderQueue(const Camera& cam, RenderOrder order)
{
	mRenderQueue.ClearQueue(RenderQueue::BucketOpaque); 
	mRenderQueue.ClearQueue(RenderQueue::BucketTransparent);
	mRenderQueue.ClearQueue(RenderQueue::BucketTranslucent);	// Particles

	GetRootSceneNode()->OnUpdateRenderQueues(cam, order);
}

void SceneManager::UpdateBackgroundQueue( const Camera& cam )
{
	mRenderQueue.ClearQueue(RenderQueue::BucketBackground); 

	// Update skynode same with camera positon, add sky box to render queue
	if (mSkyBox)
	{
		mSkyBox->SetPosition( cam.GetPosition() );
		mRenderQueue.AddToQueue( RenderQueueItem(mSkyBox, 0), RenderQueue::BucketBackground);
	}
}

void SceneManager::UpdateLightQueue( const Camera& cam )
{
	mLightQueue.clear();

	for (Light* light : mAllSceneLights)
	{
		switch (light->GetLightType())
		{
		case LT_PointLight:
			{
				BoundingSpheref sphere(light->GetDerivedPosition(), light->GetRange());
				if (cam.Visible(sphere))
					mLightQueue.push_back(light);
			}
			break;
		case LT_SpotLight:
			{
				mLightQueue.push_back(light);
			}
			break;
		default:
			mLightQueue.push_back(light);
		}
	}

	std::sort(mLightQueue.begin(), mLightQueue.end(), [](Light* lhs, Light* rhs) { return lhs->GetLightType() < rhs->GetLightType(); });
}

void SceneManager::UpdateOverlayQueue()
{
	mRenderQueue.ClearQueue(RenderQueue::BucketOverlay);

	for (SpriteBatch* batch : mSpriteBatchs)
		batch->OnUpdateRenderQueue( mRenderQueue );

	//for (Sprite* sprite : mSprites)
	//{
	//	if (sprite->Empty() == false)
	//	{
	//		RenderQueueItem item;
	//		item.Renderable = sprite;

	//		// ignore render order, only handle state change order
	//		item.SortKey = (float)sprite->GetMaterial()->GetEffect()->GetResourceHandle();
	//		.AddToQueue(item, RenderQueue::BucketOverlay);
	//	}
	//}
}

SpriteBatch* SceneManager::CreateSpriteBatch( const shared_ptr<Effect>& effect )
{
	SpriteBatch* batch = new SpriteBatch(effect);
	mSpriteBatchs.push_back(batch);
	return batch;
}

SpriteBatch* SceneManager::CreateSpriteBatch()
{
	SpriteBatch* batch = new SpriteBatch();
	mSpriteBatchs.push_back(batch);
	return batch;
}


}
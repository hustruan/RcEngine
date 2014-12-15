#ifndef SceneManager_h__
#define SceneManager_h__

//  [8/20/2012 Ruan]
#include <Core/Prerequisites.h>
#include <Graphics/Renderable.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/RenderQueue.h>

namespace RcEngine {

class SkyBox;
class SceneObject;
class SpriteBatch;

typedef std::vector<Light*> LightQueue;

class _ApiExport SceneManager
{
public:
	typedef void (*ResTypeInitializationFunc)();
	typedef void (*ResTypeReleaseFunc)();
	typedef SceneObject* (*ResTypeFactoryFunc)( const String& name, const NameValuePairList* params);

	struct SceneObjectRegEntry
	{
		String					   typeString;
		ResTypeInitializationFunc  initializationFunc;  // Called when type is registered
		ResTypeReleaseFunc         releaseFunc;  // Called when type is unregistered
		ResTypeFactoryFunc         factoryFunc;  // Factory to create resource object
	};

public:
	SceneManager();
	virtual ~SceneManager();

	/**
	 * Register a scene object type.
	 */
	void RegisterType(uint32_t type, const String& typeString, ResTypeInitializationFunc inf,
		ResTypeReleaseFunc rf, ResTypeFactoryFunc ff );

	/**
	 * Get root scene node. if not created, create it.
	 */
	SceneNode* GetRootSceneNode();

	/**
	 * Get sky scene node, attach sky scene object to this node
	 */
	SceneNode* GetSkySceneNode(); 

	/**
	 * Create a new scene node, this will track the node in scene manager.
	 */
	SceneNode* CreateSceneNode( const String& name );

	/**
	 * Destroy a scene node, this will delete the scene node.
	 */
	void DestroySceneNode( SceneNode* node );

	SceneNode* FindSceneNode( const String& name ) const;

	Entity* CreateEntity( const String& entityName, const String& meshName, const String& groupName );
	
	Light* CreateLight( const String& name, uint32_t lightType);
	const std::vector<Light*>& GetSceneLights() const  { return mAllSceneLights; }

	SkyBox* CreateSkyBox( const String& skyName, const String& resName, const String& groupName );

	/**
	 * Update all scene graph node and transform.
	 */
	void UpdateSceneGraph(float delta);

	void UpdateLightQueue(const Camera& cam);

	/**
	 * Update render queue, and remove scene node outside of the camera frustum.
	 */
	void UpdateRenderQueue(shared_ptr<Camera> camera, RenderOrder order, uint32_t renderBuckets, uint32_t filterIgnore);
	
	void UpdateOverlayQueue();

	RenderQueue& GetRenderQueue()						{ return mRenderQueue; }
	const RenderQueue& GetRenderQueue() const			{ return mRenderQueue; }

	// Return lights affect current view frustum
	LightQueue& GetLightQueue()							{ return mLightQueue; }
	const LightQueue& GetLightQueue() const				{ return mLightQueue; }
	
	AnimationController* GetAnimationController() const;

	// Create SpriteBatch with effect. NULL for default sprite effect
	SpriteBatch* CreateSpriteBatch();
	SpriteBatch* CreateSpriteBatch(const shared_ptr<Effect>& effect);
	void DestrySpriteBatch(SpriteBatch* batch);

protected:
	void ClearScene();
	virtual SceneNode* CreateSceneNodeImpl( const String& name );

protected:
	// Registry of scene object types
	std::map< uint32_t, SceneObjectRegEntry >  mRegistry; 

	// scene object collection, first dimension is scene object type
	std::map< uint32_t, std::vector<SceneObject*> > mSceneObjectCollections;

	// Keep track of all scene node
    std::vector<SceneNode*> mAllSceneNodes;		// [0] is root node

	SceneNode* mSkySceneNode;

	// Keep track of all scene lights
	std::vector<Light*> mAllSceneLights;

	std::vector<SpriteBatch*> mSpriteBatchs;

	AnimationController* mAnimationController;

	RenderQueue mRenderQueue;
	LightQueue  mLightQueue;
};


}


#endif // SceneManager_h__


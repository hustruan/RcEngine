#ifndef SceneNode_h__
#define SceneNode_h__

#include <Core/Prerequisites.h>
#include <Scene/Node.h>
#include <Math/BoundingBox.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

class SceneObject;
class SceneNodeVisitor;

class _ApiExport SceneNode : public Node
{
public:
	SceneNode(SceneManager* scene, const String& name);
	virtual ~SceneNode();

	/**
	 * Get the world bounding box of this scene node, camera can cull if box out view frustum.
	 */
	const BoundingBoxf& GetWorldBoundingBox() const;

	void AttachObject( SceneObject* obj );
	void DetachOject( SceneObject* obj );
	void DetachAllObject(); 

	uint32_t GetNumAttachedObjects() const	{ return mAttachedObjects.size(); }
	
	SceneObject* GetAttachedObject(uint32_t index) const { return mAttachedObjects[index];  }

	/**
	 * Get an attached object by given name.
	 */
	SceneObject* GetAttachedObject( const String& name );

	/**
	 * Creates an named new SceneNode as a child of this node.
	 */
	SceneNode* CreateChildSceneNode( const String& name, const float3& translate = float3::Zero(),  const Quaternionf& rotate = Quaternionf::Identity());

	/** 
	 * Return the scene to which a node belongs.
	 */
	SceneManager* GetScene() const { return mScene; }

	/**
	 * Sets the scene to which a node belongs.
	 */
	void SetScene( SceneManager* scene );

	/**
	 * Called when scene manager render queue update.
	 */
	void OnUpdateRenderQueues(const Camera& cam, RenderOrder order, uint32_t buckterFilter, uint32_t filterIgnore);
	
protected:
	virtual Node* CreateChildImpl( const String& name );

	/**
	 * Update world bound, this will merge a bound including all attached object 
	 * and the bound of all child scene node.
	 */
	void UpdateWorldBounds() const;

	void OnPostUpdate() {}

protected:

	mutable BoundingBoxf mWorldBounds;

	SceneManager* mScene;

	std::vector<SceneObject*> mAttachedObjects;
};


}




#endif // SceneNode_h__

#ifndef MovableObject_h__
#define MovableObject_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Scene/SceneManager.h>
#include <Math/BoundingBox.h>
#include <Math/Matrix.h>


namespace RcEngine {

enum SceneObejctType
{
	SOT_Entity = 0,
	SOT_Light,
	SOT_Camera,
	SOT_Particle,
	SOT_Count
};

/**
 * Base class for Objects in a scene graph, contained by scene nodes, referenced 
 * from Ogre. Note that scene management use the same design concept whit Ogre. 
 * We try to decouple the struct(the scene graph) from its content.Unlike other 
 * engines use complex inheritance hierarchy, subclass every type from scene node.
 * We use Composition, all node content data subclass scene object. Like Entity, 
 * Camera, Light. The scene object itself does not hold SRT data, that is stored 
 * in the parent scene node. 
 */
class _ApiExport SceneObject
{
	friend class SceneNode;

public:
	SceneObject( const String& name, SceneObejctType type, bool renderable = false );
	virtual ~SceneObject();

	const String& GetName() const { return mName; }

	SceneObejctType GetSceneObjectType() const { return mType; }

	bool Renderable() const	{ return mRenderable; }

	/**
	 * Return a undefined bounding sphere, subclass must overload
	 * this function to return a real bounding sphere if it actually
	 * have a bounding volume.
	 */
	virtual const BoundingBoxf& GetWorldBoundingBox() const;

	const float4x4& GetWorldTransform() const;

	/**
	 * Called when scene manger update render queue.
	 */
	virtual void OnUpdateRenderQueue( RenderQueue* renderQueue, const Camera& cam, RenderOrder order );
	
	SceneNode* GetParentNode() const { return mParentNode; }

	bool IsVisible() const { return mVisible; }

	void SetVisible( bool visible ) { mVisible = visible; }

	bool IsAttached() const  { return mParentNode != nullptr; }

protected:

	virtual void OnAttach( SceneNode* node ) ;
	virtual void OnDetach( SceneNode* node ) ;

protected:
	
	String mName;

	SceneObejctType mType;

	bool mRenderable;

	BoundingBoxf mBoundingBox;

	SceneNode* mParentNode;

	uint32_t mFlag;

	bool mVisible;
	 
};

}



#endif // MovableObject_h__

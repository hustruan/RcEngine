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
 * from Ogre. Note that scene management use the same design concept with Ogre. 
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
	enum Flags
	{
		NoDraw			= 1UL << 0,
		NoCastShadow	= 1UL << 1,
		NoRayQuery	    = 1UL << 2,
		Inactive		= 1UL << 3
	};

public:
	SceneObject( const String& name, SceneObejctType type, bool renderable = false );
	virtual ~SceneObject();

	inline const String& GetName() const				{ return mName; }

	inline SceneObejctType GetSceneObjectType() const	{ return mType; }

	inline bool IsAttached() const						{ return mParentNode != nullptr; }

	inline SceneNode* GetParentNode() const				{ return mParentNode; }

	void SetFlags(uint32_t flags)					    { mFlags = flags; }
	inline uint32_t GetFlags() const					{ return mFlags; }
	
	void SetActive(bool bActive);
	inline bool IsActive() const						{ return (mFlags & Inactive) == 0;}
	inline bool Renderable() const						{ return (mFlags & NoDraw) == 0; }

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
	virtual void OnUpdateRenderQueue( RenderQueue* renderQueue, const Camera& cam, RenderOrder order, uint32_t buckets, uint32_t filterIgnore );

protected:

	virtual void OnAttach( SceneNode* node ) ;
	virtual void OnDetach( SceneNode* node ) ;

protected:
	
	String mName;

	SceneObejctType mType;

	BoundingBoxf mBoundingBox;

	SceneNode* mParentNode;

	uint32_t mFlags;
};

}



#endif // MovableObject_h__

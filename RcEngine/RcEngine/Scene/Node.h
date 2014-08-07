#ifndef Node_h__
#define Node_h__

#include <Core/Prerequisites.h>
#include <Math/Vector.h>
#include <Math/Quaternion.h>
#include <Math/Matrix.h>

namespace RcEngine {


// Node dirty flags
#define NODE_DIRTY_WORLD 1
#define NODE_DIRTY_BOUNDS 2
#define NODE_DIRTY_ALL (NODE_DIRTY_WORLD | NODE_DIRTY_BOUNDS)


/**
 * Class representing a general-purpose node, has transform information, which is 
 * stored in local coordinated system relative to it's parent.
 * This is an abstract class - concrete classes are based on this for specific 
 * purposes, e.g. SceneNode, Bone
 */
class _ApiExport Node
{
public: 
	enum TransformSpace
	{
		TS_Local,
		TS_Parent,
		TS_World
	};

public:
	Node();
	Node( const String& name, Node* parent = 0 );
	virtual ~Node();

	/**
	 * Set node name.
	 */
	void SetName( const String& name )	{ mName = name; }

	/**
	 * Get node name.
	 */
	const String& GetName() const { return mName; }


	/**
	 * Moves the node along the Cartesian axes.
	 * TS_Local: Translate node from current position in local coordinate system along current rotated axes.
	 * TS_World: Translate node from current position in world coordinate system along unrotated axes.
	 */
	void Translate( const float3& d, TransformSpace relativeTo = TS_Parent );

	void Rotate( const Quaternionf& rot, TransformSpace relativeTo = TS_Parent );
	
	/**
	 * Set node local position relative to its parent.
	 */
	void SetPosition(const float3& position);

	/** 
	 * Get node local position relative to its parent.
	 */
	const float3& GetPosition() const { return mPosition; }

	/**
	 * Set node local rotation relative to its parent.
	 */
	void SetRotation(const Quaternionf& rotation);

	/**
	 * Get node local rotation relative to its parent.
	 */
	const Quaternionf& GetRotation() const	{ return mRotation; }

	/**
	 * Set node local scale relative to its parent.
	 */
	void SetScale(const float3& scale);

	/**
	 * Get node local scale relative to its parent.
	 */
	const float3& GetScale() const { return mScale; }

	/**
	 * Set node local transform relative to its parent.
	 */
	void SetTransform( const float3& position, const Quaternionf& rotation );

	/**
	 * Set node local transform relative to its parent.
	 */
	void SetTransform( const float3& position, const Quaternionf& rotation, const float3& scale );

	/**
	 * Get local transform matrix relative to its parent.
	 */
	float4x4 GetTransform() const; 

	/** 
	 * Set node world position.
	 */
	void SetWorldPosition( const float3& position );

	/**
	 * Get world position.
	 */
	float3 GetWorldPosition() const;

	/**
	 * Set world rotation.
	 */
	void SetWorldRotation( const Quaternionf& rotation );

	/**
	 * Get world rotation.
	 */
	Quaternionf GetWorldRotation() const;

	/**
	 * Get world direction.
	 */
	float3 GetWorldDirection() const;

	/**
	 * Get world scale.
	 */
	float3 GetWorldScale() const;

	/**
	 * Set world transform matrix.
	 */
	void SetWorldTransform( const float3& position, const Quaternionf& rotation );

	/**
	 * Get world transform matrix.
	 */
	const float4x4& GetWorldTransform() const;

	/** 
	 * Get attached child node count.
	 */
	uint32_t GetNumChildren( bool recursive = false ) const;

	/**
	 * Get a child node with specify name.
	 */
	Node* GetChild( const String& name ) const;

	/**
	 * Get all children.
	 */
	const std::vector<Node*>& GetChildren( ) const { return mChildren; }

	/**
	 * Creates an named new Node as a child of this node.
	 */
	Node* CreateChild( const String& name, const float3& translate,  const Quaternionf& rotate );

	/**
	 * Add a new child node.
	 */
	void AttachChild( Node* child );

	/**
	 * Remove a new child node.
	 * Does not delete the node, just detaches it from this parent, potentially to be reattached elsewhere. 
	 */
	void DetachChild( Node* child );

	/**
	 * Detach all child node.
	 */
	void DetachAllChildren();

	/**
	 * Set parent node.
	 */
	void SetParent( Node* parent );

	/**
	 * Get parent node.
	 */
	Node* GetParent() const { return mParent; }

	/**
	 * Update node, this will cause to recalculate world transform if node is dirty.
	 */
	void Update( );

	void NeedUpdate();

protected:

	/**
	 * Do derived class specify creation, derived class must overload this method.
	 */
	virtual Node* CreateChildImpl( const String& name ) = 0;

	virtual void OnPreUpdate( )  { }
	virtual void OnPostUpdate( ) { }

	virtual void OnChildNodeAdded( Node* node ) ;
	virtual void OnChildNodeRemoved( Node* node );
	
	virtual void UpdateWorldTransform() const;

	void PropagateDirtyDown( uint32_t dirtyFlag );
	void PropagateDirtyUp( uint32_t dirtyFlag );

protected:

	String mName;
	Node* mParent;
	std::vector<Node*> mChildren;	

	float3 mPosition;
	float3 mScale;
	Quaternionf mRotation;
	
	//mutable float3 mDerivedPosition;
	//mutable float3 mDerivedScale;
	//mutable Quaternionf mDerivedRotation;

	mutable float4x4 mWorldTransform;

	mutable uint8_t mDirtyBits;
};

}



#endif // Node_h__

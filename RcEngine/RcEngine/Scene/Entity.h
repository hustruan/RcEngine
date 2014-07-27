#ifndef Entity_h__
#define Entity_h__

#include <Core/Prerequisites.h>
#include <Scene/SceneObject.h>
#include <Graphics/Renderable.h>
#include <Graphics/Skeleton.h>

namespace RcEngine {

class SubEntity;
class AnimationPlayer;
class SkinnedAnimationPlayer;
class BoneFollower;

/** 
 * Defines an instance of a discrete, scene object based on a Mesh
 */
class _ApiExport Entity : public SceneObject 
{
	friend class SubEntity;

public:
	Entity( const String& name, const shared_ptr<Mesh>& mesh );
	~Entity();
	
	const BoundingBoxf& GetWorldBoundingBox() const;
	const BoundingBoxf& GetLocalBoundingBox() const;

	const shared_ptr<Mesh>& GetMesh() const							{ return mMesh; }

	uint32_t GetNumSubEntities() const								{ return mSubEntityList.size(); }

	SubEntity* GetSubEntity( uint32_t index ) const					{ return mSubEntityList[index]; }

	bool HasSkeleton() const;
	shared_ptr<Skeleton> GetSkeleton();

	bool HasSkeletonAnimation() const;
	AnimationPlayer* GetAnimationPlayer();

	void OnUpdateRenderQueue(RenderQueue* renderQueue, const Camera& cam, RenderOrder order);

	// Create a SceneNode take bone as parent
	BoneSceneNode* CreateBoneSceneNode(const String& nodeName, const String& boneName);

protected:
	void Initialize();
	void UpdateAnimation();

	void OnAttach( SceneNode* node );
	void OnDetach( SceneNode* node );

public:
	static SceneObject* FactoryFunc(const String& name, const NameValuePairList* params = 0);

protected:
	shared_ptr<Mesh> mMesh;
	shared_ptr<Skeleton> mSkeleton;
	
	mutable BoundingBoxf mWorldBoundingBox;

	vector<SubEntity*> mSubEntityList;
	
	vector<BoneSceneNode*> mBoneSceneNodes;

	vector<float4x4> mSkinMatrices;
	uint32_t mNumSkinMatrices;

	SkinnedAnimationPlayer* mAnimationPlayer;
};


}


#endif // Entity_h__

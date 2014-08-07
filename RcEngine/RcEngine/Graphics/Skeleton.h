#ifndef Skeleton_h__
#define Skeleton_h__

#include <Core/Prerequisites.h>
#include <Scene/SceneNode.h>

namespace RcEngine {

class Bone;

class _ApiExport Skeleton
{
public:
	Skeleton();
	~Skeleton();

	void Update( float delta );

	uint32_t GetNumBones() const { return mBones.size(); }

	Bone* GetBone( uint32_t index ) const;
	Bone* GetBone( const String& name ) const;

	Bone* AddBone(const String& name, Bone* parent);

	shared_ptr<Skeleton> Clone();

public:
	static shared_ptr<Skeleton> LoadFrom( Stream& source, uint32_t numBones );

private:
	std::vector<Bone*> mBones;
};

class _ApiExport Bone : public Node
{
	friend class Skeleton;

public:
	Bone(const String& name, uint32_t boneIndex, Bone* parent = 0);

	uint32_t GetBoneIndex() const { return mBoneIndex; }

	const float4x4& GetOffsetMatrix() const { return mOffsetMatrix; }

	void CalculateBindPose();	

protected:

	// Do not use this method to create bone node, Bone node only created when load from mesh file
	virtual Node* CreateChildImpl(const String& name);

protected:
	uint32_t mBoneIndex;
	float4x4 mOffsetMatrix;
};

/**
 * Specular SceneNode which can be attached on Bone node.
 */
class _ApiExport BoneSceneNode : public SceneNode
{
public:
	/**
	 * @Param worldSceneNode, if exits take worldSceneNode's transform into consideration .
	 */
	BoneSceneNode(SceneManager* scene, const String& name, SceneNode* worldSceneNode = nullptr);

	void SetWorldSceneNode(SceneNode* worldSceneNode);
	inline SceneNode* GetWorldSceneNode() const { return mWorldSceneNode; }

protected:
	// need to consider entity's transform
	virtual void UpdateWorldTransform() const;

	virtual Node* CreateChildImpl(const String& name);

protected:
	SceneNode* mWorldSceneNode;
};



} // Namespace RcEngine

#endif // Skeleton_h__
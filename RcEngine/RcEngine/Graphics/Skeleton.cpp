#include <Graphics/Skeleton.h>
#include <IO/FileStream.h>
#include <Core/XMLDom.h>
#include <Math/MathUtil.h>
#include <Core/Exception.h>

namespace RcEngine {

Bone::Bone( const String& name, uint32_t boneID, Bone* parent )
	: Node(name, parent),
	  mBoneIndex(boneID)
{

}

void Bone::CalculateBindPose()
{
	float4x4 bindPose = GetWorldTransform();
	mOffsetMatrix = bindPose.Inverse();
}

Node* Bone::CreateChildImpl( const String& name )
{
	ENGINE_EXCEPT(Exception::ERR_RT_ASSERTION_FAILED, "Shound't call Bone::CreateChildImpl", "Bone::CreateChildImpl");
	return nullptr;
}

Skeleton::Skeleton()
{
}

Skeleton::~Skeleton()
{
	// Delete all bones
	for (Bone* bone : mBones)
		delete bone;
}

Bone* Skeleton::GetBone( const String& name ) const
{
	for (Bone* bone : mBones)
	{
		if (bone->GetName() == name)
			return bone;
	}

	return nullptr;
}

Bone* Skeleton::GetBone( uint32_t index ) const
{
	assert(index < mBones.size());
	return mBones[index];
}

shared_ptr<Skeleton> Skeleton::LoadFrom( Stream& source, uint32_t numBones )
{
	shared_ptr<Skeleton> skeleton = std::make_shared<Skeleton>();

	unordered_map<Bone*, int32_t> boneParentsMap;

	skeleton->mBones.resize(numBones);
	for (uint32_t i = 0; i < numBones; ++i)
	{
		String boneName = source.ReadString();
		int32_t parentBoneIdx = source.ReadInt();

		Bone* bone = new Bone(boneName, i, nullptr);

		float3 bindPos;
		source.Read(&bindPos,sizeof(float3));
		bone->SetPosition(bindPos);

		Quaternionf bindRot;
		source.Read(&bindRot, sizeof(Quaternionf));
		bone->SetRotation(bindRot);

		float3 bindScale;
		source.Read(&bindScale,sizeof(float3));
		bone->SetScale(bindScale);

		boneParentsMap[bone] = parentBoneIdx;
		skeleton->mBones[i] = bone;
	}

	for (uint32_t i = 0; i < numBones; ++i)
	{
		Bone* bone = skeleton->mBones[i];
		int32_t parentIndex = boneParentsMap[bone];

		if (parentIndex != -1)
			skeleton->mBones[parentIndex]->AttachChild(bone);
	}

	for (uint32_t i = 0; i < numBones; ++i)
		skeleton->mBones[i]->CalculateBindPose();

	return skeleton;
}

shared_ptr<Skeleton> Skeleton::Clone()
{
	shared_ptr<Skeleton> skeleton = std::make_shared<Skeleton>();

	skeleton->mBones.resize(mBones.size());
	for (size_t i = 0; i < mBones.size(); ++i)
	{
		Bone* newBone =  new Bone( mBones[i]->GetName(), i, nullptr);
		newBone->SetPosition(mBones[i]->GetPosition());
		newBone->SetRotation(mBones[i]->GetRotation());
		newBone->SetScale(mBones[i]->GetScale());
		newBone->mOffsetMatrix = mBones[i]->mOffsetMatrix;

		skeleton->mBones[i] = newBone;
	}

	for (size_t i = 0; i < mBones.size(); ++i)
	{
		Bone* parentBone = (static_cast<Bone*>(mBones[i]->GetParent()));
		if (parentBone)
		{
			uint32_t parentIndex =  parentBone->GetBoneIndex();
			skeleton->mBones[parentIndex]->AttachChild(skeleton->mBones[i]);
		}
	}

	return skeleton;
}

Bone* Skeleton::AddBone( const String& name, Bone* parent )
{
	Bone* bone = new Bone(name, mBones.size(), parent);
	mBones.push_back(bone);
	return bone;
}

//////////////////////////////////////////////////////////////////////////
BoneSceneNode::BoneSceneNode( SceneManager* scene, const String& name, SceneNode* worldSceneNode /*= nullptr*/ )
	: SceneNode(scene, name),
	  mWorldSceneNode(worldSceneNode)
{

}

void BoneSceneNode::UpdateWorldTransform() const
{
	// Update bone transform
	SceneNode::UpdateWorldTransform();

	if (mWorldSceneNode)
	{
		mWorldTransform = mWorldTransform * mWorldSceneNode->GetWorldTransform();
	}
}

Node* BoneSceneNode::CreateChildImpl( const String& name )
{
	return new BoneSceneNode(mScene, name);
}

void BoneSceneNode::SetWorldSceneNode( SceneNode* worldSceneNode )
{
	mWorldSceneNode = worldSceneNode;
	if (mWorldSceneNode)
		mScene = mWorldSceneNode->GetScene();
	PropagateDirtyDown(NODE_DIRTY_WORLD | NODE_DIRTY_BOUNDS);
}

} // Namespace RcEngine

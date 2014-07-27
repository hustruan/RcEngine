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
	skeleton->mBones.resize(numBones);

	for (uint32_t i = 0; i < numBones; ++i)
	{
		String boneName = source.ReadString();
		int32_t parentBoneIdx = source.ReadInt();

		Bone* parent = (parentBoneIdx == -1) ? nullptr : skeleton->mBones[parentBoneIdx];
		Bone* bone = new Bone(boneName, i, parent);

		float3 bindPos;
		source.Read(&bindPos,sizeof(float3));
		bone->SetPosition(bindPos);

		Quaternionf bindRot;
		source.Read(&bindRot, sizeof(Quaternionf));
		bone->SetRotation(bindRot);

		float3 bindScale;
		source.Read(&bindScale,sizeof(float3));
		bone->SetScale(bindScale);

		bone->CalculateBindPose();

		skeleton->mBones[i] = bone;	
	}

	return skeleton;
}

shared_ptr<Skeleton> Skeleton::Clone()
{
	shared_ptr<Skeleton> skeleton = std::make_shared<Skeleton>();

	skeleton->mBones.resize(mBones.size());
	for (size_t iBone = 0; iBone < mBones.size(); ++iBone)
	{
		Bone* parentBoneOld = static_cast<Bone*>(mBones[iBone]->GetParent());

		Bone* parentBoneNew = parentBoneOld ? skeleton->mBones[parentBoneOld->GetBoneIndex()] : nullptr;
		Bone* newBone =  new Bone( mBones[iBone]->GetName(), iBone, parentBoneNew);
		newBone->SetPosition(mBones[iBone]->GetPosition());
		newBone->SetRotation(mBones[iBone]->GetRotation());
		newBone->SetScale(mBones[iBone]->GetScale());
		newBone->CalculateBindPose();
		skeleton->mBones[iBone] = newBone;
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

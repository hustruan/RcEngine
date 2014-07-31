#include <Scene/Entity.h>
#include <Scene/SubEntity.h>
#include <Scene/SceneNode.h>
#include <Scene/SceneManager.h>
#include <Graphics/Mesh.h>
#include <Graphics/Effect.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Camera.h>
#include <Graphics/Material.h>
#include <Graphics/AnimationClip.h>
#include <Graphics/AnimationController.h>
#include <Graphics/Animation.h>
#include <Graphics/AnimationState.h>
#include <Graphics/RenderQueue.h>
#include <Core/Environment.h>
#include <Core/Exception.h>
#include <IO/PathUtil.h>
#include <Math/MathUtil.h>
#include <Resource/ResourceManager.h>

namespace RcEngine {

Entity::Entity( const String& name, const shared_ptr<Mesh>& mesh )
	: SceneObject(name, SOT_Entity, true),
	mNumSkinMatrices(0), 
	mMesh(mesh), 
	mAnimationPlayer(nullptr),
	mSkeleton( mesh->GetSkeleton() ? mesh->GetSkeleton()->Clone() : 0 )
{
	Initialize();

	printf("Create Entity: %s\n", name.c_str());
}

Entity::~Entity()
{
	printf("Delete Entity: %s\n", mName.c_str());

	for (SubEntity* subEntiry : mSubEntityList)
		SAFE_DELETE(subEntiry);
	mSubEntityList.clear();

	mSkeleton = nullptr;

	// Detach all bone scene nodes
	for (BoneSceneNode* boenSceneNode : mBoneSceneNodes)
		delete boenSceneNode;

	SAFE_DELETE(mAnimationPlayer);
}


void Entity::Initialize()
{
	const String& meshGroup = mMesh->GetResourceGroup();
	String meshDirectory = PathUtil::GetPath(mMesh->GetResourceName()); 

	for (uint32_t i = 0; i < mMesh->GetNumMeshPart(); ++i)
	{
		shared_ptr<MeshPart> meshPart = mMesh->GetMeshPart(i);
		
		SubEntity* subEnt = new SubEntity(this, meshPart);

		String matResName = meshDirectory + meshPart->GetMaterialName();
		subEnt->SetMaterial( ResourceManager::GetSingleton().GetResourceByName<Material>(RT_Material, matResName, meshGroup) );

		mSubEntityList.push_back( subEnt );
	}

	if (HasSkeleton())
	{
		mNumSkinMatrices = mSkeleton->GetNumBones();
		mSkinMatrices.resize(mNumSkinMatrices);
	}

	if (mParentNode)
	{
		mParentNode->NeedUpdate();
	}
}

const BoundingBoxf& Entity::GetWorldBoundingBox() const
{
	if (!mParentNode)
	{
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Entity:" + mName + " Haven't attach to a scene node yet, world bound not exits",
			"Entity::GetWorldBoundingSphere");
	}

	if (!mMesh)
	{
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Entity:" + mName + " doesn't load a mesh yet",
			"Entity::GetWorldBoundingSphere");
	}

	mWorldBoundingBox = Transform(mMesh->GetBoundingBox(), mParentNode->GetWorldTransform());

	return mWorldBoundingBox;
}

const BoundingBoxf& Entity::GetLocalBoundingBox() const
{
	if (!mMesh)
	{
		ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Entity:" + mName + " doesn't load a mesh yet",
			"Entity::GetWorldBoundingSphere");
	}

	return mMesh->GetBoundingBox();
}

bool Entity::HasSkeleton() const
{
	return mSkeleton != nullptr;
}

shared_ptr<Skeleton> Entity::GetSkeleton()
{
	return mSkeleton;
}

bool Entity::HasSkeletonAnimation() const
{
	return (mMesh->GetSkeleton() != nullptr);
}

AnimationPlayer* Entity::GetAnimationPlayer()
{
	if (!mAnimationPlayer && mMesh->GetSkeleton())
	{
		mAnimationPlayer = new SkinnedAnimationPlayer(mSkeleton);
	}

	return mAnimationPlayer;
}

void Entity::OnUpdateRenderQueue(RenderQueue* renderQueue, const Camera& camera, RenderOrder order)
{
	// Add each visible SubEntity to the queue
	for (SubEntity* subEntity : mSubEntityList)
	{
		BoundingBoxf subWorldBoud = Transform(subEntity->GetBoundingBox(), mParentNode->GetWorldTransform());

		// Todo  mesh part world bounding has some bugs.
		if(camera.Visible(subWorldBoud))
		{
			float sortKey = 0;
			RenderQueue::Bucket bucket = (RenderQueue::Bucket)subEntity->GetMaterial()->GetQueueBucket();

			switch( order )
			{
			case RO_StateChange:
				sortKey = (float)subEntity->GetMaterial()->GetEffect()->GetResourceHandle();
				break;
			case RO_FrontToBack:
				sortKey = NearestDistToAABB( camera.GetPosition(), subWorldBoud.Min, subWorldBoud.Max);
				break;
			case RO_BackToFront:
				sortKey = -NearestDistToAABB( camera.GetPosition(), subWorldBoud.Min, subWorldBoud.Max);
				break;
			}
			
			if (bucket == RenderQueue::BucketTransparent)
			{
				// Transparent object must render from furthest to nearest
				sortKey = -NearestDistToAABB( camera.GetPosition(), subWorldBoud.Min, subWorldBoud.Max);
			}

			renderQueue->AddToQueue(RenderQueueItem(subEntity, sortKey), bucket);			
		}
	}

	// Update animation 
	if (HasSkeleton())
	{
		UpdateAnimation();

		for (BoneSceneNode* boneSceneNode : mBoneSceneNodes)
		{
			boneSceneNode->OnUpdateRenderQueues(camera, order);
		}
	}
}

void Entity::UpdateAnimation()
{
	if (mAnimationPlayer)
	{
		for (auto& kv : mAnimationPlayer->GetAllClip())
		{
			AnimationState* animState = kv.second;
			animState->Apply();
		}

		// Note: the model's world transform will be baked in the skin matrices
		for (uint32_t i = 0; i < mSkeleton->GetNumBones(); ++i)
		{
			Bone* bone = mSkeleton->GetBone(i);
			auto world = bone->GetWorldTransform();
			mSkinMatrices[i] = bone->GetOffsetMatrix() * bone->GetWorldTransform();
		}
	}
	else
	{
		for (uint32_t i = 0; i < mSkeleton->GetNumBones(); ++i)
			mSkinMatrices[i].MakeIdentity();
	}
}

BoneSceneNode* Entity::CreateBoneSceneNode( const String& nodeName, const String& boneName )
{
	if (!HasSkeleton())
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "This entity's mesh has no skeleton to attach object to.",
			"Entity::attachObjectToBone");
	}

	Bone* pBone = mSkeleton->GetBone(boneName);

	if (!pBone)
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Cannot locate bone named " + boneName,
			"Entity::attachObjectToBone");
	}

	BoneSceneNode* newNode;

	if (mParentNode)
		newNode = new BoneSceneNode(mParentNode->GetScene(), nodeName, mParentNode);
	else
		newNode = new BoneSceneNode(nullptr, nodeName, nullptr);
	
	pBone->AttachChild(newNode);

	mBoneSceneNodes.push_back(newNode);
	return newNode;
}

SceneObject* Entity::FactoryFunc( const String& name, const NameValuePairList* params)
{
	// must have mesh parameter
	shared_ptr<Mesh> pMesh;

	if (params)
	{
		String groupName = "General";

		NameValuePairList::const_iterator found = params->find("ResourceGroup");
		if (found != params->end())
			groupName = found->second;

		found = params->find("Mesh");
		if (found != params->end())
		{
			// Get mesh (load if required)
			pMesh = std::static_pointer_cast<Mesh>(
				ResourceManager::GetSingleton().GetResourceByName(RT_Mesh, found->second, groupName));	
		}
	}
	else
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Create entity failed.", "Entity::FactoryFunc");

	return new Entity(name, pMesh);
}

void Entity::OnAttach( SceneNode* node )
{
	SceneObject::OnAttach(node);

	for (BoneSceneNode* boneSceneNode : mBoneSceneNodes)
	{
		boneSceneNode->SetWorldSceneNode(mParentNode);
	}
}

void Entity::OnDetach( SceneNode* node )
{
	SceneObject::OnDetach(node);
	for (BoneSceneNode* boneSceneNode : mBoneSceneNodes)
	{
		boneSceneNode->SetWorldSceneNode(nullptr);
	}
}

}
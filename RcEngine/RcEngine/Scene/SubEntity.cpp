#include <Scene/SubEntity.h>
#include <Scene/Entity.h>
#include <Scene/SceneNode.h>
#include <Scene/SceneManager.h>
#include <Graphics/Mesh.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Material.h>
#include <Core/Exception.h>
#include <Math/MathUtil.h>
#include <Resource/ResourceManager.h>

namespace RcEngine {

SubEntity::SubEntity( Entity* parent, const shared_ptr<MeshPart>& meshPart )
	: mMeshPart(meshPart), mParent(parent), mRenderOperation(new RenderOperation)
{

}

SubEntity::~SubEntity()
{
	int a = 0;
}

const shared_ptr<Material>& SubEntity::GetMaterial() const
{
	return mMaterial;
}

void SubEntity::SetMaterial( const shared_ptr<Material>& mat )
{
	mMaterial = mat;
	mMaterial->Load();
}

void SubEntity::SetMaterial( const String& matName, const String& group )
{
	mMaterial = std::static_pointer_cast<Material>(
		ResourceManager::GetSingleton().GetResourceByName(RT_Material, matName, group));

	mMaterial->Load();

	// if use material animation, we must use material clone
	//mMaterial = std::static_pointer_cast<Material>(mMaterial->Clone());
}

const String& SubEntity::GetName() const
{
	return mMeshPart->GetName();
}

EffectTechnique* SubEntity::GetTechnique() const
{
	return mMaterial->GetCurrentTechnique();
}

void SubEntity::GetWorldTransforms( float4x4* xform ) const
{
	if (!mParent->mNumSkinMatrices || !mParent->HasSkeletonAnimation())
	{
		// no skeleton animation
		*xform = mParent->GetWorldTransform();
	}
	else
	{
		// hardware skin
		if (mParent->HasSkeletonAnimation())
		{
			assert(mParent->mNumSkinMatrices != 0);
			size_t i = 0;
			for (i = 0; i < mParent->mNumSkinMatrices; ++i)
				xform[i] = mParent->mSkinMatrices[i];

			// last matrix is scene node world matrix
			xform[i] = mParent->GetWorldTransform();
		}
		else
		{
			// All animations disabled, use parent entity world transform only
			std::fill_n(xform, mParent->mNumSkinMatrices + 1, mParent->GetWorldTransform());
		}
	}
}

uint32_t SubEntity::GetWorldTransformsCount() const
{
	return 1 + mParent->mNumSkinMatrices;
}

const shared_ptr<RenderOperation>& SubEntity::GetRenderOperation() const
{
	mMeshPart->GetRenderOperation(*mRenderOperation, 0);
	return mRenderOperation;
}

const BoundingBoxf& SubEntity::GetBoundingBox() const
{
	return mMeshPart->GetBoundingBox();
}


}
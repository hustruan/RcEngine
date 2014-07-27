#include <Scene/SceneObject.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Graphics/RenderQueue.h>

namespace RcEngine {

// undefined bounding sphere
static const BoundingBoxf UnDefineBoundingBox;


SceneObject::SceneObject( const String& name, SceneObejctType type, bool renderable /*= false*/ )
	: mName(name), mType(type), mRenderable(renderable), mParentNode(nullptr), mVisible(true)
{

}

SceneObject::~SceneObject()
{

}

const BoundingBoxf& SceneObject::GetWorldBoundingBox() const
{
	return UnDefineBoundingBox;
}


const float4x4& SceneObject::GetWorldTransform() const
{
	if (mParentNode)
		return mParentNode->GetWorldTransform();
	else
		return float4x4::Identity();
}

void SceneObject::OnAttach( SceneNode* node )
{
	assert(!mParentNode || !node);
	mParentNode = node;
}

void SceneObject::OnDetach( SceneNode* node )
{
	mParentNode = nullptr;
}

void SceneObject::OnUpdateRenderQueue( RenderQueue* renderQueue, const Camera& cam, RenderOrder order )
{

}



}
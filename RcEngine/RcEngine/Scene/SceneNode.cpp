#include <Scene/SceneNode.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneObject.h>
#include <Scene/Light.h>
#include <Graphics/Camera.h>
#include <Math/MathUtil.h>
#include <Core/Exception.h>


namespace RcEngine {

SceneNode::SceneNode( SceneManager* scene, const String& name )
	: Node(name)
{
	SetScene(scene);
}

SceneNode::~SceneNode()
{
	for ( auto iter = mAttachedObjects.begin(); iter != mAttachedObjects.end(); ++iter )
	{
		(*iter)->OnDetach(this);
	}
	mAttachedObjects.clear();
}

void SceneNode::SetScene( SceneManager* scene )
{
	mScene = scene;
}


Node* SceneNode::CreateChildImpl( const String& name )
{
	assert(mScene);
	return mScene->CreateSceneNode(name);
}

SceneNode* SceneNode::CreateChildSceneNode( const String& name, const float3& translate, const Quaternionf& rotate )
{
	return static_cast<SceneNode*>( CreateChild(name, translate, rotate) );
}

void SceneNode::AttachObject( SceneObject* obj )
{
	if (obj->IsAttached())
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS,  "Object already attached to a SceneNode", "SceneNode::attachObject");
	}

	mAttachedObjects.push_back(obj);

	// since we have attach a scene object, bound may invalid.
	PropagateDirtyUp(NODE_DIRTY_BOUNDS);

	obj->OnAttach(this);
}

void SceneNode::DetachOject( SceneObject* obj )
{
	auto found = std::find(mAttachedObjects.begin(), mAttachedObjects.end(), obj);
	if (found != mAttachedObjects.end())
	{
		obj->OnDetach(this);
		mAttachedObjects.erase(found);

		// since we have detach a scene object, bound may invalid.
		PropagateDirtyUp(NODE_DIRTY_BOUNDS);
	}
}

void SceneNode::DetachAllObject()
{
	for ( auto iter = mAttachedObjects.begin(); iter != mAttachedObjects.end(); ++iter )
	{
		(*iter)->OnDetach(this);
	}
	mAttachedObjects.clear();

	// Make sure bounds get updated (must go right to the top)
	PropagateDirtyUp(NODE_DIRTY_BOUNDS);
}

SceneObject* SceneNode::GetAttachedObject( const String& name )
{
	auto found = std::find_if(mAttachedObjects.begin(), mAttachedObjects.end(), [&name](SceneObject* obj){
						return obj->GetName() == name;
					});

	if (found == mAttachedObjects.end())
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Attached movable object named " + name +
			" does not exist.", "SceneNode::GetAttachedObject");
	}

	return *found;
}

void SceneNode::UpdateWorldBounds() const
{
	if (mDirtyBits & NODE_DIRTY_BOUNDS)
	{
		mWorldBounds.SetNull();

		/**
		 * Merge all attached object's bound first, note that if object doesn't have bound,
		 * the Merge operation doesn't have effect.
		 */
		for (auto iter = mAttachedObjects.begin(); iter != mAttachedObjects.end(); ++iter)
		{
			mWorldBounds.Merge((*iter)->GetWorldBoundingBox());
		}

		for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			SceneNode* childNode = static_cast<SceneNode*>(*iter);
			mWorldBounds.Merge(childNode->GetWorldBoundingBox());
		}

		mDirtyBits &= ~NODE_DIRTY_BOUNDS;
	}
}

const BoundingBoxf& SceneNode::GetWorldBoundingBox() const
{
	UpdateWorldBounds();
	return mWorldBounds;
}

uint32_t SceneNode::GetNumAttachedObjects() const
{
	return mAttachedObjects.size();
}

void SceneNode::OnUpdateRenderQueues( const Camera& camera, RenderOrder order, uint32_t buckterFilter, uint32_t filterIgnore )
{
	const BoundingBoxf& worldBound = GetWorldBoundingBox();

	if (!camera.Visible(GetWorldBoundingBox()))
		return;

	RenderQueue& renderQueue = mScene->GetRenderQueue();
	for (SceneObject* pSceneObject : mAttachedObjects)
	{
		if (pSceneObject->IsActive() && pSceneObject->Renderable())
			pSceneObject->OnUpdateRenderQueue(&renderQueue, camera, order, buckterFilter, filterIgnore);
	}

	// recursively call children
	for (Node* node : mChildren)
	{
		SceneNode* child = static_cast<SceneNode*>(node);
		child->OnUpdateRenderQueues(camera, order, buckterFilter, filterIgnore);
	}
}

}
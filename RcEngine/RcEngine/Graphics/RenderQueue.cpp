#include <Graphics/RenderQueue.h>
#include <Core/Exception.h>

namespace RcEngine {

RenderQueue::RenderQueue()
{
	// set up default bucket
	mRenderBuckets.insert(std::make_pair(BucketOpaque, new RenderBucket()));
	mRenderBuckets.insert(std::make_pair(BucketTransparent, new RenderBucket()));
	mRenderBuckets.insert(std::make_pair(BucketBackground, new RenderBucket()));
	mRenderBuckets.insert(std::make_pair(BucketTranslucent, new RenderBucket()));
	mRenderBuckets.insert(std::make_pair(BucketOverlay, new RenderBucket()));
}

RenderQueue::~RenderQueue()
{
	for (auto& kv : mRenderBuckets)
		delete kv.second;
}

RenderBucket& RenderQueue::GetRenderBucket( Bucket bucket, bool sortBucket /*= true*/ )
{
	if (mRenderBuckets.find(bucket) == mRenderBuckets.end())
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Render bucket not exits!",  "RenderQueue::GetRenderBucket");
	}

	RenderBucket& renderBucker = (*mRenderBuckets[bucket]);

	if (sortBucket)
	{
		std::sort(renderBucker.begin(), renderBucker.end(), [](const RenderQueueItem& lhs, const RenderQueueItem& rhs) {
			return lhs.SortKey < rhs.SortKey; });
	}

	return renderBucker;
}

std::map<RenderQueue::Bucket, RenderBucket*>& RenderQueue::GetAllRenderBuckets( bool sortBucket /*= true*/ )
{
	if (sortBucket)
	{
		for (auto& kv : mRenderBuckets)
		{
			std::sort(kv.second->begin(), kv.second->end(), [](const RenderQueueItem& lhs, const RenderQueueItem& rhs) {
				return lhs.SortKey < rhs.SortKey; });
		}
	}

	return mRenderBuckets;
}


void RenderQueue::AddToQueue( RenderQueueItem item, Bucket bucket )
{
	if (mRenderBuckets.find(bucket) == mRenderBuckets.end())
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Render bucket not exits!",  "RenderQueue::AddToQueue");
	}

	mRenderBuckets[bucket]->push_back(item);
}

void RenderQueue::AddRenderBucket( Bucket bucket )
{
	if (mRenderBuckets.find(bucket) != mRenderBuckets.end())
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Render bucket already exits!",  "RenderQueue::AddRenderBucket");
	}

	mRenderBuckets.insert(std::make_pair(bucket, new RenderBucket()));
}

void RenderQueue::ClearAllQueue()
{
	for (auto iter = mRenderBuckets.begin(); iter != mRenderBuckets.end(); ++iter)
	{
		iter->second->clear();
	}
}

void RenderQueue::ClearQueue( Bucket bucket )
{
	mRenderBuckets[bucket]->clear();
}




}

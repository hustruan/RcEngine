#ifndef AnimationClip_h__
#define AnimationClip_h__

#include <Core/Prerequisites.h>
#include <Math/Vector.h>
#include <Math/Quaternion.h>
#include <Resource/Resource.h>

namespace RcEngine {

class _ApiExport AnimationClip : public Resource
{
public:
	friend class AnimationPlayer;
	friend class AnimationState;

	struct _ApiExport KeyFrame
	{
		// The time offset from the start of the animation to this keyframe.
		float Time;

		float3 Translation;
		Quaternionf Rotation;
		float3 Scale;
	};

	struct _ApiExport AnimationTrack
	{
		// Bone name
		String Name;
		vector<KeyFrame> KeyFrames;

		int32_t GetKeyFrameIndex( float time ) const;
	};

public:
	AnimationClip(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group );
	~AnimationClip();

	const String& GetClipName() const    { return mClipName; }
	void SetClipName(const String& name) { mClipName = name; }

	/**
	 * Get AnimationClip's duration in seconds
	 */
	float GetDuration() const { return mDuration; }

protected:
	void LoadImpl();
	void UnloadImpl();

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

private:	
	float mDuration;
	String mClipName;
	vector<AnimationTrack> mAnimationTracks;
};



}


#endif // AnimationClip_h__

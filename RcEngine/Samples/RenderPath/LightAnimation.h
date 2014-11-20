#ifndef LightAnimation_h__
#define LightAnimation_h__

#include <cmath>
#include <random>
#include <vector>
#include <unordered_map>
#include <Scene/Light.h>

using namespace RcEngine;

class LightAnimation
{
	enum LightAnimationType
	{
		AnimStatic = 0, 
		AnimLine,
		AnimCircle,
		AnimEllipse,
		AnimCount
	};

	struct LineAnimatedLight
	{
		int LineIndex;
		int FloorIndex;
		Light* Light;
	};

	struct EllipseAnimatedLight
	{
		float EllipseWidth, EllipseHeight; // ∂Ã°¢≥§÷·
		float Angle;
		float Height;
		float AnimationSpeed;
		Light* Light;
	};

public:
	LightAnimation();
	~LightAnimation();

	void Move(float elapsedTime);
	void SetupLights();
	uint32_t GetNumLights() const { return mNumTotalLights; }

private:
	void CreateLightsWithAnimLine();
	void CreateLightsWithAnimStatic();
	void CreateLightsWithAnimEllipse();

	void MoveAnimLine(float elapsedTime);
	void MoveAnimStatic(float elapsedTime);
	void MoveAnimEllipse(float elapsedTime);


	void DebugDrawLights();

private:
	uint32_t mNumTotalLights;
	std::vector<LineAnimatedLight> mLineAnimatedLights;
	std::vector<EllipseAnimatedLight> mEllipseAnimatedLights;
};


#endif // LightAnimation_h__

#ifndef LightAnimation_h__
#define LightAnimation_h__

#include <vector>
#include <cmath>
#include <random>
#include <Math/Vector.h>
#include <Scene/Light.h>

using namespace RcEngine;

class LightAnimation
{
public:
	struct LightParam
	{
		float2 LightAttenuation;

		// For animation
		float Radius;
		float Angle;
		float Height;
		float AnimationSpeed;

		Light* Light;

		//Light() {}

		//// Directional
		//Light(const float3& color, const float3& direction)
		//	: LightType(LT_DirectionalLigt), LightColor(color), LightDirection(direction) {}

		//// Point
		//Light(const float3& color, const float3& position, float attenuationBegin, float attenuationEnd)
		//	: LightType(LT_PointLight), LightColor(color), LightPosition(position), LightAttenuation(attenuationBegin, attenuationEnd) {}
	
		//// Spot
		//Light(const float3& color, const float3& position, const float3& direction, 
		//	float innerAngle, float outAngle, float spotFalloff, float attenuationBegin, float attenuationEnd)
		//	: LightType(LT_SpotLight), LightColor(color), LightPosition(position), LightDirection(direction),
		//	  SpotFalloff(cosf(innerAngle), cosf(outAngle), spotFalloff), LightAttenuation(attenuationBegin, attenuationEnd) {}
	};

public:
	LightAnimation();
	~LightAnimation();

	void Move(float elapsedTime);

	void RandonPointLight(int numLight);

	void AddPointLight(const float3& position);

	//void RecordLight(const CFirstPersonCamera& camera, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void SaveLights(const String& filename);
	void LoadLights(const String& filename);

public:
	std::vector<LightParam> mLights;

private:
	float mTotalTime;
};


#endif // LightAnimation_h__

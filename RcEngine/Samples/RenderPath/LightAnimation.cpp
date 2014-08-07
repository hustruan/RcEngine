#include "LightAnimation.h"
#include <fstream>
#include <Core/Environment.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>

#define D3DX_PI 3.1415926f

const float MaxRadius = 100.0f;
const float AttenuationStartFactor = 0.8f;

// Use a constant seed for consistency
std::mt19937 rng(1337);

std::uniform_real<float> RadiusNormDist(0.0f, 1.0f);
std::uniform_real<float> AngleDist(0.0f, 360.0f); 
std::uniform_real<float> HeightDist(0.0f, 20.0f);
std::uniform_real<float> AnimationSpeedDist(2.0f, 20.0f);
std::uniform_int<int> AnimationDirection(0, 1);
std::uniform_real<float> HueDist(0.0f, 1.0f);
std::uniform_real<float> IntensityDist(0.4f, 0.8f);
std::uniform_real<float> AttenuationDist(40.0f, 80.0f);
std::uniform_real<float> SpotInnerAngleDist(30.0f, 45.0f);
std::uniform_real<float> SpotOuterAngleDist(30.0f, 45.0f);
std::uniform_real<float> SpotDropoffDist(0.0f, 50.0f);

namespace {

float3 HueToRGB(float hue)
{
	float intPart;
	float fracPart = modf(hue * 6.0f, &intPart);
	int region = static_cast<int>(intPart);

	switch (region) {
	case 0: return float3(1.0f, fracPart, 0.0f);
	case 1: return float3(1.0f - fracPart, 1.0f, 0.0f);
	case 2: return float3(0.0f, 1.0f, fracPart);
	case 3: return float3(0.0f, 1.0f - fracPart, 1.0f);
	case 4: return float3(fracPart, 0.0f, 1.0f);
	case 5: return float3(1.0f, 0.0f, 1.0f - fracPart);
	};

	return float3(0.0f, 0.0f, 0.0f);
}

}


LightAnimation::LightAnimation()
	: mTotalTime(0.0f)
{

}

LightAnimation::~LightAnimation()
{

}

void LightAnimation::Move( float elapsedTime )
{
	mTotalTime += elapsedTime;

	// Update positions of active lights
	for (unsigned int i = 0; i < mLights.size(); ++i) 
	{
		// Only animate point light
		if (mLights[i].Light->GetLightType() == LT_PointLight)
		{
			float angle = mLights[i].Angle + mTotalTime * mLights[i].AnimationSpeed;

			float3 lightPos = float3(mLights[i].Radius * cosf(angle),
									 mLights[i].Height,
									 mLights[i].Radius * sinf(angle));

			mLights[i].Light->SetPosition(lightPos);
		}		
	}
}

void LightAnimation::RandonPointLight( int numLight )
{
	SceneManager& sceneMan = *Environment::GetSingleton().GetSceneManager();

	for (int i = 0; i < numLight; ++i)
	{
		LightParam lightParam;

		lightParam.Light = sceneMan.CreateLight("PointLight" + std::to_string(i), LT_PointLight);
		lightParam.Light->SetLightColor( IntensityDist(rng) * HueToRGB(HueDist(rng)) );
		lightParam.Light->SetRange(AttenuationDist(rng));

		//light.LightAttenuation.Y() = AttenuationDist(rng);
		//light.LightAttenuation.X() = AttenuationStartFactor * light.LightAttenuation.X();
		
		lightParam.Radius = std::sqrt(RadiusNormDist(rng)) * MaxRadius;
		lightParam.Angle = AngleDist(rng) * D3DX_PI / 180.0f;
		lightParam.Height = HeightDist(rng);
		// Normalize by arc length
		lightParam.AnimationSpeed = (AnimationDirection(rng) * 2 - 1) * AnimationSpeedDist(rng) / lightParam.Radius;

		mLights.push_back(lightParam);
	}

	Move(0);
}

void LightAnimation::AddPointLight( const float3& position )
{
	SceneManager& sceneMan = *Environment::GetSingleton().GetSceneManager();

	LightParam lightParam;

	lightParam.Light = sceneMan.CreateLight("PointLight" + std::to_string(mLights.size()), LT_PointLight);
	lightParam.Light->SetLightColor(HueToRGB(HueDist(rng)));
	lightParam.Light->SetLightIntensity(IntensityDist(rng));
	lightParam.Light->SetRange(AttenuationDist(rng));
	lightParam.Light->SetAttenuation(1.0f, 0.0f);
	lightParam.Light->SetPosition(position);
	sceneMan.GetRootSceneNode()->AttachObject(lightParam.Light);

	//light.LightAttenuation.Y() = AttenuationDist(rng);
	//light.LightAttenuation.X() = AttenuationStartFactor * light.LightAttenuation.X();

	lightParam.Radius = std::sqrt(position.X()*position.X() + position.Z()*position.Z());
	lightParam.Angle = atan2f(position.Z(), position.X());
	lightParam.Height = position.Y();
	// Normalize by arc length
	lightParam.AnimationSpeed = (AnimationDirection(rng) * 2 - 1) * AnimationSpeedDist(rng) / lightParam.Radius;

	mLights.push_back(lightParam);
}


//void LightAnimation::RecordLight( const CFirstPersonCamera& camera, UINT uMsg, WPARAM wParam, LPARAM lParam )
//{
//	switch(uMsg)
//	{
//	case WM_KEYDOWN:
//		{
//			if (wParam == '1' + LT_PointLight)
//			{
//				Light light;
//				
//				light.LightType = LT_PointLight;
//
//				light.LightColor = IntensityDist(rng) * HueToRGB(HueDist(rng));
//				light.LightAttenuation.Y() = AttenuationDist(rng);
//				light.LightAttenuation.X() = AttenuationStartFactor * light.LightAttenuation.Y();
//
//				const float3& cameraPos = *camera.GetEyePt();
//				light.Height = cameraPos.Y();
//				light.Radius = (std::min)(sqrtf(cameraPos.X()*cameraPos.X()+cameraPos.z*cameraPos.z), 100.0f);
//				light.Angle = atan2f(cameraPos.z, cameraPos.X());
//				if (light.Angle < 0) light.Angle += 2.0f * D3DX_PI;
//				
//				// Normalize by arc length
//				light.AnimationSpeed = (AnimationDirection(rng) * 2 - 1) * AnimationSpeedDist(rng) / light.Radius;
//				
//				mLights.push_back(light);
//
//				Move(0);
//			}
//		}
//		break;
//	}
//}

//void LightAnimation::SaveLights( const String& filename )
//{
//	if (mLights.empty())
//		return;
//
//	stream* pFile = fopen(filename.c_str(), "w");
//	if (pFile)
//	{
//		for (size_t i = 0; i < mLights.size(); ++i)
//		{
//			float3 lightColor = mLights[i].Light->GetLightColor();
//			float3 lightPosition = mLights[i].Light->GetDerivedPosition();
//			float intensity = mLights[i].Light->GetLightIntensity();
//			float range = mLights[i].Light->GetRange();
//
//			fprintf(pFile, "%f %f %f %f %f %f %f %f \n", lightPosition[0], lightPosition[1], lightPosition[2],
//												  lightColor[0], lightColor[1], lightColor[2], 
//												  intensity, range);
//		}
//
//		fclose(pFile);
//	}
//}

void LightAnimation::LoadLights( const std::string& filename )
{
	SceneManager& sceneMan = *Environment::GetSingleton().GetSceneManager();

	float3 lightColor, lightPosition;
	float intensity, range;

	std::ifstream stream(filename);
	std::string line;
	while(std::getline(stream, line))
	{
		std::stringstream ss;
		ss << line;
		ss >> lightPosition[0]>>lightPosition[1]>>lightPosition[2]>>
			  lightColor[0]>>lightColor[1]>>lightColor[2]>>intensity>>range;

		LightParam lightParam;

		lightParam.Light = sceneMan.CreateLight("PointLight" + std::to_string(mLights.size()), LT_PointLight);
		lightParam.Light->SetLightColor(lightColor);
		lightParam.Light->SetLightIntensity(intensity);
		lightParam.Light->SetRange(range);
		lightParam.Light->SetAttenuation(1.0f, 0.0f);
		lightParam.Light->SetPosition(lightPosition);
		sceneMan.GetRootSceneNode()->AttachObject(lightParam.Light);

		//light.LightAttenuation.Y() = AttenuationDist(rng);
		//light.LightAttenuation.X() = AttenuationStartFactor * light.LightAttenuation.X();

		lightParam.Radius = std::sqrt(lightPosition.X()*lightPosition.X() + lightPosition.Z()*lightPosition.Z());
		lightParam.Angle = atan2f(lightPosition.Z(), lightPosition.X());
		lightParam.Height = lightPosition.Y();
		// Normalize by arc length
		lightParam.AnimationSpeed = (AnimationDirection(rng) * 2 - 1) * AnimationSpeedDist(rng) / lightParam.Radius;

		mLights.push_back(lightParam);
	}
}





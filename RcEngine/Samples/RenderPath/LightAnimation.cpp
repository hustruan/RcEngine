#include "LightAnimation.h"
#include <fstream>
#include <Core/Environment.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Graphics/DebugDrawManager.h>

const float MaxRadius = 100.0f;
const float AttenuationStartFactor = 0.8f;

const float AnimLineMoveSpeed = 100.0f;

const float SponzaFloorHeightRange[][2] = { { 50.0f, 300.0f },
											 { 480.0f, 730.0f } };
const float3 SponzaExtent[] = {
	float3(1240, 0, -510),
	float3(1240, 0, 610),
	float3(-1350, 0, 610),
	float3(-1350, 0, -510),
};

const float3 SponzaCenter(-100, 0, 55);

// Use a constant seed for consistency
std::mt19937 rng(1337);

std::uniform_real<float> SignedNormDist(-1.0f, 1.0f);
std::uniform_real<float> NormDist(-1.0f, 1.0f);
std::uniform_real<float> RadiusNormDist(0.0f, 1.0f);
std::uniform_real<float> AngleDist(0.0f, Mathf::TWO_PI); 
std::uniform_real<float> AnimationSpeedDist(Mathf::ToRadian(5.0f), Mathf::ToRadian(10.0f));
std::uniform_int<int> AnimationDirection(0, 1);
std::uniform_real<float> HueDist(0.0f, 1.0f);
std::uniform_real<float> IntensityDist(0.4f, 0.8f);
std::uniform_real<float> AttenuationDist(40.0f, 80.0f);

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
	: mNumTotalLights(0)
{

}

LightAnimation::~LightAnimation()
{

}

void LightAnimation::Move( float elapsedTime )
{
	MoveAnimLine(elapsedTime);
	MoveAnimEllipse(elapsedTime);
}

void LightAnimation::DebugDrawLights()
{
	SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();
	const LightQueue& lightQueue = sceneMan->GetLightQueue();

	for (Light* light : lightQueue)
	{
		if (light->GetLightType() == LT_PointLight)
		{
			ColorRGBA color(light->GetLightColor().X(), light->GetLightColor().Y(), light->GetLightColor().Y(), 1.0f);
			DebugDrawManager::GetSingleton().DrawSphere(light->GetDerivedPosition(), light->GetRange() * 0.5f, color, true);
		}
	}
}

void LightAnimation::SetupLights()
{
	CreateLightsWithAnimStatic();
	CreateLightsWithAnimLine();
	CreateLightsWithAnimEllipse();
}

void LightAnimation::CreateLightsWithAnimStatic()
{
	const float3 SponzaExtentInner[] = {
		float3(750,  10, -120),
		float3(750,  10, 230),
		float3(-950, 10, 230),
		float3(-950, 10, -100) };

	SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

	const int NumLightX = 40;
	const int NumLightZ = 12;
	for (int i = 0; i < NumLightZ; ++i)
	{
		float t = float(i) / NumLightZ;
		float3 start = Lerp(SponzaExtentInner[0], SponzaExtentInner[1], t);
		float3 end = Lerp(SponzaExtentInner[3], SponzaExtentInner[2], t);

		for (int j = 0; j < NumLightX; ++j)
		{
			Light* light = sceneMan->CreateLight("", LT_PointLight);
			light->SetPosition(Lerp(start, end, float(j)/NumLightX));

			float3 color = IntensityDist(rng) * HueToRGB(HueDist(rng));
			light->SetLightColor(color);

			light->SetLightIntensity(1.0);
			light->SetAttenuation(1.0f, 0.0f);
			light->SetRange(25.0f);
			sceneMan->GetRootSceneNode()->AttachObject(light);

			mNumTotalLights++;
		}
	}
}

void LightAnimation::CreateLightsWithAnimLine()
{
	SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();
	
	const int NumBandLights[] = { 10, 20 };

	for (int iFloor = 0; iFloor < 2; ++iFloor)
	{
		for (int iLine = 0; iLine < 4; ++iLine)
		{
			const int NumLight = NumBandLights[iLine & 0x1];
			const float3 LeftPos = SponzaExtent[iLine];
			const float3 RightPos = SponzaExtent[(iLine+1)%4];

			for (int32_t iLight = 0; iLight <  NumLight; ++iLight)
			{
				float3 position = Lerp(LeftPos, RightPos, float(iLight) / NumLight);

				position.Y() = SponzaFloorHeightRange[iFloor][0];

				//position.Y() = Lerp(SponzaFloorHeightRange[iFloor][0], SponzaFloorHeightRange[iFloor][1], NormDist(rng));

				float3 color = IntensityDist(rng) * HueToRGB(HueDist(rng));

				Light* light = sceneMan->CreateLight("", LT_PointLight);
				light->SetPosition(position);
				light->SetLightColor(color);
				light->SetLightIntensity(1.0);
				light->SetAttenuation(1.0f, 0.0f);
				light->SetRange(100);
				sceneMan->GetRootSceneNode()->AttachObject(light);

				LineAnimatedLight aminLight;
				aminLight.Light = light;
				aminLight.LineIndex = iLine;
				aminLight.FloorIndex = iFloor;
				mLineAnimatedLights.push_back(aminLight);

				mNumTotalLights++;
			}
		}
	}
}

void LightAnimation::MoveAnimLine( float elapsedTime )
{
	for (LineAnimatedLight& lineLight : mLineAnimatedLights)
	{
		float3 pos = lineLight.Light->GetDerivedPosition();

		switch (lineLight.LineIndex)
		{
		case 0:
			{
				pos.Z() += AnimLineMoveSpeed * elapsedTime;
				if (pos.Z() > SponzaExtent[1].Z())
				{
					pos.Z() = SponzaExtent[1].Z();
					lineLight.LineIndex = 1;
				}
			}
			break;
		case 1:
			{
				pos.X() -= AnimLineMoveSpeed * elapsedTime;
				if (pos.X() < SponzaExtent[2].X())
				{
					pos.X() = SponzaExtent[2].X();
					lineLight.LineIndex = 2;
				}
			}
			break;
		case 2:
			{
				pos.Z() -= AnimLineMoveSpeed * elapsedTime;
				if (pos.Z() < SponzaExtent[3].Z())
				{
					pos.Z() = SponzaExtent[3].Z();
					lineLight.LineIndex = 3;
				}
			}
			break;
		case 3:
			{
				pos.X() += AnimLineMoveSpeed * elapsedTime;
				if (pos.X() > SponzaExtent[0].X())
				{
					pos.X() = SponzaExtent[0].X();
					lineLight.LineIndex = 0;
				}
			}
			break;
		default:
			break;
		}

		lineLight.Light->SetPosition(pos);
	}
}

void LightAnimation::CreateLightsWithAnimEllipse()
{
	SceneManager* sceneMan = Environment::GetSingleton().GetSceneManager();

	const int NumEllipses = 10;
	const int NumLightsEllipse = 40;

	for (int i = 0; i < NumEllipses; ++i)
	{
		for (int j = 0; j < NumLightsEllipse; ++j)
		{
			EllipseAnimatedLight aminLight;

			aminLight.EllipseWidth = 950.0f;
			aminLight.EllipseHeight = 175.0f;
			aminLight.Height = Lerp(100.0f, 1000.0f, float(i) / NumEllipses);

			//aminLight.Angle = Mathf::TWO_PI / NumLightsEllipse * j;
			aminLight.Angle = AngleDist(rng);
			aminLight.AnimationSpeed = (AnimationDirection(rng) * 2 - 1) * AnimationSpeedDist(rng);

			float3 pos(aminLight.EllipseWidth * cosf(aminLight.Angle), aminLight.Height, aminLight.EllipseHeight * sinf(aminLight.Angle));
			float3 color = IntensityDist(rng) * HueToRGB(HueDist(rng));

			Light* light = sceneMan->CreateLight("", LT_PointLight);
			light->SetPosition(pos);
			light->SetLightColor(color);
			light->SetLightIntensity(1.0);
			light->SetAttenuation(1.0f, 0.0f);
			light->SetRange(100);
			sceneMan->GetRootSceneNode()->AttachObject(light);

			aminLight.Light = light;
			mEllipseAnimatedLights.push_back(aminLight);

			mNumTotalLights++;
		}
	}
}

void LightAnimation::MoveAnimEllipse( float elapsedTime )
{
	for (EllipseAnimatedLight& aminLight : mEllipseAnimatedLights)
	{
		aminLight.Angle += aminLight.AnimationSpeed * elapsedTime;
		float3 pos(aminLight.EllipseWidth * cosf(aminLight.Angle), aminLight.Height, aminLight.EllipseHeight * sinf(aminLight.Angle));
		aminLight.Light->SetPosition(pos);
	}
}




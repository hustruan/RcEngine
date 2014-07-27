#ifndef GraphicsScriptLoaderInternal_h__
#define GraphicsScriptLoaderInternal_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/RenderState.h>
#include <Graphics/GraphicsResource.h>
#include <Core/XMLDom.h>
#include <Core/Exception.h>
#include <Core/Utility.h>
#include <Math/Vector.h>

namespace RcEngine {

namespace Internal {

inline bool StringToBool(const String& str)
{
	if (str == "true")
		return true;
	return false;
}

inline uint8_t ColorWriteMask(bool r, bool g, bool b, bool a)
{
	uint8_t mask = 0;
	if (r)	mask |= CWM_Red;
	if (g)	mask |= CWM_Green;
	if (b)	mask |= CWM_Blue;
	if (a)	mask |= CWM_Alpha;
	return mask;
}

inline float3 StringToFloat3(const String& str)
{
	float3 value;
	std::sscanf(str.c_str(), "%f %f %f", &value[0], &value[1], &value[2]);
	return value;
}

class StateDescDefs
{
private:
	StateDescDefs()
	{
		mDefs.insert(std::make_pair("Never", CF_Never));
		mDefs.insert(std::make_pair("Always",  CF_Always));
		mDefs.insert(std::make_pair("Less", CF_Less));
		mDefs.insert(std::make_pair("LessEqual", CF_LessEqual));
		mDefs.insert(std::make_pair("Equal", CF_Equal));
		mDefs.insert(std::make_pair("NotEqual", CF_NotEqual));
		mDefs.insert(std::make_pair("GreaterEqual", CF_GreaterEqual));
		mDefs.insert(std::make_pair("Greater", CF_Greater));

		mDefs.insert(std::make_pair("Keep", SOP_Keep));
		mDefs.insert(std::make_pair("Zero",  SOP_Zero));
		mDefs.insert(std::make_pair("Replace", SOP_Replace));
		mDefs.insert(std::make_pair("Incr", SOP_Incr));
		mDefs.insert(std::make_pair("Decr", SOP_Decr));
		mDefs.insert(std::make_pair("Invert", SOP_Invert));
		mDefs.insert(std::make_pair("IncrWrap", SOP_Incr_Wrap));
		mDefs.insert(std::make_pair("DecrWrap", SOP_Decr_Wrap));

		mDefs.insert(std::make_pair("Solid", FM_Solid));
		mDefs.insert(std::make_pair("WireFrame",  FM_WireFrame));

		mDefs.insert(std::make_pair("None", CM_None));
		mDefs.insert(std::make_pair("Front", CM_Front));
		mDefs.insert(std::make_pair("Back", CM_Back));

		mDefs.insert(std::make_pair("Add", BOP_Add));
		mDefs.insert(std::make_pair("Sub", BOP_Sub));
		mDefs.insert(std::make_pair("RevSub", BOP_Rev_Sub));
		mDefs.insert(std::make_pair("Max", BOP_Max));
		mDefs.insert(std::make_pair("Min", BOP_Min));

		//mDefs.insert(std::make_pair("Zero",  ABF_Zero));
		mDefs.insert(std::make_pair("One", ABF_One));
		mDefs.insert(std::make_pair("SrcAlpha", ABF_Src_Alpha));
		mDefs.insert(std::make_pair("InvSrcAlpha", ABF_Inv_Src_Alpha));
		mDefs.insert(std::make_pair("DstAlpha", ABF_Dst_Alpha));
		mDefs.insert(std::make_pair("InvDestAlpha", ABF_Inv_Dst_Alpha));
		mDefs.insert(std::make_pair("SrcColor", ABF_Src_Color));
		mDefs.insert(std::make_pair("DestColor", ABF_Dst_Color));
		mDefs.insert(std::make_pair("InvSrcColor", ABF_Inv_Src_Color));
		mDefs.insert(std::make_pair("InvDestColor", ABF_Inv_Dst_Color));
		mDefs.insert(std::make_pair("SrcAlphaSat", ABF_Src_Alpha_Sat));
	}

public:
	static StateDescDefs& GetInstance()
	{
		static StateDescDefs singleton;
		return singleton;
	}

	uint32_t GetState(const String& str)
	{
		unordered_map<String, uint32_t>::const_iterator iter = mDefs.find(str);
		if (iter == mDefs.end())
			ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "State parameter " + str + "error!", "StateDescDefs::GetState");

		return iter->second;
	}

private:
	unordered_map<String, uint32_t> mDefs;
};

class SamplerDefs
{
public:
	static SamplerDefs& GetSingleton() 
	{
		static SamplerDefs singleton;
		return singleton;
	}

	uint32_t GetSamplerState(const String& name)
	{
		auto iter = mDefs.find(name);
		if (iter != mDefs.end())
			return iter->second;

		return TF_Min_Mag_Mip_Point;
	}

private:
	SamplerDefs()
	{
		mDefs.insert(std::make_pair("Min_Mag_Map_Point", TF_Min_Mag_Mip_Point));
		mDefs.insert(std::make_pair("Min_Point_Map_Linear_Mip_Point", TF_Min_Mag_Point_Mip_Linear));
		mDefs.insert(std::make_pair("Min_Point_Mag_Linear_Mip_Point", TF_Min_Point_Mag_Linear_Mip_Point));
		mDefs.insert(std::make_pair("Min_Point_Mag_Mip_Linear", TF_Min_Point_Mag_Mip_Linear));
		mDefs.insert(std::make_pair("Min_Linear_Mag_Mip_Point", TF_Min_Linear_Mag_Mip_Point));
		mDefs.insert(std::make_pair("Min_Linear_Mag_Point_Mip_Linear", TF_Min_Linear_Mag_Point_Mip_Linear));
		mDefs.insert(std::make_pair("Min_Mag_Linear_Mip_Point", TF_Min_Mag_Linear_Mip_Point));
		mDefs.insert(std::make_pair("Min_Mag_Mip_Linear", TF_Min_Mag_Mip_Linear));
		mDefs.insert(std::make_pair("Anisotropic", TF_Anisotropic));

		mDefs.insert(std::make_pair("Wrap", TAM_Wrap));
		mDefs.insert(std::make_pair("Mirror", TAM_Mirror));
		mDefs.insert(std::make_pair("Clamp", TAM_Clamp));
		mDefs.insert(std::make_pair("Border", TAM_Border));
		mDefs.insert(std::make_pair("MirrorOnce", TAM_Mirror_Once));

		mDefs.insert(std::make_pair("Never", CF_Never));
		mDefs.insert(std::make_pair("Alway",  CF_Always));
		mDefs.insert(std::make_pair("Less", CF_Less));
		mDefs.insert(std::make_pair("LessEqual", CF_LessEqual));
		mDefs.insert(std::make_pair("Equal", CF_Equal));
		mDefs.insert(std::make_pair("NotEqual", CF_NotEqual));
		mDefs.insert(std::make_pair("GreaterEqual", CF_GreaterEqual));
		mDefs.insert(std::make_pair("Greater", CF_Greater));

		mDefs.insert(std::make_pair("VertexShader", ST_Vertex));
		mDefs.insert(std::make_pair("PixelShader", ST_Pixel));
	}

private:
	unordered_map<String, uint32_t> mDefs;
};

class EffectParamsUsageDefs
{
public:
	EffectParamsUsageDefs()
	{
		mDefs.insert(std::make_pair("WorldMatrix", EPU_WorldMatrix));
		mDefs.insert(std::make_pair("ViewMatrix",  EPU_ViewMatrix));
		mDefs.insert(std::make_pair("ProjectionMatrix", EPU_ProjectionMatrix));
		mDefs.insert(std::make_pair("WorldViewMatrix", EPU_WorldViewMatrix));
		mDefs.insert(std::make_pair("ViewProjectionMatrix", EPU_ViewProjectionMatrix));
		mDefs.insert(std::make_pair("WorldViewProjection", EPU_WorldViewProjection));
		mDefs.insert(std::make_pair("WorldInverseTranspose", EPU_WorldInverseTranspose));
		mDefs.insert(std::make_pair("WorldMatrixInverse", EPU_WorldMatrixInverse));
		mDefs.insert(std::make_pair("ViewMatrixInverse", EPU_ViewMatrixInverse));
		mDefs.insert(std::make_pair("ProjectionMatrixInverse", EPU_ProjectionMatrixInverse));
		mDefs.insert(std::make_pair("AmbientMaterialColor", EPU_Material_Ambient_Color));
		mDefs.insert(std::make_pair("DiffuseMaterialColor", EPU_Material_Diffuse_Color));
		mDefs.insert(std::make_pair("SpecularMaterialColor", EPU_Material_Specular_Color));
		mDefs.insert(std::make_pair("SpecularMaterialPower", EPU_Material_Power));
		mDefs.insert(std::make_pair("EmissiveMaterialColor", EPU_Material_Emissive_Color));
		mDefs.insert(std::make_pair("DiffuseMaterialMap", EPU_Material_DiffuseMap));
		mDefs.insert(std::make_pair("SpecularMaterialMap", EPU_Material_SpecularMap));
		mDefs.insert(std::make_pair("NormalMaterialMap", EPU_Material_NormalMap));
		mDefs.insert(std::make_pair("LightColor", EPU_Light_Color));
		mDefs.insert(std::make_pair("LightFalloff", EPU_Light_Attenuation));
		mDefs.insert(std::make_pair("LightDirection", EPU_Light_Dir));
		mDefs.insert(std::make_pair("LightPosition", EPU_Light_Position));
		mDefs.insert(std::make_pair("CameraPosition", EPU_Camera_Position));
	}

	static EffectParamsUsageDefs& GetInstance()
	{
		static EffectParamsUsageDefs singleton;
		return singleton;
	}

	EffectParameterUsage GetUsageType(const String& str)
	{
		unordered_map<String, EffectParameterUsage>::iterator iter = mDefs.find(str);
		if (iter != mDefs.end())
			return iter->second;

		return EPU_Unknown;
	}

private:
	unordered_map<String, EffectParameterUsage> mDefs;
};

void CollectRenderStates(XMLNodePtr passNode, DepthStencilStateDesc& dsDesc, BlendStateDesc& blendDesc, RasterizerStateDesc& rasDesc,
								ColorRGBA& blendFactor, uint32_t& sampleMask, uint16_t& frontStencilRef, uint16_t& backStencilRef);


void CollectShaderMacro(const XMLNodePtr& node, std::vector<ShaderMacro>& shaderMacros);

}

}


#endif // GraphicsScriptLoaderInternal_h__

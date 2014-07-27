#include <Graphics/GraphicsScriptLoader.h>

namespace RcEngine {

void Internal::CollectRenderStates( XMLNodePtr passNode, DepthStencilStateDesc& dsDesc, BlendStateDesc& blendDesc, RasterizerStateDesc& rasDesc, ColorRGBA& blendFactor, uint32_t& sampleMask, uint16_t& frontStencilRef, uint16_t& backStencilRef )
{
	XMLNodePtr stateNode;
	for (stateNode = passNode->FirstNode("State");  stateNode; stateNode = stateNode->NextSibling("State"))
	{
		String name = stateNode->AttributeString("name", "");
		String value = stateNode->AttributeString("value", "");

		if (name == "DepthEnable")
		{
			dsDesc.DepthEnable = StringToBool(value);
		}
		else if (name == "DepthWriteMask")
		{
			dsDesc.DepthWriteMask = StringToBool(value);
		}
		else if (name == "DepthFunc")
		{
			dsDesc.DepthFunc = (CompareFunction)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "StencilEnable")
		{
			dsDesc.StencilEnable = StringToBool(value);
		}
		else if (name == "StencilReadMask")
		{
			dsDesc.StencilReadMask = LexicalCast<uint8_t>(value);
		}
		else if (name == "StencilWriteMask")
		{
			dsDesc.StencilWriteMask = LexicalCast<uint8_t>(value);
		}
		else if (name == "FrontStencilRef")
		{
			frontStencilRef = LexicalCast<uint16_t>(value);
		}
		else if (name == "BackStencilRef")
		{
			backStencilRef = LexicalCast<uint16_t>(value);
		}
		else if (name == "FrontStencilFailOp")
		{
			dsDesc.FrontStencilFailOp = (StencilOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "FrontStencilPassOp")
		{
			dsDesc.FrontStencilPassOp = (StencilOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "FrontStencilDepthFailOp")
		{
			dsDesc.FrontStencilDepthFailOp = (StencilOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "FrontStencilFunc")
		{
			dsDesc.FrontStencilFunc = (CompareFunction)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "BackStencilFailOp")
		{
			dsDesc.BackStencilFailOp = (StencilOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "BackStencilPassOp")
		{
			dsDesc.BackStencilPassOp = (StencilOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "BackStencilDepthFailOp")
		{
			dsDesc.BackStencilDepthFailOp = (StencilOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "BackStencilFunc")
		{
			dsDesc.BackStencilFunc = (CompareFunction)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "AlphaToCoverageEnable")
		{
			blendDesc.AlphaToCoverageEnable = StringToBool(value);
		}
		else if (name == "IndependentBlendEnable")
		{
			blendDesc.IndependentBlendEnable  = StringToBool(value);
		}
		else if (name == "BlendEnable")
		{
			uint32_t index = stateNode->AttributeUInt("index", 0);
			blendDesc.RenderTarget[index].BlendEnable = StringToBool(value);
		}
		else if (name == "SrcBlend")
		{	
			uint32_t index = stateNode->AttributeUInt("index", 0);
			blendDesc.RenderTarget[index].SrcBlend = (AlphaBlendFactor)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "DestBlend")
		{
			uint32_t index = stateNode->AttributeUInt("index", 0);
			blendDesc.RenderTarget[index].DestBlend = (AlphaBlendFactor)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "BlendOp")
		{
			uint32_t index = stateNode->AttributeUInt("index", 0);
			blendDesc.RenderTarget[index].BlendOp = (BlendOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "SrcBlendAlpha")
		{
			uint32_t index = stateNode->AttributeUInt("index", 0);
			blendDesc.RenderTarget[index].SrcBlendAlpha = (AlphaBlendFactor)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "DestBlendAlpha")
		{
			uint32_t index = stateNode->AttributeUInt("index", 0);
			blendDesc.RenderTarget[index].DestBlendAlpha = (AlphaBlendFactor)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "BlendOpAlpha")
		{
			uint32_t index = stateNode->AttributeUInt("index", 0);
			blendDesc.RenderTarget[index].BlendOpAlpha = (BlendOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "ColorWriteMask")
		{
			uint32_t index = stateNode->AttributeUInt("index", 0);
			bool rEnable = StringToBool( stateNode->AttributeString("r", "true") );
			bool gEnable = StringToBool( stateNode->AttributeString("g", "true") );
			bool bEnable = StringToBool( stateNode->AttributeString("b", "true") );
			bool aEnable = StringToBool( stateNode->AttributeString("a", "true") );

			blendDesc.RenderTarget[index].ColorWriteMask = ColorWriteMask(rEnable, gEnable, bEnable, aEnable);
		}
		else if (name == "BlendFactor")
		{
			float r = stateNode->AttributeFloat("r", 0);
			float g = stateNode->AttributeFloat("g", 0);
			float b = stateNode->AttributeFloat("b", 0);
			float a = stateNode->AttributeFloat("a", 0);
			blendFactor = ColorRGBA(r,g,b,a);
		}
		else if (name == "SampleMask")
		{
			sampleMask = LexicalCast<uint32_t>(value);
		}
		else if (name == "FillMode")
		{
			rasDesc.PolygonFillMode = (FillMode)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "CullMode")
		{
			rasDesc.PolygonCullMode = (CullMode)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "FrontCounterClockwise")
		{
			rasDesc.FrontCounterClockwise = StringToBool(value);
		}
		else if (name == "DepthBias")
		{
			rasDesc.DepthBias = LexicalCast<float>(value);
		}
		else if (name == "SlopeScaledDepthBias")
		{
			rasDesc.SlopeScaledDepthBias = LexicalCast<float>(value);
		}
		else if (name == "DepthClipEnable")
		{
			rasDesc.DepthClipEnable = StringToBool(value);
		}
		else if (name == "ScissorEnable")
		{
			rasDesc.ScissorEnable = StringToBool(value);
		}
		else if (name == "MultisampleEnable")
		{
			rasDesc.MultisampleEnable = StringToBool(value);
		}
		else
		{
			ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, name + ": Unknown effect state!",
				"CollectRenderStates");
		}
	}
}

void Internal::CollectShaderMacro( const XMLNodePtr& node, std::vector<ShaderMacro>& shaderMacros )
{
	for (XMLNodePtr macroNode = node->FirstNode("Macro"); macroNode; macroNode = macroNode->NextSibling("Macro"))
	{
		ShaderMacro macro;
		macro.Name = macroNode->Attribute("name")->ValueString();	
		macro.Definition = macroNode->AttributeString("value", "");
		shaderMacros.push_back(macro);
	}
}

}
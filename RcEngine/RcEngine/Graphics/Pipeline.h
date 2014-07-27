#ifndef Pipeline_h__
#define Pipeline_h__

#include <Core/Prerequisites.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/ColorRGBA.h>
#include <Resource/Resource.h>
#include <Core/XMLDom.h>
#include <Core/Variant.h>

namespace RcEngine {

class _ApiExport PipelineCmdParam
{
public:
	PipelineCmdParam() : ValueString(nullptr) {}
	~PipelineCmdParam() { SAFE_DELETE(ValueString); }

	PipelineCmdParam(const PipelineCmdParam& rhs) 
		: ValueBasic(rhs.ValueBasic), ValueString(nullptr)
	{
		if (rhs.ValueString) 
			ValueString = new String(rhs.ValueString->c_str());
	}

	PipelineCmdParam& operator = (const PipelineCmdParam& rhs)
	{
		ValueBasic = rhs.ValueBasic;
		if (rhs.ValueString) 
			ValueString = new String(rhs.ValueString->c_str());

		return *this;
	}

	bool GetBoolean() const									{ return ValueBasic.ValueBoolean; }
	float GetFloat() const									{ return ValueBasic.ValueFloat; }
	int32_t GetInt() const									{ return ValueBasic.ValueInt; }
	
	void SetFloat(float f)									{ ValueBasic.ValueFloat = f; }
	void SetInt(int32_t i)									{ ValueBasic.ValueInt = i; }
	void SetBoolean(bool b)									{ ValueBasic.ValueBoolean = b; }

	const String& GetString() const							{ return *ValueString; }	
	void SetString(const String& str)
	{
		SAFE_DELETE(ValueString);
		ValueString = new String(str);
	}
	
	const ColorRGBA& GetColor() const
	{
		return (*reinterpret_cast<const ColorRGBA*>(&ValueBasic));
	}

	void SetColor(const ColorRGBA& clr)
	{
		(*reinterpret_cast<ColorRGBA*>(&ValueBasic)) = clr;
	}


private:

	struct VariantValue 
	{
		union 
		{
			int32_t ValueInt;
			float ValueFloat;
			bool ValueBoolean;
		};

		float Pad[3]; // for float4
	};

	VariantValue ValueBasic;
	String* ValueString;
};



class _ApiExport Pipeline : public Resource
{
	friend class Renderer;

public:
	struct RenderTarget
	{
		RenderTarget() : SampleCount(1), SampleQuality(0) {}

		Attachment Attach;
		PixelFormat Format;
		uint32_t SampleCount;
		uint32_t SampleQuality;

		shared_ptr<Texture> RenderTexture;
	};

	struct PipeFrameBuffer
	{
		String Name;
		uint32_t Width, Height;
		std::vector<RenderTarget> RenderTargets;
		shared_ptr<FrameBuffer> FrameBuffer;
	};

	enum CommandFlag
	{
		SwitchFrameBuffer,
		ClearTarget,
		BindBuffer,
		DrawGeometry,
		DrawLightShape,
		DrawQuad,
		DrawOverlays,	
		SetUniform,
	};

	struct PipelineCommand
	{
		CommandFlag	Command;
		std::vector< PipelineCmdParam > Params;
		std::vector< PipelineCommand* > SubCommands;
		PipelineCommand( CommandFlag command ) : Command(command) { }
		~PipelineCommand() { for (PipelineCommand* subcmd : SubCommands) delete subcmd; }
	};

	struct PipelineStage
	{
		String Name;
		shared_ptr<Material> MaterialLink;
		std::vector<PipelineCommand*> Commands;
		bool Enabled;

		PipelineStage() : MaterialLink( 0x0 ) {}
	};

public:
	Pipeline(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	~Pipeline();

	void Resize(uint32_t width, uint32_t height);

	const shared_ptr<Material>& GetMaterial(int32_t idx) const			{ return mMaterials[idx]; }
	const shared_ptr<FrameBuffer>& GetFrameBuffer(int32_t idx) const	{ return mFrameBuffers[idx].FrameBuffer; }
	const shared_ptr<Texture>& GetRenderTarget(int32_t fbIdx, int32_t rtIdx) const
	{ 
		return mFrameBuffers[fbIdx].RenderTargets[rtIdx].RenderTexture;
	}

	

public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

private:
	void LoadImpl();
	void UnloadImpl();

	void ParseStage(const XMLNodePtr& node);
	void ParseSetup(const XMLNodePtr& node);
	int32_t LoadMaterial(const String& resName, const String& resGroup);

	void CreateFrameBuffers();

	int32_t GetFrameBufferIndex(const String& name) const;
	int32_t GetRenderTargetIndex(const String& fbName, Attachment attach) const;

private:
	String mPipelineName;
	std::vector<PipeFrameBuffer> mFrameBuffers;
	std::vector<PipelineStage> mPipelineStages;
	std::vector<shared_ptr<Material>> mMaterials; 
};

}


#endif // Pipeline_h__
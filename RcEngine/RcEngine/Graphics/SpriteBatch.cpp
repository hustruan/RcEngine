#include <Graphics/SpriteBatch.h>
#include <Graphics/Renderable.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/Material.h>
#include <Graphics/Effect.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/GraphicsResource.h>
#include <Graphics/VertexDeclaration.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/Font.h>
#include <Graphics/Camera.h>
#include <Graphics/RenderOperation.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneObject.h>
#include <Scene/SceneNode.h>
#include <Resource/ResourceManager.h>
#include <Math/MathUtil.h>
#include <Core/Environment.h>
#include <Core/Exception.h>
#include <MainApp/Application.h>
#include <MainApp/Window.h>


namespace {

using namespace RcEngine;

template<typename Real>
static void Rotate(Vector<Real,2>& point, const Vector<Real,2>& origin, Real rotation)
{
	Real cosAngle = cos(rotation);
	Real sinAngle = sin(rotation);

	Real newX, newY;

	if (origin.X() == Real(0) && origin.Y() == Real(0))
	{
		newX = point.X() * cosAngle - point.Y() * sinAngle;
		newY = point.X() * sinAngle + point.Y() * cosAngle;

	}
	else
	{
		point -= origin;
		newX = point.X() * cosAngle - point.Y() * sinAngle + origin.X();
		newY = point.X() * sinAngle + point.Y() * cosAngle + origin.Y();
	}	

	point = Vector<Real,2>(newX, newY);
}

}

namespace RcEngine {

SpriteBatch::SpriteBatch()
{
	mEffect = ResourceManager::GetSingleton().GetResourceByName<Effect>(RT_Effect, "Sprite.effect.xml", "General");
	mSpriteTexParam = mEffect->GetParameterByName("SpriteTexture");
}	

SpriteBatch::SpriteBatch( const shared_ptr<Effect>& effect )
{
	assert(effect && effect->IsLoaded() == true);
	mEffect = effect;
	mSpriteTexParam = mEffect->GetParameterByName("SpriteTexture");
}

SpriteBatch::~SpriteBatch()
{

}

void SpriteBatch::Begin( )
{
	std::map<shared_ptr<Texture>, Sprite*>::iterator it;
	for (it = mBatches.begin(); it != mBatches.end(); ++it)
		it->second->ClearAll();
}

void SpriteBatch::End()
{
	// Todo:
	// Remove empty SpriteEntity
}

void SpriteBatch::Draw( const shared_ptr<Texture>& texture, const Rectanglef& dest, const IntRect* src, const ColorRGBA& color, float rotAngle /*= 0*/, const float2& origin /*= float2::Zero()*/, float layerDepth /*= 0.0f*/ )
{
	if (color.A() <= 0)
		return;

	uint32_t texWidth = texture->GetWidth();
	uint32_t texHeight = texture->GetHeight();

	IntRect srcRect = src ? (*src) : IntRect(0, 0, texWidth, texHeight);

	if (mBatches.find(texture) == mBatches.end())
	{
		mBatches[texture] = new Sprite(*this, texture);
	}

	Sprite* spriteEntity = mBatches[texture];
	assert(spriteEntity);
	
	float2 topLeft = float2(dest.Left(), dest.Top());
	float2 topRight = float2(dest.Right(), dest.Top());
	float2 bottomLeft = float2(dest.Left(), dest.Bottom());
	float2 bottomRight = float2(dest.Right(), dest.Bottom());

	float2 pivotPoint(origin);
	pivotPoint.X() *= dest.Width;
	pivotPoint.Y() *= dest.Height;
	pivotPoint.X() += dest.X;
	pivotPoint.Y() += dest.Y;
	
	Rotate(topLeft, pivotPoint, rotAngle);
	Rotate(topRight, pivotPoint, rotAngle);
	Rotate(bottomLeft, pivotPoint, rotAngle);
	Rotate(bottomRight, pivotPoint, rotAngle);

	// Calculate the points on the texture
	float u1 = srcRect.Left() / (float)texWidth;
	float v1 = srcRect.Top()/ (float)texHeight;
	float u2 = srcRect.Right() / (float)texWidth;
	float v2 = srcRect.Bottom() / (float)texHeight;

	vector<SpriteVertex>& vertices = spriteEntity->GetVertices();
	vector<uint16_t>& indices = spriteEntity->GetIndices();

	uint16_t lastIndex = static_cast<uint16_t>(vertices.size());

	SpriteVertex spriteVertex;
	vertices.reserve(vertices.size() + 4);
	
	spriteVertex.Position = float3(topLeft.X(), topLeft.Y(), layerDepth);
	spriteVertex.TexCoord = float2(u1, v1);
	spriteVertex.Color = color;
	vertices.push_back(spriteVertex);

	spriteVertex.Position = float3(bottomLeft.X(), bottomLeft.Y(), layerDepth);
	spriteVertex.TexCoord = float2(u1, v2);
	spriteVertex.Color = color;
	vertices.push_back(spriteVertex);

	spriteVertex.Position = float3(bottomRight.X(), bottomRight.Y(), layerDepth);
	spriteVertex.TexCoord = float2(u2, v2);
	spriteVertex.Color = color;
	vertices.push_back(spriteVertex);

	spriteVertex.Position = float3(topRight.X(), topRight.Y(), layerDepth);
	spriteVertex.TexCoord = float2(u2, v1);
	spriteVertex.Color = color;
	vertices.push_back(spriteVertex);
	
	indices.reserve(indices.size() + 6);
	indices.push_back(lastIndex + 0);
	indices.push_back(lastIndex + 1);
	indices.push_back(lastIndex + 2);
	indices.push_back(lastIndex + 2);
	indices.push_back(lastIndex + 3);
	indices.push_back(lastIndex + 0);
}

void SpriteBatch::Draw( const shared_ptr<Texture>& texture, const float2& position, const ColorRGBA& color, float layerDepth /*= 0.0f*/  )
{
	Draw(texture, position, nullptr, color, layerDepth);
}

void SpriteBatch::Draw( const shared_ptr<Texture>& texture, const float2& position, const IntRect* sourceRectangle, const ColorRGBA& color, float layerDepth /*= 0.0f*/  )
{
	Rectanglef destination(position.X(), position.Y(), (float)texture->GetWidth(), (float)texture->GetHeight());
	Draw(texture, destination, sourceRectangle, color, 0.0f, float2::Zero(), layerDepth);
}

void SpriteBatch::Draw( const shared_ptr<Texture>& texture, const Rectanglef& destinationRectangle, const ColorRGBA& color , float layerDepth /*= 0.0f*/  )
{
	Draw(texture, destinationRectangle, NULL, color, 0.0f, float2::Zero(), layerDepth);
}

void SpriteBatch::Draw( const shared_ptr<Texture>& texture, const Rectanglef& destinationRectangle, const IntRect* sourceRectangle, const ColorRGBA& color , float layerDepth /*= 0.0f*/  )
{
	Draw(texture, destinationRectangle, sourceRectangle, color, 0.0f, float2::Zero(), layerDepth);
}

void SpriteBatch::Draw( const shared_ptr<Texture>& texture, const float2& position, const IntRect* sourceRectangle, const ColorRGBA& color, float rotation, const float2& origin, float scale, float layerDepth )
{
	int32_t width, height;
	if (sourceRectangle)
	{
		width = (int32_t)(sourceRectangle->Width * scale);
		height = (int32_t)(sourceRectangle->Height * scale);
	}
	else
	{
		width = (int32_t)(texture->GetWidth() * scale);
		height = (int32_t)(texture->GetHeight() * scale);
	}

    Rectanglef destination = Rectanglef(position.X(), position.Y(), (float)width, (float)height);
	Draw(texture, destination, sourceRectangle, color, rotation, float2::Zero(), layerDepth);
}

void SpriteBatch::Draw( const shared_ptr<Texture>& texture, const float2& position, const IntRect* sourceRectangle, const ColorRGBA& color, float rotation, const float2& origin, const float2& scale, float layerDepth )
{
	int32_t width, height;
	if (sourceRectangle)
	{
		width = (int32_t)(sourceRectangle->Width * scale.X());
		height = (int32_t)(sourceRectangle->Height * scale.Y());
	}
	else
	{
		width = (int32_t)(texture->GetWidth() * scale.X());
		height = (int32_t)(texture->GetHeight() * scale.Y());
	}

	Rectanglef destination = Rectanglef(position.X(), position.Y(), (float)width, (float)height);
	Draw(texture, destination, sourceRectangle, color, rotation, float2::Zero(), layerDepth);
}

void SpriteBatch::Flush()
{
	Window* mainWindow = Application::msApp->GetMainWindow();
	mInvWindowSize = float2(1.0f / mainWindow->GetWidth(), 1.0f / mainWindow->GetHeight());
	mEffect->GetParameterByName("InvWindowSize")->SetValue(mInvWindowSize);

	for (auto it = mBatches.begin(); it != mBatches.end(); ++it)
		it->second->UpdateGeometryBuffers();       
}

void SpriteBatch::OnUpdateRenderQueue( RenderQueue& renderQueue )
{
	for (auto it = mBatches.begin(); it != mBatches.end(); ++it)
	{
		RenderQueueItem item;
		item.Renderable = it->second;

		// ignore render order, only handle state change order
		item.SortKey = (float)mEffect->GetResourceHandle();
		renderQueue.AddToQueue(item, RenderQueue::BucketOverlay);
	}
}

//-----------------------------------------------------------------------------------------------------------------
Sprite::Sprite( SpriteBatch& batch, shared_ptr<Texture> texture )
	: mBatch(batch),
	  mSpriteTexture(texture),
	  mDirty(true)
{
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	ResizeGeometryBuffers(NumInitSprites*4, NumInitSprites*6);

	VertexElement elements[3] = 
	{
		VertexElement(0, VEF_Float3, VEU_Position, 0),
		VertexElement(sizeof(float3), VEF_Float2, VEU_TextureCoordinate, 0),
		VertexElement(sizeof(float3) + sizeof(float2), VEF_Float4, VEU_Color, 0)
	};

	static_assert(sizeof(SpriteVertex) == sizeof(float) *9, "Error");

	mRenderOperation = std::make_shared<RenderOperation>();
	mRenderOperation->PrimitiveType = PT_Triangle_List;
	mRenderOperation->BindVertexStream(0, mVertexBuffer);
	mRenderOperation->BindIndexStream(mIndexBuffer, IBT_Bit16);
	mRenderOperation->VertexDecl = factory->CreateVertexDeclaration(elements, ARRAY_SIZE(elements));
}

Sprite::~Sprite()
{
	
}

void Sprite::UpdateGeometryBuffers()
{
	if (mDirty)
	{
		if (mVertices.size() && mInidces.size())
		{
			ResizeGeometryBuffers(mVertices.size(), mInidces.size());

			uint32_t vbSize = sizeof(SpriteVertex) * mVertices.size();
			uint32_t ibSize = sizeof(uint16_t) * mInidces.size();

			void* vbData = mVertexBuffer->Map(0, vbSize, RMA_Write_Discard);
			memcpy(vbData, &mVertices[0], vbSize);
			mVertexBuffer->UnMap();

			void* ibData = mIndexBuffer->Map(0, ibSize, RMA_Write_Discard);
			memcpy(ibData, &mInidces[0], ibSize);
			mIndexBuffer->UnMap();
		}
		
		mRenderOperation->IndexStart = 0;
		mRenderOperation->IndexCount = mInidces.size();

		mDirty = false;
	}
}

void Sprite::ResizeGeometryBuffers( size_t numVertex, size_t numIndex )
{
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	uint32_t vbSize = numVertex * sizeof(SpriteVertex);
	uint32_t ibSize = numIndex * sizeof(uint16_t);

	if (!mVertexBuffer)
	{
		mVertexBuffer = factory->CreateIndexBuffer(vbSize, EAH_CPU_Write | EAH_GPU_Read, BufferCreate_Vertex, NULL);
	}
	else 
	{
		uint32_t currVBSize = mVertexBuffer->GetBufferSize();
		if (currVBSize < vbSize)
		{
			mVertexBuffer = factory->CreateVertexBuffer(currVBSize * 2, EAH_CPU_Write | EAH_GPU_Read, BufferCreate_Vertex, NULL);
			mRenderOperation->BindVertexStream(0, mVertexBuffer);
		}
	}

	if (!mIndexBuffer)
	{
		mIndexBuffer = factory->CreateVertexBuffer(ibSize, EAH_CPU_Write | EAH_GPU_Read, BufferCreate_Index, NULL);
	}
	else 
	{
		uint32_t currIBSize = mIndexBuffer->GetBufferSize();
		if (currIBSize < ibSize)
		{
			mIndexBuffer = factory->CreateIndexBuffer(currIBSize * 2, EAH_CPU_Write | EAH_GPU_Read, BufferCreate_Index, NULL);
			mRenderOperation->BindIndexStream(mIndexBuffer, IBT_Bit16);
		}
	}
}

void Sprite::ClearAll()
{
	mVertices.resize(0);
	mInidces.resize(0);
	mDirty = true;
}

vector<SpriteVertex>& Sprite::GetVertices()
{
	mDirty = true;
	return mVertices;
}

vector<uint16_t>& Sprite::GetIndices()
{
	mDirty = true;
	return mInidces;
}

void Sprite::Render()
{
	EffectTechnique* technique = mBatch.mEffect->GetCurrentTechnique();

	mBatch.mSpriteTexParam->SetValue(mSpriteTexture->GetShaderResourceView());
	Environment::GetSingleton().GetRenderDevice()->Draw(technique, *mRenderOperation);
}

const shared_ptr<Material>& Sprite::GetMaterial() const
{
	ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Shoudn't call this", "Sprite::GetMaterial");
}

void Sprite::OnRenderBegin()
{
	ENGINE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Shoudn't call this", "Sprite::OnRenderBegin");
}



}
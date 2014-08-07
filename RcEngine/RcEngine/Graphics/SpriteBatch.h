#ifndef SpriteBatch_h__
#define SpriteBatch_h__

#include <Core/Prerequisites.h>
#include <Math/Matrix.h>
#include <Math/Rectangle.h>
#include <Math/ColorRGBA.h>
#include <Graphics/Renderable.h>
#include <Scene/SceneObject.h>

namespace RcEngine {

// Forward declaration
class Sprite;

struct SpriteVertex
{
	float3 Position;
	float2 TexCoord;
	ColorRGBA Color;
};

class _ApiExport SpriteBatch
{
	friend class Sprite;

public:
	/**
	 * Default sprite batch, use to render normal 2D sprite
	 */
	SpriteBatch();

	/**
	 * Sprite batch with specular material
	 */
	SpriteBatch(const shared_ptr<Effect>& effect);
	~SpriteBatch();

	void Begin();
	void End();
	void Flush();
	
	/**
	 * Adds a sprite to a batch of sprites for rendering using the specified texture, 
	 * destination rectangle, source rectangle, color, rotation, origin, effects and layer.
	 *
	 * @param destinationRectangle: A rectangle that specifies (in screen coordinates) the destination for drawing
	 * the sprite. If this rectangle is not the same size as the source rectangle, the sprite will be 
	 * scaled to fit.
	 *
	 * @param sourceRectangle: A rectangle that specifies (in texels) the source texels from a texture. Use null
	 * to draw the entire texture.
	 *
	 * @param color: The color to tint a sprite. Use Color.White for full color with no tinting.
	 *
	 * @param rotation: Specifies the angle (in radians) to rotate the sprite about its center.
	 *
	 * @param origin: The sprite origin; the default is (0,0) which represents the upper-left corner.
	 *
	 * @param layerDepth: The depth of a layer. By default, 0 represents the front layer and 1 represents
	 * a back layer. Use SpriteSortMode if you want sprites to be sorted during drawing.
	 */
	void Draw(const shared_ptr<Texture>& texture, const Rectanglef& destinationRectangle, const IntRect* sourceRectangle, 
		      const ColorRGBA& color, float rotation, const float2& origin, float layerDepth = 0.0f);

	void Draw(const shared_ptr<Texture>& texture, const float2& position, const IntRect* sourceRectangle, 
		      const ColorRGBA& color, float rotation, const float2& origin, float scale, float layerDepth = 0.0f);

	void Draw(const shared_ptr<Texture>& texture, const float2& position, const IntRect* sourceRectangle, 
		      const ColorRGBA& color, float rotation, const float2& origin, const float2& scale, float layerDepth = 0.0f);

	void Draw(const shared_ptr<Texture>& texture, const Rectanglef& destinationRectangle, const ColorRGBA& color, float layerDepth = 0.0f);
	void Draw(const shared_ptr<Texture>& texture, const Rectanglef& destinationRectangle, const IntRect* sourceRectangle, 
		      const ColorRGBA& color, float layerDepth = 0.0f);

	void Draw(const shared_ptr<Texture>& texture, const float2& position, const ColorRGBA& color, float layerDepth = 0.0f);
	void Draw(const shared_ptr<Texture>& texture, const float2& position, const IntRect* sourceRectangle, const ColorRGBA& color, float layerDepth = 0.0f);


public_internal:
	void OnUpdateRenderQueue(RenderQueue& renderQueue);

private:
	uint32_t mSortMode;
	shared_ptr<Effect> mEffect;
	EffectParameter* mSpriteTexParam;

	float2 mInvWindowSize;

	// SpriteEntity is keep track by SceneManager, So SceneManager will delete it when destroy
	std::map<shared_ptr<Texture>, Sprite*> mBatches;
};


/**
 */
class Sprite : public Renderable
{
	friend class SpriteBatch;

public:
	Sprite(SpriteBatch& batch, shared_ptr<Texture> texture);
	~Sprite();

	// no world transform
	uint32_t GetWorldTransformsCount() const { return 0; }

	void GetWorldTransforms(float4x4* xform) const { }
	const shared_ptr<RenderOperation>& GetRenderOperation() const { return mRenderOperation; }
	
	const shared_ptr<Material>& GetMaterial() const;
	void OnRenderBegin();
	void Render();

	inline bool Empty() const { return mInidces.empty(); }

	vector<SpriteVertex>& GetVertices();
	vector<uint16_t>& GetIndices();

	void ClearAll();

private:
	void UpdateGeometryBuffers();
	void ResizeGeometryBuffers(size_t numVertex, size_t numIndex);

private:
	SpriteBatch& mBatch;

	shared_ptr<Texture> mSpriteTexture;
	shared_ptr<RenderOperation> mRenderOperation;
	
	shared_ptr<VertexDeclaration> mVertexDecl;
	shared_ptr<GraphicsBuffer> mVertexBuffer;
	shared_ptr<GraphicsBuffer> mIndexBuffer;

	enum { NumInitSprites = 100 };
	vector<SpriteVertex> mVertices;
	vector<uint16_t> mInidces;

	bool mDirty;
};

}

#endif // SpriteBatch_h__

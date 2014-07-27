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
public:
	/**
	 * Default sprite batch, use to render normal 2D sprite
	 */
	SpriteBatch();

	/**
	 * Sprite batch with specular material
	 */
	SpriteBatch(const shared_ptr<Material>& material);
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

private:
	uint32_t mSortMode;
	shared_ptr<Material> mSpriteMaterial;

	// SpriteEntity is keep track by SceneManager, So SceneManager will delete it when destroy
	std::map<shared_ptr<Texture>, Sprite*> mBatches;
};


/**
 */
class Sprite : public Renderable
{
public:
	Sprite();
	~Sprite();

	const shared_ptr<Material>& GetMaterial() const { return mSpriteMaterial; }

	// no world transform
	uint32_t GetWorldTransformsCount() const { return 0; }

	void GetWorldTransforms(float4x4* xform) const { }

	const shared_ptr<RenderOperation>& GetRenderOperation() const { return mRenderOperation; }

	bool Empty() const;

	void ClearAll();

	vector<SpriteVertex>& GetVertices();

	vector<uint16_t>& GetIndices();

	void SetProjectionMatrix(const float4x4& mat);

	void OnRenderBegin();

	void UpdateGeometryBuffers();

public_internal:
	void SetSpriteContent(const shared_ptr<Texture>& tex, const shared_ptr<Material>& mat);

private:
	shared_ptr<Texture> mSpriteTexture;
	shared_ptr<Material> mSpriteMaterial;
	shared_ptr<RenderOperation> mRenderOperation;
	vector<SpriteVertex> mVertices;
	vector<uint16_t> mInidces;

	shared_ptr<GraphicsBuffer> mVertexBuffer;
	shared_ptr<GraphicsBuffer> mIndexBuffer;

	EffectParameter* mWindowSizeParam;

	bool mDirty;
};

}

#endif // SpriteBatch_h__

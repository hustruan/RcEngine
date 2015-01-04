#ifndef Sky_h__
#define Sky_h__

#include <Core/Prerequisites.h>
#include <Scene/SceneObject.h>
#include <Graphics/Mesh.h>
#include <Math/Matrix.h>

namespace RcEngine {

class _ApiExport SkyBox : public SceneObject
{
public:
	SkyBox( const String& name, const shared_ptr<Texture>& cubeSkyTexture );
	~SkyBox();

	void SetTexture(const shared_ptr<Texture>& texure);

	const BoundingBoxf& GetWorldBoundingBox() const;
	void OnUpdateRenderQueue( RenderQueue* renderQueue, const Camera& camera, RenderOrder order, uint32_t buckterFilter, uint32_t filterIgnore );

public:
	static SceneObject* FactoryFunc(const String& name, const NameValuePairList* params = 0);

private:
	class Renderable* mCubeBox;
};

}

#endif // Sky_h__

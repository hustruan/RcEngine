#ifndef Geometry_h__
#define Geometry_h__

#include <Core/Prerequisites.h>
#include <Graphics/RenderFactory.h>

namespace RcEngine {

void BuildShpereOperation(RenderOperation& rop, float radius, int32_t nRings = 30, int32_t nSegments = 30);
void BuildConeOperation(RenderOperation& rop, float radius, float height, int32_t nCapSegments = 30);
void BuildBoxOperation(RenderOperation& rop);
void BuildFrustumOperation(RenderOperation& rop);
void BuildFullscreenTriangle(RenderOperation& rop);

}

#endif // Geometry_h__

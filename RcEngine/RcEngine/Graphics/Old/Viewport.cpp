#include <Graphics/Viewport.h>
#include <Graphics/Camera.h>

namespace RcEngine {

Viewport::Viewport( )	
	: AttachCamera( new Camera() )
{

}

Viewport::Viewport( uint32_t left, uint32_t top, uint32_t width, uint32_t height )
	: Top(top), Left(left), Width(width), Height(height), AttachCamera( new Camera() )
{

}


Viewport::~Viewport()
{
	SAFE_DELETE(AttachCamera);
}

bool Viewport::operator==( const Viewport& rhs )
{
	if ((Left != rhs.Left) || (Top != rhs.Top) || 
		(Width != rhs.Width) || (Height != rhs.Height))
		return false;
	return true;
}

} // Namespace RcEngine
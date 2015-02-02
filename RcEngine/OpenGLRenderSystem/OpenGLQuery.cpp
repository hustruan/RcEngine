#include "OpenGLQuery.h"

namespace RcEngine {

OpenGLQuery::OpenGLQuery()
{	
	glGenQueries(2, mQuery);
}

OpenGLQuery::~OpenGLQuery()
{
	glDeleteQueries(2, mQuery);
}

}
#ifndef Prerequisites_h__
#define Prerequisites_h__

////////////////////////////////////////////////////////////////////////////////
// Compile Define
////////////////////////////////////////////////////////////////////////////////
#include "CompileConfig.h"

//////////////////////////////////////////////////////////////////////////////
// StableSTLHeader
////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <array>
#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <string>
#include <functional>
#include <cassert>
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <memory>
#include <limits>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <cstdio>
#include <cstdarg>
#include <ctime>

// 3rd party includes.
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
#include <FastDelegate/FastDelegate.h>
#include <FastDelegate/FastDelegateBind.h>

typedef fastdelegate::FastDelegate0<> EventHandler;
typedef fastdelegate::FastDelegate1<bool*, void> CancellableEventHandler;
typedef fastdelegate::FastDelegate3<uint32_t, bool, float> InputActionHandler;
typedef fastdelegate::FastDelegate3<uint32_t, bool, float> InputStateHandler;
typedef fastdelegate::FastDelegate3<uint32_t, int32_t, float> InputRangeHandler;

//------------------------------------------------------------------------------
// Dll Define
//------------------------------------------------------------------------------
#	ifdef BUILD_STATIC
#		define _ApiExport
#	else 
#		ifdef BUILD_DLL
#			define _ApiExport __declspec( dllexport )
#		else     
#			define _ApiExport __declspec( dllimport )
#		endif
#	endif


//////////////////////////////////////////////////////////////////////////
using std::shared_ptr;
using std::weak_ptr;
using std::vector;
using std::unordered_map;
using std::unordered_set;

typedef std::string String;
typedef std::wstring WString;
typedef std::basic_istringstream<String::value_type>  StringStream;
typedef std::basic_istringstream<WString::value_type> WStringStream;

/// Name / value parameter pair (first = name, second = value)
typedef std::map<String, String> NameValuePairList;

/////////////////////////////////////////////////////////////////////////////
#define public_internal public
#define SAFE_DELETE(p) if(p) { delete p; p = NULL;}

// Compile time type-safe array size
template <typename T, int N> char(&dim_helper(T(&)[N]))[N];
#define ARRAY_SIZE(x) (sizeof(dim_helper(x)))

// For normal pointer 
template<class Type, class SourceType>
Type static_cast_checked(SourceType* item)
{
	assert(!item || dynamic_cast<Type>(item));
	return static_cast<Type>(item);
}

// For shared_ptr
template <class Ty, class Other>
shared_ptr<Ty> static_pointer_cast_checked(const shared_ptr<Other>& sp)
{
	assert(!sp || std::dynamic_pointer_cast<Ty>(sp));
	return std::static_pointer_cast<Ty>(sp);
}


namespace RcEngine {

class RenderView;
class FrameBuffer;
class ShaderResourceView;
class UnorderedAccessView;
class Texture;
class GraphicsBuffer;
class DepthStencilState;
class RasterizerState;
class BlendState;
class SamplerState;
class Shader;
class VertexDeclaration;
class ShaderPipeline;
class RenderOperation;
class EffectParameter;
class EffectTechnique;
class EffectPass;

class Stream;

class Camera;
class Renderable;
class Mesh;
class MeshPart;
class Model;
class Material;
class Pipeline;
class Renderer;
class Effect;
class Font;
class SpriteBatch;
class RenderQueue;
class SimpleBox;
class SceneManager;
class Node;
class Light;
class SceneNode;
class Entity;
class SpriteBatch;
class AnimationPlayer;
class AnimationState;
class AnimationClip;
class AnimationController;

class UIWindow;
class Label;
class Button;
class Slider;
class ScrollBar;
class CheckBox;
class TextEdit;
class ListBox;
class ComboBox;
class LineEdit;
class RadioButton;
class RadioButtonGroup;
class Menu;
class UIManager;



} // Namespace RcEngine

#endif // Prerequisites_h__

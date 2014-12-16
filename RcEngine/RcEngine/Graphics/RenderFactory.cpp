#include <Graphics/RenderFactory.h>
#include <Graphics/GraphicsResource.h>
#include <MainApp/Application.h>
#include <IO/FileSystem.h>
#include <Core/Environment.h>
#include <Core/Utility.h>
#include <Core/Exception.h>
#include <Core/Profiler.h>
#include <Graphics/Image.h>
#include <IO/PathUtil.h>

namespace RcEngine {

RenderFactory::RenderFactory()
{
	Environment::GetSingleton().mRenderFactory = this;
}

shared_ptr<BlendState> RenderFactory::CreateBlendState( const BlendStateDesc& desc )
{
	if (mBlendStatePool.find(desc) == mBlendStatePool.end())
	{
		mBlendStatePool[desc] = CreateBlendStateImpl(desc);
	}

	return mBlendStatePool[desc];
}

shared_ptr<SamplerState> RenderFactory::CreateSamplerState( const SamplerStateDesc& desc )
{
	if (mSamplerStatePool.find(desc) == mSamplerStatePool.end())
	{
		mSamplerStatePool[desc] = CreateSamplerStateImpl(desc);
	}

	return mSamplerStatePool[desc];
}

shared_ptr<RasterizerState> RenderFactory::CreateRasterizerState( const RasterizerStateDesc& desc )
{
	if (mRasterizerStatePool.find(desc) == mRasterizerStatePool.end())
	{
		mRasterizerStatePool[desc] = CreateRasterizerStateImpl(desc);
	}

	return mRasterizerStatePool[desc];
}

shared_ptr<DepthStencilState> RenderFactory::CreateDepthStencilState( const DepthStencilStateDesc& desc )
{
	if (mDepthStecilStatePool.find(desc) == mDepthStecilStatePool.end())
	{
		mDepthStecilStatePool[desc] = CreateDepthStencilStateImpl(desc);
	}

	return mDepthStecilStatePool[desc];
}

shared_ptr<Shader> RenderFactory::LoadShaderFromFile( ShaderType shaderType, const String& filename, const ShaderMacro* macros, uint32_t macroCount, const String& entryPoint /*= ""*/ )
{
	size_t shaderSeed = 0;

	HashCombine(shaderSeed, filename);
	HashCombine(shaderSeed, entryPoint);
	for (uint32_t i = 0; i < macroCount; ++i)
	{
		HashCombine(shaderSeed, macros[i].Name);
		HashCombine(shaderSeed, macros[i].Definition);
	}

	if (mShaderPool.find(shaderSeed) == mShaderPool.end())
	{
		shared_ptr<Shader> shader = CreateShader(shaderType);
		
		String shaderFile;
		if ( Application::msApp->GetAppSettings().RHDeviceType == RD_Direct3D11)
		{
			shaderFile = "HLSL/" + filename + ".hlsl";
		}
		else 
		{
			shaderFile = "GLSL/" + filename + ".glsl";
		}

		//ENGINE_CPU_AUTO_PROFIER("Load Shader");
		shader->LoadFromFile(FileSystem::GetSingleton().Locate(shaderFile), macros, macroCount, entryPoint);

		mShaderPool[shaderSeed] = shader;
	}

	return mShaderPool[shaderSeed];
}

//shared_ptr<Texture> RenderFactory::LoadTextureFromFile( const String& filename )
//{
//	auto storage = gli::load_dds(filename.c_str());
//	auto dim = storage.dimensions(0);
//	
//	gli::storage::size_type numLayers = storage.layers();
//	gli::storage::size_type numLevels = storage.levels();
//	gli::storage::size_type numFaces = storage.faces();
//
//	if (numFaces > 1) assert(numFaces == 6);
//
//	uint32_t elementSize = numFaces > 1 ? (numFaces*numLevels*numLayers) : numLevels*numLayers;
//	std::vector<ElementInitData> imageData(elementSize);
//
//	PixelFormat format = PixelFormat((uint32_t)storage.format());
//
//	if (storage.faces() > 1)
//	{
//		if (storage.layers() > 1)
//		{
//			gli::textureCubeArray textureCubeArray(storage);
//		}
//		else
//		{
//			gli::textureCube textureCube(storage);
//		}
//
//		return CreateTextureCube(dim.x, dim.y, format, storage.layers(), storage.levels(), 0, 1, EAH_GPU_Read, TexCreate_ShaderResource, &imageData[0]);
//	}
//	else if (dim.x > 0 && dim.y > 0 && dim.z > 1)
//	{
//		gli::texture3D texture3D(storage);
//
//	}
//	else if (dim.x > 0 && dim.y > 0)
//	{
//		if (storage.layers() > 1)
//		{
//			gli::texture2DArray texture2DArray(storage);
//			/*for (gli::texture2DArray::size_type layer = 0; layer < texture2DArray.layers(); ++layer)
//			{
//			for(gli::texture2D::size_type level = 0; level < texture2DArray.levels(); ++level)
//			{
//			uint32_t index = layer*numLevels+level;
//			imageData[level].pData = texture2DArray[layer][level].data();
//			imageData[level].rowPitch = storage.layerSize(layer, layer, level, level);
//			imageData[level].slicePitch = 0;
//			}
//			}*/
//		}
//		else
//		{
//			gli::texture2D texture2D(storage);
//			for(gli::texture2D::size_type level = 0; level < numLevels; ++level)
//			{
//				imageData[level].pData = texture2D[level].data();
//				imageData[level].rowPitch = storage.levelSize(level) / dim.y; // texture line pitch
//				imageData[level].slicePitch = 0;
//			}
//		}
//
//		return CreateTexture2D(dim.x, dim.y, format, numLayers, numLevels, 1, 0, EAH_GPU_Read, TexCreate_ShaderResource, &imageData[0]);
//	}
//	else if (dim.x > 0)
//	{
//		if (storage.layers() > 1)
//		{
//			gli::texture1DArray texture1DArray(storage);
//		}
//		else
//		{
//			gli::texture1D texture1D(storage);
//			for(gli::texture2D::size_type level = 0; level < texture1D.levels(); ++level)
//			{
//				imageData[level].pData = texture1D[level].data();
//				imageData[level].rowPitch = storage.levelSize(level);
//				imageData[level].slicePitch = 0;
//			}
//		}
//
//		return CreateTexture1D(dim.x, format, storage.layers(), storage.levels(), EAH_GPU_Read, TexCreate_ShaderResource, &imageData[0]);
//	}
//	else
//	{
//		assert(false);
//	}
//
//	ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Internal Error", "RenderFactory::LoadTextureFromFile");
//}

shared_ptr<Texture> RenderFactory::LoadTextureFromFile( const String& filename )
{
	Image image;
	if (image.LoadImageFromDDS(filename.c_str()) == false)
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, filename + " not found!", "RenderFactory::LoadTextureFromFile");

	uint32_t numLayers = image.GetLayers();
	uint32_t numLevels = image.GetLevels();

	switch (image.GetType())
	{
	case TT_Texture1D:
		{
			std::vector<ElementInitData> imageData(numLayers * numLevels);
			for (uint32_t layer = 0; layer < numLayers; ++layer)
				for (uint32_t level = 0; level < numLevels; ++level)
				{
					imageData[layer * numLevels + level].pData = image.GetLevel(level, layer);
				}
			return CreateTexture1D(image.GetWidth(), image.GetFormat(), numLayers, numLevels, EAH_GPU_Read, TexCreate_ShaderResource, &imageData[0]);
		}
		break;
	case TT_Texture2D:
		{
			std::vector<ElementInitData> imageData(numLayers * numLevels);
			for (uint32_t layer = 0; layer < numLayers; ++layer)
				for (uint32_t level = 0; level < numLevels; ++level)
				{
					imageData[layer * numLevels + level].pData = image.GetLevel(level, layer);
					imageData[layer * numLevels + level].rowPitch = image.GetRowPitch(level);
				}
			return CreateTexture2D(image.GetWidth(), image.GetHeight(), image.GetFormat(), numLayers, numLevels, 1, 0, EAH_GPU_Read, TexCreate_ShaderResource, &imageData[0]);
		}
		break;
	case TT_Texture3D:
		{
			assert(numLayers == 1);
			std::vector<ElementInitData> imageData(numLevels);

			for (uint32_t level = 0; level < numLevels; ++level)
			{
				imageData[level].pData = image.GetLevel(level);
				imageData[level].rowPitch = image.GetRowPitch(level);
				imageData[level].slicePitch = image.GetSlicePitch(level);
			}

			return CreateTexture3D(image.GetWidth(), image.GetHeight(), image.GetHeight(), image.GetFormat(), numLevels, EAH_GPU_Read, TexCreate_ShaderResource, &imageData[0]);
		}
		break;
	case TT_TextureCube:
		{
			std::vector<ElementInitData> imageData(numLayers * numLevels * CMF_Count);
			uint32_t subImageIndex = 0;
			for (uint32_t layer = 0; layer < numLayers; ++layer)
				for (uint32_t face = CMF_PositiveX; face < CMF_Count; ++face)
					for (uint32_t level = 0; level < numLevels; ++level)
					{
						imageData[subImageIndex].pData = image.GetLevel(level, layer, (CubeMapFace)face);
						imageData[subImageIndex].rowPitch = image.GetRowPitch(level);
						subImageIndex++;
					}
			
			return CreateTextureCube(image.GetWidth(), image.GetHeight(), image.GetFormat(), numLayers, numLevels, 1, 0, EAH_GPU_Read, TexCreate_ShaderResource, &imageData[0]);
		}
		break;
	}

	ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Internal Error", "RenderFactory::LoadTextureFromFile");
}

void RenderFactory::SaveTextureToFile(const String& filename, const shared_ptr<Texture>& texture)
{
	String path, name, ext;
	PathUtil::SplitPath(filename, path, name, ext);
	if (Application::msApp->GetAppSettings().RHDeviceType == RD_Direct3D11)
	{
		name = path + name + "D3D" + ext;
	}
	else
	{
		name = path + name + "OGL" + ext;
	}

	Image img;
	img.CopyImageFromTexture(texture);
	img.SaveImageToFile(name);
}

void RenderFactory::SaveLinearDepthTextureToFile( const String& filename, const shared_ptr<Texture>& texture, float projM33, float projM43 )
{
	String path, name, ext;
	PathUtil::SplitPath(filename, path, name, ext);
	if (Application::msApp->GetAppSettings().RHDeviceType == RD_Direct3D11)
	{
		name = path + name + "D3D" + ext;
	}
	else
	{
		name = path + name + "OGL" + ext;
	}

	Image img;
	img.CopyImageFromTexture(texture);
	img.SaveLinearDepthToFile(name, projM33, projM43);
}


}
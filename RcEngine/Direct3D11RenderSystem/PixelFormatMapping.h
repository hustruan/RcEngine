DXGI_FORMAT                                RcEngine            		OpenGL(internalformat, format, type)     

// A four-component, 32-bit unsigned-normalized integer format that supports 8-bit alpha.
DXGI_FORMAT_B8G8R8A8_UNORM                 PF_B8G8R8A8         		(GL_RGBA8,          GL_BGRA,      GL_UNSIGNED_BYTE)    (B0, G1, R2, A3)              
DXGI_FORMAT_B8G8R8A8_SNORM*                PF_B8G8R8A8_SNORM   		(GL_RGBA8_SNORM,    GL_BGRA,      GL_UNSIGNED_BYTE)    (B0, G1, R2, A3)    
DXGI_FORMAT_B8G8R8A8_UNORM_SRGB  
DXGI_FORMAT_B8G8R8A8_TYPELESS

DXGI_FORMAT_R32G32B32A32_FLOAT             PF_R32G32B32A32_FLOAT    (GL_RGBA32F,        GL_RGBA       GL_FLOAT)
DXGI_FORMAT_R32G32B32A32_UINT   		   PF_R32G32B32A32_UINT	    (GL_RGBA32UI,       GL_RGBA       GL_UNSIGNED_INT)
DXGI_FORMAT_R32G32B32A32_SINT              PF_R32G32B32A32_SINT     (GL_RGBA32I,        GL_RGBA       GL_INT)
DXGI_FORMAT_R32G32B32A32_TYPELESS 

DXGI_FORMAT_R32G32B32_FLOAT                PF_R32G32B32_FLOAT       (GL_RGB32F,         GL_RGB       GL_FLOAT)
DXGI_FORMAT_R32G32B32_UINT                 PF_R32G32B32_UINT        (GL_RGB32UI,        GL_RGB       GL_UNSIGNED_INT)
DXGI_FORMAT_R32G32B32_SINT                 PF_R32G32B32_SINT        (GL_RGB32I,         GL_RGB       GL_INT)
DXGI_FORMAT_R32G32B32_TYPELESS             

DXGI_FORMAT_R16G16B16A16_FLOAT             PF_R16G16B16A16_FLOAT    (GL_RGBA16F         GL_RGBA       GL_HALF_FLOAT)
DXGI_FORMAT_R16G16B16A16_UNORM             PF_R16G16B16A16          (GL_RGBA16          GL_RGBA       GL_UNSIGNED_SHORT)
DXGI_FORMAT_R16G16B16A16_UINT              PF_R16G16B16A16_UINT     (GL_RGBA16UI        GL_RGBA       GL_UNSIGNED_SHORT)  
DXGI_FORMAT_R16G16B16A16_SNORM             PF_R16G16B16A16_SNORM    (GL_RGBA16_SNORM    GL_RGBA       GL_SHORT)
DXGI_FORMAT_R16G16B16A16_SINT              PF_R16G16B16A16_SINT     (GL_RGBA16I         GL_RGBA       GL_SHORT)
DXGI_FORMAT_R16G16B16A16_TYPELESS          


DXGI_FORMAT_R32G32_FLOAT                   PF_R32G32_FLOAT          (GL_RG32F           GL_RG         GL_FLOAT)
DXGI_FORMAT_R32G32_UINT                    PF_R32G32_UINT           (GL_RG32UI          GL_RG         GL_UNSIGNED_INT)
DXGI_FORMAT_R32G32_SINT                    PF_R32G32_SINT           (GL_RG32I           GL_RG         GL_INT)
DXGI_FORMAT_R32G32_TYPELESS                


 DXGI_FORMAT_R32G8X24_TYPELESS            = 19,
  DXGI_FORMAT_D32_FLOAT_S8X24_UINT         = 20,
  DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS     = 21,
  DXGI_FORMAT_X32_TYPELESS_G8X24_UINT      = 22,
  
  
  
DXGI_FORMAT_R10G10B10A2_UNORM              PF_R10G10B10A2           (GL_RGB10_A2        GL_RGBA       GL_UNSIGNED_INT_10_10_10_2)
DXGI_FORMAT_R10G10B10A2_UINT               PF_R10G10B10A2_UINT      (GL_RGB10_A2UI      

GL_RGBA       GL_UNSIGNED_INT_10_10_10_2)
DXGI_FORMAT_R10G10B10A2_TYPELESS   
  
DXGI_FORMAT_R11G11B10_FLOAT                PF_R11G11B10_FLOAT       (GL_R11F_G11F_B10F  GL_RGB        GL_UNSIGNED_INT_10F_11F_11F_REV) 

DXGI_FORMAT_R8G8B8A8_UNORM                 PF_R8G8B8A8              (GL_RGBA8           GL_RGBA       GL_UNSIGNED_BYTE)
DXGI_FORMAT_R8G8B8A8_UINT                  PF_R8G8B8A8_UINT         (GL_RGBA8UI         GL_RGBA       GL_UNSIGNED_BYTE)
DXGI_FORMAT_R8G8B8A8_SNORM                 PF_R8G8B8A8_SNORM        (GL_RGBA8_SNORM     GL_RGBA       GL_BYTE)
DXGI_FORMAT_R8G8B8A8_SINT                  PF_R8G8B8A8_SINT         (GL_RGBA8I          GL_RGBA       GL_BYTE)
DXGI_FORMAT_R8G8B8A8_UNORM_SRGB     
DXGI_FORMAT_R8G8B8A8_TYPELESS           
 

DXGI_FORMAT_R16G16_UNORM                   PF_R16G16                (GL_RG16            GL_RG       GL_UNSIGNED_SHORT)
DXGI_FORMAT_R16G16_UINT                    PF_R16G16_UINT           (GL_RG16UI          GL_RG       GL_UNSIGNED_SHORT)
DXGI_FORMAT_R16G16_SNORM                   PF_R16G16_SNORM          (GL_RG16_SNORM      GL_RG       GL_SHORT)
DXGI_FORMAT_R16G16_SINT                    PF_R16G16_SINT           (GL_RG16I           GL_RG       GL_SHORT)
DXGI_FORMAT_R16G16_FLOAT                   PF_R16G16_FLOAT          (GL_RG16F           GL_RG       GL_HALF_FLOAT)
DXGI_FORMAT_R16G16_TYPELESS                
 

DXGI_FORMAT_R32_FLOAT                      PF_R32_FLOAT             (GL_R32F            GL_RED      GL_FLOAT)
DXGI_FORMAT_R32_UINT                       PF_R32_UINT              (GL_R32UI           GL_RED      GL_UNSIGNED_INT)
DXGI_FORMAT_R32_SINT                       PF_R32_SINT              (GL_R32I            GL_RED      GL_INT)
DXGI_FORMAT_R32_TYPELESS                 

DXGI_FORMAT_R24G8_TYPELESS               = 44,

DXGI_FORMAT_R24_UNORM_X8_TYPELESS        = 46,
DXGI_FORMAT_X24_TYPELESS_G8_UINT         = 47,



DXGI_FORMAT_R8G8_UNORM                     PF_R8G8                  (GL_RG8             GL_RG       GL_UNSIGNED_BYTE)
DXGI_FORMAT_R8G8_UINT                      PF_R8G8_UINT             (GL_RG8UI           GL_RG       GL_UNSIGNED_BYTE
DXGI_FORMAT_R8G8_SNORM                     PF_R8G8_SNORM            (GL_RG8_SNORM       GL_RG       GL_BYTE)
DXGI_FORMAT_R8G8_SINT                      PF_R8G8_SINT             (GL_RG8I            GL_RG       GL_BYTE)
DXGI_FORMAT_R8G8_TYPELESS                  

DXGI_FORMAT_R16_UNORM                      PF_R16                   (GL_R16            GL_RED       GL_UNSIGNED_SHORT)
DXGI_FORMAT_R16_UINT                       PF_R16_UINT              (GL_R16UI          GL_RED       GL_UNSIGNED_SHORT)
DXGI_FORMAT_R16_SNORM                      PF_R16_SNORM             (GL_R16_SNORM      GL_RED       GL_SHORT)
DXGI_FORMAT_R16_SINT                       PF_R16_SINT              (GL_R16I           GL_RED       GL_SHORT)
DXGI_FORMAT_R16_FLOAT                      PF_R16_FLOAT             (GL_R16F           GL_RED       GL_HALF_FLOAT)
DXGI_FORMAT_R16_TYPELESS                   

DXGI_FORMAT_R8_UNORM                       PF_R8                    (GL_R8              GL_RED       GL_UNSIGNED_BYTE)
DXGI_FORMAT_R8_UINT                        PF_R8_UINT               (GL_R8UI            GL_RED       GL_UNSIGNED_BYTE)
DXGI_FORMAT_R8_SNORM                       PF_R8_SNORM              (GL_R8_SNORM        GL_RED       GL_BYTE)
DXGI_FORMAT_R8_SINT                        PF_R8_SINT               (GL_R8I             GL_RED       GL_BYTE)
DXGI_FORMAT_R8_TYPELESS                    

DXGI_FORMAT_A8_UNORM                       PF_
DXGI_FORMAT_R1_UNORM                       PF_

DXGI_FORMAT_R9G9B9E5_SHAREDEXP             PF_
DXGI_FORMAT_R8G8_B8G8_UNORM                PF_
DXGI_FORMAT_G8R8_G8B8_UNORM                PF_

DXGI_FORMAT_BC1_TYPELESS                   PF_
DXGI_FORMAT_BC1_UNORM                      PF_
DXGI_FORMAT_BC1_UNORM_SRGB                 PF_

DXGI_FORMAT_BC2_TYPELESS                   PF_
DXGI_FORMAT_BC2_UNORM                      PF_
DXGI_FORMAT_BC2_UNORM_SRGB                 PF_

DXGI_FORMAT_BC3_TYPELESS                   PF_
DXGI_FORMAT_BC3_UNORM                      PF_
DXGI_FORMAT_BC3_UNORM_SRGB                 PF_

DXGI_FORMAT_BC4_TYPELESS                   PF_
DXGI_FORMAT_BC4_UNORM                      PF_
DXGI_FORMAT_BC4_SNORM                      PF_

DXGI_FORMAT_BC5_TYPELESS                   PF_
DXGI_FORMAT_BC5_UNORM                      PF_
DXGI_FORMAT_BC5_SNORM                      PF_

DXGI_FORMAT_B5G6R5_UNORM                   PF_B5G6R5              (GL_RGB             GL_RGB       GL_UNSIGNED_SHORT_5_6_5_REV)   
DXGI_FORMAT_B5G5R5A1_UNORM                 PF_B5G5R5A1            (GL_RGB5_A1         GL_BGR       GL_UNSIGNED_SHORT_5_5_5_1)   

DXGI_FORMAT_B8G8R8X8_UNORM                 PF_B8G8R8X8            (GL_RGBA8           GL_BGR       GL_UNSIGNED_BYTE)   
DXGI_FORMAT_B8G8R8X8_TYPELESS              PF_
DXGI_FORMAT_B8G8R8X8_UNORM_SRGB            PF_

DXGI_FORMAT_B8G8R8A8_UNORM                 PF_B8G8R8A8            (GL_RGBA8           GL_BGR       GL_UNSIGNED_BYTE)   
DXGI_FORMAT_B8G8R8A8_TYPELESS              PF_
DXGI_FORMAT_B8G8R8A8_UNORM_SRGB            PF_

DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM     PF_
DXGI_FORMAT_BC6H_TYPELESS                  PF_
DXGI_FORMAT_BC6H_UF16                      PF_
DXGI_FORMAT_BC6H_SF16                      PF_
DXGI_FORMAT_BC7_TYPELESS                   PF_
DXGI_FORMAT_BC7_UNORM                      PF_
DXGI_FORMAT_BC7_UNORM_SRGB                 PF_
DXGI_FORMAT_FORCE_UINT                     PF_

DXGI_FORMAT_D16_UNORM                      PF_D16                   (GL_DEPTH_COMPONENT16  GL_DEPTH_COMPONENT     GL_UNSIGNED_SHORT)
DXGI_FORMAT_D24_UNORM_S8_UINT              PF_D24S8                 (GL_DEPTH24_STENCIL8  GL_DEPTH_STENCIL     GL_UNSIGNED_INT_24_8)
DXGI_FORMAT_D32_FLOAT                      PF_D32_FLOAT             (GL_DEPTH_COMPONENT32  GL_DEPTH_COMPONENT32F  GL_FLOAT)  // Shadow texture


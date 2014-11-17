#ifndef DeepGBufferRadiosity_h__
#define DeepGBufferRadiosity_h__

#include <Graphics/RenderPath.h>

namespace RcEngine {

struct DeepGBufferRadiositySettings
{
	bool                        Enabled;

    /** Total number of direct samples to take at each pixel.  Must be greater
        than 2.  The default is 19.  Higher values increase image quality. */
    int                         NumSamples;

    /** Radius in world-space units */
    float                       Radius;

    /** Number of iterations to do each frame */
    int                         NumBounces;

    /** Bias addresses two quality parameters.  The first is that a
        tessellated concave surface should geometrically exhibit
        stronger occlusion near edges and vertices, but this is
        often undesirable if the surface is supposed to appear as a
        smooth curve.  Increasing bias increases the maximum
        concavity that can occur before DeepGBufferRadiosity begins.

        The second is that due to limited precision in the depth
        buffer, a surface could appear to occlude itself.
    */
    float                       Bias;

    /** Set to true to drastically increase performance by increasing cache efficiency, at the expense of accuracy */
    bool                        UseMipMaps;



    /** Default is to step in 2-pixel intervals. This constant can be increased while R decreases to improve
        performance at the expense of some dithering artifacts. 
    
        Morgan found that a scale of 3 left a 1-pixel checkerboard grid that was
        unobjectionable after shading was applied but eliminated most temporal incoherence
        from using small numbers of sample taps.

        Must be at least 1.
    */
    int                         BlurStepSize;

    /** Filter radius in pixels. This will be multiplied by blurStepSize. Default is 4. */
    int                         BlurRadius;
    
    /** Increase to make depth edges crisper. Decrease to reduce flicker. Default is 1.0. */
    float                       EdgeSharpness;


    /** If true, ensure that the "bilateral" weights are monotonically decreasing moving away from the current pixel. Default is true. */
    bool                        MonotonicallyDecreasingBilateralWeights;

    /** 
        Increases quality and stability of DeepGBufferRadiosity, with a performance hit.

        If true, requires the depth peel buffer to be non-null.
    */ 
    bool                        UseDepthPeelBuffer;   

    /**
        A hint for how far (in meters) to buffer the depth peel for the ao.
        
        Since AmbientOcclusion does not perform the depth peel, this is commonly 
        read by the application, which in turn performs the depth peel and passes
        the resulting buffer into AmbientOcclusion.
    */
    float                       DepthPeelSeparationHint;

    /** Compute DeepGBufferRadiosity for the second layer */
    bool                        ComputePeeledLayer;


    /** 
        How much to discount previous frame's bounces as input into the radiosity iteration.
        the range is [0.0, 1.0], with 1.0 having no information propograted between frames as input,
        and 0.0 being no damping whatsoever. Default is 1.0.
     */
    float                       PropagationDamping;

    /** 
        If true, uses the normal at each sample in calculating the contribution. 
        Set to true to reduce light leaking and increase accuracy. Set to false to
        greatly reduce bandwidth and thus increase performance. Default is true.
    */
    bool                        UseTapNormal;

    /** NPR term for increasing indirect illumination when it is an unsaturated value. 1.0 is physically-based. */
    float                       UnsaturatedBoost;

    /** NPR term for increasing indirect illumination when it is an saturated value. 1.0 is physically-based. */
    float                       SaturatedBoost;

    /** Use Oct16 to encode normals. This decreases bandwidth at the cost of extra computation. Default is false. */
    bool                        UseOct16;

    /** The index of the largest mipLevel to use during gather. Increase to reduce bandwidth, decrease to improve quality. Default is 0. */
    int                         MinMipLevel;


    /** The proportion of the guard band to caclulate radiosity for.
    
        Because temporal filtering and multiple scattering events both read the output of
        the indirect pass as the input to the next indirect pass, said pass must output
        closer to the full resolution of the input, rather than the final output size.

        1.0 gives full quality, 0.0 gives maximum performance. Default is 1.0. */
    float                       ComputeGuardBandFraction;
};

class DeepGBufferRadiosity : public RenderPath
{
private:

	class GBuffer
	{
	public:
		enum Field
		{
			DepthStencil = 0,
			Lambertain,
			Glossy,
			Normal,
			ScreenSpaceVelocity,
			Count 
		};

	public:		
		void CreateBuffers(uint32_t width, uint32_t height);
		const shared_ptr<ShaderResourceView>& GetTextureSRV(Field field) const;

	public:
		shared_ptr<FrameBuffer> mFrameBuffer;
		shared_ptr<Texture> mTextures[GBuffer::Count];
	};


public:
	DeepGBufferRadiosity(void);
	~DeepGBufferRadiosity(void);

private:
	void CreateBuffers(uint32_t width, uint32_t height);
	void RenderLambertianOnly();
	void GenerateGBuffers();

private:

	DeepGBufferRadiositySettings mSettings;

	// GBuffers
	shared_ptr<GBuffer> mGBuffer;
	shared_ptr<GBuffer> mPeeledGBuffer;

	// Initial Radiosity
	shared_ptr<FrameBuffer> mLambertianDirectFrameBuffer;
	shared_ptr<Texture> mLambertDirectBuffer;

	shared_ptr<FrameBuffer> mPeeledLambertianDirectFrameBuffer;
	shared_ptr<Texture> mPeeledLambertDirectBuffer;

	shared_ptr<Texture> mPreviousRadiosityBuffer;
	shared_ptr<Texture> mPreviousDepthBuffer;

	// Effect
	shared_ptr<Effect> mLambertianOnlyEffect;
};	


}




#endif // DeepGBufferRadiosity_h__


[[Vertex=PassThroughVSPoint]]

uniform mat4 World;
uniform mat4 ViewProj;


layout (location = 0) in vec3 iControlPoint;

out gl_PerVertex { 
	vec4 gl_Position; // Bezier Patch Control Point
	float gl_PointSize;
};

void main()
{
	gl_Position = vec4(iControlPoint, 1.0) * World * ViewProj;
	gl_PointSize = 5.0;
}

[[Fragment=PSTeapotPoint]]

uniform vec3 Color;

layout (location = 0) out vec4 oFragColor;

void main()
{
	oFragColor = vec4(Color, 1);
}


[[Vertex=PassThroughVS]]

uniform mat4 World;

layout (location = 0) in vec3 iControlPoint;

out gl_PerVertex { 
	vec4 gl_Position; // Bezier Patch Control Point
};

void main()
{
	gl_Position = vec4(iControlPoint, 1.0) * World;
}

[[TessControl=TCSTeapot]]

layout (vertices = 16) out;

uniform int TessLevel;

in gl_PerVertex { 
	vec4 gl_Position; 
} gl_in[];

out gl_PerVertex { 
	vec4 gl_Position;
} gl_out[];

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	if (gl_InvocationID == 0)
	{
		gl_TessLevelOuter[0] = float(TessLevel);
		gl_TessLevelOuter[1] = float(TessLevel);
	    gl_TessLevelOuter[2] = float(TessLevel);
		gl_TessLevelOuter[3] = float(TessLevel);

		gl_TessLevelInner[0] = float(TessLevel);
		gl_TessLevelInner[1] = float(TessLevel);
	}
}

[[TessEval=TESTeapot]]

layout( quads, equal_spacing, ccw ) in;

uniform mat4 ViewProj;

in gl_PerVertex { 
	vec4 gl_Position; 
} gl_in[];

// Output
layout (location = 0) out vec4 oPosWS;
layout (location = 1) out vec2 oTex;
layout (location = 2) out vec3 oNormalWS;

out gl_PerVertex { 
	vec4 gl_Position;
};

// Compute bernstein and partial derivatives of bernstein
void Bernstein(out float[4] b, out float[4] db, float t)
{
	float t1 = (1.0 - t);
	float t12 = t1 * t1;
	
	// Bernstein polynomials
	b[0] = t12 * t1;
	b[1] = 3.0 * t12 * t;
	b[2] = 3.0 * t1 * t * t;
	b[3] = t * t * t;

	// Derivatives
	db[0] = -3.0 * t1 * t1;
	db[1] = -6.0 * t * t1 + 3.0 * t12;
	db[2] = -3.0 * t * t + 6.0 * t * t1;
	db[3] = 3.0 * t * t;
}

void main()
{
	float bu[4], bv[4], dbu[4], dbv[4];
	Bernstein(bu, dbu, gl_TessCoord.x);
	Bernstein(bv, dbv, gl_TessCoord.y);

	oPosWS = vec4(0);
	vec4 du = vec4(0);
	vec4 dv = vec4(0);

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			oPosWS += bu[i]*bv[j]*gl_in[i*4+j].gl_Position;

			du += dbu[i]*bv[j]*gl_in[i*4+j].gl_Position;
			dv += bu[i]*dbv[j]*gl_in[i*4+j].gl_Position;
		}

	gl_Position = oPosWS * ViewProj;
	oNormalWS  = cross(du.xyz, dv.xyz);
}

[[Fragment=PSTeapot]]

layout (location = 0) in vec4 iPosWS;
layout (location = 1) in vec2 iTex;
layout (location = 2) in vec3 iNormalWS;

const vec3 LightPos = vec3(100, 100, 100);

layout (location = 0) out vec4 oFragColor;

void main()
{
	vec3 L = normalize(LightPos - iPosWS.xyz);
	vec3 N = normalize(iNormalWS);
	oFragColor = vec4(1.0, 0, 0, 1.0) * max(dot(N, L), 0.0);
}
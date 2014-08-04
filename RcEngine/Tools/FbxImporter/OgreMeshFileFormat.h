/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2013 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __MeshFileFormat_H__
#define __MeshFileFormat_H__


	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Resources
	*  @{
	*/
/** Definition of the OGRE .mesh file format 

    .mesh files are binary files (for read efficiency at runtime) and are arranged into chunks 
    of data, very like 3D Studio's format.
    A chunk always consists of:
        unsigned short CHUNK_ID        : one of the following chunk ids identifying the chunk
        unsigned long  LENGTH          : length of the chunk in bytes, including this header
        void*          DATA            : the data, which may contain other sub-chunks (various data types)
    
    A .mesh file can contain both the definition of the Mesh itself, and optionally the definitions
    of the materials is uses (although these can be omitted, if so the Mesh assumes that at runtime the
    Materials referred to by name in the Mesh are loaded/created from another source)

    A .mesh file only contains a single mesh, which can itself have multiple submeshes.

*/

	enum MeshChunkID {
        M_HEADER                = 0x1000,
            // char*          version           : Version number check
        M_MESH                = 0x3000,
			// bool skeletallyAnimated   // important flag which affects h/w buffer policies
            // Optional M_GEOMETRY chunk
            M_SUBMESH             = 0x4000, 
                // char* materialName
                // bool useSharedVertices
                // unsigned int indexCount
                // bool indexes32Bit
                // unsigned int* faceVertexIndices (indexCount)
                // OR
                // unsigned short* faceVertexIndices (indexCount)
                // M_GEOMETRY chunk (Optional: present only if useSharedVertices = false)
                M_SUBMESH_OPERATION = 0x4010, // optional, trilist assumed if missing
                    // unsigned short operationType
                M_SUBMESH_BONE_ASSIGNMENT = 0x4100,
                    // Optional bone weights (repeating section)
                    // unsigned int vertexIndex;
                    // unsigned short boneIndex;
                    // float weight;
    			// Optional chunk that matches a texture name to an alias
                // a texture alias is sent to the submesh material to use this texture name
                // instead of the one in the texture unit with a matching alias name
                M_SUBMESH_TEXTURE_ALIAS = 0x4200, // Repeating section
                    // char* aliasName;
                    // char* textureName;

            M_GEOMETRY          = 0x5000, // NB this chunk is embedded within M_MESH and M_SUBMESH
                // unsigned int vertexCount
				M_GEOMETRY_VERTEX_DECLARATION = 0x5100,
					M_GEOMETRY_VERTEX_ELEMENT = 0x5110, // Repeating section
						// unsigned short source;  	// buffer bind source
						// unsigned short type;    	// VertexElementType
						// unsigned short semantic; // VertexElementSemantic
						// unsigned short offset;	// start offset in buffer in bytes
						// unsigned short index;	// index of the semantic (for colours and texture coords)
				M_GEOMETRY_VERTEX_BUFFER = 0x5200, // Repeating section
					// unsigned short bindIndex;	// Index to bind this buffer to
					// unsigned short vertexSize;	// Per-vertex size, must agree with declaration at this index
					M_GEOMETRY_VERTEX_BUFFER_DATA = 0x5210,
						// raw buffer data
            M_MESH_SKELETON_LINK = 0x6000,
                // Optional link to skeleton
                // char* skeletonName           : name of .skeleton to use
            M_MESH_BONE_ASSIGNMENT = 0x7000,
                // Optional bone weights (repeating section)
                // unsigned int vertexIndex;
                // unsigned short boneIndex;
                // float weight;
            M_MESH_LOD = 0x8000,
                // Optional LOD information
                // string strategyName;
                // unsigned short numLevels;
                // bool manual;  (true for manual alternate meshes, false for generated)
                M_MESH_LOD_USAGE = 0x8100,
                // Repeating section, ordered in increasing depth
				// NB LOD 0 (full detail from 0 depth) is omitted
				// LOD value - this is a distance, a pixel count etc, based on strategy
                // float lodValue;
                    M_MESH_LOD_MANUAL = 0x8110,
                    // Required if M_MESH_LOD section manual = true
                    // String manualMeshName;
                    M_MESH_LOD_GENERATED = 0x8120,
                    // Required if M_MESH_LOD section manual = false
					// Repeating section (1 per submesh)
                    // unsigned int indexCount;
                    // bool indexes32Bit
                    // unsigned short* faceIndexes;  (indexCount)
                    // OR
                    // unsigned int* faceIndexes;  (indexCount)
            M_MESH_BOUNDS = 0x9000,
                // float minx, miny, minz
                // float maxx, maxy, maxz
                // float radius
                    
			// Added By DrEvil
			// optional chunk that contains a table of submesh indexes and the names of
			// the sub-meshes.
			M_SUBMESH_NAME_TABLE = 0xA000,
				// Subchunks of the name table. Each chunk contains an index & string
				M_SUBMESH_NAME_TABLE_ELEMENT = 0xA100,
	                // short index
                    // char* name
			
			// Optional chunk which stores precomputed edge data					 
			M_EDGE_LISTS = 0xB000,
				// Each LOD has a separate edge list
				M_EDGE_LIST_LOD = 0xB100,
					// unsigned short lodIndex
					// bool isManual			// If manual, no edge data here, loaded from manual mesh
                        // bool isClosed
                        // unsigned long numTriangles
                        // unsigned long numEdgeGroups
						// Triangle* triangleList
                            // unsigned long indexSet
                            // unsigned long vertexSet
                            // unsigned long vertIndex[3]
                            // unsigned long sharedVertIndex[3] 
                            // float normal[4] 

                        M_EDGE_GROUP = 0xB110,
                            // unsigned long vertexSet
                            // unsigned long triStart
                            // unsigned long triCount
                            // unsigned long numEdges
						    // Edge* edgeList
                                // unsigned long  triIndex[2]
                                // unsigned long  vertIndex[2]
                                // unsigned long  sharedVertIndex[2]
                                // bool degenerate

			// Optional poses section, referred to by pose keyframes
			M_POSES = 0xC000,
				M_POSE = 0xC100,
					// char* name (may be blank)
					// unsigned short target	// 0 for shared geometry, 
												// 1+ for submesh index + 1
					// bool includesNormals [1.8+]
					M_POSE_VERTEX = 0xC111,
						// unsigned long vertexIndex
						// float xoffset, yoffset, zoffset
						// float xnormal, ynormal, znormal (optional, 1.8+)
			// Optional vertex animation chunk
			M_ANIMATIONS = 0xD000, 
				M_ANIMATION = 0xD100,
				// char* name
				// float length
				M_ANIMATION_BASEINFO = 0xD105,
				// [Optional] base keyframe information (pose animation only)
				// char* baseAnimationName (blank for self)
				// float baseKeyFrameTime
		
				M_ANIMATION_TRACK = 0xD110,
					// unsigned short type			// 1 == morph, 2 == pose
					// unsigned short target		// 0 for shared geometry, 
													// 1+ for submesh index + 1
					M_ANIMATION_MORPH_KEYFRAME = 0xD111,
						// float time
						// bool includesNormals [1.8+]
						// float x,y,z			// repeat by number of vertices in original geometry
					M_ANIMATION_POSE_KEYFRAME = 0xD112,
						// float time
						M_ANIMATION_POSE_REF = 0xD113, // repeat for number of referenced poses
							// unsigned short poseIndex 
							// float influence

			// Optional submesh extreme vertex list chink
			M_TABLE_EXTREMES = 0xE000,
			// unsigned short submesh_index;
			// float extremes [n_extremes][3];

	/* Version 1.2 of the .mesh format (deprecated)
	enum MeshChunkID {
        M_HEADER                = 0x1000,
            // char*          version           : Version number check
        M_MESH                = 0x3000,
			// bool skeletallyAnimated   // important flag which affects h/w buffer policies
            // Optional M_GEOMETRY chunk
            M_SUBMESH             = 0x4000, 
                // char* materialName
                // bool useSharedVertices
                // unsigned int indexCount
                // bool indexes32Bit
                // unsigned int* faceVertexIndices (indexCount)
                // OR
                // unsigned short* faceVertexIndices (indexCount)
                // M_GEOMETRY chunk (Optional: present only if useSharedVertices = false)
                M_SUBMESH_OPERATION = 0x4010, // optional, trilist assumed if missing
                    // unsigned short operationType
                M_SUBMESH_BONE_ASSIGNMENT = 0x4100,
                    // Optional bone weights (repeating section)
                    // unsigned int vertexIndex;
                    // unsigned short boneIndex;
                    // float weight;
            M_GEOMETRY          = 0x5000, // NB this chunk is embedded within M_MESH and M_SUBMESH
			*/
                // unsigned int vertexCount
                // float* pVertices (x, y, z order x numVertices)
                M_GEOMETRY_NORMALS = 0x5100,    //(Optional)
                    // float* pNormals (x, y, z order x numVertices)
                M_GEOMETRY_COLOURS = 0x5200,    //(Optional)
                    // unsigned long* pColours (RGBA 8888 format x numVertices)
                M_GEOMETRY_TEXCOORDS = 0x5300    //(Optional, REPEATABLE, each one adds an extra set)
                    // unsigned short dimensions    (1 for 1D, 2 for 2D, 3 for 3D)
                    // float* pTexCoords  (u [v] [w] order, dimensions x numVertices)
			/*
            M_MESH_SKELETON_LINK = 0x6000,
                // Optional link to skeleton
                // char* skeletonName           : name of .skeleton to use
            M_MESH_BONE_ASSIGNMENT = 0x7000,
                // Optional bone weights (repeating section)
                // unsigned int vertexIndex;
                // unsigned short boneIndex;
                // float weight;
            M_MESH_LOD = 0x8000,
                // Optional LOD information
                // unsigned short numLevels;
                // bool manual;  (true for manual alternate meshes, false for generated)
                M_MESH_LOD_USAGE = 0x8100,
                // Repeating section, ordered in increasing depth
				// NB LOD 0 (full detail from 0 depth) is omitted
                // float fromSquaredDepth;
                    M_MESH_LOD_MANUAL = 0x8110,
                    // Required if M_MESH_LOD section manual = true
                    // String manualMeshName;
                    M_MESH_LOD_GENERATED = 0x8120,
                    // Required if M_MESH_LOD section manual = false
					// Repeating section (1 per submesh)
                    // unsigned int indexCount;
                    // bool indexes32Bit
                    // unsigned short* faceIndexes;  (indexCount)
                    // OR
                    // unsigned int* faceIndexes;  (indexCount)
            M_MESH_BOUNDS = 0x9000
                // float minx, miny, minz
                // float maxx, maxy, maxz
                // float radius

			// Added By DrEvil
			// optional chunk that contains a table of submesh indexes and the names of
			// the sub-meshes.
			M_SUBMESH_NAME_TABLE,
				// Subchunks of the name table. Each chunk contains an index & string
				M_SUBMESH_NAME_TABLE_ELEMENT,
	                // short index
                    // char* name

	*/
    };
	/** @} */
	/** @} */
	
	  /// Vertex element semantics, used to identify the meaning of vertex buffer contents
	enum VertexElementSemantic {
		/// Position, 3 reals per vertex
		VES_POSITION = 1,
		/// Blending weights
		VES_BLEND_WEIGHTS = 2,
        /// Blending indices
        VES_BLEND_INDICES = 3,
		/// Normal, 3 reals per vertex
		VES_NORMAL = 4,
		/// Diffuse colours
		VES_DIFFUSE = 5,
		/// Specular colours
		VES_SPECULAR = 6,
		/// Texture coordinates
		VES_TEXTURE_COORDINATES = 7,
        /// Binormal (Y axis if normal is Z)
        VES_BINORMAL = 8,
        /// Tangent (X axis if normal is Z)
        VES_TANGENT = 9,
        /// The  number of VertexElementSemantic elements (note - the first value VES_POSITION is 1) 
        VES_COUNT = 9
	};

	/// Vertex element type, used to identify the base types of the vertex contents
	enum VertexElementType
    {
        VET_FLOAT1 = 0,
        VET_FLOAT2 = 1,
        VET_FLOAT3 = 2,
        VET_FLOAT4 = 3,
        /// alias to more specific colour type - use the current rendersystem's colour packing
		VET_COLOUR = 4,
		VET_SHORT1 = 5,
		VET_SHORT2 = 6,
		VET_SHORT3 = 7,
		VET_SHORT4 = 8,
        VET_UBYTE4 = 9,
        /// D3D style compact colour
        VET_COLOUR_ARGB = 10,
        /// GL style compact colour
        VET_COLOUR_ABGR = 11,
		VET_DOUBLE1 = 12,
        VET_DOUBLE2 = 13,
        VET_DOUBLE3 = 14,
        VET_DOUBLE4 = 15,
        VET_USHORT1 = 16,
        VET_USHORT2 = 17,
        VET_USHORT3 = 18,
        VET_USHORT4 = 19,      
        VET_INT1 = 20,
        VET_INT2 = 21,
        VET_INT3 = 22,
        VET_INT4 = 23,
        VET_UINT1 = 24,
        VET_UINT2 = 25,
        VET_UINT3 = 26,
        VET_UINT4 = 27
    };


	enum OperationType {
			/// A list of points, 1 vertex per point
            OT_POINT_LIST = 1,
			/// A list of lines, 2 vertices per line
            OT_LINE_LIST = 2,
			/// A strip of connected lines, 1 vertex per line plus 1 start vertex
            OT_LINE_STRIP = 3,
			/// A list of triangles, 3 vertices per triangle
            OT_TRIANGLE_LIST = 4,
			/// A strip of triangles, 3 vertices for the first triangle, and 1 per triangle after that 
            OT_TRIANGLE_STRIP = 5,
			/// A fan of triangles, 3 vertices for the first triangle, and 1 per triangle after that
            OT_TRIANGLE_FAN = 6,
			/// Patch control point operations, used with tesselation stages
			OT_PATCH_1_CONTROL_POINT	= 7,
			OT_PATCH_2_CONTROL_POINT	= 8,
			OT_PATCH_3_CONTROL_POINT	= 9,
			OT_PATCH_4_CONTROL_POINT	= 10,
			OT_PATCH_5_CONTROL_POINT	= 11,
			OT_PATCH_6_CONTROL_POINT	= 12,
			OT_PATCH_7_CONTROL_POINT	= 13,
			OT_PATCH_8_CONTROL_POINT	= 14,
			OT_PATCH_9_CONTROL_POINT	= 15,
			OT_PATCH_10_CONTROL_POINT	= 16,
			OT_PATCH_11_CONTROL_POINT	= 17,
			OT_PATCH_12_CONTROL_POINT	= 18,
			OT_PATCH_13_CONTROL_POINT	= 19,
			OT_PATCH_14_CONTROL_POINT	= 20,
			OT_PATCH_15_CONTROL_POINT	= 21,
			OT_PATCH_16_CONTROL_POINT	= 22,
			OT_PATCH_17_CONTROL_POINT	= 23,
			OT_PATCH_18_CONTROL_POINT	= 24,
			OT_PATCH_19_CONTROL_POINT	= 25,
			OT_PATCH_20_CONTROL_POINT	= 26,
			OT_PATCH_21_CONTROL_POINT	= 27,
			OT_PATCH_22_CONTROL_POINT	= 28,
			OT_PATCH_23_CONTROL_POINT	= 29,
			OT_PATCH_24_CONTROL_POINT	= 30,
			OT_PATCH_25_CONTROL_POINT	= 31,
			OT_PATCH_26_CONTROL_POINT	= 32,
			OT_PATCH_27_CONTROL_POINT	= 33,
			OT_PATCH_28_CONTROL_POINT	= 34,
			OT_PATCH_29_CONTROL_POINT	= 35,
			OT_PATCH_30_CONTROL_POINT	= 36,
			OT_PATCH_31_CONTROL_POINT	= 37,
			OT_PATCH_32_CONTROL_POINT	= 38
        };

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Animation
	*  @{
	*/
/** Definition of the OGRE .skeleton file format 

    .skeleton files are binary files (for read efficiency at runtime) and are arranged into chunks 
    of data, very like 3D Studio's format.
    A chunk always consists of:
        unsigned short CHUNK_ID        : one of the following chunk ids identifying the chunk
        unsigned long  LENGTH          : length of the chunk in bytes, including this header
        void*          DATA            : the data, which may contain other sub-chunks (various data types)
    
    A .skeleton file contains both the definition of the Skeleton object and the animations it contains. It
    contains only a single skeleton but can contain multiple animations.


*/
    enum SkeletonChunkID {
        SKELETON_HEADER            = 0x1000,
            // char* version           : Version number check
			SKELETON_BLENDMODE		   = 0x1010, // optional
				// unsigned short blendmode		: SkeletonAnimationBlendMode
		
        SKELETON_BONE              = 0x2000,
        // Repeating section defining each bone in the system. 
        // Bones are assigned indexes automatically based on their order of declaration
        // starting with 0.

            // char* name                       : name of the bone
            // unsigned short handle            : handle of the bone, should be contiguous & start at 0
            // Vector3 position                 : position of this bone relative to parent 
            // Quaternion orientation           : orientation of this bone relative to parent 
            // Vector3 scale                    : scale of this bone relative to parent 

        SKELETON_BONE_PARENT       = 0x3000,
        // Record of the parent of a single bone, used to build the node tree
        // Repeating section, listed in Bone Index order, one per Bone

            // unsigned short handle             : child bone
            // unsigned short parentHandle   : parent bone

        SKELETON_ANIMATION         = 0x4000,
        // A single animation for this skeleton

            // char* name                       : Name of the animation
            // float length                      : Length of the animation in seconds
		
			SKELETON_ANIMATION_BASEINFO = 0x4010,
			// [Optional] base keyframe information
			// char* baseAnimationName (blank for self)
			// float baseKeyFrameTime

            SKELETON_ANIMATION_TRACK = 0x4100,
            // A single animation track (relates to a single bone)
            // Repeating section (within SKELETON_ANIMATION)
                
                // unsigned short boneIndex     : Index of bone to apply to

                SKELETON_ANIMATION_TRACK_KEYFRAME = 0x4110,
                // A single keyframe within the track
                // Repeating section

                    // float time                    : The time position (seconds)
                    // Quaternion rotate            : Rotation to apply at this keyframe
                    // Vector3 translate            : Translation to apply at this keyframe
                    // Vector3 scale                : Scale to apply at this keyframe
		SKELETON_ANIMATION_LINK         = 0x5000
		// Link to another skeleton, to re-use its animations

			// char* skeletonName					: name of skeleton to get animations from
			// float scale							: scale to apply to trans/scale keys

    };
	/** @} */
	/** @} */
#endif

#include "Common.h"
#include "Font.h"
#include "nvImage/include/nvImage.h"

#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "nvImaged.lib")
#pragma comment(lib, "opengl32.lib")

using namespace std;
using namespace DistanceField;

Font* gFont;

//wstring gUnicodeString =L"\t濛文件格式：\n\
//						 若不明确就标为未知\n\
//						 表演者：	若不明确就标为未知\n\
//						 专辑：		若不明确就标为未知\n\
//						 持续时间：01:01:00超过1小时；\n\
//						 09:09不足小时；00:09不足1分钟\n\
//						 glBindTexture(GL_TEXTURE_2D,pCharTex->m_texID);\n\
//						 glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );\n\
//						 glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );\n\
//						 glEnable(GL_BLEND);\n\
//						 glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);" ;

wstring gUnicodeString =L"譅";

GLuint gTexture;
GLuint gProgram;
GLint  gTexParam;

void init(void)
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glEnable ( GL_COLOR_MATERIAL );
	glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
	glEnable ( GL_TEXTURE_2D );
	glDisable ( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	/*glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);*/

	int status;
	bool valid;

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	std::ifstream vif("distance-field.vert");
	std::string vStr((std::istreambuf_iterator<char>(vif)), std::istreambuf_iterator<char>());
	const char* vs =  vStr.c_str();
	glShaderSource(vertexShaderID, 1, &vs, NULL);
	glCompileShader(vertexShaderID);	
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &status);
	valid = (status != 0);

	if (!valid)
	{
		string err;
		int length;
		glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &length);
		err.resize(length);
		glGetShaderInfoLog(vertexShaderID, length, &length, &err[0]);
		printf("%s\n", err.c_str());
	}


	GLuint pixelShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	std::ifstream fif("distance-field.frag");
	std::string fStr((std::istreambuf_iterator<char>(fif)), std::istreambuf_iterator<char>());
	const char* fs =  fStr.c_str();
	glShaderSource(pixelShaderID, 1, &fs, NULL);
	glCompileShader(pixelShaderID);
	glGetShaderiv(pixelShaderID, GL_COMPILE_STATUS, &status);
	valid = (status != 0);

	if (!valid)
	{
		string err;
		int length;
		glGetShaderiv(pixelShaderID, GL_INFO_LOG_LENGTH, &length);
		err.resize(length);
		glGetShaderInfoLog(pixelShaderID, length, &length, &err[0]);
		printf("%s\n", err.c_str());
	}

	gProgram = glCreateProgram();
	glAttachShader(gProgram, vertexShaderID);
	glAttachShader(gProgram, pixelShaderID);
	glLinkProgram(gProgram);

	glGetProgramiv(gProgram, GL_LINK_STATUS, &status);
	valid = (status != 0);

	if (!valid)
	{
		string err;
		int length;
		glGetProgramiv(gProgram, GL_INFO_LOG_LENGTH, &length);
		err.resize(length);
		glGetProgramInfoLog(gProgram, length, &length, &err[0]);
		printf("%s\n", err.c_str());
	}
	
	gTexParam = glGetUniformLocation(gProgram, "texture");

	gFont = new Font("msyh.ttf", 32, gUnicodeString);

	glUniform1i(gTexParam, gFont->mTextureID);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


	// Create font texture
	/*glGenTextures(1, &gTexture);
	glBindTexture(GL_TEXTURE_2D,gTexture);

	nv::Image image;
	image.loadImageFromFile("DistanceFieldAum.png");

	glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, image.getWidth(), image.getHeight(), 0,
		GL_ALPHA,GL_UNSIGNED_BYTE,image.getLevel(0));

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );*/
}

void reshape( int w, int h )
{
	// Reset the coordinate system before modifying

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
	glOrtho(0,w,h,0, -1, 1);
	// Set the clipping volume
	glMatrixMode(GL_MODELVIEW);
}

void display( void )
{
	glClearColor(0.0f , 0.0f , 0.0f , 1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();

	glUseProgram(gProgram);

	/*glUseProgram(0);
	glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);*/

	//glBindTexture(GL_TEXTURE_2D, gTexture);
	//glActiveTexture(GL_TEXTURE0);

	//glColor3f(1, 0, 0);
	//glBegin(GL_QUADS);
	//	glTexCoord2f(0, 0); glVertex3f(100, 100,  1.0f);

	//	// top right
	//	glTexCoord2f(1, 0); glVertex3f(400, 100    ,  1.0f);

	//	// bottom right
	//	glTexCoord2f(1, 1);  glVertex3f(400 , 400,  1.0f);

	//	// bottom left
	//	glTexCoord2f(1, 0); glVertex3f(100, 400,  1.0f);
	//glEnd();

	//gFont->DrawText(gUnicodeString, 10, 50, 64);
	gFont->DrawText(gUnicodeString, 100, 300, 256);

	/*gFont->DrawText(L"阮濛铠，123456789", 10, 100, 64);
	gFont->DrawText(L"其是只说不做的那种。你要创业他们便说创业很艰苦失败率很高；你成功了他们", 10, 150, 32);
	gFont->DrawText(L"厦门南普陀寺，许愿的塔扔硬币，我两次扔入，愿望能成真", 10, 200, 32);
	gFont->DrawText(L"【支付通道开启】成功抢购#小米手机1S青春版#的用户已经可", 10, 250, 32);
	gFont->DrawText(L"中國風新娘禮服，美死了", 10, 300, 32);*/

	glutSwapBuffers();
}

void keyboard ( unsigned char key, int x, int y )  // Create Keyboard Function
{
	switch ( key ) {
	case 27:        // When Escape Is Pressed...
		exit ( 0 );   // Exit The Program
		break;        // Ready For Next Case
	default:        // Now Wrap It Up
		break;
	}
}

void arrow_keys ( int a_keys, int x, int y )  // Create Special Function (required for arrow keys)
{
	switch ( a_keys ) {
	case GLUT_KEY_UP:     // When Up Arrow Is Pressed...
		glutFullScreen ( ); // Go Into Full Screen Mode
		break;
	case GLUT_KEY_DOWN:               // When Down Arrow Is Pressed...
		glutReshapeWindow ( 800, 800 ); // Go Into A 500 By 500 Window
		break;
	default:
		break;
	}
}

int main ( int argc, char** argv )   // Create Main Function For Bringing It All Together
{
	glutInit            ( &argc, argv ); // Erm Just Write It =)
	glutInitDisplayMode ( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA ); // Display Mode
	glutInitWindowPosition (0,0);
	glutInitWindowSize  ( 500, 500 ); // If glutFullScreen wasn't called this is the window size
	glutCreateWindow    ( "NeHe Lesson 6- Ported by Rustad" ); // Window Title (argv[0] for current directory as title)
	
	glewInit();
	
	init ();
	//glutFullScreen      ( );          // Put Into Full Screen
	glutDisplayFunc     ( display );  // Matching Earlier Functions To Their Counterparts
	glutReshapeFunc     ( reshape );
	glutKeyboardFunc    ( keyboard );
	glutSpecialFunc     ( arrow_keys );
	glutIdleFunc			 ( display );
	glutMainLoop        ( );          // Initialize The Main Loop
	return 1;
}

/*
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include <vector>
#include <fstream>
#include <iostream>


#ifdef _MSC_VER
#define MIN __min
#define MAX __max
#else
#define MIN std::min
#define MAX std::max
#endif


// Define some fixed size types.

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;


// Try to figure out what endian this machine is using. Note that the test
// below might fail for cross compilation; additionally, multi-byte
// characters are implementation-defined in C preprocessors.

#if (('1234' >> 24) == '1')
#elif (('4321' >> 24) == '1')
#define BIG_ENDIAN
#else
#error "Couldn't determine the endianness!"
#endif


// A simple 32-bit pixel.

union Pixel32
{
	Pixel32()
		: integer(0) { }
	Pixel32(uint8 bi, uint8 gi, uint8 ri, uint8 ai = 255)
	{
		b = bi;
		g = gi;
		r = ri;
		a = ai;
	}

	uint32 integer;

	struct
	{
#ifdef BIG_ENDIAN
		uint8 a, r, g, b;
#else // BIG_ENDIAN
		uint8 b, g, r, a;
#endif // BIG_ENDIAN
	};
};


struct Vec2
{
	Vec2() { }
	Vec2(float a, float b)
		: x(a), y(b) { }

	float x, y;
};


struct Rect
{
	Rect() { }
	Rect(float left, float top, float right, float bottom)
		: xmin(left), xmax(right), ymin(top), ymax(bottom) { }

	void Include(const Vec2 &r)
	{
		xmin = MIN(xmin, r.x);
		ymin = MIN(ymin, r.y);
		xmax = MAX(xmax, r.x);
		ymax = MAX(ymax, r.y);
	}

	float Width() const { return xmax - xmin + 1; }
	float Height() const { return ymax - ymin + 1; }

	float xmin, xmax, ymin, ymax;
};


// TGA Header struct to make it simple to dump a TGA to disc.

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(push, 1)
#pragma pack(1)               // Dont pad the following struct.
#endif

struct TGAHeader
{
	uint8   idLength,           // Length of optional identification sequence.
		paletteType,        // Is a palette present? (1=yes)
		imageType;          // Image data type (0=none, 1=indexed, 2=rgb,
	// 3=grey, +8=rle packed).
	uint16  firstPaletteEntry,  // First palette index, if present.
		numPaletteEntries;  // Number of palette entries, if present.
	uint8   paletteBits;        // Number of bits per palette entry.
	uint16  x,                  // Horiz. pixel coord. of lower left of image.
		y,                  // Vert. pixel coord. of lower left of image.
		width,              // Image width in pixels.
		height;             // Image height in pixels.
	uint8   depth,              // Image color depth (bits per pixel).
		descriptor;         // Image attribute flags.
};

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(pop)
#endif


bool
	WriteTGA(const std::string &filename,
	const Pixel32 *pxl,
	uint16 width,
	uint16 height)
{
	std::ofstream file(filename.c_str(), std::ios::binary);
	if (file)
	{
		TGAHeader header;
		memset(&header, 0, sizeof(TGAHeader));
		header.imageType  = 2;
		header.width = width;
		header.height = height;
		header.depth = 32;
		header.descriptor = 0x20;

		file.write((const char *)&header, sizeof(TGAHeader));
		file.write((const char *)pxl, sizeof(Pixel32) * width * height);

		return true;
	}
	return false;
}


// A horizontal pixel span generated by the FreeType renderer.

struct Span
{
	Span() { }
	Span(int _x, int _y, int _width, int _coverage)
		: x(_x), y(_y), width(_width), coverage(_coverage) { }

	int x, y, width, coverage;
};

typedef std::vector<Span> Spans;


// Each time the renderer calls us back we just push another span entry on
// our list.

void
	RasterCallback(const int y,
	const int count,
	const FT_Span * const spans,
	void * const user) 
{
	Spans *sptr = (Spans *)user;
	for (int i = 0; i < count; ++i) 
		sptr->push_back(Span(spans[i].x, y, spans[i].len, spans[i].coverage));
}


// Set up the raster parameters and render the outline.

void
	RenderSpans(FT_Library &library,
	FT_Outline * const outline,
	Spans *spans) 
{
	FT_Raster_Params params;
	memset(&params, 0, sizeof(params));
	params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
	params.gray_spans = RasterCallback;
	params.user = spans;

	FT_Outline_Render(library, outline, &params);
}


// Render the specified character as a colored glyph with a colored outline
// and dump it to a TGA.

void
	WriteGlyphAsTGA(FT_Library &library,
	const std::string &fileName,
	wchar_t ch,
	FT_Face &face,
	int size,
	const Pixel32 &fontCol,
	const Pixel32 outlineCol,
	float outlineWidth)
{
	// Set the size to use.
	if (FT_Set_Char_Size(face, size << 6, size << 6, 90, 90) == 0)
	{
		// Load the glyph we are looking for.
		FT_UInt gindex = FT_Get_Char_Index(face, ch);
		if (FT_Load_Glyph(face, gindex, FT_LOAD_NO_BITMAP) == 0)
		{
			// Need an outline for this to work.
			if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
			{
				// Render the basic glyph to a span list.
				Spans spans;
				RenderSpans(library, &face->glyph->outline, &spans);

				// Next we need the spans for the outline.
				Spans outlineSpans;

				// Set up a stroker.
				FT_Stroker stroker;
				FT_Stroker_New(library, &stroker);
				FT_Stroker_Set(stroker,
					(int)(outlineWidth * 64),
					FT_STROKER_LINECAP_ROUND,
					FT_STROKER_LINEJOIN_ROUND,
					0);

				FT_Glyph glyph;
				if (FT_Get_Glyph(face->glyph, &glyph) == 0)
				{
					FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
					// Again, this needs to be an outline to work.
					if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
					{
						// Render the outline spans to the span list
						FT_Outline *o =
							&reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
						RenderSpans(library, o, &outlineSpans);
					}

					// Clean up afterwards.
					FT_Stroker_Done(stroker);
					FT_Done_Glyph(glyph);

					// Now we need to put it all together.
					if (!spans.empty())
					{
						// Figure out what the bounding rect is for both the span lists.
						Rect rect(spans.front().x,
							spans.front().y,
							spans.front().x,
							spans.front().y);
						for (Spans::iterator s = spans.begin(); s != spans.end(); ++s)
						{
							rect.Include(Vec2(s->x, s->y));
							rect.Include(Vec2(s->x + s->width - 1, s->y));
						}
						for (Spans::iterator s = outlineSpans.begin(); s != outlineSpans.end(); ++s)
						{
							rect.Include(Vec2(s->x, s->y));
							rect.Include(Vec2(s->x + s->width - 1, s->y));
						}

#if 0
						// This is unused in this test but you would need this to draw
						// more than one glyph.
						float bearingX = face->glyph->metrics.horiBearingX >> 6;
						float bearingY = face->glyph->metrics.horiBearingY >> 6;
						float advance = face->glyph->advance.x >> 6;
#endif
						// Get some metrics of our image.
						int imgWidth = rect.Width(),
							imgHeight = rect.Height(),
							imgSize = imgWidth * imgHeight;

						// Allocate data for our image and clear it out to transparent.
						Pixel32 *pxl = new Pixel32[imgSize];
						memset(pxl, 0, sizeof(Pixel32) * imgSize);

						// Loop over the outline spans and just draw them into the image.
						for (Spans::iterator s = outlineSpans.begin(); s != outlineSpans.end(); ++s)
							for (int w = 0; w < s->width; ++w)
								pxl[(int)((imgHeight - 1 - (s->y - rect.ymin)) * imgWidth
								+ s->x - rect.xmin + w)] =
								Pixel32(outlineCol.r, outlineCol.g, outlineCol.b,
								s->coverage);

						// Then loop over the regular glyph spans and blend them into
						// the image.
						for (Spans::iterator s = spans.begin();
							s != spans.end(); ++s)
							for (int w = 0; w < s->width; ++w)
							{
								Pixel32 &dst =
									pxl[(int)((imgHeight - 1 - (s->y - rect.ymin)) * imgWidth
									+ s->x - rect.xmin + w)];
								Pixel32 src = Pixel32(fontCol.r, fontCol.g, fontCol.b,
									s->coverage);
								dst.r = (int)(dst.r + ((src.r - dst.r) * src.a) / 255.0f);
								dst.g = (int)(dst.g + ((src.g - dst.g) * src.a) / 255.0f);
								dst.b = (int)(dst.b + ((src.b - dst.b) * src.a) / 255.0f);
								dst.a = MIN(255, dst.a + src.a);
							}

							// Dump the image to disk.
							WriteTGA(fileName, pxl, imgWidth, imgHeight);

							delete [] pxl;
					}
				}
			}
		}
	}
}


int
	main(int argc,
	char **argv)
{

	const char* fontname = "msyh.ttf";

	// Initialize FreeType.
	FT_Library library;
	FT_Init_FreeType(&library);

	// Open up a font file.
	std::ifstream fontFile(fontname, std::ios::binary);
	if (fontFile)
	{
		// Read the entire file to a memory buffer.
		fontFile.seekg(0, std::ios::end);
		std::fstream::pos_type fontFileSize = fontFile.tellg();
		fontFile.seekg(0);
		unsigned char *fontBuffer = new unsigned char[fontFileSize];
		fontFile.read((char *)fontBuffer, fontFileSize);

		// Create a face from a memory buffer.  Be sure not to delete the memory
		// buffer until you are done using that font as FreeType will reference
		// it directly.
		FT_Face face;
		FT_New_Memory_Face(library, fontBuffer, fontFileSize, 0, &face);

		// Dump out a single glyph to a tga.
		WriteGlyphAsTGA(library,
			"outline.tga",
			L'全',
			face,
			100,
			Pixel32(255, 90, 30),
			Pixel32(255, 255, 255),
			3.0f);

		// Now that we are done it is safe to delete the memory.
		delete [] fontBuffer;
	}

	// Clean up the library
	FT_Done_FreeType(library);

	return 1;
}
 */
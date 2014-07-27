#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <cstdint>
#include <cassert>
#include <vector>
#include <map>
#include <algorithm>
#include <GL/glew.h>
#include <GL/glut.h>

#include <ft2build.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include FT_FREETYPE_H

#include "pfm.h"

#pragma comment(lib, "freetype")
#pragma comment(lib, "opengl32")
#pragma comment(lib, "glew32")

#define INFINITY 99999999999
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

// Round and INFINITY only defined in C99
double round(double r) {
	return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

struct FontGlyph
{
	int32_t Width, Height;
	// Glyph X, Y offset from origin.
	int32_t OffsetX, OffsetY;
	// the horizontal advance value for the glyph. 
	int32_t Advance;

	std::vector<unsigned char> Data;
};

GLuint gTexture;
GLuint gProgram;
GLint  gTexParam;
FontGlyph gGlyph;
float angle = 0;

void load_glyph( const char *  filename,  const wchar_t charcode,
				 const float   highres_size, const float lowres_size,
				 const float   padding, FontGlyph& glyph )
{
	FT_Library library;
	FT_Face face;

	FT_Init_FreeType( &library );
	FT_New_Face( library, filename, 0, &face );
	FT_Select_Charmap( face, FT_ENCODING_UNICODE );
	FT_UInt glyph_index = FT_Get_Char_Index( face, charcode );

	// Render glyph at high resolution (highres_size points)
	FT_Set_Char_Size( face, highres_size*64, 0, 72, 72 );
	FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT);
	FT_GlyphSlot slot = face->glyph;
	FT_Bitmap bitmap = slot->bitmap;

	// Allocate high resolution buffer
	size_t highres_width  = bitmap.width + 2*padding*highres_size;
	size_t highres_height = bitmap.rows + 2*padding*highres_size;
	double * highres_data = (double *) malloc( highres_width*highres_height*sizeof(double) );
	memset( highres_data, 0, highres_width*highres_height*sizeof(double) );

	// Copy high resolution bitmap with padding and normalize values
	for( int j=0; j < bitmap.rows; ++j )
	{
		for( int i=0; i < bitmap.width; ++i )
		{
			int x = i + padding;
			int y = j + padding;
			highres_data[y*highres_width+x] = bitmap.buffer[j*bitmap.width+i]/255.0;
		}
	}

	// Compute distance map
	distance_map( highres_data, highres_width, highres_height );

	// Allocate low resolution buffer
	size_t lowres_width  = round(highres_width * lowres_size/highres_size);
	size_t lowres_height = round(highres_height * lowres_width/(float) highres_width);
	double * lowres_data = (double *) malloc( lowres_width*lowres_height*sizeof(double) );
	memset( lowres_data, 0, lowres_width*lowres_height*sizeof(double) );

	// Scale down highres buffer into lowres buffer
	resize( highres_data, highres_width, highres_height, lowres_data, lowres_width,  lowres_height );

	// Convert the (double *) lowres buffer into a (unsigned char *) buffer and
	// rescale values between 0 and 255.
	//unsigned char * data = (unsigned char *) malloc( lowres_width*lowres_height*sizeof(unsigned char) );
	
	glyph.Data.resize(lowres_width*lowres_height);

	for( size_t j=0; j < lowres_height; ++j )
	{
		for( size_t i=0; i < lowres_width; ++i )
		{
			double v = lowres_data[j*lowres_width+i];
			glyph.Data[j*lowres_width+i] = (int) (255*(1-v));
		}
	}


	// Compute new glyph information from highres value
	float ratio = lowres_size / highres_size;
	size_t pitch = lowres_width * sizeof( unsigned char );

	glyph.OffsetX = (slot->bitmap_left + padding*highres_width) * ratio;
	glyph.OffsetY = (slot->bitmap_top + padding*highres_height) * ratio;
	glyph.Width   = lowres_width;
	glyph.Height  = lowres_height;
    glyph.Advance = ratio * face->glyph->advance.x/64.0;

    free( highres_data );
	free( lowres_data );
	//free(data);
}

int Size = 128;

void init(void)
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glEnable ( GL_TEXTURE_2D );
	glDisable ( GL_CULL_FACE );

	glGenTextures(1, &gTexture);
	glBindTexture(GL_TEXTURE_2D, gTexture);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	load_glyph("msyhbd.ttf", L'A', 512, 32, 0.1, gGlyph);

	std::vector<unsigned char> TextureData(Size * Size, 0);

	for (size_t y = 0; y < gGlyph.Height; ++y)
	{
		for (size_t x = 0; x < gGlyph.Width; ++x)
		{
			TextureData[y*Size+x] = gGlyph.Data[y*gGlyph.Width+x];
		}
	}

	/*std::vector<double> downData(Size * Size / 4, 0);
	resize(&TextureData[0],Size, Size, &downData[0], Size/2, Size/2);

	std::vector<unsigned char> downDataUC(Size * Size / 4, 0);
	std::transform(downData.begin(), downData.end(), downDataUC.begin(), [](double c){return int(c*255);});*/

	glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, Size, Size, 0, GL_ALPHA, GL_UNSIGNED_BYTE, &TextureData[0]);	


	//WritePfm("test.pfm", 64, 64, 1, &TextureData[0]);

	// Load shaders
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
		std::string err;
		int length;
		glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &length);
		err.resize(length);
		glGetShaderInfoLog(vertexShaderID, length, &length, &err[0]);
		printf("%s\n", err.c_str());
	}


	GLuint pixelShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	std::ifstream fif("distance-field-3.frag");
	std::string fStr((std::istreambuf_iterator<char>(fif)), std::istreambuf_iterator<char>());
	const char* fs =  fStr.c_str();
	glShaderSource(pixelShaderID, 1, &fs, NULL);
	glCompileShader(pixelShaderID);
	glGetShaderiv(pixelShaderID, GL_COMPILE_STATUS, &status);
	valid = (status != 0);

	if (!valid)
	{
		std::string err;
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
		std::string err;
		int length;
		glGetProgramiv(gProgram, GL_INFO_LOG_LENGTH, &length);
		err.resize(length);
		glGetProgramInfoLog(gProgram, length, &length, &err[0]);
		printf("%s\n", err.c_str());
	}

	gTexParam = glGetUniformLocation(gProgram, "texture");
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

void display( void )
{
	glClearColor(0.1f , 0.1f , 0.1f , 1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//glLoadIdentity();

    glUseProgram( gProgram );

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexture);
	glEnable( GL_TEXTURE_2D );
	//glEnable( GL_BLEND );
	glEnableIndexedEXT(GL_BLEND, 0);

	glBlendFuncSeparate(GL_SRC_ALPHA, 
		GL_ONE_MINUS_SRC_ALPHA,
		GL_ONE, 
		GL_ZERO);

	
	//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	GLuint handle = glGetUniformLocation( gProgram, "texture" );
	glUniform1i( handle, 0);

	int width = 512;
	int height = 512;
	if( gGlyph.Width > gGlyph.Height )
		height = gGlyph.Height  * width/(float)gGlyph.Width;
	else
		width = gGlyph.Width * height/(float)gGlyph.Height;
	int x = 0 - width/2;
	int y = 0 - height/2;

	glPushMatrix();
	glTranslatef(256,256,0);
	//glRotatef(angle, 0,0,1);
	float s = .025+.975*(1+1/*cos(angle/100.0)*/)/2.;
	glScalef(s,s,s);

	float s0 = 0.0;
	float t0 = 0.0;
	float s1 = (gGlyph.Width) / float(Size);
	float t1 = (gGlyph.Height) / float(Size);

	glBegin(GL_QUADS);
	glTexCoord2f( s0, t1 ); glVertex2f( x, y );
	glTexCoord2f( s0, t0 ); glVertex2f( x, y+height );
	glTexCoord2f( s1, t0 ); glVertex2f( x+width, y+height );
	glTexCoord2f( s1, t1 ); glVertex2f( x+width, y );
	glEnd();
	glPopMatrix();

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

void timer( int fps )
{
	glutPostRedisplay();
	glutTimerFunc( 1000.0/fps, timer, fps );
	angle += .5;
}

int main ( int argc, char** argv )   // Create Main Function For Bringing It All Together
{
	glutInit            ( &argc, argv ); // Erm Just Write It =)
	glutInitDisplayMode ( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB  ); // Display Mode
	glutInitWindowPosition (0,0);
	glutInitWindowSize  ( 512, 512 ); // If glutFullScreen wasn't called this is the window size
	glutCreateWindow    ( "NeHe Lesson 6- Ported by Rustad" ); // Window Title (argv[0] for current directory as title)
	 glutTimerFunc( 1000.0/60, timer, 60 );

	glewInit();
	glEnable(GL_ALPHA);
	
	init ();
	glutDisplayFunc     ( display );  // Matching Earlier Functions To Their Counterparts
	glutReshapeFunc     ( reshape );
	glutKeyboardFunc    ( keyboard );
	glutIdleFunc		( display );
	glutMainLoop        ( );          // Initialize The Main Loop

	return 1;
}

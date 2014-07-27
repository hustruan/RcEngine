#ifndef Common_h__
#define Common_h__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <exception>
#include <cstdint>
#include <cassert>
#include <list>
#include <unordered_map>
#include <map>

#include <amp.h>

#include <GL/glew.h>
#include <GL/glut.h>

void SaveTGA(int32_t w, int32_t h, uint8_t* pData);
void SaveTexture2D(const char* file, GLuint texID, int32_t width, int32_t height);

#endif // Common_h__
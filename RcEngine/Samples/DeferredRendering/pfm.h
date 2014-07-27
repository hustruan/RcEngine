#ifndef _PFM_H_
#define _PFM_H_

int ReadPfm(const char *fn, int &resX, int &resY, float*& data);
int WritePfm(const char *fn, int resX, int resY, int channels, const float* data);
int WritePfm3D(const char *fn, int resX, int resY, int resZ, int tiles, int channels, const float* data, float* buffer=0);

#endif
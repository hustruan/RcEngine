#include "pfm.h"
#include <stdio.h>
#include <string.h>

int WritePfm(const char *fn, int resX, int resY, int channels, const float* data)
{
	if(channels!=1&&channels!=3&&channels!=4)
	{
		return -2;
	}
	FILE* f;
	errno_t err=fopen_s(&f,fn,"wb");
	if(err!=0) return -1;
	const char* indicator;	
	switch(channels)
	{
	case 1:
		indicator="Pf";
		break;
	case 3:
		indicator="PF";
		break;
	case 4:
		indicator="P4";
		break;
	default:
		break;
	}
	fprintf_s(f,"%s\n%d %d\n%f\n",indicator,resX,resY,-1.f);		
	int written=fwrite(data,sizeof(float)*channels,resX*resY,f);
	if(written!=resX*resY)
	{
		fclose(f);
		return -3;
	}
	fclose(f);
	return 0;
}

int WritePfm3D(const char *fn, int resX, int resY, int resZ, int tiles, int channels, const float* data, float* buffer/*=NULL*/)
{
	 bool allocated=false;
	 if(buffer==NULL) 
	 {
	 	buffer=new float[resX*resY*tiles*tiles*channels];
	 	allocated=true;
	 }
	 memset(buffer,0,sizeof(float)*resX*resY*tiles*tiles*channels);
	 for(int z=0;z<resZ;z++)
	 {
	 	int tx=z%tiles;
	 	int ty=z/tiles;
	 	for(int x=0;x<resX;x++)
	 	{			
	 		for(int y=0;y<resY;y++)
	 		{
	 			int old_id=x+y*resX+z*resX*resY;
	 			int new_id=(tx*resX+x)+(ty*resY+y)*resX*tiles;
	 			for(int c=0;c<channels;c++)
	 			{
	 				buffer[channels*new_id+c]=data[channels*old_id+c];
	 			}
	 		}
	 	}
	 }
	 WritePfm(fn,resX*tiles,resY*tiles,channels,buffer);
	 if(allocated) delete[] buffer;
	return 0;
}

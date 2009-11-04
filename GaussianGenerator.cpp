#include "GaussianGenerator.h"

#include "OpenGL/Texture.h"

inline float evalHermite(float pA, float pB, float vA, float vB, float u)
{
	float u2=(u*u), u3=u2*u;
	float B0 = 2*u3 - 3*u2 + 1;
	float B1 = -2*u3 + 3*u2;
	float B2 = u3 - 2*u2 + u;
	float B3 = u3 - u;
	return( B0*pA + B1*pB + B2*vA + B3*vB );
}


unsigned char* createGaussianMap(int N)
{
	float *M = new float[2*N*N];
	unsigned char *B = new unsigned char[4*N*N];
	float X,Y,Y2,Dist;
	float Incr = 2.0f/N;
	int i=0;  
	int j = 0;
	Y = -1.0f;
	//float mmax = 0;
	for (int y=0; y<N; y++, Y+=Incr)
	{
		Y2=Y*Y;
		X = -1.0f;
		for (int x=0; x<N; x++, X+=Incr, i+=2, j+=4)
		{
			Dist = (float)sqrtf(X*X+Y2);
			if (Dist>1) Dist=1;
			M[i+1] = M[i] = evalHermite(1.0f,0,0,0,Dist);
			B[j+3] = B[j+2] = B[j+1] = B[j] = (unsigned char)(M[i] * 255);
		}
	}
	delete [] M;
	return(B);
}    


GlTexture* GaussianGenerator::make2D(int size)
{
	uchar* data = createGaussianMap(size);

	GlTexture *tex = new GlTexture();
	tex->init(GL_TEXTURE_2D, QSize(size, size), 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 
		data, GL_LINEAR, GL_LINEAR, GL_REPEAT);
	delete data; // GL_LINEAR_MIPMAP_LINEAR

	return tex;
}
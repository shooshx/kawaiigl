#ifndef GAUSSIANGENERATOR_H_INCLUDED
#define GAUSSIANGENERATOR_H_INCLUDED
#pragma once

class GlTexture;

class GaussianGenerator
{
public:
	GaussianGenerator() {}
	
	GlTexture* make2D(int size);
};


#endif
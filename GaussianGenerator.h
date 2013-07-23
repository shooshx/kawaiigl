#ifndef GAUSSIANGENERATOR_H_INCLUDED
#define GAUSSIANGENERATOR_H_INCLUDED
#pragma once

class GlTexture;

class GaussianGenerator
{
public:
	static GlTexture* make2D(int size);
};


class CheckersGen
{
public:
	static GlTexture* make3D(int size);
};


#endif
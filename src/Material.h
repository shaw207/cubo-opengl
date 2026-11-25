#pragma once
#include "common.h"
#include "libMath.h"
#include "Texture.h"
using namespace libMath;
class Material
{
public:
	float ka=1, kd=1, ks=1;
	bool usaTextura=false;
	vec4float color = {1,1,1,1};
	int shiny = 200;
	float alpha = 0.5f;
	Texture* textura = nullptr;
	Material() = default;
	~Material() { delete textura; }
	Material(std::string textureName, float ka, float kd, float ks);
};

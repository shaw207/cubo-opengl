#include "Material.h"

Material::Material(std::string textureName, float ka, float kd, float ks)
{
	this->ka = ka;
	this->kd = kd;
	this->ks = ks;
	this->textura = new Texture(textureName);
	this->usaTextura = true;
}

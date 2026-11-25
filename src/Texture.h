#pragma once
#include "common.h"
using namespace std;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
}pixel_t;

class Texture
{
public:
    int w=0, h=0;
    unsigned int GLId=0xFFFFFFFF; //identificador opengl
    string fileName = "";
    vector <pixel_t> rawImage;

    Texture(string fileName);
    ~Texture() { glDeleteTextures(1, &GLId); }

    void updateTexture(); //copiar datos agpu

};

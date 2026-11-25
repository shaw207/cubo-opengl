#pragma once
#include "common.h"
#include "libMath.h"

using namespace libMath;
class Luz
{
public:
    vec4float posicion = { 0,0,0,1 };
    vec4float direccion = { 0,0,-1,0 };

    float Ia = 0, Id = 0, Is = 0;
    vec4float color = { 1,1,1,1 };
    bool activa = true;

    Luz(vec4float pos, vec4float color = { 1,1,1,1 },
             float Ia = 0.2, float Id = 0.3, float Is = 1);
    Luz(vec4float direction, vec4float pos, vec4float color,
             float Ia, float Id, float Is);

    void moveObject(double timeStep);
};

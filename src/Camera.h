#pragma once

#include "common.h"
#include "Colliders.h"
#include "libMath.h"

using namespace libMath;
using namespace std;

class Camera
{
public:
    vec4float pos;
    vec4float rot;
    vec4float lookAt;
    vec4float lookAtPrime;
    vec4float forward, right, up;
    ColliderTree<Sphere>* coll = nullptr;
    float moveSpeed = 1.0f;
    float mouseSensitivity = 0.07f;

    Camera(vec4float pos, vec4float lookAt, vec4float up = { 0,1,0,0 });
    ~Camera();
    matrix4x4f computeViewMatrix();
    matrix4x4f computeProjectionMatrix(float zNear, float zFar, float fovy, float aspectRatio);

    void updateCollider();
    void moveObject(double timeStep, const ColliderTree<Sphere>* obstacle);
};

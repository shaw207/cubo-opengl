#include "Camera.h"
#include "EventManager.h"
#include <math.h>

Camera::Camera(vec4float pos, vec4float lookAt, vec4float up)
{
    this->pos = pos;
    this->rot = { 0,0,0,0 };
    this->lookAt= lookAt;
    this->up = up;
    this->forward = normalize(lookAt - pos);
    this->right = normalize(cross(forward, up));
    this->lookAtPrime = forward;
    this->moveSpeed = 1.0f;
    this->mouseSensitivity = 0.07f;

    rot.y = atan2f(forward.z, forward.x) * 180.0f / (float)M_PI;
    rot.x = asinf(forward.y) * 180.0f / (float)M_PI;

    coll = new ColliderTree<Sphere>(new colliderNode);
    coll->root->bv = new Sphere();

    particle3D* particle = new particle3D;
    particle->pos = { 0.0f,0.0f,0.0f,1.0f };
    particle->size = { 0.25f,0.25f,0.25f,0.0f };
    coll->root->bv->addParticle(particle);
    updateCollider();
}

Camera::~Camera()
{
    delete coll;
}

matrix4x4f Camera::computeViewMatrix()
{
    matrix4x4f view = make_identityf();
    this->forward = normalize(lookAt - pos);

    right = normalize(cross(forward, up));
    vec4float upCamera = normalize(cross(right, forward));
    vec4float position = make_vec4float(-dot(right, pos), -dot(upCamera, pos), dot(forward, pos), 1);
    view.matV[0] = right;
    view.matV[1] = upCamera;
    view.matV[2] = -1.0f*forward;
    view.mat2D[0][3] = position.x;
    view.mat2D[1][3] = position.y;
    view.mat2D[2][3] = position.z;
    view.matV[3] = { 0,0,0,1 };

    return view;
}

matrix4x4f Camera::computeProjectionMatrix(float zNear, float zFar, float fovy, float aspectRatio)
{
    matrix4x4f projMatrix = make_identityf();
    projMatrix.mat2D[0][0] =  1.0f / (aspectRatio * tanf(fovy * 0.5f));
    projMatrix.mat2D[1][1] =  1.0f / tan(fovy * 0.5f);
    projMatrix.mat2D[2][2] = -(zFar + zNear) / (zFar - zNear);
    projMatrix.mat2D[2][3] = -2.0f * zFar * zNear / (zFar - zNear);
    projMatrix.mat2D[3][2] = -1.0f;
    projMatrix.mat2D[3][3] = 0.0f;
    return projMatrix;
}

void Camera::updateCollider()
{
    if (coll != nullptr)
        ColliderTree<Sphere>::updateTree(coll->root, make_traslate(pos.x, pos.y, pos.z));
}

void Camera::moveObject(double timeStep, const ColliderTree<Sphere>* obstacle)
{
    float speed = moveSpeed;
    float speedRot = 50.0f;
    if (EventManager::keyMap[GLFW_KEY_LEFT_SHIFT])
        speed *= 2.0f;

    vec4float oldPos = pos;
    vec4float oldLookAt = lookAt;
    vec4float oldRot = rot;
    vec4float oldForward = forward;
    vec4float oldRight = right;
    vec4float oldUp = up;

    if (EventManager::keyMap[GLFW_KEY_H])
        rot.y += speedRot * (float)timeStep;

    if (EventManager::keyMap[GLFW_KEY_F])
        rot.y -= speedRot * (float)timeStep;

    rot.y += (float)EventManager::deltaMX * mouseSensitivity;
    rot.x -= (float)EventManager::deltaMY * mouseSensitivity;

    float yawRad = toRadians(rot.y);
    float pitchRad = toRadians(rot.x);

    vec4float dir = make_vec4float(
        cosf(pitchRad) * cosf(yawRad),
        sinf(pitchRad),
        cosf(pitchRad) * sinf(yawRad),
        0.0f
    );

    dir = normalize(dir);

    vec4float worldUp = make_vec4float(0.0f, 1.0f, 0.0f, 0.0f);
    vec4float rightMove = normalize(cross(dir, worldUp));
    vec4float upMove = normalize(cross(rightMove, dir));

    if (EventManager::keyMap[GLFW_KEY_W] || EventManager::keyMap[GLFW_KEY_UP])
    {
       pos = pos + dir * speed * (float)timeStep;
    }

    if (EventManager::keyMap[GLFW_KEY_S] || EventManager::keyMap[GLFW_KEY_DOWN])
    {
        pos = pos - dir * speed * (float)timeStep;
    }

    if (EventManager::keyMap[GLFW_KEY_A] || EventManager::keyMap[GLFW_KEY_LEFT])
    {
        pos = pos - rightMove * speed * (float)timeStep;
    }

    if (EventManager::keyMap[GLFW_KEY_D] || EventManager::keyMap[GLFW_KEY_RIGHT])
    {
        pos = pos + rightMove * speed * (float)timeStep;
    }

    if (EventManager::keyMap[GLFW_KEY_Q])
    {
        pos = pos + upMove * speed * (float)timeStep;
    }

    if (EventManager::keyMap[GLFW_KEY_E])
    {
        pos = pos - upMove * speed * (float)timeStep;
    }

    forward = dir;
    right = rightMove;
    up = upMove;
    lookAt = pos + forward;

    updateCollider();

    if (obstacle && ColliderTree<Sphere>::collisionTest(coll->root, obstacle->root))
    {
        pos = oldPos;
        lookAt = oldLookAt;
        rot = oldRot;
        forward = oldForward;
        right = oldRight;
        up = oldUp;
        updateCollider();
    }
}

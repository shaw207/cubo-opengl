#include "Colliders.h"

#include <algorithm>
#include <cmath>

Collider::~Collider()
{
    for (auto& p : particleList)
        delete p;
    particleList.clear();
}

bool Sphere::collisionTest(Collider* coll)
{
    Sphere* otherSphere = dynamic_cast<Sphere*>(coll);
    if (otherSphere != nullptr)
        return distance(center, otherSphere->center) <= (radius + otherSphere->radius);

    Box* otherBox = dynamic_cast<Box*>(coll);
    if (otherBox != nullptr)
        return otherBox->collisionTest(this);

    return false;
}

void Sphere::addParticle(particle3D* p)
{
    particleList.push_back(p);

    max.x = std::max(max.x, p->pos.x + p->size.x * 0.5f);
    max.y = std::max(max.y, p->pos.y + p->size.y * 0.5f);
    max.z = std::max(max.z, p->pos.z + p->size.z * 0.5f);

    min.x = std::min(min.x, p->pos.x - p->size.x * 0.5f);
    min.y = std::min(min.y, p->pos.y - p->size.y * 0.5f);
    min.z = std::min(min.z, p->pos.z - p->size.z * 0.5f);

    vec4float size = max - min;
    centerPrime = min + size * 0.5f;
    centerPrime.w = 1.0f;
    center = centerPrime;
    radiusPrime = length(size) * 0.5f;
    radius = radiusPrime;
}

void Sphere::updateCollider(matrix4x4f mat)
{
    center = mat * centerPrime;

    vec4float xAxis = mat * make_vec4float(radiusPrime, 0.0f, 0.0f, 0.0f);
    vec4float yAxis = mat * make_vec4float(0.0f, radiusPrime, 0.0f, 0.0f);
    vec4float zAxis = mat * make_vec4float(0.0f, 0.0f, radiusPrime, 0.0f);

    radius = std::max(length(xAxis), std::max(length(yAxis), length(zAxis)));
}

bool Box::collisionTest(Collider* coll)
{
    Box* otherBox = dynamic_cast<Box*>(coll);
    if (otherBox != nullptr)
    {
        return std::abs(center.x - otherBox->center.x) <= (halfSize.x + otherBox->halfSize.x) &&
            std::abs(center.y - otherBox->center.y) <= (halfSize.y + otherBox->halfSize.y) &&
            std::abs(center.z - otherBox->center.z) <= (halfSize.z + otherBox->halfSize.z);
    }

    Sphere* otherSphere = dynamic_cast<Sphere*>(coll);
    if (otherSphere != nullptr)
    {
        float closestX = std::max(center.x - halfSize.x, std::min(otherSphere->center.x, center.x + halfSize.x));
        float closestY = std::max(center.y - halfSize.y, std::min(otherSphere->center.y, center.y + halfSize.y));
        float closestZ = std::max(center.z - halfSize.z, std::min(otherSphere->center.z, center.z + halfSize.z));

        vec4float closest = { closestX, closestY, closestZ, 1.0f };
        return distance(closest, otherSphere->center) <= otherSphere->radius;
    }

    return false;
}

void Box::addParticle(particle3D* p)
{
    particleList.push_back(p);

    max.x = std::max(max.x, p->pos.x + p->size.x * 0.5f);
    max.y = std::max(max.y, p->pos.y + p->size.y * 0.5f);
    max.z = std::max(max.z, p->pos.z + p->size.z * 0.5f);

    min.x = std::min(min.x, p->pos.x - p->size.x * 0.5f);
    min.y = std::min(min.y, p->pos.y - p->size.y * 0.5f);
    min.z = std::min(min.z, p->pos.z - p->size.z * 0.5f);

    vec4float size = max - min;
    centerPrime = min + size * 0.5f;
    centerPrime.w = 1.0f;
    center = centerPrime;
    halfSizePrime = size * 0.5f;
    halfSizePrime.w = 0.0f;
    halfSize = halfSizePrime;
}

void Box::updateCollider(matrix4x4f mat)
{
    center = mat * centerPrime;

    vec4float xAxis = mat * make_vec4float(halfSizePrime.x, 0.0f, 0.0f, 0.0f);
    vec4float yAxis = mat * make_vec4float(0.0f, halfSizePrime.y, 0.0f, 0.0f);
    vec4float zAxis = mat * make_vec4float(0.0f, 0.0f, halfSizePrime.z, 0.0f);

    halfSize = {
        std::abs(xAxis.x) + std::abs(yAxis.x) + std::abs(zAxis.x),
        std::abs(xAxis.y) + std::abs(yAxis.y) + std::abs(zAxis.y),
        std::abs(xAxis.z) + std::abs(yAxis.z) + std::abs(zAxis.z),
        0.0f
    };
}

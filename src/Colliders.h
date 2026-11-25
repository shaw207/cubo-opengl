#pragma once

#include "common.h"
#include "libMath.h"

using namespace libMath;

typedef struct
{
    vec4float pos;
    vec4float size;
} particle3D;

class Collider
{
public:
    vector<particle3D*> particleList;
    vec4float max = { -1000000.0f,-1000000.0f,-1000000.0f,1.0f };
    vec4float min = { 1000000.0f,1000000.0f,1000000.0f,1.0f };

    virtual ~Collider();
    virtual bool collisionTest(Collider* coll) = 0;
    virtual void addParticle(particle3D* particle) = 0;
    virtual void updateCollider(matrix4x4f mat) = 0;
};

class Sphere : public Collider
{
public:
    float radius = 0.0f;
    float radiusPrime = 0.0f;
    vec4float center = { 0,0,0,1 };
    vec4float centerPrime = { 0,0,0,1 };

    bool collisionTest(Collider* coll) override;
    void addParticle(particle3D* particle) override;
    void updateCollider(matrix4x4f mat) override;
};

class Box : public Collider
{
public:
    vec4float center = { 0,0,0,1 };
    vec4float halfSize = { 0,0,0,0 };
    vec4float centerPrime = { 0,0,0,1 };
    vec4float halfSizePrime = { 0,0,0,0 };

    bool collisionTest(Collider* coll) override;
    void addParticle(particle3D* particle) override;
    void updateCollider(matrix4x4f mat) override;
};

typedef struct colliderNode
{
    Collider* bv = nullptr;
    struct colliderNode* sons[2] = { nullptr, nullptr };
} colliderNode;

template <class TypeCollider>
class ColliderTree
{
public:
    colliderNode* root = nullptr;

    ColliderTree(colliderNode* root) : root(root) {};
    ~ColliderTree()
    {
        destroyNode(root);
        root = nullptr;
    }

    static bool collisionTest(colliderNode* coll, colliderNode* otherColl)
    {
        if (coll == nullptr || otherColl == nullptr || coll->bv == nullptr || otherColl->bv == nullptr)
            return false;

        if (!coll->bv->collisionTest(otherColl->bv))
            return false;

        bool collHasSons = coll->sons[0] != nullptr || coll->sons[1] != nullptr;
        bool otherHasSons = otherColl->sons[0] != nullptr || otherColl->sons[1] != nullptr;

        if (!collHasSons && !otherHasSons)
            return true;

        bool collisionDetected = false;

        if (collHasSons && otherHasSons)
        {
            for (int i = 0; i < 2; ++i)
                for (int j = 0; j < 2; ++j)
                    collisionDetected |= collisionTest(coll->sons[i], otherColl->sons[j]);
        }
        else if (collHasSons)
        {
            for (int i = 0; i < 2; ++i)
                collisionDetected |= collisionTest(coll->sons[i], otherColl);
        }
        else
        {
            for (int j = 0; j < 2; ++j)
                collisionDetected |= collisionTest(coll, otherColl->sons[j]);
        }

        return collisionDetected;
    }

    static void updateTree(colliderNode* root, matrix4x4f mat)
    {
        if (root == nullptr || root->bv == nullptr)
            return;

        root->bv->updateCollider(mat);

        if (root->sons[0] != nullptr)
        {
            updateTree(root->sons[0], mat);
            updateTree(root->sons[1], mat);
        }
    }

private:
    static void destroyNode(colliderNode* node)
    {
        if (node == nullptr)
            return;

        destroyNode(node->sons[0]);
        destroyNode(node->sons[1]);
        delete node->bv;
        delete node;
    }
};

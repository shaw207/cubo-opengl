#pragma once
#include "libMath.h"
#include "common.h"
#include "Shader.h"
#include "Material.h"
#include "Colliders.h"

using namespace std;
using namespace libMath;

typedef struct {
    vec4float posicion;
    vec4float color;
    vec4float normal;
    vec4float texturaCoords;
}vertex_t;

class Object3D
{
public:
    static inline unsigned int idCounter;

    unsigned int objectId;

    vector<vertex_t> vertexList;
    vector<unsigned int> indexList;
    vec4float posicion = { 0,0,0,1 };
    vec4float rotacion = { 0,0,0,1 };
    vec4float escalado = { 1,1,1,1 };

    Shader* prg = nullptr;
    Material* mat = new Material();
    string frsModel = "";

    ColliderTree<Sphere>* coll = nullptr;

    Object3D(string frsModel);
    virtual ~Object3D();
    void loadFromFile();
    void buildSphereCollider();
    void updateCollider();

    matrix4x4f computeModelMatrix();

};

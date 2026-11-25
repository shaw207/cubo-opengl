#include "Object3D.h"
#include <algorithm>
#include <cstdlib>

Object3D::Object3D(string frsModel)
{
    objectId = idCounter++;

    this->frsModel = frsModel;
    loadFromFile();
    buildSphereCollider();
}

Object3D::~Object3D()
{
    delete coll;
    delete prg;
    delete mat;
}

void Object3D::buildSphereCollider()
{
    delete coll;
    coll = new ColliderTree<Sphere>(new colliderNode);
    coll->root->bv = new Sphere();

    if (vertexList.empty())
        return;

    if (indexList.empty())
    {
        particle3D* particle = new particle3D;
        particle->pos = vertexList[0].posicion;
        particle->size = { 0.0f,0.0f,0.0f,0.0f };
        coll->root->bv->addParticle(particle);
        updateCollider();
        return;
    }

    for (size_t i = 0; i + 2 < indexList.size(); i += 3)
    {
        vec4float v0 = vertexList[indexList[i]].posicion;
        vec4float v1 = vertexList[indexList[i + 1]].posicion;
        vec4float v2 = vertexList[indexList[i + 2]].posicion;

        vec4float minPos = {
            std::min(v0.x, std::min(v1.x, v2.x)),
            std::min(v0.y, std::min(v1.y, v2.y)),
            std::min(v0.z, std::min(v1.z, v2.z)),
            1.0f
        };

        vec4float maxPos = {
            std::max(v0.x, std::max(v1.x, v2.x)),
            std::max(v0.y, std::max(v1.y, v2.y)),
            std::max(v0.z, std::max(v1.z, v2.z)),
            1.0f
        };

        particle3D* particle = new particle3D;
        particle->pos = minPos + (maxPos - minPos) * 0.5f;
        particle->pos.w = 1.0f;
        particle->size = maxPos - minPos;
        particle->size.w = 0.0f;
        coll->root->bv->addParticle(particle);
    }

    updateCollider();
}

void Object3D::updateCollider()
{
    if (coll != nullptr)
        ColliderTree<Sphere>::updateTree(coll->root, computeModelMatrix());
}

matrix4x4f Object3D::computeModelMatrix()
{
    matrix4x4f model =
        make_traslate(posicion.x, posicion.y, posicion.z) *
        make_rotate(rotacion.x, rotacion.y, rotacion.z) *
        make_scale(escalado.x, escalado.y, escalado.z);
    return model;
}

namespace {
std::istringstream nextDataLine(std::ifstream& source)
{
    std::string line;
    while (std::getline(source >> std::ws, line))
    {
        if (!line.empty() && line.front() != '#' && static_cast<unsigned char>(line.front()) != 0xE2)
            return std::istringstream(line);
    }
    throw std::runtime_error("Fin inesperado del modelo FRS.");
}

int rowCount(std::ifstream& source)
{
    int count;
    if (!(nextDataLine(source) >> count) || count < 0)
        throw std::runtime_error("Numero de filas FRS no valido.");
    return count;
}

void readAttributes(std::ifstream& source, std::vector<vertex_t>& vertices,
                    vec4float vertex_t::* field, int channels)
{
    int rows = rowCount(source);
    for (int i = 0; i < rows; ++i)
    {
        auto input = nextDataLine(source);
        std::vector<float> values;
        for (float v; input >> v;) values.push_back(v);
        if (values.size() <= static_cast<size_t>(channels))
            throw std::runtime_error("Atributo FRS incompleto.");
        size_t firstValue = values.size() - channels;
        vec4float attribute{};
        for (int c = 0; c < channels; ++c) attribute.v[c] = values[firstValue + c];
        for (size_t j = 0; j < firstValue; ++j)
        {
            float id = values[j];
            if (!(id >= 0 && id < vertices.size()) || std::floor(id) != id)
                throw std::runtime_error("Indice FRS no valido.");
            vertices[static_cast<size_t>(id)].*field = attribute;
        }
    }
}
}

void Object3D::loadFromFile()
{
    std::ifstream file(frsModel);
    if (!file) throw std::runtime_error("No se pudo abrir " + frsModel);
    vertexList.resize(rowCount(file));
    for (auto& vertex : vertexList)
    {
        auto line = nextDataLine(file);
        if (!(line >> vertex.posicion.x >> vertex.posicion.y >> vertex.posicion.z))
            throw std::runtime_error("Vertice FRS incompleto.");
        vertex.posicion.w = 1.0f;
    }
    indexList.resize(static_cast<size_t>(rowCount(file)) * 3);
    for (size_t i = 0; i < indexList.size(); i += 3)
    {
        auto line = nextDataLine(file);
        for (size_t j = i; j < i + 3; ++j)
        {
            if (!(line >> indexList[j]) || indexList[j] >= vertexList.size())
                throw std::runtime_error("Cara FRS no valida.");
        }
    }
    readAttributes(file, vertexList, &vertex_t::color, 4);
    readAttributes(file, vertexList, &vertex_t::normal, 4);
    readAttributes(file, vertexList, &vertex_t::texturaCoords, 2);
    std::string textureFile, vertexShader, fragmentShader;
    nextDataLine(file) >> textureFile;
    nextDataLine(file) >> vertexShader;
    nextDataLine(file) >> fragmentShader;

    prg = new Shader();
    prg->addShaderProgram(vertexShader);
    prg->addShaderProgram(fragmentShader);
    prg->linkProgram();

    mat->textura = new Texture(textureFile);
    mat->usaTextura = true;

}

#include "Render.h"
#include "EventManager.h"

Render::Render()
{

    if (!glfwInit())
        throw std::runtime_error("No se pudo iniciar GLFW.");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(960, 720, "Cubo OpenGL", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("Se necesita OpenGL 3.3 para ejecutar la demo.");
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress))
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error("No se pudo cargar OpenGL.");
    }
    glfwSwapInterval(1);
    EventManager::initEventManager(window);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glClearColor(0.05f, 0.05f, 0.07f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Render::~Render()
{
    for (auto& entry : bufferObjectList)
    {
        auto& buffer = entry.second;
        glDeleteBuffers(1, &buffer.vertexBufferId);
        glDeleteBuffers(1, &buffer.indexBufferId);
        glDeleteVertexArrays(1, &buffer.bufferId);
    }
    for (auto* obj : objectList) delete obj;
    delete cam;
    delete luz;
    delete luzExtra;
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Render::setupObject(Object3D* obj)
{
    //generar buffers
    bufferObject_t bo = { 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF };
    glGenVertexArrays(1, &bo.bufferId); //generar lista de buffers
    glGenBuffers(1, &bo.vertexBufferId); //genera un buffer de vértices
    glGenBuffers(1, &bo.indexBufferId); //genera un buffer de ids de vértice
    //copiar datos
        //activar buffer
    glBindVertexArray(bo.bufferId);//activar lista de buffers
    glBindBuffer(GL_ARRAY_BUFFER, bo.vertexBufferId); //activar buffer de vertices
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * obj->vertexList.size(),
        obj->vertexList.data(), GL_STATIC_DRAW);    //copiar datos a buffer

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo.indexBufferId); //activar buffer de identificadores
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * obj->indexList.size(),
        obj->indexList.data(), GL_STATIC_DRAW);    //copiar datos a buffer

    //guardar identificadores
    bufferObjectList[obj->objectId] = bo;

}

void Render::addObject(Object3D* obj)
{
    //setup de objeto
    setupObject(obj);
    //añadirlo a lista de futuros objetos
    objectList.push_back(obj);
}

void Render::setLight(Luz* newLight)
{
    if (luz != nullptr && luz != newLight)
        delete luz;
    luz = newLight;
}

void Render::setExtraLight(Luz* newLight)
{
    if (luzExtra != nullptr && luzExtra != newLight)
        delete luzExtra;
    luzExtra = newLight;
}

void Render::updateObjects(double timeStep)
{
    if (!objectList.empty())
        cam->moveObject(timeStep, objectList.front()->coll);

    luz->moveObject(timeStep);
    luzExtra->moveObject(timeStep);

    double t = glfwGetTime();
    luzExtra->posicion = {
        2.8f * (float)cos(t),
        2.0f + 0.8f * (float)sin(t * 1.3),
        2.5f + 1.2f * (float)sin(t),
        1.0f
    };
    luzExtra->color = {
        0.35f + 0.35f * (float)sin(t * 0.8),
        0.45f + 0.35f * (float)sin(t * 1.1 + 2.0),
        0.75f + 0.20f * (float)sin(t * 0.9 + 4.0),
        1.0f
    };

}

void Render::drawObjects()
{
    //por cada objeto
    for(auto& obj: objectList){

        //setear buffers
        obj->prg->useProgram();
        auto bo = bufferObjectList[obj->objectId]; //recuperar lista buffers
        glBindVertexArray(bo.bufferId);//activar lista de buffers
        glBindBuffer(GL_ARRAY_BUFFER, bo.vertexBufferId); //activar buffer de vertices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo.indexBufferId); //activar buffer de identificadores

        matrix4x4f model = obj->computeModelMatrix();
        //setear matriz modelo
        //matriz Modelo Vista Proyección

        constexpr float verticalFov = 65.0f * 3.14159265f / 180.0f;
        auto mvp = transpose(
            cam->computeProjectionMatrix(0.01f, 100.0f, verticalFov, 4.0f / 3.0f) *
            cam->computeViewMatrix() *
            model);

        obj->prg->setMatrix(mvp, "MVP");
        obj->prg->setMatrix(transpose(model), "M");
        obj->prg->setCamera(cam);
        obj->prg->setLight(luz);
        obj->prg->setLight(luzExtra, "luzExtra");
        obj->prg->setMaterial(obj->mat);

        //describir datos:
        obj->prg->setAttributeData("vPos",4,GL_FLOAT, sizeof(vertex_t),(void*)offsetof(vertex_t, posicion));
        obj->prg->setAttributeData("vColor", 4, GL_FLOAT, sizeof(vertex_t), (void*)offsetof(vertex_t, color));
        obj->prg->setAttributeData("vNormal", 4, GL_FLOAT, sizeof(vertex_t), (void*)offsetof(vertex_t, normal));
        obj->prg->setAttributeData("vTextureCoords", 4, GL_FLOAT,
                 sizeof(vertex_t), (void*)offsetof(vertex_t, texturaCoords));

        //dibujar
        glDrawElements(GL_TRIANGLES,static_cast<GLsizei>(obj->indexList.size()), GL_UNSIGNED_INT, nullptr);
    }

}

void Render::mainLoop()
{
    double lastTime = glfwGetTime();

    while (!salir)
    {
        EventManager::updateEvents();

        //hacer cosas
            //calcular tiempo de render
        double newTime = glfwGetTime();
        double deltaTime = newTime - lastTime;
        lastTime = newTime;

        updateObjects(deltaTime);

        if (EventManager::keyMap[GLFW_KEY_ESCAPE] || glfwWindowShouldClose(window))
            salir = true;

        //limpiar
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //dibujar
        drawObjects();
        //intercambiar buffers
        glfwSwapBuffers(window);

    }

}

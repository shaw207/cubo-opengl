#pragma once
#include "Object3D.h"
#include "Camera.h"
#include "Luz.h"

class Render
{
    public:
        typedef struct {
            unsigned int bufferId;
            unsigned int vertexBufferId;
            unsigned int indexBufferId;
        } bufferObject_t;

        Camera* cam = new Camera({ 3.05f, 2.10f, 4.10f, 1 }, { 0, 0, 0, 1 });
        Luz* luz = new Luz({ 0,0,3,1 });
        Luz* luzExtra = new Luz({ -2,2,2,1 }, { 0.2f,0.6f,1.0f,1.0f }, 0.05f, 0.55f, 0.6f);

        vector<Object3D*> objectList; //lista de objetos que se tratan ahora

        map<unsigned int, bufferObject_t> bufferObjectList; //lista de bufferObjects por objeto
        bool salir = false;
        GLFWwindow* window = nullptr;

        Render();
        ~Render();
        //crea datos en gpu
        void setupObject(Object3D* obj);

        void addObject(Object3D* obj);
        void setLight(Luz* newLight);
        void setExtraLight(Luz* newLight);
        //actualiza estado de objetos
        void updateObjects(double timeStep);

        void drawObjects();

        //bucle principal renderizado
        void mainLoop();

};

//ESTA MACRO SOLO APARECE UNA VEZ EN EL ARCHIVO QUE CONTENGA EL MAIN()
#define GLAD_BIN
#include "common.h"
#include "Object3D.h"
#include <iostream>
#include "Render.h"

#include <filesystem>
#include <windows.h>

// La demo carga sus recursos desde la carpeta data/ junto al ejecutable.
static void useExecutableDirectory()
{
    std::wstring path(32768, L'\0');
    const DWORD n = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
    if (n == 0 || n >= path.size())
        throw std::runtime_error("No se pudo localizar el ejecutable.");
    path.resize(n);
    std::filesystem::current_path(std::filesystem::path(path).parent_path());
}

static void run()
{
    useExecutableDirectory();
    Render renderer;

    auto cubo = new Object3D("data/cubeText.frs");
    cubo->posicion = { 0.0f, 0.0f, 0.0f, 1.0f };
    cubo->escalado = { 1.0f, 1.0f, 1.0f, 1.0f };
    cubo->mat->alpha = 1.0f;
    cubo->mat->ka = 0.7f;
    cubo->mat->kd = 1.0f;
    cubo->mat->ks = 0.8f;
    cubo->mat->shiny = 96;

    renderer.setLight(new Luz(
        { 3.0f, 3.0f, 3.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f, 1.0f },
        0.25f,
        0.75f,
        1.0f
    ));
    renderer.setExtraLight(new Luz(
        { -2.0f, 2.0f, 2.0f, 1.0f },
        { 0.2f, 0.6f, 1.0f, 1.0f },
        0.05f,
        0.55f,
        0.6f
    ));

    renderer.addObject(cubo);
    renderer.mainLoop();

}

int main()
{
    try
    {
        run();
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << std::endl;
        MessageBoxA(nullptr, error.what(), "Cubo OpenGL", MB_OK | MB_ICONERROR);
        return 1;
    }
}

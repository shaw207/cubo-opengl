cmake_minimum_required(VERSION 3.21)
project(CuboOpenGL LANGUAGES CXX)

if(NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "La biblioteca GLFW incluida corresponde a MSVC x64.")
endif()

add_executable(CuboOpenGL WIN32
    src/MainPRGR_2024.cpp
    src/Camera.cpp
    src/Colliders.cpp
    src/EventManager.cpp
    src/Luz.cpp
    src/Material.cpp
    src/Object3D.cpp
    src/Render.cpp
    src/Shader.cpp
    src/Texture.cpp
)
target_compile_features(CuboOpenGL PRIVATE cxx_std_20)
target_compile_options(CuboOpenGL PRIVATE /utf-8 /W3)
target_compile_definitions(CuboOpenGL PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
set_property(TARGET CuboOpenGL PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
target_include_directories(CuboOpenGL SYSTEM PRIVATE third_party/include)
target_link_libraries(CuboOpenGL PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/lib/glfw3_mt.lib" user32 gdi32 shell32)
target_link_options(CuboOpenGL PRIVATE /ENTRY:mainCRTStartup)

set(assets data/cubeText.frs data/ladosCubo.png data/shader.vertex data/shader.fragment)
foreach(asset IN LISTS assets)
    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/runtime/${asset}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/runtime/data"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/${asset}" "${CMAKE_CURRENT_BINARY_DIR}/runtime/${asset}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${asset}"
        VERBATIM)
    list(APPEND runtime_assets "${CMAKE_CURRENT_BINARY_DIR}/runtime/${asset}")
endforeach()
add_custom_target(CuboAssets DEPENDS ${runtime_assets})
add_dependencies(CuboOpenGL CuboAssets)
set_target_properties(CuboOpenGL PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/runtime/$<0:>")
install(TARGETS CuboOpenGL RUNTIME DESTINATION .)
install(FILES ${assets} DESTINATION data)
install(FILES THIRD-PARTY.txt DESTINATION .)

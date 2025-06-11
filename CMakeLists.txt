cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(KDTreeExample)

# Establece la versión de C++ que usarás (C++11 es necesario para SDSL)
set(CMAKE_CXX_STANDARD 11)

# Incluir las rutas de encabezado de SDSL
include_directories(external/sdsl-lite/include)

# Agregar el directorio donde está la librería estática de SDSL
link_directories(external/sdsl-lite/build/lib)

# Agregar los archivos fuente
add_executable(KDTreeExample 
    src/main.cpp 
    src/KDTree.cpp)

# Enlazar con la librería estática de SDSL (especificamos el archivo libsdsl.a)
target_link_libraries(KDTreeExample sdsl)


#include "KDTree.h"

int main() {
    // Definimos algunos puntos en un espacio 2D (con 2 dimensiones)
    vector<vector<int>> points = {{1, 0}, {1, 1}, {2,2}, {5,7},{2,5}};

    // Tamaño de la grilla y número de dimensiones
    int S = pow(2,3);  // Tamaño de la grilla (2 celdas por dimensión)
    int d = 2;  // Dimensiones (2D)

    // Crear el KDTree y construir el bitvector
    KDTree tree(points, S, d, true);
    tree.build_tree();

    return 0;
}


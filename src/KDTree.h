#ifndef KDTREE_H
#define KDTREE_H

#include <vector>
#include <set>
#include <queue>
#include <sdsl/bit_vectors.hpp>

using namespace std;
using namespace sdsl;

// Estructura que representa una grilla
struct Grid {
    vector<pair<int, int>> ranges;  // Rangos en cada dimensión [start, end]
    set<vector<int>> points;        // Conjunto de puntos en esta grilla
    int next;
    Grid(int d, int S);  // Declaración del constructor
};

// Estructura que representa la respuesta de la grilla
struct Response {
    bit_vector bv;                 // Bitvector con 2^d elementos
    vector<Grid> subgrids;         // Subgrillas resultantes con puntos

    Response(int d, bool use_bi);               // Declaración del constructor
};

class KDTree {
public:
    vector<vector<int>> points;  // Lista de puntos
    int S;                       // Tamaño de la grilla
    int d;                       // Dimensiones del espacio
    queue<Grid> grids;          // Arreglo de grillas

    Response (KDTree::*get_response)(Grid&);
    
    KDTree(const vector<vector<int>>& pts, int S, int d, bool use_bi = false);
    bit_vector representation;
    void build_tree();
    Response get_resp(Grid& g);
    Response get_resp_bi(Grid& g);
};

#endif


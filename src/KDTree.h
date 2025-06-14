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
    int next;                       // Eje en el cual se realiza el proceso de construcción (para kdtree bi)
    Grid(int d, int S);  // Declaración del constructor
};

// Estructura que representa la respuesta de la grilla
struct Response {
    bit_vector bv;                 // Bitvector con la respuesta de la grilla
    vector<Grid> subgrids;         // Subgrillas resultantes con puntos

    Response(int d, bool use_bi);               // Declaración del constructor
};

class KDTree {
public:
    vector<vector<int>> points;  // Lista de puntos
    int S;                       // Tamaño de la grilla
    int d;                       // Dimensiones del espacio
    queue<Grid> grids;          // Arreglo de grillas
    bool bi=false;              // Señala si es k**dtree o kdtree


    Response (KDTree::*get_response)(Grid&);  // Mtodo de creación de las subgrillas
    
    KDTree(const vector<vector<int>>& pts, int S, int d, bool use_bi = false); // Declaración del constructor
    
    bit_vector representation; // Vector de bits que representa al árbol
    
    void build_tree(); // Constructor del vector de bits
    
    // Metodos para obtener respuestas de una grilla
    Response get_resp(Grid& g);
    Response get_resp_bi(Grid& g);
    
    // Metodos de reporte
    void report(KDTree t, vector<pair<int,int>> ranges);
    
    void report_aux(int v, vector<pair<int,int>> ranges, vector<int> resp, int S, bit_vector bv, rank_support_v<1> rank_1);
    
    void report_bi_aux(int v, vector<pair<int,int>> ranges, vector<int> resp, vector<int> S, bit_vector bv, rank_support_v<1> rank_1);
};

#endif


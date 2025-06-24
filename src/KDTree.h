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
    vector<pair<long long, long long>> ranges;  // Rangos en cada dimension [start, end]
    set<vector<long long>> points;        // Conjunto de puntos en esta grilla
    long long next;                       // Eje en el cual se realiza el proceso de construcción (para kdtree bi)
    Grid(long long d, long long S); 
    Grid(); // Declaracion del constructor
};

// Estructura que representa la respuesta de la grilla
struct Response {
    bit_vector bv;                 // Bitvector con la respuesta de la grilla
    vector<Grid> subgrids;         // Subgrillas resultantes con puntos

    Response(long long d, bool use_bi);               // Declaracion del constructor
};

class KDTree {
public:
    vector<vector<long long>> points;  // Lista de puntos
    long long S;                       // Tamaño de la grilla
    long long d;                       // Dimensiones del espacio
    queue<Grid> grids;          // Arreglo de grillas
    bool bi=false;              // Señala si es k**dtree o kdtree


    Response (KDTree::*get_response)(Grid&);  // Mtodo de creacion de las subgrillas
    
    KDTree(const vector<vector<long long>>& pts, long long S, long long d, bool use_bi = false); // Declaración del constructor
    
    bit_vector representation; // Vector de bits que representa al arbol
    
    void build_tree(); // Constructor del vector de bits
    
    // Metodos para obtener respuestas de una grilla
    Response get_resp(Grid& g);
    Response get_resp_bi(Grid& g);
    
    // Metodos de reporte
    void report(KDTree t, vector<pair<long long,long long>> ranges);
    
    void report_aux(long long v, vector<pair<long long,long long>> ranges, vector<long long> resp, long long S, bit_vector bv, rank_support_v<1> rank_1);
    
    void report_bi_aux(long long v, vector<pair<long long,long long>> ranges, vector<long long> resp, vector<long long> S, bit_vector bv, rank_support_v<1> rank_1);
};

#endif


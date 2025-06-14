#include "KDTree.h"

int main() {
    // Definimos algunos puntos en un espacio 2D (con 2 dimensiones)
    vector<vector<int>> points = {{1,1,1,1,1,1,1,1,1,1,1,1},{4,7,2,1,10,6,4,8,5,5,6,7},{10,1,4,2,7,4,7,6,5,8,9,10},{14,5,10,1,1,4,7,8,5,4,5,6},{8,8,8,8,8,8,8,8,8,8,8,8},{7,7,7,7,7,7,7,7,7,7,7,7},{5,6,6,4,10,5,7,7,7,7,1,2}};

    // Tamaño de la grilla y número de dimensiones
    int S = pow(2,4);  // Tamaño de la grilla (2 celdas por dimensión)
    int d = 12;  // Dimensiones (2D)

    // Crear el KDTree y construir el bitvector
    cout<<"Creating K**D Tree"<<endl;
    KDTree tree(points, S, d, false);
    tree.build_tree();
    cout<<"Creating KD Tree"<<endl;
    KDTree tree_bi(points,S,d,true);
    tree_bi.build_tree();
    
    vector<pair<int,int>> ranges = {{1,10},{1,12},{3,15},{1,8},{6,16},{4,14},{2,15},{5,12},{1,16},{3,9},{1,16},{1,16}};
    
    cout<<endl<<"Reporting in ranges "<<endl<<endl;
    cout<<"K**D tree:"<<endl;
    tree.report(tree,ranges);
    cout<<"KD tree:"<<endl;
    tree_bi.report(tree_bi,ranges);
    
    return 0;
}


#include "KDTree.h"
#include <iostream>
#include <cmath>

using namespace std;

// Constructor de la clase Grid
Grid::Grid(int d, int S) {
    ranges.resize(d, {0, S - 1});
}

// Constructor de la clase Response
Response::Response(int d,bool use_bi) {
    if (use_bi){
        bv.resize(2);
    } else {
    bv.resize(pow(2, d));
    }
}

// Constructor de la clase KDTree
KDTree::KDTree(const vector<vector<int>>& pts, int S, int d, bool use_bi) : points(pts), S(S), d(d) {
    Grid root(d, S);
    for (auto& p : points) {
        root.points.insert(p);
    }
    root.next=0;
    grids.push(root);
    
    if (use_bi) {
        get_response = &KDTree::get_resp_bi;
    } else {
        get_response = &KDTree::get_resp;
    }
}

// Función principal de construcción: genera bitvector y divide las grillas
void KDTree::build_tree() {
    
    while (!grids.empty()) {
        
        Grid g = grids.front();
        grids.pop();

        Response resp = (this->*get_response)(g);

        representation.resize(representation.size()+resp.bv.size());
        
        for (int i = 0; i < resp.bv.size();i++){
            representation[i+representation.size()-resp.bv.size()]=resp.bv[i];
            
        }
     
        for (auto& subgrid : resp.subgrids) {
            bool is_trivial = true;
            for (const auto& range : subgrid.ranges) {
                if (range.first != range.second) {
                    is_trivial = false;
                    break;
                }
            }

            if (!is_trivial) {
                grids.push(subgrid);
                
            } 
        }

      
    
    
    }
    cout << representation <<endl;
}

// Algoritmo get_resp que genera el bitvector y las subgrillas
Response KDTree::get_resp(Grid& g) {
    Response resp(d, false);
    
    vector<vector<pair<int, int>>> subranges(pow(2, d));   

    for (int i = 0; i < pow(2, d); ++i) {
        for (int j = 0; j < d; ++j) {
            int mid = (g.ranges[j].second - g.ranges[j].first + 1) / 2;  // Para asegurar divisiones en mitades exactas
            if (i & (1 << j)) {
                subranges[i].push_back({g.ranges[j].first + mid, g.ranges[j].second});
            } else {
                subranges[i].push_back({g.ranges[j].first, g.ranges[j].first + mid - 1});
            }
        }
    }

    vector<Grid> subgrids(pow(2, d), Grid(d, S));
    
    for (auto& p : g.points) {
        for (int i = 0; i < pow(2, d); ++i) {
            bool in_subgrid = true;
            for (int j = 0; j < d; ++j) {
                if (p[j] < subranges[i][j].first || p[j] > subranges[i][j].second) {
                    in_subgrid = false;
                    break;
                }
            }

            if (in_subgrid) {
                subgrids[i].points.insert(p);
                subgrids[i].next=(g.next+1)%d;
                resp.bv[i] = 1;  // Marcar el bit correspondiente
                
                // Asignar los rangos de subranges a las subgrillas
                subgrids[i].ranges = subranges[i];
            }
        }
    }

    for (int i = 0; i < subgrids.size(); ++i) {
        if (!subgrids[i].points.empty()) {
            resp.subgrids.push_back(subgrids[i]);
        }
    }

    return resp;
}

// Algoritmo get_resp que genera el bitvector y las subgrillas
Response KDTree::get_resp_bi(Grid& g) {
    Response resp(d,true);

    vector<vector<pair<int, int>>> subranges(2);
        
    int axis = g.next;
       
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < d; ++j) {
            if (j==axis){
                int mid = (g.ranges[j].second - g.ranges[j].first + 1) / 2;  // Para asegurar divisiones en mitades exactas
                if (i ==1) {
                    subranges[i].push_back({g.ranges[j].first + mid, g.ranges[j].second});
                } else {
                    subranges[i].push_back({g.ranges[j].first, g.ranges[j].first + mid - 1});
                }
            } else {
                subranges[i].push_back(g.ranges[j]);
            
            }
        }
    }



    // Dividir los puntos en subgrillas según los rangos
    vector<Grid> subgrids(2, Grid(d, S));
    for (auto& p : g.points) {
        for (int i = 0; i < 2; ++i) {
            bool in_subgrid = true;
            for (int j = 0; j < d; ++j) {
                if (p[j] < subranges[i][j].first || p[j] > subranges[i][j].second) {
                    in_subgrid = false;
                    break;
                }
            }

            if (in_subgrid) {
                subgrids[i].points.insert(p);
                subgrids[i].next=(g.next+1)%d;
                resp.bv[i] = 1;  // Marcar el bit correspondiente
                
                // Asignar los rangos de subranges a las subgrillas
                subgrids[i].ranges = subranges[i];
                subgrids[i].next=(axis+1)%d;
            }
        }
    }

    // Imprimir las subgrillas generadas con sus rangos actualizados
    
    for (int i = 0; i < subgrids.size(); ++i) {
        if (!subgrids[i].points.empty()) {
         
            resp.subgrids.push_back(subgrids[i]);
        }
    }

    return resp;
}


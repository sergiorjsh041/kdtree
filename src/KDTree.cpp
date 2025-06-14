#include "KDTree.h"
#include <iostream>
#include <cmath>

using namespace std;

// Función recursiva para generar las combinaciones
void generate_combinations(int depth, vector<int>& current_combination, const vector<int>& inf, const vector<int>& sup, vector<vector<int>>& result) {
    // Si hemos alcanzado la profundidad d, agregamos la combinación actual a los resultados
    if (depth == inf.size()) {
        result.push_back(current_combination);
        return;
    }

    // Iterar sobre el rango de la dimensión actual (de inf[depth] a sup[depth])
    for (int i = inf[depth]; i <= sup[depth]; ++i) {
        current_combination[depth] = i;  // Asignamos el valor de la dimensión actual
        generate_combinations(depth + 1, current_combination, inf, sup, result);  // Llamada recursiva para la siguiente dimensión
    }
}

// Funciones sacadas del libro del profesor Gonzalo Navarro
bool childrenlabeled(bit_vector bv, int v,int l, int sigma){ 
    return bv[sigma*(v-1)+l-1]==1;
}

int labeledchild(rank_support_v<1> rank, int v, int l, int sigma){
    return rank(sigma*(v-1)+l)+1;
}



// Constructor de la clase Grid
Grid::Grid(int d, int S) {
    ranges.resize(d, {1, S});
}

// Constructor de la clase Response
Response::Response(int d,bool use_bi) {
    if (use_bi){
        bv.resize(2);
        bv[0]=0;
        bv[1]=0;
    } else {
    bv.resize(pow(2, d));
    for (int i=0;i<pow(2,d);i++){
        bv[i]=0;
    }
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
    bi=use_bi;
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


void KDTree::report(KDTree t, vector<pair<int,int>> ranges){
    vector<int> resp(d,0);
    rank_support_v<1> rank_1(&(t.representation));
    if (bi){
        vector<int> S(d,t.S);
        report_bi_aux(1,ranges,resp,S,t.representation,rank_1);
    } else {
        report_aux(1,ranges,resp,t.S,t.representation,rank_1);
    }
}

void KDTree::report_aux(int v, vector<pair<int, int>> ranges, vector<int> resp, int S, bit_vector bv, rank_support_v<1> rank_1) {
    if (S == 1) {
        cout << "####\nPoint reported: ";
        for (int i : resp) {
            cout << i + 1 << " ";  // Imprimir los puntos reportados
        }
        cout << endl<<"####\n";
    } else {
        int l = S / 2;
        vector<int> inf;
        vector<int> sup;

        // Calcular los rangos inf y sup para cada dimensión
        for (int i = 0; i < d; i++) {
            int inf_val = ceil((double)(max(ranges[i].first - resp[i], 1)) / (double) l);
            int sup_val = ceil((double)(min(ranges[i].second - resp[i], S)) / (double) l);
            inf.push_back(inf_val);
            sup.push_back(sup_val);
        }

        // Contenedor para almacenar las combinaciones
        vector<vector<int>> combinations;

        // Variable temporal para la combinación actual
        vector<int> current_combination(d); // Para almacenar la combinación de cada dimensión
        // Llamar a la función recursiva para generar las combinaciones
        generate_combinations(0, current_combination, inf, sup, combinations);

        
        for (const auto& combination : combinations) {

            int offset = combination[0];

            for (int i = 1; i < combination.size(); i++) {
                offset += pow(2, i) * (combination[i] - 1);
            }


            // Verificar si la subgrilla está etiquetada y acceder a la subgrilla correspondiente
            if (childrenlabeled(bv, v, offset, pow(2,d))) {
            
                int newv = labeledchild(rank_1, v, offset, pow(2,d));
                vector<int> newresp;

                for (int i = 0; i < d; i++) {
                    newresp.push_back(resp[i] + (combination[i] - 1) * l);
                }

                

                report_aux(newv, ranges, newresp, l, bv, rank_1);
            }
        }
    }
}

void KDTree::report_bi_aux(int v, vector<pair<int, int>> ranges, vector<int> resp, vector<int> S, bit_vector bv, rank_support_v<1> rank_1) {

    if (S[0] == 1) {
        cout << "####\nPoint reported: ";
        for (int i : resp) {
            cout << i + 1 << " ";  // Imprimir los puntos reportados
        }
        cout << endl<<"####\n";
    } else {
        int l = S[0] / 2;
        int inf = ceil((double)(max(ranges[0].first - resp[0], 1)) / (double) l);
        int sup = ceil((double)(min(ranges[0].second - resp[0], S[0])) / (double) l);

        for (int i = inf; i <= sup; i++) {
            int offset = i;


            // Verificar si la subgrilla está etiquetada y acceder a la subgrilla correspondiente
            if (childrenlabeled(bv, v, offset, 2)) {
                int newv = labeledchild(rank_1, v, offset, 2);
                vector<int> newresp;

                for (int j = 1; j < d; j++) {
                    newresp.push_back(resp[j]);
                }
                newresp.push_back(resp[0] + (i - 1) * l);

          

                vector<pair<int, int>> newranges = ranges;

                // Rotar el primer rango
                pair<int, int> first = newranges.front();
                newranges.erase(newranges.begin());
                newranges.push_back(first);

                vector<int> newS = S;
                newS.erase(newS.begin());
                newS.push_back(l);

                
                report_bi_aux(newv, newranges, newresp, newS, bv, rank_1);
            }
        }
    }
}




#include "KDTree.h"
#include <iostream>
#include <cmath>
#include <map>

using namespace std;

// Funcion recursiva para generar las combinaciones
void generate_combinations(long long depth, vector<long long>& current_combination, const vector<long long>& inf, const vector<long long>& sup, vector<vector<long long>>& result) {
    // Si hemos alcanzado la profundidad d, agregamos la combinacion actual a los resultados
    if (depth == inf.size()) {
        result.push_back(current_combination);
        return;
    }

    // Iterar sobre el rango de la dimension actual (de inf[depth] a sup[depth])
    for (long long i = inf[depth]; i <= sup[depth]; ++i) {
        current_combination[depth] = i;  // Asignamos el valor de la dimension actual
        generate_combinations(depth + 1, current_combination, inf, sup, result);  // Llamada recursiva para la siguiente dimension
    }
}

// Funciones sacadas del libro del profesor Gonzalo Navarro
bool childrenlabeled(bit_vector bv, long long v,long long l, long long sigma){ 
    return bv[sigma*(v-1)+l-1]==1;
}

long long labeledchild(rank_support_v<1> rank, long long v, long long l, long long sigma){
    return rank(sigma*(v-1)+l)+1;
}

// Constructur nulo para correctitud del codigo
Grid::Grid() {
    next=0;
}


// Constructor de la clase Grid, se setean los d rangos en [1,S]
Grid::Grid(long long d, long long S) {
    ranges.resize(d, {1, S});
}

// Constructor de la clase Response, depende del tipo de arbol utilizado
Response::Response(long long d,bool use_bi) {
    if (use_bi){
        bv.resize(2);
        bv[0]=0;
        bv[1]=0;
    } else {
    bv.resize(pow(2, d));
    for (long long i=0;i<pow(2,d);i++){
        bv[i]=0;
    }
    }
}

// Constructor de la clase KDTree
KDTree::KDTree(const vector<vector<long long>>& pts, long long S, long long d, bool use_bi) : points(pts), S(S), d(d) {
    Grid root(d, S);
    for (auto& p : points) {
        root.points.insert(p);
    }
    root.next=0;
    grids.push(root); // Se inserta la raiz al conjunto de grillas para inicializar la construccion
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
        for (long long i = 0; i < resp.bv.size();i++){
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
    
    //cout << representation <<endl;
}

// Algoritmo get_resp que genera el bitvector y las subgrillas para el k**d tree
Response KDTree::get_resp(Grid& g) {
    Response resp(d, false);
  
    std::map<long long, Grid> subgrids;
   
    vector<long long> nonemptyindex;
   
   // se recorren los puntos para insertarlos en una subgrilla
    for (auto& p : g.points) {
        long long index=0;
        vector<pair<long long, long long>> subrange;
        for (long long i = 0; i<d;i++){
            long long mid = (g.ranges[i].second-g.ranges[i].first+1)/2;
            if (p[i]>=g.ranges[i].first+mid){
                index += (long long) pow(2,i);
                subrange.push_back({g.ranges[i].first+mid,g.ranges[i].second});
            } else {
                subrange.push_back({g.ranges[i].first,g.ranges[i].first+mid-1});
            
            }
        } 
        
        // se setea el bitvector en 1 donde se requiere
        if (std::find(nonemptyindex.begin(), nonemptyindex.end(), index) == nonemptyindex.end()) {
            Grid thisgrid = Grid(d,S);
            subgrids.insert({index,thisgrid});
            
            nonemptyindex.push_back(index);
            resp.bv[index]=1;
            subgrids[index].next=(g.next+1)%d;
            subgrids[index].ranges=subrange;
        }
        
        subgrids[index].points.insert(p);
       
    }
    // las grillas no vacias se agregan a las subgrillas de la respuesta,
    for (auto& index : nonemptyindex){
        resp.subgrids.push_back(subgrids[index]);
    }

    return resp;
}

// Algoritmo get_resp que genera el bitvector y las subgrillas
Response KDTree::get_resp_bi(Grid& g) {
    Response resp(d,true);

    vector<vector<pair<long long, long long>>> subranges(2);
        
    long long axis = g.next;
       
    // se generan los subrangos
    for (long long i = 0; i < 2; ++i) {
        for (long long j = 0; j < d; ++j) {
            if (j==axis){
                long long mid = (g.ranges[j].second - g.ranges[j].first + 1) / 2;  // Para asegurar divisiones en mitades exactas
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



    // Dividir los puntos en subgrillas segun los rangos
    vector<Grid> subgrids(2, Grid(d, S));
    for (auto& p : g.points) {
        for (long long i = 0; i < 2; ++i) {
            bool in_subgrid = true;
            for (long long j = 0; j < d; ++j) {
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

    
    // se añaden las grillas no vacias a la respuesta    
    for (long long i = 0; i < subgrids.size(); ++i) {
        if (!subgrids[i].points.empty()) {
         
            resp.subgrids.push_back(subgrids[i]);
        }
    }

    return resp;
}

// reporte general, dependiendo del tipo del arbol se ejecuta la funcion auxiliar correspondiente
void KDTree::report(KDTree t, vector<pair<long long,long long>> ranges){
    vector<long long> resp(d,0);
    rank_support_v<1> rank_1(&(t.representation));
    if (bi){
        vector<long long> S(d,t.S);
        report_bi_aux(1,ranges,resp,S,t.representation,rank_1);
    } else {
        report_aux(1,ranges,resp,t.S,t.representation,rank_1);
    }
}

// algoritmo basado en libro del profesor Navarro
void KDTree::report_aux(long long v, vector<pair<long long, long long>> ranges, vector<long long> resp, long long S, bit_vector bv, rank_support_v<1> rank_1) {
    if (S == 1) {
        //cout << "####\nPoint reported: ";
        //for (long long i : resp) {
        //    cout << i + 1 << " ";  // Imprimir los puntos reportados
        //}
        //cout << endl<<"####\n";
    } else {
        long long l = S / 2;
        vector<long long> inf;
        vector<long long> sup;

        // Calcular los rangos inf y sup para cada dimensión
        for (long long i = 0; i < d; i++) {
            long long inf_val = ceil((double)(max(ranges[i].first - resp[i],(long long) 1)) / (double) l);
            long long sup_val = ceil((double)(min(ranges[i].second - resp[i], S)) / (double) l);
            inf.push_back(inf_val);
            sup.push_back(sup_val);
        }

        // Contenedor para almacenar las combinaciones
        vector<vector<long long>> combinations;

        // Variable temporal para la combinacion actual
        vector<long long> current_combination(d); // Para almacenar la combinacion de cada dimension
        // Llamar a la funcion recursiva para generar las combinaciones
        generate_combinations(0, current_combination, inf, sup, combinations);

        
        for (const auto& combination : combinations) {

            long long offset = combination[0];

            for (long long i = 1; i < combination.size(); i++) {
                offset += pow(2, i) * (combination[i] - 1);
            }


            // Verificar si la subgrilla esta etiquetada y acceder a la subgrilla correspondiente
            if (childrenlabeled(bv, v, offset, pow(2,d))) {
            
                long long newv = labeledchild(rank_1, v, offset, pow(2,d));
                vector<long long> newresp;

                for (long long i = 0; i < d; i++) {
                    newresp.push_back(resp[i] + (combination[i] - 1) * l);
                }

                

                report_aux(newv, ranges, newresp, l, bv, rank_1);
            }
        }
    }
}

// variante para la version bi, similar a la otra funcion auxiliar, salvo por una interpretacion geometrica de los puntos
void KDTree::report_bi_aux(long long v, vector<pair<long long, long long>> ranges, vector<long long> resp, vector<long long> S, bit_vector bv, rank_support_v<1> rank_1) {

    if (S[0] == 1) {
        //cout << "####\nPoint reported: ";
        //for (long long i : resp) {
        //    cout << i + 1 << " ";  // Imprimir los puntos reportados
        //}
        //cout << endl<<"####\n";
    } else {
        long long l = S[0] / 2;
        long long inf = ceil((double)(max(ranges[0].first - resp[0], (long long) 1)) / (double) l);
        long long sup = ceil((double)(min(ranges[0].second - resp[0], S[0])) / (double) l);

        for (long long i = inf; i <= sup; i++) {
            long long offset = i;


            // Verificar si la subgrilla esta etiquetada y acceder a la subgrilla correspondiente
            if (childrenlabeled(bv, v, offset, 2)) {
                long long newv = labeledchild(rank_1, v, offset, 2);
                vector<long long> newresp;

                for (long long j = 1; j < d; j++) {
                    newresp.push_back(resp[j]);
                }
                newresp.push_back(resp[0] + (i - 1) * l);

          

                vector<pair<long long, long long>> newranges = ranges;

                // Rotar el primer rango
                pair<long long, long long> first = newranges.front();
                newranges.erase(newranges.begin());
                newranges.push_back(first);

                vector<long long> newS = S;
                newS.erase(newS.begin());
                newS.push_back(l);

                
                report_bi_aux(newv, newranges, newresp, newS, bv, rank_1);
            }
        }
    }
}




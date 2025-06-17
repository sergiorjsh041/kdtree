/*
#include "KDTree.h"

int main() {
    // Definimos algunos puntos en un espacio 2D (con 2 dimensiones)
    vector<vector<long long>> points = {{6,8},{2,1},{3,3},{2,2}};

    // Tamaño de la grilla y número de dimensiones
    long long S = pow(2,3);  // Tamaño de la grilla (2 celdas por dimensión)
    long long d = 2;  // Dimensiones (2D)

    // Crear el KDTree y construir el bitvector
    cout<<"Creating K**D Tree"<<endl;
    KDTree tree(points, S, d, false);
    tree.build_tree();
    cout<<"Creating KD Tree"<<endl;
    KDTree tree_bi(points,S,d,true);
    tree_bi.build_tree();
    
    
    
    return 0;
}

*/


#include "KDTree.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>
#include <fstream>
#include <random>
#include <filesystem>  // Para manejar carpetas

using namespace std;
namespace fs = std::filesystem;

void generate_uniform_points(vector<vector<long long>>& points, long long n, long long S, long long d) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(1, S);

    for (long long i = 0; i < n; ++i) {
        vector<long long> point;
        for (long long j = 0; j < d; ++j) {
            point.push_back(distrib(gen));
        }
        points.push_back(point);
    }
}

void generate_normal_points(vector<vector<long long>>& points, long long n, long long S, long long d, long long mean, long long stddev) {
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<> distrib(mean, stddev);

    for (long long i = 0; i < n; ++i) {
        vector<long long> point;
        for (long long j = 0; j < d; ++j) {
            long long value = round(distrib(gen));  // Convert to integer
            // Limitar el valor de los puntos dentro de los límites [1, S]
            value = max((long long) 1, min(S, value));
            point.push_back(value);
        }
        points.push_back(point);
    }
}

int main() {
    vector<long long> ds = {4, 8, 16, 32, 64, 128, 256, 512, 1024};
    long long S = 256;
    vector<long long> ns = {
        static_cast<long long>(pow(2, 8)),
        static_cast<long long>(pow(2, 10)),
        static_cast<long long>(pow(2, 12))
    };
    vector<long long> std_devs = {25, 50, 75};

    for (long long d_val : ds) {
        std::cout << "d: " << d_val << std::endl;
        for (long long n_val : ns) {
            std::cout << "n: " << n_val << std::endl;
            for (long long i = 0; i < 1; ++i) {
                std::cout << i << " iteration" << std::endl;
                vector<vector<long long>> points_uniform;
                generate_uniform_points(points_uniform, n_val, S, d_val);

                vector<vector<long long>> points_normal_25;
                generate_normal_points(points_normal_25, n_val, S, d_val, S / 2, 25);
                vector<vector<long long>> points_normal_50;
                generate_normal_points(points_normal_50, n_val, S, d_val, S / 2, 50);
                vector<vector<long long>> points_normal_75;
                generate_normal_points(points_normal_75, n_val, S, d_val, S / 2, 75);

                KDTree tree_uniform(points_uniform, S, d_val, false);
                tree_uniform.build_tree();
                KDTree tree_bi_uniform(points_uniform, S, d_val, true);
                tree_bi_uniform.build_tree();

                KDTree tree_n25(points_normal_25, S, d_val, false);
                tree_n25.build_tree();
                KDTree tree_bi_n25(points_normal_25, S, d_val, true);
                tree_bi_n25.build_tree();

                KDTree tree_n50(points_normal_50, S, d_val, false);
                tree_n50.build_tree();
                KDTree tree_bi_n50(points_normal_50, S, d_val, true);
                tree_bi_n50.build_tree();

                KDTree tree_n75(points_normal_75, S, d_val, false);
                tree_n75.build_tree();
                KDTree tree_bi_n75(points_normal_75, S, d_val, true);
                tree_bi_n75.build_tree();

                auto save_tree = [&](const string& base_dir, const string& type, KDTree& tree) {
                    string path = base_dir + "/d_" + to_string(d_val) + "/n_" + to_string(n_val) + "/" + type + "/";
                    fs::create_directories(path);
                    string filename = path + "rep_" + to_string(i + 1) + ".bin";
                    ofstream ofs(filename, ios::binary);
                    tree.representation.serialize(ofs);
                    ofs.close();
                };

                // Guardar todos los árboles en sus rutas correspondientes
                save_tree("kdtrees", "uniform", tree_uniform);
                save_tree("bikdtrees", "uniform", tree_bi_uniform);

                save_tree("kdtrees", "normal_std25", tree_n25);
                save_tree("bikdtrees", "normal_std25", tree_bi_n25);

                save_tree("kdtrees", "normal_std50", tree_n50);
                save_tree("bikdtrees", "normal_std50", tree_bi_n50);

                save_tree("kdtrees", "normal_std75", tree_n75);
                save_tree("bikdtrees", "normal_std75", tree_bi_n75);

                cout << "Archivos creados para d = " << d_val << ", n = " << n_val << " (iteración " << i + 1 << ")" << endl;
            }
        }
    }

    return 0;
}



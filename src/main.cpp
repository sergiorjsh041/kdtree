#include "KDTree.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>
#include <fstream>
#include <random>
#include <filesystem>  // Para manejar carpetas
#include <chrono>      // Para medir el tiempo
#include <algorithm>   // Para std::swap

using namespace std;
namespace fs = std::filesystem;

// Funcion para generar un rango de consulta
vector<pair<long long, long long>> generate_query_range(long long d, long long S, mt19937& gen) {
    vector<pair<long long, long long>> ranges;
    uniform_int_distribution<long long> dist_coord(1, S);
    const double range_factor = 0.2; // El rango cubrira a lo mas un 0.2 del espacio en cada dimension

    for (long long i = 0; i < d; ++i) {
        long long a = dist_coord(gen);
        long long range_size = max(static_cast<long long>(S * range_factor), 1LL);
        long long b = a + range_size;
        if (b > S) {
            b = S;
            a = b - range_size;
            if (a < 1) a = 1;
        }
        ranges.push_back({a, b});
    }
    return ranges;
}

// Funcion para medir el tiempo de reporte
double measure_report_time(KDTree& tree, const vector<pair<long long, long long>>& ranges) {
    auto start = chrono::high_resolution_clock::now();
    tree.report(tree, ranges);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    return elapsed.count() * 1000; // Convertir a milisegundos
}

// genera puntos de manera uniforme
void generate_uniform_points(set<vector<long long>>& points, long long n, long long S, long long d) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(1, S);

    while (points.size() < n) {
        vector<long long> point;
        for (long long j = 0; j < d; ++j) {
            point.push_back(distrib(gen));
        }
        points.insert(point);
    }
}

// genera puntos en una distribucion normal, para mitigar costos, despues de una cantidad de puntos repetidos, genera el resto de manera uniforme
void generate_normal_points(set<vector<long long>>& points, long long n, long long S, long long d, long long mean, long long stddev) {
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<> distrib(mean, stddev);

    long long failed_attempts = 0;
    long long max_failed_attempts = 2 * n;

    while (points.size() < n) {
        vector<long long> point;
        for (long long j = 0; j < d; ++j) {
            long long value = round(distrib(gen));
            value = max((long long)1, min(S, value));
            point.push_back(value);
        }

        if (points.count(point) == 0) {
            points.insert(point);
        } else {
            failed_attempts++;
            if (failed_attempts > max_failed_attempts) {
                cout << "Switching to uniform distribution after " << failed_attempts << " failed attempts." << endl;
                uniform_int_distribution<> distrib_uniform(1, S);
                while (points.size() < n) {
                    vector<long long> point_uniform;
                    for (long long j = 0; j < d; ++j) {
                        point_uniform.push_back(distrib_uniform(gen));
                    }
                    points.insert(point_uniform);
                }
                break;
            }
        }
    }
}

// guarda los arboles en las carpetas correspondientes
void save_tree(const string& base_dir, const string& type, KDTree& tree, const string& filename) {
    string path = base_dir + "/d_" + to_string(tree.d) + "/n_" + to_string(tree.points.size()) + "/" + type + "/";
    fs::create_directories(path);
    string full_path = path + filename;
    ofstream ofs(full_path, ios::binary);
    tree.representation.serialize(ofs);
    ofs.close();
}

// construye los arboles, calcula sus tiempos
void measure_build_time(KDTree& tree, const string& tree_type, const string& filename, 
                        const string& base_dir, long long d_val, long long n_val, long long i) {
    auto start_time = chrono::high_resolution_clock::now();
    tree.build_tree();
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end_time - start_time;
    double elapsed_time = duration.count() * 1000;
    cout << tree_type << " for d=" << d_val << ", n=" << n_val << " took " << elapsed_time << " ms" << endl;

    save_tree(base_dir, tree_type, tree, filename);

    string time_file = "construction_times.txt";
    ofstream time_ofs(base_dir + "/d_" + to_string(d_val) + "/n_" + to_string(n_val) + "/" + tree_type + "/" + time_file, ios::app);
    time_ofs << tree_type << ", d=" << d_val << ", n=" << n_val << ", Iteration=" << i + 1
             << ", Time=" << elapsed_time << " ms, File=" << filename << endl;
    time_ofs.close();
}

int main() {
    vector<long long> ds = {2, 3, 4, 5, 6, 7, 8, 9, 10};
    long long S = 1024;
    vector<long long> ns = {
        static_cast<long long>(pow(2, 10)),
        static_cast<long long>(pow(2, 14)),
        static_cast<long long>(pow(2, 18))
    };
    vector<long long> std_devs = {25, 50, 75};
    const int num_queries = 10;
    random_device rd;
    mt19937 gen(rd());

    for (long long d_val : ds) {
        cout << "d: " << d_val << endl;
        for (long long n_val : ns) {
            cout << "n: " << n_val << endl;
            for (long long i = 0; i < 1; ++i) {
                cout << i << " iteration" << endl;
                
                // Generar puntos
                set<vector<long long>> points_uniform, points_normal_25, points_normal_50, points_normal_75;
                
                cout << "generating uniform points" << endl;
                generate_uniform_points(points_uniform, n_val, S, d_val);
                
                cout << "generating normal points with stddev=25" << endl;
                generate_normal_points(points_normal_25, n_val, S, d_val, S / 2, std_devs[0]);
                
                cout << "generating normal points with stddev=50" << endl;
                generate_normal_points(points_normal_50, n_val, S, d_val, S / 2, std_devs[1]);
                
                cout << "generating normal points with stddev=75" << endl;
                generate_normal_points(points_normal_75, n_val, S, d_val, S / 2, std_devs[2]);
                
                // Convertir a vectores
                vector<vector<long long>> points_uniform_vec(points_uniform.begin(), points_uniform.end());
                vector<vector<long long>> points_normal_25_vec(points_normal_25.begin(), points_normal_25.end());
                vector<vector<long long>> points_normal_50_vec(points_normal_50.begin(), points_normal_50.end());
                vector<vector<long long>> points_normal_75_vec(points_normal_75.begin(), points_normal_75.end());

                // Construir árboles
                vector<KDTree> trees;
                vector<string> tree_types;
                vector<string> filenames;
                vector<string> base_dirs;
                
                // Uniform
                trees.push_back(KDTree(points_uniform_vec, S, d_val, false));
                tree_types.push_back("uniform");
                filenames.push_back("KDTree_" + to_string(d_val) + "_" + to_string(n_val) + "_uniform_" + to_string(i + 1) + ".bin");
                base_dirs.push_back("../kdtrees");
                
                trees.push_back(KDTree(points_uniform_vec, S, d_val, true));
                tree_types.push_back("bi_uniform");
                filenames.push_back("KDTree_bi_" + to_string(d_val) + "_" + to_string(n_val) + "_uniform_" + to_string(i + 1) + ".bin");
                base_dirs.push_back("../bikdtrees");
                
                // Normal 25
                trees.push_back(KDTree(points_normal_25_vec, S, d_val, false));
                tree_types.push_back("normal_25");
                filenames.push_back("KDTree_" + to_string(d_val) + "_" + to_string(n_val) + "_normal_25_" + to_string(i + 1) + ".bin");
                base_dirs.push_back("../kdtrees");
                
                trees.push_back(KDTree(points_normal_25_vec, S, d_val, true));
                tree_types.push_back("bi_normal_25");
                filenames.push_back("KDTree_bi_" + to_string(d_val) + "_" + to_string(n_val) + "_normal_25_" + to_string(i + 1) + ".bin");
                base_dirs.push_back("../bikdtrees");
                
                // Normal 50
                trees.push_back(KDTree(points_normal_50_vec, S, d_val, false));
                tree_types.push_back("normal_50");
                filenames.push_back("KDTree_" + to_string(d_val) + "_" + to_string(n_val) + "_normal_50_" + to_string(i + 1) + ".bin");
                base_dirs.push_back("../kdtrees");
                
                trees.push_back(KDTree(points_normal_50_vec, S, d_val, true));
                tree_types.push_back("bi_normal_50");
                filenames.push_back("KDTree_bi" + to_string(d_val) + "_" + to_string(n_val) + "_normal_50_" + to_string(i + 1) + ".bin");
                base_dirs.push_back("../bikdtrees");
                
                // Normal 75
                trees.push_back(KDTree(points_normal_75_vec, S, d_val, false));
                tree_types.push_back("normal_75");
                filenames.push_back("KDTree_" + to_string(d_val) + "_" + to_string(n_val) + "_normal_75_" + to_string(i + 1) + ".bin");
                base_dirs.push_back("../kdtrees");
                
                trees.push_back(KDTree(points_normal_75_vec, S, d_val, true));
                tree_types.push_back("bi_normal_75");
                filenames.push_back("KDTree_bi" + to_string(d_val) + "_" + to_string(n_val) + "_normal_75_" + to_string(i + 1) + ".bin");
                base_dirs.push_back("../bikdtrees");
                
                // Construir arboles y medir tiempo de construccion
                for (int j = 0; j < trees.size(); j++) {
                    measure_build_time(trees[j], tree_types[j], filenames[j], base_dirs[j], d_val, n_val, i);
                }
                if (d_val <= 8){
                // Generar rangos de consulta
                vector<vector<pair<long long, long long>>> query_ranges;
                for (int q = 0; q < num_queries; q++) {
                    query_ranges.push_back(generate_query_range(d_val, S, gen));
                }
                
                // Medir tiempos de reporte
                for (int j = 0; j < trees.size(); j++) {
                    double total_report_time = 0;
                    for (int q = 0; q < num_queries; q++) {
                        total_report_time += measure_report_time(trees[j], query_ranges[q]);
                    }
                    double avg_report_time = total_report_time / num_queries;
                    
                    // Guardar tiempo de reporte
                    string report_time_file = base_dirs[j] + "/d_" + to_string(d_val) + "/n_" + to_string(n_val) + "/" + 
                                            tree_types[j] + "/report_times.txt";
                    ofstream ofs(report_time_file, ios::app);
                    ofs << "d=" << d_val << ", n=" << n_val << ", iter=" << i+1 << 
                         ", avg_report_time=" << avg_report_time << " ms" << endl;
                    ofs.close();
                }}
                
                cout << "Archivos creados para d = " << d_val << ", n = " << n_val << " (iteración " << i + 1 << ")" << endl;
            }
        }
    }

    return 0;
}



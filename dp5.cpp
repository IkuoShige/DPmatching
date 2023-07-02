#include <iostream>
#include <fstream>
#include <cmath>
#define MAXSIZE 254
#define DIMENSION 15
#define NUMOFFILE 100
#define FNAME_OUTPUT "./output/output001.txt"
#define TEMP_NUM 11 //11,12,21,22から選択, %03dで指定すること。
#define MITI_NUM 21 //11,12,21,22から選択, %03dで指定すること。
#define DATA_FILE "./output/data_window_size.txt"
#define SCRIPT_FILE "./output/script_window_size.gp"

using namespace std;

struct McepData {
    char name[20];
    char onso[50];
    int flame;
    double mcepdata[MAXSIZE][DIMENSION];
};

void generateDataFile(const double tangokankyori[], int rows, int cols) {
    ofstream dataFile(DATA_FILE);
    if (dataFile.is_open()) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                dataFile << tangokankyori[i * cols + j] << " ";
            }
            dataFile << endl;
        }
        dataFile.close();
        cout << "Data file has been generated as data.txt." << endl;
    } else {
        cerr << "Unable to open data file." << endl;
    }
}

void generateScriptFile() {
    ofstream scriptFile(SCRIPT_FILE);
    if (scriptFile.is_open()) {
        scriptFile << "set hidden3d" << endl;
        scriptFile << "set term png" << endl;
        scriptFile << "set output \"plot_window_size.png\"" << endl;
        scriptFile << "splot \"" << DATA_FILE << "\" matrix with lines" << endl;
        scriptFile.close();
        cout << "Script file has been generated as script.gp." << endl;
    } else {
        cerr << "Unable to open script file." << endl;
    }
}

int main() {
    int i, j, k, h0, h;
    ifstream fp_temp, fp_miti;
    McepData city_temp, city_miti; //テンプレートのデータと未知入力データ
    char ch0[200];
    double d[MAXSIZE][MAXSIZE];      //局所距離
    double g[MAXSIZE][MAXSIZE];      //累積距離
    double tangokankyori[NUMOFFILE]; //単語間距離
    double tangokankyori_min;
    int num_matchfname = 0;
    int count = 0;

    // 斜め遷移の重みパラメータ
    double diagonalWeight = 2.0; // 1.0やsqrt(2.0)などに変更可能

    for (h0 = 0; h0 < NUMOFFILE; h0++) {
        sprintf(ch0, "./city%03d/city%03d_%03d.txt", TEMP_NUM, TEMP_NUM, h0 + 1);
        fp_temp.open(ch0);
        if (!fp_temp.is_open()) {
            cout << "temp file open error!!" << endl;
            exit(EXIT_FAILURE);
        }
        fp_temp.getline(city_temp.name, sizeof(city_temp.name));
        fp_temp.getline(city_temp.onso, sizeof(city_temp.onso));
        fp_temp.getline(ch0, sizeof(ch0));
        city_temp.flame = atoi(ch0);
        for (i = 0; i < city_temp.flame; i++) {
            for (j = 0; j < DIMENSION; j++) {
                fp_temp >> city_temp.mcepdata[i][j];
            }
        }
        for (h = 0; h < NUMOFFILE; h++) {
            sprintf(ch0, "./city%03d/city%03d_%03d.txt", MITI_NUM, MITI_NUM, h + 1);
            fp_miti.open(ch0);
            if (!fp_miti.is_open()) {
                cout << "miti file open error!!" << endl;
                exit(EXIT_FAILURE);
            }
            fp_miti.getline(city_miti.name, sizeof(city_miti.name));
            fp_miti.getline(city_miti.onso, sizeof(city_miti.onso));
            fp_miti.getline(ch0, sizeof(ch0));
            city_miti.flame = atoi(ch0);
            for (i = 0; i < city_miti.flame; i++) {
                for (j = 0; j < DIMENSION; j++) {
                    fp_miti >> city_miti.mcepdata[i][j];
                }
            }
            int max_flame = max(city_temp.flame, city_miti.flame);
            int window_size = abs(city_temp.flame - city_miti.flame) + 120; //整合窓のサイズ
            for (i = 0; i < city_temp.flame; i++) {
                for (j = max(0, i - window_size); j < min(city_miti.flame, i + window_size + 1); j++) {
                    d[i][j] = 0;
                    for (k = 0; k < DIMENSION; k++) {
                        d[i][j] += (city_temp.mcepdata[i][k] - city_miti.mcepdata[j][k]) * (city_temp.mcepdata[i][k] - city_miti.mcepdata[j][k]);
                    }
                    d[i][j] = sqrt(d[i][j]);
                }
            }
            g[0][0] = d[0][0];
            for (i = 1; i < city_temp.flame; i++) {
                g[i][0] = g[i - 1][0] + d[i][0];
            }
            for (j = 1; j < city_miti.flame; j++) {
                g[0][j] = g[0][j - 1] + d[0][j];
            }
            for (i = 1; i < city_temp.flame; i++) {
                for (j = 1; j < city_miti.flame; j++) {
                    double a = g[i][j - 1] + d[i][j];
                    double b = g[i - 1][j - 1] + diagonalWeight * d[i][j];
                    double c = g[i - 1][j] + d[i][j];
                    g[i][j] = a;
                    if (b < g[i][j]) {
                        g[i][j] = b;
                    }
                    if (c < g[i][j]) {
                        g[i][j] = c;
                    }
                }
            }
            tangokankyori[h] = g[city_temp.flame - 1][city_miti.flame - 1] / (city_temp.flame + city_miti.flame);
            fp_miti.close();
        }
        tangokankyori_min = tangokankyori[0];
        num_matchfname = 0;
        for (h = 1; h < NUMOFFILE; h++) {
            if (tangokankyori_min > tangokankyori[h]) {
                tangokankyori_min = tangokankyori[h];
                num_matchfname = h;
            }
        }
        fp_temp.close();
        if (num_matchfname == h0) {
            count++;
        }
        if (num_matchfname != h0) {
            /*cout << "===========Result NOT Matchng===========" << endl;
            cout << "city_temp     : city" << TEMP_NUM << "/city" << TEMP_NUM << "_" << h0 + 1 << ".txt" << endl;
            cout << "city_miti     : city" << MITI_NUM << "/city" << MITI_NUM << "_" << num_matchfname + 1 << ".txt" << endl;
            cout << "tangokankyori : " << tangokankyori_min << endl;*/
            /*output += "===========Result NOT Matchng===========\n";
            output += "city_temp     : city" + std::to_string(TEMP_NUM) + "/city" + std::to_string(TEMP_NUM) + "_" + std::to_string(h0 + 1) + ".txt\n";
            output += "city_miti     : city" + std::to_string(MITI_NUM) + "/city" + std::to_string(MITI_NUM) + "_" + std::to_string(num_matchfname + 1) + ".txt\n";
            output += "tangokankyori : " + std::to_string(tangokankyori_min) + "\n";*/
        }
    }

    cout << "正答率 " << (count * 100) / NUMOFFILE << "% です。" << endl;

    generateDataFile(tangokankyori, NUMOFFILE, NUMOFFILE);
    generateScriptFile();

    system("gnuplot ./output/script_window_size.gp");

    return 0;
}

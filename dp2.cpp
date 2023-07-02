#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>

#define DIMENSION 24
#define NUMOFFILE 100
#define FNAME_OUTPUT "output.txt"

using namespace std;

struct data {
    char name[256];
    char onso[256];
    int flame;
    double mcepdata[256][24];
};

int main() {
    struct data city_temp, city_miti;
    double d[256][256], g[256][256];
    double tangokankyori[10000];
    double tangokankyori_min;
    int i, j, k, h, h0, count, num_matchfname;
    char ch0[256];
    ifstream fp_temp, fp_miti;
    string output;

    count = 0;

    for (h0 = 0; h0 < NUMOFFILE; h0++) {
        sprintf(ch0, "./city%03d/city%03d_%03d.txt", h0 / 100 + 11, h0 / 100 + 11, h0 % 100 + 1);
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
        fp_temp.close();

        for (h = 0; h < NUMOFFILE; h++) {
            sprintf(ch0, "./city%03d/city%03d_%03d.txt", h / 100 + 11, h / 100 + 11, h % 100 + 1);
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
            fp_miti.close();

            for (i = 0; i < city_temp.flame; i++) {
                for (j = 0; j < city_miti.flame; j++) {
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
                    g[i][j] = min(g[i - 1][j] + d[i][j], min(g[i][j - 1] + d[i][j], g[i - 1][j - 1] + 2 * d[i][j]));
                }
            }

            tangokankyori[h] = g[city_temp.flame - 1][city_miti.flame - 1];

            if (h == 0 || tangokankyori[h] < tangokankyori_min) {
                tangokankyori_min = tangokankyori[h];
                num_matchfname = h;
            }
        }

        if (tangokankyori_min < 10) {
            count++;
            output += "temp file " + to_string(h0 + 1) + " matches to miti file " + to_string(num_matchfname + 1) + "\n";
            output += "tangokankyori_min : " + to_string(tangokankyori_min) + "\n";
            cout << "temp file " << h0 + 1 << " matchs to miti file " << num_matchfname + 1 << endl;
            cout << "tangokankyori_min : " << tangokankyori_min << endl;
        }
    }

    output += "一致したパターンの数: " + to_string(count) + "\n";
    cout << "一致したパターンの数: " << count << endl;

    ofstream outputFile(FNAME_OUTPUT);
    if (outputFile.is_open()) {
        outputFile << output;
        outputFile.close();
        cout << "結果が " << FNAME_OUTPUT << " に書き込まれました。" << endl;
    } else {
        cout << "出力ファイルを開くことができませんでした。" << endl;
    }

    return 0;
}

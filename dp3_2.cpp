#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#define MAXSIZE   254
#define DIMENSION 15
#define NUMOFFILE 100
#define FNAME_OUTPUT "./output/output001.txt"
#define TEMP_NUM 11 //11,12,21,22から選択, %03dで指定すること。
#define MITI_NUM 21 //11,12,21,22から選択, %03dで指定すること。
using namespace std;

void writeResultToFile(const std::string& filename, const std::string& result) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << result;
        file.close();
        std::cout << "結果が " << filename << " に書き込まれました。" << std::endl;
    } else {
        std::cerr << "ファイルを開けませんでした。" << std::endl;
    }
}

struct McepData {
    char name[20];
    char onso[50];
    int flame;
    double mcepdata[MAXSIZE][DIMENSION];
};

int main() {
    int h0, h, i, j, k;
    ifstream fp_temp, fp_miti;
    McepData city_temp, city_miti; //テンプレートのデータと未知入力データ
    char ch0[200];
    double d[MAXSIZE][MAXSIZE];      //局所距離
    double g[MAXSIZE][MAXSIZE];      //累積距離
    double tangokankyori[NUMOFFILE]; //単語間距離
    double tangokankyori_min;
    int num_matchfname = 0;
    int count = 0;

    vector<vector<double>> backtrack;

    cout << "city" << TEMP_NUM << "とcity" << MITI_NUM << "の認識実験を開始します。" << endl;

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
            //DP計算
            for (i = 0; i < city_temp.flame; i++) {
                for (j = 0; j < city_miti.flame; j++) {
                    d[i][j] = 0;
                    for (k = 0; k < DIMENSION; k++) {
                        d[i][j] += pow(city_temp.mcepdata[i][k] - city_miti.mcepdata[j][k], 2);
                    }
                    d[i][j] = sqrt(d[i][j]);
                }
            }
            //累積距離計算
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
            cout << h0 + 1 << " vs " << h + 1 << ": " << tangokankyori[h] << endl;
            fp_miti.close();

            if (h0 == 0 && h == 0) {
                tangokankyori_min = tangokankyori[h];
                num_matchfname = h;
            } else if (tangokankyori[h] < tangokankyori_min) {
                tangokankyori_min = tangokankyori[h];
                num_matchfname = h;
            }
        }
        fp_temp.close();
        if (num_matchfname == h0) {
            count++;
            vector<double> match;
            match.push_back((double)TEMP_NUM);
            match.push_back((double)MITI_NUM);
            match.push_back(tangokankyori_min);
            backtrack.push_back(match);
        }
    }

    // 結果をファイルに書き込む
    std::string result;
    for (const auto& match : backtrack) {
        result += "city" + std::to_string((int)match[0]) + "とcity" + std::to_string((int)match[1]) + "の認識結果: " + std::to_string(match[2]) + "\n";
    }
    writeResultToFile(FNAME_OUTPUT, result);

    cout << "認識終了" << endl;
    cout << "一致数：" << count << endl;
    cout << "検索対象の音素ファイル数：" << NUMOFFILE << endl;

    return 0;
}

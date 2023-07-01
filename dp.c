#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#define MAXSIZE   254
#define DIMENTION 15
#define NUMOFFILE 100
#define FNAME_OUTPUT "./output001.txt"
#define TEMP_NUM 11 //11,12,21,22から選択, %03dで指定すること。
#define MITI_NUM 21 //11,12,21,22から選択, %03dで指定すること。
typedef struct {
    char name[20];
    char onso[50];
    int  flame;
    double mcepdata[MAXSIZE][DIMENTION];
} mcepdata_t;

int main(void)
{
	int  h0, h, i, j, k;
	FILE *fp_temp, *fp_miti, *fp_output;
	mcepdata_t city_temp, city_miti; //テンプレートのデータと未知入力データ
	char ch0[200]; 
	double d[MAXSIZE][MAXSIZE];      //局所距離
	double g[MAXSIZE][MAXSIZE];      //累積距離
	double tangokankyori[NUMOFFILE]; //単語間距離
	double tangokankyori_min; 
	int    num_matchfname = 0;
	int    count = 0;

    printf("city%03dとcity%03dの認識実験を開始します。\n", TEMP_NUM, MITI_NUM);

	for (h0 = 0; h0 < NUMOFFILE; h0++) {
		sprintf(ch0, "./city%03d/city%03d_%03d.txt",TEMP_NUM, TEMP_NUM, h0 + 1);
		if ((fp_temp = fopen(ch0, "r")) == NULL) {
			printf("temp file open error!!\n");
			exit(EXIT_FAILURE);
		}
		fgets(city_temp.name, sizeof(city_temp.name), fp_temp);
		fgets(city_temp.onso, sizeof(city_temp.onso), fp_temp);
		fgets(ch0,sizeof(ch0), fp_temp);
		city_temp.flame = atoi(ch0);
		for (i = 0; i < city_temp.flame; i++) {
            for (j = 0; j < DIMENTION; j++) {
				fscanf(fp_temp, "%lf", &city_temp.mcepdata[i][j]);
			}
		}
		for (h = 0; h < NUMOFFILE; h++) {
			sprintf(ch0, "./city%03d/city%03d_%03d.txt",MITI_NUM, MITI_NUM, h + 1);
			if ((fp_miti = fopen(ch0, "r")) == NULL) {
				printf("miti file open error!!\n");
				exit(EXIT_FAILURE);
			}
			fgets(city_miti.name, sizeof(city_miti.name), fp_miti);
			fgets(city_miti.onso, sizeof(city_miti.onso), fp_miti);
			fgets(ch0,sizeof(ch0), fp_miti);
			city_miti.flame = atoi(ch0);
			for (i = 0; i < city_miti.flame; i++) {
				for (j = 0; j < DIMENTION; j++) {
					fscanf(fp_miti, "%lf", &city_miti.mcepdata[i][j]);
				}
			}
			for (i = 0; i < city_temp.flame; i++) {
				for (j = 0; j < city_miti.flame; j++) {
                    d[i][j] = 0;
					for (int k = 0; k < DIMENTION; k++) {
						d[i][j] += (city_temp.mcepdata[i][k] - city_miti.mcepdata[j][k]) * (city_temp.mcepdata[i][k] - city_miti.mcepdata[j][k]); 
					}
                    d[i][j] = sqrtl(d[i][j]);
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
					double b = g[i - 1][j - 1] + 2 * d[i][j];
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
			fclose(fp_miti);
		}
        tangokankyori_min = tangokankyori[0];
        num_matchfname = 0;
		for (h = 1; h < NUMOFFILE; h++) {
			if (tangokankyori_min > tangokankyori[h] ) {
				tangokankyori_min = tangokankyori[h];
				num_matchfname = h;
			}
		}
		fclose(fp_temp);
		if (num_matchfname == h0 ) {
			count++;
		}
		if (num_matchfname != h0 ) {
            printf("===========Result NOT Matchng===========\n");
            printf("city_temp     : city%03d/city%03d_%03d.txt\n", TEMP_NUM, TEMP_NUM, h0 + 1);
            printf("city_miti     : city%03d/city%03d_%03d.txt\n", MITI_NUM, MITI_NUM, num_matchfname + 1);
            printf("tangokankyori : %f\n", tangokankyori_min);
        }
	}
	printf("正答率 %d%% です。\n", count); 
	return 0;
}

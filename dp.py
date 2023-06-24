import math

MAXSIZE = 254
DIMENSION = 15
NUMOFFILE = 100
FNAME_OUTPUT = "./output001.txt"
TEMP_NUM = 11  # 11,12,21,22から選択, %03dで指定すること。
MITI_NUM = 21  # 11,12,21,22から選択, %03dで指定すること。

class McepData:
    def __init__(self):
        self.name = ""
        self.onso = ""
        self.flame = 0
        self.mcepdata = [[0.0] * DIMENSION for _ in range(MAXSIZE)]

def main():
    print(f"city{TEMP_NUM:03d}とcity{MITI_NUM:03d}の認識実験を開始します。")

    count = 0

    for h0 in range(NUMOFFILE):
        ch0 = f"./city{TEMP_NUM:03d}/city{TEMP_NUM:03d}_{h0 + 1:03d}.txt"
        with open(ch0, "r") as fp_temp:
            city_temp = McepData()
            city_temp.name = fp_temp.readline().strip()
            city_temp.onso = fp_temp.readline().strip()
            city_temp.flame = int(fp_temp.readline())
            for i in range(city_temp.flame):
                line = fp_temp.readline()
                city_temp.mcepdata[i] = list(map(float, line.split()))

            tangokankyori = []
            for h in range(NUMOFFILE):
                ch0 = f"./city{MITI_NUM:03d}/city{MITI_NUM:03d}_{h + 1:03d}.txt"
                with open(ch0, "r") as fp_miti:
                    city_miti = McepData()
                    city_miti.name = fp_miti.readline().strip()
                    city_miti.onso = fp_miti.readline().strip()
                    city_miti.flame = int(fp_miti.readline())
                    for i in range(city_miti.flame):
                        line = fp_miti.readline()
                        city_miti.mcepdata[i] = list(map(float, line.split()))

                    d = [[0.0] * MAXSIZE for _ in range(MAXSIZE)]
                    g = [[0.0] * MAXSIZE for _ in range(MAXSIZE)]

                    for i in range(city_temp.flame):
                        for j in range(city_miti.flame):
                            d[i][j] = sum((city_temp.mcepdata[i][k] - city_miti.mcepdata[j][k]) ** 2 for k in range(DIMENSION))
                            d[i][j] = math.sqrt(d[i][j])

                    g[0][0] = d[0][0]
                    for i in range(1, city_temp.flame):
                        g[i][0] = g[i - 1][0] + d[i][0]
                    for j in range(1, city_miti.flame):
                        g[0][j] = g[0][j - 1] + d[0][j]
                    for i in range(1, city_temp.flame):
                        for j in range(1, city_miti.flame):
                            a = g[i][j - 1] + d[i][j]
                            b = g[i - 1][j - 1] + 2 * d[i][j]
                            c = g[i - 1][j] + d[i][j]
                            g[i][j] = a
                            if b < g[i][j]:
                                g[i][j] = b
                            if c < g[i][j]:
                                g[i][j] = c

                    tangokankyori.append(g[city_temp.flame - 1][city_miti.flame - 1] / (city_temp.flame + city_miti.flame))

        tangokankyori_min = min(tangokankyori)
        num_matchfname = tangokankyori.index(tangokankyori_min)

        if num_matchfname == h0:
            count += 1
        else:
            print("===========Result NOT Matchng===========")
            print(f"city_temp     : city{TEMP_NUM:03d}/city{TEMP_NUM:03d}_{h0 + 1:03d}.txt")
            print(f"city_miti     : city{MITI_NUM:03d}/city{MITI_NUM:03d}_{num_matchfname + 1:03d}.txt")
            print(f"tangokankyori : {tangokankyori_min}")

    print(f"正答率 {count / NUMOFFILE * 100:.0f}% です。")

if __name__ == "__main__":
    main()

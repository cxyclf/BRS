// chengke.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <random>
#include "Readfile.h"
#include <vector>
#include "query.h"
#include <fstream>

#include <algorithm>
#include <string>

using namespace std;
const double zgf = 8; // 早高峰时间
const double sigma = 2; //早高峰标准差
const double dPercentageOfMorning = 0.52; //上午人数比例
const double wgf = 17; // 晚高峰时间
const double sigma2 = 3; // 晚高峰标准差
const double maxAllowDistance = 32000;
const double lambda = 4;//泊松分布参数
const int iDefaultBin = 12;//将旅行距离划分成iDefaultBin份
//以随机生成的数据中最长的旅行距离，除以iDefaultBin得每份大小。每份所含数据个数服从参数为lambda的泊松分布
const int iNumOfCK = 2000000;             //订单数量
const int totalPassengers = 4100000;      // 总乘客数
const int vehicle_speed = 8.4;   //车辆速度  m/s


// 统计数据数量
vector<int> countData(const vector<double> &data, int bin) {
    vector<int> counts(bin, 0);
    for (int i = 0; i < data.size(); i++) {
        if (data[i] < 0 || data[i] >= bin)
            continue;
        int index = data[i];
        counts[index]++;
    }
    return counts;
}

// 绘制直方图
void drawHistogram(const vector<double>& data, int bin, int height=20) {
    vector<int> counts = countData(data, bin);
    int maxHeight = 0;

    for (int i = 0; i < counts.size(); i++) {
        if (counts[i] > maxHeight) {
            maxHeight = counts[i];
        }
    }
    for (int i = 0; i < bin; ++i)
        counts[i] = counts[i] * height / maxHeight;
    maxHeight = height;
    for (int i = maxHeight; i > 0; i--) {
        cout << "| ";
        for (int j = 0; j < counts.size(); j++) {
            if (counts[j] >= i) {
                cout << "*";
            }
            else {
                cout << " ";
            }
            cout << " | ";
        }
        cout << endl;
    }
    cout << "+-";
    for (int j = 0; j < counts.size(); j++) {
        cout << "--+-";
    }
    cout << endl;
    cout << "| ";
    for (int i = 0; i < counts.size(); i++) {
        cout << i;
        if (i < 10)
            cout << " ";
        cout<< "| ";
    }
    cout << endl;
}

void genStartTime(int iNum, vector<double> & vData)
{
    // 设定随机数生成器的种子
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> dist(zgf, sigma);

    vData.resize(iNum);

    int iNumOfMorning = dPercentageOfMorning * iNum;
    for (int i = 0; i < iNumOfMorning; ++i) {
        double value;
        do {
            value = dist(gen);
        } while (value < 0.0 || value > 24.0);
        vData[i] = value;
    }


    std::normal_distribution<double> dist2(wgf, sigma2);
    //int iNumOfAfternoon = iNum - iNumOfMorning;
    for (int i = iNumOfMorning; i < iNum; ++i) {
        double value;
        do {
            value = dist2(gen);
        } while (value < 0.0 || value > 24.0);
        vData[i] = value;
    }
}

void genPossion(int iNum, vector<double>& vData) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::poisson_distribution<int> dist(lambda);
    vData.resize(iNum);

    for (int i = 0; i < iNum; ++i)
        vData[i] = dist(gen);
}
class VD
{
public:
    int v1, v2;
    double distance;
    bool operator < (const VD& other) const
    {
        return distance < other.distance;
    }
};
class TravelDistance {
public:
    TravelDistance(string _city, int _NumOfCK) :city(_city),iNumOfCK(_NumOfCK),dod(_NumOfCK) { indexName = city + ".index"; prepare(); }
    string city;
    string indexName;
    int iNumOfCK;
    int iVNum;
    QUERY query;
    void prepare()
    {
        FILE* file = NULL;
        //file = fopen(indexName.c_str(), "r");
        fopen_s(&file, indexName.c_str(), "r");
        if (file == NULL)
            buildIndex(city);
        fopen_s(&file, indexName.c_str(), "r");
        assert(file);
        fclose(file);
        query.readIndex(indexName);
        iVNum = query.iVNum;
    }
    
    class DistributionOfDistance
    {
    public:
        DistributionOfDistance(int _NumOfCK) :bin(iDefaultBin), iNumOfCK(_NumOfCK) {}
        int bin;//将最长距离分为多少份
        int iNumOfCK;
        vector<double> range;
        vector<int> countNeed;
        vector<int> countCurrent;
        void initial()
        {
            countCurrent.resize(bin, 0);
            countNeed.resize(bin, 0);

            vector<double> vDistance;
            genPossion(iNumOfCK, vDistance);
            for (int i = 0; i < iNumOfCK; i++) {
                int index = vDistance[i];
                if (index > bin - 1)
                    index = bin - 1;
                countNeed[index]++;
            }
        }
        void setRange(double maxvalue)
        {
            range.resize(bin, 0);
            double gap = maxvalue / bin;
            for (int ii = 0; ii < bin; ++ii)
                range[ii] = gap * (ii+1);
        }
    };

    DistributionOfDistance dod;
    void gendist(vector<VD>& vCollect)
    {
        dod.initial();
        VD vd;
        int iBufferSize = 10 * iNumOfCK;
        vector<VD> vBuffer(iBufferSize);
        double maxdist=0, avgdist=0;

        /*
        for (int ii = 0; ii < iBufferSize; ++ii)
        {
            vBuffer[ii].v1 = rand() / (double)RAND_MAX * (iVNum - 0.00001);
            vBuffer[ii].v2 = rand() / (double)RAND_MAX * (iVNum - 0.00001);
            vBuffer[ii].distance = query.distanceQuery(vBuffer[ii].v1, vBuffer[ii].v2);
            maxdist = Max(vBuffer[ii].distance, maxdist);
            avgdist += vBuffer[ii].distance;
        }
        */
        for (int ii = 0; ii < iBufferSize; ++ii)
        {
            do {
                // 重新生成v1和v2直到生成的距离小于或等于某个最大允许值maxAllowDistance
                vBuffer[ii].v1 = rand() / (double)RAND_MAX * (iVNum - 0.00001);
                vBuffer[ii].v2 = rand() / (double)RAND_MAX * (iVNum - 0.00001);
                vBuffer[ii].distance = query.distanceQuery(vBuffer[ii].v1, vBuffer[ii].v2);
            } while (vBuffer[ii].distance > maxAllowDistance); // 如果距离大于maxAllowDistance，则重新生成

            maxdist = Max(vBuffer[ii].distance, maxdist); // 更新最大距离
            avgdist += vBuffer[ii].distance; // 更新平均距离的累计值
        }

        avgdist /= iBufferSize;
        dod.setRange(maxdist);
        srand((unsigned)time(NULL));
        int iCount = 0, ibuffer = 0;
        while (true)
        {
            int v1, v2;
            double distance;
            if (ibuffer < iBufferSize)
            {
                v1 = vBuffer[ibuffer].v1;
                v2 = vBuffer[ibuffer].v2;
                distance = vBuffer[ibuffer].distance;
                ibuffer++;
            }
            else {
                v1 = rand() / (double)RAND_MAX * (iVNum - 0.00001);
                v2 = rand() / (double)RAND_MAX * (iVNum - 0.00001);
                distance = query.distanceQuery(v1, v2);
            }
            for (int kk = 0; kk < dod.bin; ++kk)
                if (distance < dod.range[kk] || kk == dod.bin - 1)
                {
                    if (dod.countCurrent[kk] >= dod.countNeed[kk])
                        break;

                    vd.v1 = v1; vd.v2 = v2; vd.distance = distance;
                    vCollect.push_back(vd);
                    dod.countCurrent[kk]++;
                    ++iCount;
                    break;
                }
            if (iCount == iNumOfCK)
                break;
        }
        random_shuffle(vCollect.begin(), vCollect.end());
    }
};

void genPassengerCounts(int iNumOfCK, vector<int>& passengerCounts, int totalPassengers) {
    passengerCounts.resize(iNumOfCK, 1); // 每个乘客初始化为 1
    int remainingPassengers = totalPassengers - iNumOfCK; // 需要再分配的乘客数量

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disInt(1, 3);

    // 随机分配剩余乘客数
    for (int i = 0; i < iNumOfCK && remainingPassengers > 0; ++i) {
        int increment = disInt(gen); // 生成1到3之间的随机数
        increment = min(increment, remainingPassengers);
        passengerCounts[i] += increment;
        remainingPassengers -= increment;
    }

    // 随机打乱分配
    std::shuffle(passengerCounts.begin(), passengerCounts.end(), gen);
}

int main() {
    const int bin = 24;//24小时
    vector<double> vStart;
    genStartTime(iNumOfCK, vStart);
    drawHistogram(vStart, bin);
    sort(vStart.begin(), vStart.end());

    vector<double> vPossion;
    genPossion(iNumOfCK, vPossion);
    drawHistogram(vPossion, iDefaultBin);
    string city("a");
    TravelDistance td(city,iNumOfCK);
    vector<VD> vCollect;
    td.gendist(vCollect);

    /*
    //开始时间，起始站点，终止站点，距离
    for (int ii = 0; ii < iNumOfCK; ++ii)
        cout << vStart[ii] <<'\t'<< vCollect[ii].v1 << '\t' << vCollect[ii].v2 << '\t' << vCollect[ii].distance << endl;
    */


    vector<int> passengerCounts;
    genPassengerCounts(iNumOfCK, passengerCounts, totalPassengers);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.5, 1.8);   //生成1.5到1.8之间的随机数

    // 打开CSV文件
    ofstream outputFile("output.csv");
    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open output file!" << endl;
        return 1;
    }
    // 输出到CSV文件
    int x = 0;
    outputFile << "Publish Time,Start Time,Deadline Time,Start Vertex,End Vertex,Distance,Passenger Count" << endl;
    //要修改为特定的日期，可以加上时间戳，比如2016年11月19日就需要加1479484800
    for (int ii = 0; ii < iNumOfCK; ++ii) {
        
        int Publish = static_cast<int>(vStart[ii] * 3600);
        int Start = Publish + 600;
        double multiplier = dis(gen);          
        int Deadline = Start+ static_cast<int>(vCollect[ii].distance / vehicle_speed) * multiplier;
        //我们会把终止时间大于这一天的订单剔除掉，所以订单数量会有所减少,影响不是很大。这里时间戳是1970.1.1
        if (Deadline <= 86400) {
            x = x + passengerCounts[ii];
            outputFile << Publish << ',' << Start << ',' << Deadline << ',' << vCollect[ii].v1 << ',' << vCollect[ii].v2 << ',' << vCollect[ii].distance << ',' << passengerCounts[ii] << endl;
        }
        cout << ii << endl;
    }
    cout << x << endl;
    // 关闭文件
    outputFile.close();

    return 0;
}
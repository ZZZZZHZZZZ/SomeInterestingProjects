#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <random>
#include <time.h>
#include <queue>
#include <fstream>
using namespace std;
struct Point {
	int x;
	int y;
	Point(int inix, int iniy) :x(inix), y(iniy) {}
};
//互不相关的分布估计算法
//求解 解空间大小为 M 的 TSP 问题
void generateFirst(vector<vector<int>>& solutionVec, const vector<vector<int>>& citVec, int M, int N);
int min(const vector<int>& citVec, const vector<int>& temp);
void readFromTxt(string filename, vector<Point>& pointVec);
void generateCityVec(const vector<Point>& pointVec, vector<vector<int>>& cityAndLengthVec);
bool testAdapt(const queue<int>& bestAdaptQueue, const vector<pair<vector<int>, int>>& adaptabilityPairVec);
void printPairVec(const vector<pair<vector<int>, int>>& adaptabilityPairVec);
void printVec(const vector<pair<vector<int>, int>>& adapt, const vector<vector<int>>& solution);
void printVec(const vector<vector<double>>& temp);
void printVec(const vector<vector<int>>& temp);
bool cmp(const pair<vector<int>, int>& a, const pair<vector<int>, int>& b);
void caculateAdaptability(const vector<vector<int>>& solutionVec, const vector<vector<int>>& cityAndLengthVec, vector<pair<vector<int>, int>>& adaptabilityPairVec);
void lunpandu(vector<vector<double>>& PVec, const int N);
void generateSolution(vector<vector<int>>& solutionVec, const vector<vector<double>>& PVec, const int N, const int M);
void getBetterSolution(int N, const vector<vector<int>>& solutionVec, vector<vector<int>>& betterSolutionVec, const vector<pair<vector<int>, int>>& adaptabilityPairVec);
void count(const vector<vector<int>>& betterSolutionVec, vector<vector<int>>& countVec);
void calculate(const vector<vector<int>>& countVec, vector<vector<double>>& PVec, const int N);
void generateSolution2(vector<vector<int>>& solutionVec, const vector<vector<double>>& PVec, const int N, const int M);
int findCity(const vector<double>& PVec, const vector<int>& temp, const double p);
clock_t beginTime, stopTime;
int main() {

	//int N = 6;
	int M = 1000;//解空间的大小
	int minlength = 1000000;
	/*ofstream fs;
	fs.open("adapt.txt");*/

	vector<int> minsolution;
	vector<vector<int>> cityAndLengthVec;//存储每个城市到其他城市的距离，到自身的距离设定为0
	vector<Point> pointVec;
	string filename = "berlin28.txt";//每行三个数字，第一个数字为城市序号，剩下两个为城市x,y坐标。
	readFromTxt(filename, pointVec);
	generateCityVec(pointVec, cityAndLengthVec);
	/*for (auto i : cityAndLengthVec) {
		for (auto j : i) {
			cout << j << " ";
		}
		cout << endl;
	}*/
	int N = cityAndLengthVec.size();
	//                                      0 1 2 3 4 5
	//vector<vector<int>> cityAndLengthVec{  {0,3,1,6,4,3},//0
	//									   {3,0,5,7,6,6},//1
	//									   {1,5,0,2,9,4},//2
	//									   {6,7,2,0,1,7},//3
	//									   {4,6,9,1,0,8},//4
	//									   {3,6,4,7,8,0} };//5
	//例如：
	//  A B C D
	//A 0,1,2,3 
	//B 1,0,2,3
	//C 2,2,0,3 
	//D 3,3,3,0 
	vector<vector<int>> solutionVec(M, vector<int>(N, 0));//完整解空间
	vector<vector<int>> betterSolutionVec(M / 3, vector<int>(N, 0));//选优一半后解空间
	//为了方便，如果按照二维数组观察，解集中每组解按横行显示，竖行为每一组Xi的解
	//{1，  2，  3，  4，  5}  解1
	//{1，  2，  2，  4，  5}  解2
	//{1，  2，  3，  4，  5}  解3
	//{1，  2，  3，  4，  5}  解4
	//{1，  2，  3，  4，  5}  解5
	// x1   x2   x3   x4   x5  
	vector<vector<int>> countVec(N, vector<int>(N, 0));//每一个城市到其他城市的转移次数矩阵
	vector<vector<double>> PVec(N, vector<double>(N, double(1 / double(N - 1))));//每一个城市到其他城市的转移概率矩阵
	for (int k = 0; k < PVec.size(); ++k) {
		PVec[k][k] = 0;
	}
	vector<pair<vector<int>, int>> adaptabilityPairVec(M);//存储格式为<解ID，适应度>
	lunpandu(PVec, N);//按照轮盘赌的方式更新概率值
	int i = 1;
	queue<int> bestAdaptQueue;
	//如果优势解适应度均相等并且最低适应度连续二十次迭代没有变化 或迭代次数超过10000次，就停止迭代输出优势解
	beginTime = clock();
	generateFirst(solutionVec, cityAndLengthVec, M, N);
	/*printVec(solutionVec);*/
	while (testAdapt(bestAdaptQueue, adaptabilityPairVec) == false || adaptabilityPairVec[0].second != minlength) {
		if (i > 100) {
			break;
		}
		//cout << "城市转移概率矩阵(经过轮盘赌): " << endl;
		//printVec(PVec);
		caculateAdaptability(solutionVec, cityAndLengthVec, adaptabilityPairVec);//计算每一组解的适应度
		//cout << "新的完整解空间与解的适应度: " << endl;
		//printPairVec(adaptabilityPairVec);
		getBetterSolution(N, solutionVec, betterSolutionVec, adaptabilityPairVec);//获取优势解
		//cout << "优势解空间与解的适应度: " << endl;
		if (i > 20) {
			bestAdaptQueue.pop();
			bestAdaptQueue.push(adaptabilityPairVec[0].second);
		}
		else {
			bestAdaptQueue.push(adaptabilityPairVec[0].second);
		}
		//printVec(adaptabilityPairVec,betterSolutionVec);
		count(betterSolutionVec, countVec);//计数
		//cout << "城市转移次数计数矩阵: " << endl;
		//printVec(countVec);
		calculate(countVec, PVec, N);//计算概率
		//cout << "新的城市转移概率矩阵(未经过轮盘赌): " << endl;
		//printVec(PVec);
		lunpandu(PVec, N);//按照轮盘赌的方式更新概率值
		/*for (auto temp : adaptabilityPairVec[0].first) {
			cout << temp << " ";
		}
		*/
		generateSolution(solutionVec, PVec, N, M);//根据转移概率生成新的解空间
		cout << "本轮最佳适应度：" << adaptabilityPairVec[0].second << endl;
		if (adaptabilityPairVec[0].second < minlength) {
			minsolution = adaptabilityPairVec[0].first;
			minlength = adaptabilityPairVec[0].second;
		}
		/*fs << to_string(adaptabilityPairVec[0].second) << ",";*/
		cout << "----------------------------------第" << i << "轮：" << "----------------------------------" << endl;
		++i;
	}
	/*fs.close();*/
	/*cout << "已达到最优值，最终一轮迭代优势解与适应度为: " << endl;
	printVec(adaptabilityPairVec, betterSolutionVec);*/
	cout << "最优解与适应度为: " << endl;
	for (auto i : minsolution) {
		cout << i << " ";
	}
	cout << endl;
	cout << "适应度为：" << minlength << endl;
	stopTime = clock();
	cout << "Time used " << (stopTime - beginTime) << endl;
}
void readFromTxt(string filename, vector<Point>& pointVec) {
	ifstream file;
	vector<int> each_point(3, -1);
	file.open(filename);
	for (int i = 0; !file.eof(); i++)
	{
		file >> each_point[0] >> each_point[1] >> each_point[2];
		pointVec.push_back(Point(each_point[1], each_point[2]));
	}
	file.close();
}
void generateCityVec(const vector<Point>& pointVec, vector<vector<int>>& cityAndLengthVec) {
	for (int i = 0; i < pointVec.size(); ++i) {
		vector<int> lengthVec;
		for (int j = 0; j < pointVec.size(); ++j) {
			int length = sqrt((pointVec[j].x - pointVec[i].x) * (pointVec[j].x - pointVec[i].x) + (pointVec[j].y - pointVec[i].y) * (pointVec[j].y - pointVec[i].y));
			lengthVec.push_back(length);
		}
		cityAndLengthVec.push_back(lengthVec);
	}
}
bool testAdapt(const queue<int>& bestAdaptQueue, const vector<pair<vector<int>, int>>& adaptabilityPairVec) {
	if (bestAdaptQueue.size() < 20) {
		return false;
	}
	for (int i = 1; i < adaptabilityPairVec.size() / 2; ++i) {
		if (adaptabilityPairVec[i].second != adaptabilityPairVec[i - 1].second) {
			return false;
		}
	}
	queue<int>tempQueue = bestAdaptQueue;
	int temp = tempQueue.front();
	tempQueue.pop();
	while (tempQueue.empty() == false) {
		if (temp != tempQueue.front()) {
			return false;
		}
		tempQueue.pop();
	}
	return true;
}

void printPairVec(const vector<pair<vector<int>, int>>& adaptabilityPairVec) {
	for (auto i : adaptabilityPairVec) {
		cout << "< ";
		for (auto k : i.first) {
			cout << k << " ";
		}
		cout << ">   解的适应度：" << i.second << " ";
		cout << endl;
	}
}
void printVec(const vector<pair<vector<int>, int>>& adapt, const vector<vector<int>>& solution) {
	for (int i = 0; i < solution.size(); i++) {
		cout << "< ";
		for (auto k : solution[i]) {
			cout << k << " ";
		}
		cout << ">   解的适应度: " << adapt[i].second << endl;
	}
}
void printVec(const vector<vector<double>>& temp) {
	cout.setf(ios::showpoint); //将小数精度后面的0显示出来
	cout.precision(8); //设置输出精度，保留有效数字

	for (auto i : temp) {
		for (auto j : i) {
			if (j == 1) {
				cout << "1.00000000" << " ";
			}
			else {
				cout << j << " ";
			}
		}
		cout << endl;
	}
}
void printVec(const vector<vector<int>>& temp) {
	for (auto i : temp) {
		for (auto j : i) {
			cout << j << " ";
		}
		cout << endl;
	}
}
void generateFirst(vector<vector<int>>& solutionVec, const vector<vector<int>>& citVec, int M, int N) {
	srand(time(NULL));
	for (int i = 0; i < M; ++i) {
		vector<int> temp;
		int firstNumber = rand() % N;
		temp.push_back(firstNumber);
		while (temp.size() < N) {
			int cityNumber = min(citVec[firstNumber], temp);
			temp.push_back(cityNumber);
			firstNumber = cityNumber;
		}
		solutionVec[i] = temp;
	}
}
int min(const vector<int>& citVec, const vector<int>& temp) {
	int minlength = 10000;
	int number = 0;
	for (int i = 0; i < citVec.size(); ++i) {
		if (find(temp.begin(), temp.end(), i) != temp.end()) {
			continue;
		}
		if (minlength > citVec[i]) {
			minlength = citVec[i];
			number = i;
		}
	}
	return number;
}

bool cmp(const pair<vector<int>, int>& a, const pair<vector<int>, int>& b) {
	//排序用的临时算法
	return a.second < b.second;
}
void caculateAdaptability(const vector<vector<int>>& solutionVec, const vector<vector<int>>& cityAndLengthVec, vector<pair<vector<int>, int>>& adaptabilityPairVec) {
	//对当前解空间中的每组解计算适应度并按升序排序
	//排序后存储在一个pair构成的vector中，存储格式为<原解ID，适应度>
	for (int i = 0; i < solutionVec.size(); ++i) {
		int adapt = cityAndLengthVec[solutionVec[i][0]][solutionVec[i][solutionVec[i].size() - 1]];
		for (int j = 0; j < solutionVec[i].size() - 1; ++j) {
			adapt = adapt + cityAndLengthVec[solutionVec[i][j]][solutionVec[i][j + 1]];
		}
		adaptabilityPairVec[i] = pair<vector<int>, int>(solutionVec[i], adapt);
	}
	sort(adaptabilityPairVec.begin(), adaptabilityPairVec.end(), cmp);
}
void getBetterSolution(int N, const vector<vector<int>>& solutionVec, vector<vector<int>>& betterSolutionVec, const vector<pair<vector<int>, int>>& adaptabilityPairVec) {
	//按照排序后的适应度Vector中的解ID，选择解空间中的前一半
	int i = 0;
	for (auto pair : adaptabilityPairVec) {
		if (i == adaptabilityPairVec.size() / 3) {
			break;
		}
		betterSolutionVec[i] = adaptabilityPairVec[i].first;
		++i;
	}
}
void count(const vector<vector<int>>& betterSolutionVec, vector<vector<int>>& countVec) {
	//根据选优后的解空间对每一个城市到其他城市的转移次数进行统计
	for (auto solution : betterSolutionVec) {
		for (int i = 0; i < solution.size() - 1; ++i) {
			++countVec[solution[i]][solution[i + 1]];
		}
	}
}
void calculate(const vector<vector<int>>& countVec, vector<vector<double>>& PVec, const int N) {
	//根据转移次数矩阵计算每一个城市到其他城市的转移概率矩阵
	vector<int>temp;
	for (auto i : countVec) {
		int sum = 0;
		for (auto j : i) {
			sum = sum + j;
		}
		temp.push_back(sum);
	}
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			PVec[i][j] = double(countVec[i][j]) / double(temp[i]);
		}
	}
}
void lunpandu(vector<vector<double>>& PVec, const int N) {
	//对转移概率矩阵进行轮盘赌操作，方便后续生成完整解空间
	for (int i = 0; i < N; ++i) {
		double temp = 0;
		for (int j = 0; j < N; ++j) {
			double temp2 = PVec[i][j];
			PVec[i][j] = PVec[i][j] + temp;
			temp = temp + temp2;
		}
	}
}

void generateSolution(vector<vector<int>>& solutionVec, const vector<vector<double>>& PVec, const int N, const int M) {
	//                        完整解空间VEC                          转移概率VEC                城市数量    解空间大小
	//根据每个城市到其他城市的转移概率矩阵生成达到整体的矩阵   
	srand(time(NULL));
	for (int i = 0; i < M; ++i) {
		vector<int> vals;
		int CityNumber = rand() % N;
		vals.push_back(CityNumber);
		//cout << CityNumber << " ";
		while (vals.size() < N) {
			double temp = double(rand() % 100) / (double)100;
			//cout << "<" << temp << ",";
			for (int k = 0; k < PVec[CityNumber].size(); ++k) {
				if (temp < PVec[CityNumber][k]) {
					if (find(vals.begin(), vals.end(), k) == vals.end()) {
						//cout<<CityNumber <<","<<k<< "> ";
						vals.push_back(k);
						CityNumber = k;
						k = PVec[CityNumber].size();
					}
					else {
						bool flag = false;
						while (find(vals.begin(), vals.end(), k) != vals.end()) {
							if (k == PVec[CityNumber].size() - 1 && find(vals.begin(), vals.end(), k) != vals.end()) {
								flag = true;
								break;
							}
							if (k == PVec[CityNumber].size() - 1) {
								break;
							}
							else {
								k = k + 1;
							}
						}
						if (flag == true) {
							while (find(vals.begin(), vals.end(), k) != vals.end()) {
								k = k - 1;
							}
						}
						//cout << CityNumber << "," << k << "> ";
						vals.push_back(k);
						CityNumber = k;
						k = PVec[CityNumber].size();
					}
				}
			}
		}
		//cout << endl;
		solutionVec[i] = vals;
	}
}


void generateSolution2(vector<vector<int>>& solutionVec, const vector<vector<double>>& PVec, const int N, const int M) {
	srand(time(NULL));
	for (int i = 0; i < M; ++i) {
		vector<int> temp;
		int firstNumber = rand() % N;
		temp.push_back(firstNumber);
		while (temp.size() < N) {
			double p = double(rand() % 100) / (double)100;
			int cityNumber = findCity(PVec[firstNumber], temp,p);
			temp.push_back(cityNumber);
			firstNumber = cityNumber;
		}
		solutionVec[i] = temp;
	}
}
int findCity(const vector<double>& PVec, const vector<int> &temp,const double p) {
	for (int i = 0; i < PVec.size(); ++i) {
		if (find(temp.begin(), temp.end(), i) != temp.end()) {
			continue;
		}
		if (p > PVec[i]) {
			return i;
		}
	}
	for (int i = PVec.size()-1; i >=0; --i) {
		if (find(temp.begin(), temp.end(), i) != temp.end()) {
			continue;
		}
		if (p > PVec[i]) {
			return i;
		}
	}
}


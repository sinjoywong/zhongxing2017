#include "deploy.h"
#include "includes.h"
using namespace std;
int NodeNum_Network, LinkNum, NodeNum_Blue, LinkNum_Blue, LinkNum_Red;
double **LinkUnitPrice;//为了加权更加自由，在此处使用double型
int **LinkUnitPriceReal;//因为是通过修改单价的邻接矩阵来寻找路径，所以需要一个原本的单价邻接矩阵来计算实际花费
double **LinkGreen, **LinkRed;//初始化为零，在绿色、红色路径的邻接矩阵中赋值相应权值
double *NodeGreen;//初始化为零，在绿色节点的位置赋值权值

Path **allPath;
extern int generation;      //世代数
extern int best_index, worst_index;
extern int functionmode;
extern int popsize;        //种群大小
extern int maxgeneration;  //最大世代数
extern double pc;          //交叉率
extern double pm;          //变异率
extern struct individual bestindividual, worstindividual, currentbest;  //最佳个体
extern struct individual population[POPSIZE];
extern int NodeNumLimit, NodeGreenLimit, LinkGreenLimit, NodeRedLimit;
double time_length;
clock_t start, finish;

double  Weight_GreenLink, Weight_GreenNode, Weight_RedLink;
int NodeStart, NodeEnd;
int realCost;
//----main-----
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename) {
	//加入计时器，结尾部分在performevolution()中
	start = clock();
	finish = start;
	get_split_number(topo, line_num);
	input();
	Floyd(LinkUnitPrice, NodeNum_Network, LinkNum);
	generation = 0;
	generateinitialpopulation();
	evaluatepopulation();
	//对时间和进化数目的双重控制，若不满足两个条件的任意一种则跳出进化
	//	while (time_length < 85) {
	while (generation < maxgeneration) {
		generation++;
		//if (generation >= maxgeneration || time_length > 85) {
		//	goto  jumpout;
		//}
		//加入计时器
		//			clock_t finish2 = clock();

		generatenextpopulation();
		evaluatepopulation();
		//	performevolution();
		outputtextreport();  //for debug
	}
	//	}
	//jumpout:
	printf("最大函数值等于：%f ", currentbest.fitness);
	//计算真实花费
	realCost = getRealCost();
	//cout << "real Cost:" << realCost << endl;

	for (unsigned int i = 0; i < currentbest.chrom.size(); i++) {
		cout << currentbest.chrom[i] << "->";
	}
	cout << "Real Cost:" << realCost << endl;

	//---output---
	string outString;
	char charTemp[20];
	outString += "Cost:";
	//_itoa(realCost, charTemp, 10);
	sprintf(charTemp, "%d", realCost);
	outString += charTemp;
	outString += '\n';

	outString += "PathLength:";
	//_itoa(population[best_index].chrom.size(), charTemp, 10);
	sprintf(charTemp, "%d", (int)currentbest.chrom.size());
	outString = outString + charTemp + "\n";

	outString += "Path:";
	for (unsigned int i = 0; i < currentbest.chrom.size(); i++) {
		//_itoa(population[best_index].chrom[i], charTemp, 10);
		sprintf(charTemp, "%d", currentbest.chrom[i]);
		outString = outString + charTemp + ' ';
	}
	const char *topo_file;
	topo_file = outString.c_str();
	write_result(topo_file, filename);
	//system("pause");
}
//----------------main end------------------------
void get_split_number(char * topo[MAX_EDGE_NUM], int line_num) {
	vector<int> data;
	vector<int> vec_link;
	vector<int> vec_greenlink;
	vector<int> vec_greenNode;
	vector<int> vec_redlink;
	string temp;
	char *test = topo[0];

	//获得第一行信息
	for (int i = 0; test[i] != '\n'; i++) {
		if ((test[i + 1] == ' ') || (test[i + 1] == '\n')) {
			temp += test[i];
			data.push_back(atoi(temp.c_str()));
			temp = "";
		}
		else {
			temp += test[i];
		}
	}
	NodeNumLimit = data[0];
	NodeGreenLimit = data[1];
	LinkGreenLimit = data[2];
	NodeRedLimit = data[3];
	Weight_GreenNode = 2000 * NodeGreenLimit;
	Weight_GreenLink = 4000 * LinkGreenLimit;
	Weight_RedLink = -10000 * NodeRedLimit;

	data.clear();
	test = topo[2];
	for (int i = 0; test[i] != '\n'; i++) {
		if ((test[i + 1] == ' ') || (test[i + 1] == '\n')) {
			temp += test[i];
			data.push_back(atoi(temp.c_str()));
			temp = "";
		}
		else {
			temp += test[i];
		}
	}

	NodeNum_Network = data[0];
	LinkNum = data[1];
	NodeNum_Blue = data[2];
	LinkNum_Blue = data[3];
	LinkNum_Red = data[4];

	//Allocate
	LinkUnitPrice = new double *[NodeNum_Network];
	LinkUnitPriceReal = new int  *[NodeNum_Network];
	NodeGreen = new double[NodeNum_Network];

	LinkGreen = new double *[NodeNum_Network];
	LinkRed = new double *[NodeNum_Network];
	for (int i = 0; i < NodeNum_Network; i++) {
		LinkUnitPrice[i] = new double[NodeNum_Network];
		LinkUnitPriceReal[i] = new int[NodeNum_Network];
		LinkGreen[i] = new double[NodeNum_Network];
		LinkRed[i] = new double[NodeNum_Network];
		memset(LinkUnitPrice[i], 0, NodeNum_Network * sizeof(double));
		memset(LinkUnitPriceReal[i], 0, NodeNum_Network * sizeof(int));
		memset(LinkGreen[i], 0, NodeNum_Network * sizeof(double));
		memset(LinkRed[i], 0, NodeNum_Network * sizeof(double));
	}
	memset(NodeGreen, 0, NodeNum_Network * sizeof(double));//将绿色节点的编号初始化为零，在后边的相应出现的绿色节点的位置赋给权重
														   //初始化这两个矩阵 使得不直接连通的线路单价为100000
	for (int i = 0; i < NodeNum_Network; i++) {
		for (int j = 0; j < NodeNum_Network; j++) {
			LinkUnitPrice[i][j] = 100000;
			LinkUnitPriceReal[i][j] = 100000;
		}
	}
	// 起点及终点的节点编号
	NodeStart = 0;
	NodeEnd = NodeNum_Network - 1;
	//Get link information
	//前2行格式固定,链路信息从第3行开始数
	//第二获得从第3行开始到拓扑链路信息结束
	data.clear();
	for (int j = 4; j < LinkNum + 5; j++) {
		char *test = topo[j];
		if (test[0] != '\n') {
			for (int i = 0; test[i] != '\n'; i++) {
				if ((test[i + 1] == ' ') || (test[i + 1] == '\n')) {
					temp += test[i];
					vec_link.push_back(atoi(temp.c_str()));
					temp = "";
				}
				else {
					temp += test[i];
				}
			}
		}
	}

	for (unsigned int i = 0; i != vec_link.size() / 3; i++) {
		LinkUnitPrice[vec_link[3 * i]][vec_link[3 * i + 1]] = vec_link[3 * i + 2];
		LinkUnitPriceReal[vec_link[3 * i]][vec_link[3 * i + 1]] = vec_link[3 * i + 2];
	}

	for (int i = 0; i < NodeNum_Network; i++) {
		for (int j = i; j < NodeNum_Network; j++) {
			LinkUnitPrice[j][i] = LinkUnitPrice[i][j];
			LinkUnitPriceReal[j][i] = LinkUnitPriceReal[i][j];
		}
	}
	//第三，greenNode 
	data.clear();
	for (int j = LinkNum + 6; j< LinkNum + 6 + NodeNum_Blue; j++) {
		char *test = topo[j];
		for (int i = 0; test[i] != '\0'; i++) {
			if ((test[i + 1] == ' ') || (test[i + 1] == '\0')) {
				temp += test[i];
				vec_greenNode.push_back(atoi(temp.c_str()));
				temp = "";
			}
			else {
				temp += test[i];
			}
		}
	}
	for (unsigned int i = 0; i < vec_greenNode.size(); i++) {
		NodeGreen[vec_greenNode[i]] = Weight_GreenNode;
	}
	//第4：获得greenLink

	for (int j = LinkNum + 7 + NodeNum_Blue; j< LinkNum + 7 + NodeNum_Blue + LinkNum_Blue; j++) {
		char *test = topo[j];
		for (int i = 0; test[i] != '\0'; i++) {
			if ((test[i + 1] == ' ') || (test[i + 1] == '\0')) {
				temp += test[i];
				vec_greenlink.push_back(atoi(temp.c_str()));
				temp = "";
			}
			else {
				temp += test[i];
			}
		}
	}
	for (unsigned int i = 0; i != vec_greenlink.size() / 2; i++) {
		LinkGreen[vec_greenlink[2 * i]][vec_greenlink[2 * i + 1]] = Weight_GreenLink;
		LinkGreen[vec_greenlink[2 * i + 1]][vec_greenlink[2 * i]] = Weight_GreenLink;
	}

	//第5,redLink
	for (int j = LinkNum + 8 + NodeNum_Blue + LinkNum_Blue; j< line_num; j++) {
		char *test = topo[j];
		for (int i = 0; test[i] != '\0'; i++) {
			if ((test[i + 1] == ' ') || (test[i + 1] == '\0')) {
				temp += test[i];
				vec_redlink.push_back(atoi(temp.c_str()));
				temp = "";
			}
			else {
				temp += test[i];
			}
		}
	}
	for (unsigned int i = 0; i != vec_redlink.size() / 2; i++) {
		LinkRed[vec_redlink[2 * i]][vec_redlink[2 * i + 1]] = Weight_RedLink;
		LinkRed[vec_redlink[2 * i + 1]][vec_redlink[2 * i]] = Weight_RedLink;
	}
}
void Deallocate_Arrays() {
	for (int i = 0; i < NodeNum_Network; i++) {
		delete[] LinkUnitPrice[i];
		delete[] LinkUnitPriceReal[i];
		delete[] LinkGreen[i];
		LinkUnitPrice[i] = NULL;
		allPath[i] = NULL;
		LinkUnitPriceReal[i] = NULL;
		LinkGreen[i] = NULL;
	}
	delete[] LinkUnitPrice;
	delete[] allPath;
	delete[] NodeGreen;
	delete[] LinkGreen;
	LinkUnitPrice = NULL;
	allPath = NULL;
	NodeGreen = NULL;
	LinkGreen = NULL;
	delete[] LinkUnitPriceReal;
	LinkUnitPriceReal = NULL;
}

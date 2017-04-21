#include "deploy.h"
#include "includes.h"
using namespace std;
int NodeNum_Network, NodeNum_Demand, LinkNum, ServerUnitPrice;
int **LinkMax;
int **LinkUnitPrice;
int** DemandNodeUseble; //选出查找路径时选择的节点数
int **DemandMax;
Path **allPath;
vector<vector <int> > ServerNum(POPSIZE);//返回的服务器编号,4行即4个染色体,每行为染色体中1的位置
vector<number> vn;//网络节点中出现次数最多的集合,vn[1].value vn[2].value ...
int ServerInitbyHand[1000];//题设网络节点不超过1000个,故假定服务器节点不超过300个
success sucessFinish;
extern int generation;      //世代数
extern int best_index, worst_index;
extern int functionmode;
extern int popsize;        //种群大小
extern int maxgeneration;  //最大世代数
extern double pc;          //交叉率
extern double pm;          //变异率
extern struct individual bestindividual, worstindividual, currentbest;  //最佳个体
extern struct individual population[POPSIZE];
extern int * yuanNode;
extern int **LinkMaxTemp, **LinkUnitPriceTemp, **belong, **DemandMaxTemp;
extern Path **allPathTemp;
//vector <int> ServerNum;//返回的服务器编号,4行即4个染色体,每行为染色体中1的位置
double time_length;
clock_t start, finish;

//----main-----
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename) {

	//加入计时器，结尾部分在performevolution()中
	start = clock();
	finish = start;

	get_split_number(topo, line_num);
	Allocate_result();
	Allocate_Arrays();
	Floyd(NodeNum_Network, LinkNum);

	sucessFinish.successAllCost = 10000000;
	//memset(sucessFinish.successPath, 0, sizeof(success));//初始化

	generation = 0;
	input();
	generateinitialpopulation();
	evaluatepopulation();
	//对时间和进化数目的双重控制，若不满足两个条件的任意一种则跳出进化
	while (time_length < 85) {
		while (generation < maxgeneration) {
			generation++;
			if (generation >= maxgeneration || time_length > 85) {
				goto  jumpout;
			}

			//加入计时器
			clock_t finish2 = clock();
			double time_length2 = (double)(finish2 - start) / CLOCKS_PER_SEC;
			std::cout << "generation:" << generation << " time_length2=" << time_length2 << " Cost:" << sucessFinish.successAllCost<< std::endl;

			generatenextpopulation();
			evaluatepopulation();
			performevolution();
			outputtextreport();  //for debug
		}
	}
jumpout:
	printf("最大函数值等于：%f\n", currentbest.fitness);

	string outString;
	char charTemp[20];
	//sprintf(charTemp, "%d", sucessFinish.successPathNum);
	_itoa(sucessFinish.successPathNum, charTemp, 10);
	outString = outString + charTemp + "\n\n";
	for (int i = 0; i < sucessFinish.successPathNum; i++) {
		for (int j = 0; j < sucessFinish.successPath[i].pathLenght; j++) {
	//		sprintf(charTemp, "%d", sucessFinish.successPath[i].path[j]);
			_itoa(sucessFinish.successPath[i].path[j], charTemp, 10);
			outString = outString + charTemp + ' ';
		}
//		sprintf(charTemp, "%d", sucessFinish.successPath[i].pathWitch);
			_itoa(sucessFinish.successPath[i].pathWitch, charTemp, 10);
		outString += charTemp;
		outString += '\n';
	}

	//for debug,输出总花费到文件
	
	//sprintf(charTemp, "%d", sucessFinish.successAllCost);
	_itoa(sucessFinish.successAllCost,charTemp, 10);
	outString += charTemp;
	outString += '\n';

	int allPrice = NodeNum_Demand* ServerUnitPrice;
	_itoa(allPrice, charTemp, 10);
	outString += charTemp;
	//debug end
	
	//不需要这个
	//   outString.pop_back();//write_result()中输出了一个换行符，因此这里删去一个换行符
	const char *topo_file;
	topo_file = outString.c_str();
	write_result(topo_file, filename);
	Deallocate_Arrays();
}

//----------------main end------------------------

void get_split_number(char * topo[MAX_EDGE_NUM], int line_num) {
	vector<int> data;
	vector<int> vec_link;
	vector<int> vec_demand;
	vector<int> vec_init;//用于存储所有网络节点的起点和终点,用于判断连接数最多的点作为染色体的初始基因型
	string temp;
	//Get NodeNum_Net,LinkNum,NodeNum_Demand,ServerUnitPrice

	char *test = topo[0];
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
	NodeNum_Demand = data[2];
	ServerUnitPrice = data[3];
	//Get link information
	//前三行格式固定,链路信息从第4行开始数
	for (int j = 4; j < LinkNum + 4; j++) {
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
	//Allocate space for **LinkMax and **LinkUnitPrice
	LinkMax = new int *[NodeNum_Network];
	LinkUnitPrice = new int *[NodeNum_Network];
	for (int i = 0; i < NodeNum_Network; i++) {
		LinkMax[i] = new int[NodeNum_Network];
		LinkUnitPrice[i] = new int[NodeNum_Network];
		memset(LinkMax[i], 0, NodeNum_Network * sizeof(int));
		memset(LinkUnitPrice[i], 0, NodeNum_Network * sizeof(int));
	}

	//初始化这两个矩阵 使得不直接连通的线路单价为100000 如果i=j则处理方式为之后更换路线做准备
	for (int i = 0; i < NodeNum_Network; i++) {
		for (int j = 0; j < NodeNum_Network; j++) {
			LinkUnitPrice[i][j] = 100000;
		}
	}

	//Allocate finished
	for (int i = 0; i != vec_link.size() / 4; i++) {
		// Assign link information to matrix
		LinkMax[vec_link[4 * i]][vec_link[4 * i + 1]] = vec_link[4 * i + 2];
		LinkUnitPrice[vec_link[4 * i]][vec_link[4 * i + 1]] = vec_link[4 * i + 3];
		vec_init.push_back(vec_link[4 * i]);//用于存储所有网络节点的起点和终点,用于判断连接数最多的点作为染色体的初始基因型
		vec_init.push_back(vec_link[4 * i + 1]);
	}


	//---------------------寻找vec_init中出现次数较多的几个节点作为初始染色体的基因型------------------------
	//todo:参考find_most_shown_vec.cpp
	for (unsigned int i = 0; i<vec_init.size(); i++) {
		int pos = find(vn, vec_init[i]);
		if (pos >= 0) {
			vn[pos].count++;
		}
		else {
			number n;
			n.value = vec_init[i];
			n.count = 1;
			vn.push_back(n);
		}
	}
	sort(vn.begin(), vn.end(), cmp);
	//todo: 将值赋给ServerInitbyHand[]
	for (int i = 0; i<1000; i++) {
		ServerInitbyHand[i] = 0;
	}
	for (int i = 0; i<NodeNum_Network; i++) {
		ServerInitbyHand[i] = vn[i].value;
	}
	for (int i = 0; i < NodeNum_Network; i++) {
		for (int j = i; j < NodeNum_Network; j++) {
			LinkMax[j][i] = LinkMax[i][j];
			LinkUnitPrice[j][i] = LinkUnitPrice[i][j];
		}
	}
	//Get demands information
	for (int j = LinkNum + 5; j< line_num; j++) {
		char *test = topo[j];
		for (int i = 0; test[i] != '\0'; i++) {
			if ((test[i + 1] == ' ') || (test[i + 1] == '\0')) {
				temp += test[i];
				vec_demand.push_back(atoi(temp.c_str()));
				temp = "";
			}
			else {
				temp += test[i];
			}
		}
	}
	DemandMax = new int *[NodeNum_Demand];
	for (int i = 0; i<NodeNum_Demand; i++) { DemandMax[i] = new int[2]; }
	for (int i = 0; i != vec_demand.size() / 3; i++) {
		DemandMax[i][0] = vec_demand[3 * i + 1];
		DemandMax[i][1] = vec_demand[3 * i + 2];
	}
	//remember to De-allocate arrays
}
void Allocate_Arrays() {

	bestindividual.chrom = new int[NodeNum_Network];
	worstindividual.chrom = new int[NodeNum_Network];
	currentbest.chrom = new int[NodeNum_Network];

	memset(bestindividual.chrom, 0, NodeNum_Network * sizeof(int));
	memset(worstindividual.chrom, 0, NodeNum_Network * sizeof(int));
	memset(currentbest.chrom, 0, NodeNum_Network * sizeof(int));

	for (int i = 0; i < POPSIZE; i++) {
		population[i].chrom = new int[NodeNum_Network];
		memset(population[i].chrom, 0, NodeNum_Network * sizeof(int));
	}


	DemandNodeUseble = new int*[NodeNum_Network];
	for (int i = 0; i < NodeNum_Network; i++) {
		DemandNodeUseble[i] = new int[DEMANDNODEUSBLENUM];
		memset(DemandNodeUseble[i], 0, DEMANDNODEUSBLENUM * sizeof(int));
	}
}

//------找出vector中出现次数最多的几个元素--------

int find(const vector<number> vec_init, int value) {
	for (int i = 0; i<vec_init.size(); i++) {
		if (vec_init[i].value == value)
			return i;
	}
	return -1;
}
bool cmp(const number &a, const number &b) {
	return a.count > b.count;
}
//--------------------------------

void Deallocate_Arrays()
{
	for (int i = 0; i < NodeNum_Network; i++) {
		delete[] LinkMax[i];
		delete[] LinkUnitPrice[i];
		delete[] allPath[i];
		delete[] DemandNodeUseble[i];
		LinkMax[i] = NULL;
		LinkUnitPrice[i] = NULL;
		allPath[i] = NULL;
		DemandNodeUseble[i] = NULL;


		delete[] LinkMaxTemp[i];
		delete[] LinkUnitPriceTemp[i];
		delete[] allPathTemp[i];


		LinkMaxTemp[i] = NULL;
		LinkUnitPriceTemp[i] = NULL;
		allPathTemp[i] = NULL;

	}
	delete[] LinkMax;
	delete[] LinkUnitPrice;
	delete[] allPath;
	delete[] DemandNodeUseble;

	LinkMax = NULL;
	LinkUnitPrice = NULL;
	allPath = NULL;
	DemandNodeUseble = NULL;

	delete[] LinkMaxTemp;
	delete[] LinkUnitPriceTemp;
	delete[] allPathTemp;

	LinkMaxTemp = NULL;
	LinkUnitPriceTemp = NULL;
	allPathTemp = NULL;


	for (int i = 0; i < NodeNum_Demand; i++) {
		delete[] DemandMax[i];
		DemandMax[i] = NULL;
	}
	delete[] DemandMax;
	DemandMax = NULL;

	for (int i = 0; i<MAX_VERTEX_COUNT; i++) {
		delete[] belong[i];
		delete[] DemandMaxTemp[i];
	}
	delete[] belong;
	delete[] DemandMaxTemp;

	belong = NULL;
	DemandMaxTemp = NULL;
}


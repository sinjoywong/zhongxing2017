#include "deploy.h"
#include "includes.h"
using namespace std;
int NodeNum_Network, LinkNum, NodeNum_Blue, LinkNum_Blue, LinkNum_Red;
int **LinkUnitPrice,**LinkUnitPrice_Ori;//因为是通过修改单价的邻接矩阵来寻找路径，所以需要一个原本的单价邻接矩阵来计算实际花费
int **LinkGreen;
int **LinkRed;

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
extern Path **allPathTemp;
double time_length;
clock_t start, finish;

vector<int> vec_greenNode;

int Weight_Greenlink;
int NodeStart, NodeEnd;
//----main-----
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename) {
	//加入计时器，结尾部分在performevolution()中
	start = clock();
	finish = start;

	get_split_number(topo, line_num);
	//------ Weight setting---------------
	Weight_Greenlink = 1;
	//--for green link
	LinkUnitPrice[2][4] = 0.1;
	LinkUnitPrice[4][2] = 0.1;
	LinkUnitPrice[14][13] = 0.1;
	LinkUnitPrice[13][14] = 0.1;
	//--for green node
	LinkUnitPrice[3][7] = 0.1;
	LinkUnitPrice[7][3] = 0.1;
	LinkUnitPrice[6][7] = 0.1;
	LinkUnitPrice[7][6] = 0.1;
	LinkUnitPrice[8][7] = 0.1;
	LinkUnitPrice[7][8] = 0.1;
	// for red link
	LinkUnitPrice[11][12] = 999;
	LinkUnitPrice[12][11] = 999;
//----------Weight setting end--------------	
	 
	Allocate_result();

	Floyd(NodeNum_Network, LinkNum);

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
		//	std::cout << "generation:" << generation << " time_length2=" << time_length2 << " Cost:" << sucessFinish.successAllCost<< std::endl;

			generatenextpopulation();
			evaluatepopulation();
			performevolution();
			outputtextreport();  //for debug
		}
	}
jumpout:
	printf("最大函数值等于：%f\n", currentbest.fitness);
	/*
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
	*/
	Deallocate_Arrays();
}

//----------------main end------------------------

void get_split_number(char * topo[MAX_EDGE_NUM], int line_num) {
	vector<int> data;
	vector<int> vec_link;
	vector<int> vec_greenlink;
	vector<int> vec_rednode;
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
// 起点及终点的节点编号
	NodeStart = 0;
	NodeEnd = NodeNum_Network - 1;
	//Get link information
	//前2行格式固定,链路信息从第3行开始数
	//第二获得从第3行开始到拓扑链路信息结束
	for (int j = 2; j < LinkNum + 3; j++) {
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
	LinkUnitPrice = new int *[NodeNum_Network];
	LinkUnitPrice_Ori = new int *[NodeNum_Network];
	for (int i = 0; i < NodeNum_Network; i++) {
		LinkUnitPrice[i] = new int[NodeNum_Network];
		LinkUnitPrice_Ori[i] = new int[NodeNum_Network];
		memset(LinkUnitPrice[i], 0, NodeNum_Network * sizeof(int));
		memset(LinkUnitPrice_Ori[i], 0, NodeNum_Network * sizeof(int));
	}
	//初始化这两个矩阵 使得不直接连通的线路单价为100000 如果i=j则处理方式为之后更换路线做准备
	for (int i = 0; i < NodeNum_Network; i++) {
		for (int j = 0; j < NodeNum_Network; j++) {
			LinkUnitPrice[i][j] = 1000;
			LinkUnitPrice_Ori[i][j] = 1000;
		}
	}
	for (int i = 0; i != vec_link.size() / 3; i++) {
		LinkUnitPrice[vec_link[3 * i]][vec_link[3 * i + 1]] = vec_link[3 * i + 2];
		LinkUnitPrice_Ori[vec_link[3 * i]][vec_link[3 * i + 1]] = vec_link[3 * i + 2];
	}
/*
	for (int i = 0; i != vec_link.size() / 3; i++) {
		cout << vec_link[3 * i] << ' ' << vec_link[3 * i + 1]<< ' ' << LinkUnitPrice[vec_link[3 * i]][vec_link[3 * i + 1]] << endl;
	}
*/
	for (int i = 0; i < NodeNum_Network; i++) {
		for (int j = i; j < NodeNum_Network; j++) {
			LinkUnitPrice[j][i] = LinkUnitPrice[i][j];
		//	cout << LinkUnitPrice[i][j] << endl;
		}
	}
	//第三，Get green node information
	for (int j = LinkNum + 4; j< LinkNum + 4 + NodeNum_Blue; j++) {
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
	//第4：获得
	for (int j = LinkNum + 5 + NodeNum_Blue; j< LinkNum + 5 + NodeNum_Blue + LinkNum_Blue; j++) {
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

	LinkGreen = new int *[NodeNum_Network];
	for (int i = 0; i < NodeNum_Network; i++) {
		LinkGreen[i] = new int[NodeNum_Network];
		memset(LinkGreen[i], 0, NodeNum_Network * sizeof(int));
	}
	for (int i = 0; i != vec_greenlink.size() / 2; i++) {
		LinkGreen[vec_greenlink[2 * i]][vec_greenlink[2 * i + 1]] = Weight_Greenlink;
	}

	//第5
	for (int j = LinkNum +5  + NodeNum_Blue + LinkNum_Blue; j< line_num; j++) {
		char *test = topo[j];
		for (int i = 0; test[i] != '\0'; i++) {
			if ((test[i + 1] == ' ') || (test[i + 1] == '\0')) {
				temp += test[i];
				vec_rednode.push_back(atoi(temp.c_str()));
				temp = "";
			}
			else {
				temp += test[i];
			}
		}
	}
	//remember to De-allocate arrays
}

void Deallocate_Arrays() {
	for (int i = 0; i < NodeNum_Network; i++) {
		delete[] LinkUnitPrice[i];
		delete[] allPath[i];
		LinkUnitPrice[i] = NULL;
		allPath[i] = NULL;
		delete[] allPathTemp[i];
		allPathTemp[i] = NULL;
	}
	delete[] LinkUnitPrice;
	delete[] allPath;
	LinkUnitPrice = NULL;
	allPath = NULL;
	delete[] allPathTemp;
	allPathTemp = NULL;
	
}

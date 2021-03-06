#include "includes.h"
using namespace std;
int generation, maxgeneration;      //世代数,最大世代数
int best_index, worst_index;
double pc, pm;          //交叉率，变异率
struct individual bestindividual, worstindividual, currentbest;  //最佳个体,最差个体
struct individual population[POPSIZE];
int chromlength;
extern int NodeNum_Network, LinkNum;
extern double time_length;
extern clock_t start, finish;
clock_t finish3;
extern Path **allPath;
extern int NodeStart, NodeEnd;
extern int **LinkUnitPriceReal;//为加权自由，令LinkUnitPrice is double, let LinkUnitPriceReal is int
extern double Weight_GreenLink, Weight_GreenNode, Weight_RedLink;
extern double **LinkUnitPrice;
extern double **LinkGreen, **LinkRed;//初始化为零，在绿色、红色路径的邻接矩阵中赋值相应权值
extern double *NodeGreen;//初始化为零，在绿色节点的位置赋值权值
int NodeNumLimit, NodeGreenLimit, LinkGreenLimit, NodeRedLimit;

//void weightsetting() {
//------ Weight setting---------------
//	Weight_GreenNode = 2000 * NodeGreenLimit;
//	Weight_GreenLink = 4000 * LinkGreenLimit;
//	Weight_RedLink = -999 * NodeRedLimit;
//NodeNumLimit = 1;
//}
void input() {//数据输入
	printf("初始化全局变量:\n");
	//最大世代数
	maxgeneration = 100;
	//交叉率
	//pc = 0.4;
	pc = 0.4;
	//变异率
	//pm = 0.1;
	pm = 0.1;

	//为了计算方便，使得条路径花费乘以100
	for (int i = 0; i < NodeNum_Network; i++) {
		for (int j = 0; j < NodeNum_Network; j++) {
			LinkUnitPrice[i][j] = 100 * LinkUnitPrice[i][j];
		}
	}
}

void generateinitialpopulation() { //种群初始化
	srand((unsigned)time(NULL));
	int k1, k2, k3, k4;
	for (int i = 0; i < POPSIZE; i++) {
		int k = rand() % (NodeEnd - NodeStart - 1) + 1;
		do {
			k1 = rand() % (NodeEnd - NodeStart - 1) + 1;
			k2 = rand() % (NodeEnd - NodeStart - 1) + 1;
			k3 = rand() % (NodeEnd - NodeStart - 1) + 1;
			k4 = rand() % (NodeEnd - NodeStart - 1) + 1;
		} while (k1 == k || k2 == k || k3 == k || k1 == k2 || k1 == k3 || k2 == k3 || k == k4 || k1 == k4 || k2 == k4 || k3 == k4);

		//cout << "k:" << k << " k1:"<< k1 << endl; 
		for (int m = 0; m < allPath[NodeStart][k].pathLenght; m++) {
			population[i].chrom.push_back(allPath[NodeStart][k].path[m]);
		}

		for (int m = 1; m < allPath[k][k1].pathLenght; m++) {
			population[i].chrom.push_back(allPath[k][k1].path[m]);
		}
		for (int m = 1; m < allPath[k1][k2].pathLenght; m++) {
			population[i].chrom.push_back(allPath[k1][k2].path[m]);
		}
		for (int m = 1; m < allPath[k2][k3].pathLenght; m++) {
			population[i].chrom.push_back(allPath[k2][k3].path[m]);
		}
		for (int m = 1; m < allPath[k3][k4].pathLenght; m++) {
			population[i].chrom.push_back(allPath[k3][k4].path[m]);
		}
		//此处n从1开始，否则写入的染色体会将中间值k重复两遍
		for (int n = 1; n < allPath[k4][NodeEnd].pathLenght; n++) {
			population[i].chrom.push_back(allPath[k4][NodeEnd].path[n]);
		}
	}
	//displayChroms("Initialize,BeforeDeleteCycle");
	//
	deleteCloseCycles();	//去除环
							//displayChroms("Initialize,AfterDeleteCycles");
}
void generatenextpopulation() { //生成下一代
	selectoperator();
	//one_fourth_selectoperator();
	crossoveroperator();
	mutationoperator();
}
void evaluatepopulation() {  //评价个体，求最佳个体
	calculatefitnessvalue();
	findbestandworstindividual();
	/*
	for (int i = 0; i < POPSIZE; i++) {
	cout <<"Pop["<<i<<"]:"  << " Fit:" << population[i].fitness << " Chrom:";
	for (unsigned int ii = 0; ii <population[i].chrom.size(); ii++) {
	cout << population[i].chrom[ii] << " ";
	}
	cout << endl;
	}
	*/
	//displayChroms("evaluatePopulation");
}
void calculatefitnessvalue() { //计算函数值,
	int NodeNumPunish;
	for (int i = 0; i<POPSIZE; i++) {
		population[i].fitness = 0;
		if (population[i].chrom.size() > 9) {
			NodeNumPunish = 3000;
		}
		else {
			NodeNumPunish = 0;
		}
		for (unsigned int j = 0; j != population[i].chrom.size() - 1; j++) {
			population[i].fitness += (allPath[population[i].chrom[j]][population[i].chrom[j + 1]].pathCost
				- LinkGreen[population[i].chrom[j]][population[i].chrom[j + 1]]
				- NodeGreen[population[i].chrom[j]]
				- LinkRed[population[i].chrom[j]][population[i].chrom[j + 1]]
				+ NodeNumLimit * NodeNumPunish
				);
		}
		population[i].fitness = NodeNum_Network * 500 - (population[i].fitness);
	}
}
void findbestandworstindividual() { //求最佳个体和最差个体
	double sum = 0.0;
	bestindividual = population[0];
	//worstindividual = population[0];
	for (int i = 0; i<POPSIZE; i++) {
		if (population[i].fitness>bestindividual.fitness) {
			bestindividual = population[i];
			best_index = i;
		}
		/*
		else if (population[i].fitness<worstindividual.fitness) {
		worstindividual = population[i];
		worst_index = i;
		}
		*/
		sum += population[i].fitness;
	}
	if (generation == 0) {
		currentbest = bestindividual;
	}
	else {
		if (bestindividual.fitness >= currentbest.fitness) {
			currentbest = bestindividual;
		}
	}
	//cout << "!!!best index:" << best_index << endl;
}
void performevolution() {//演示评价结果
	if (bestindividual.fitness>currentbest.fitness) {
		currentbest = population[best_index];
	}
	else {
		population[worst_index] = currentbest;
	}
	//加入计时器
	//	finish = clock();
	//	time_length = (double)(finish - start) / CLOCKS_PER_SEC;
}
void selectoperator() {//比例选择算法
	int i, index;
	double p, sum = 0.0;
	double cfitness[POPSIZE];
	//	srand((unsigned)time(NULL));
	struct individual newpopulation[POPSIZE];
	for (i = 0; i<POPSIZE; i++) {
		sum += population[i].fitness;
	}
	for (i = 0; i<POPSIZE; i++) {
		cfitness[i] = population[i].fitness / sum;
	}
	for (i = 1; i<POPSIZE; i++) {
		cfitness[i] = cfitness[i - 1] + cfitness[i];
	}
	for (i = 0; i<POPSIZE; i++) {
		p = rand() % 1000 / 1000.0;
		index = 0;
		while (p>cfitness[index]) {
			index++;
		}
		newpopulation[i] = population[index];
	}
	for (i = 0; i<POPSIZE; i++) {
		population[i] = newpopulation[i];
	}
}

//----new selector,按适应度将每一代的染色体排序，保留前1/4的2倍、中间1/2的1倍，后1/4直接去除，提升算法的收敛速度。（参考文献：基于改进算法的应用研究）
void one_fourth_selectoperator() {
	struct individual newpopulation[POPSIZE];
	//BubbleSort(population);//从小到大排列
	//	displayChroms("before select");
	quickSort(population, 0, POPSIZE - 1);
	for (int i = 0; i < POPSIZE / 4; i++) {
		newpopulation[i] = population[i + 3 * POPSIZE / 4];
	}
	for (int i = POPSIZE / 4; i < 3 * POPSIZE / 4; i++) {
		newpopulation[i] = population[i];
	}
	for (int i = 3 * POPSIZE / 4; i < POPSIZE; i++) {
		newpopulation[i] = population[i];
	}
	for (int i = 0; i < POPSIZE; i++) {
		population[i] = newpopulation[i];
	}
	//	displayChroms("after select");
}
void swap(individual population[], int i, int j) {
	double tmp = population[i].fitness;
	population[i].fitness = population[j].fitness;
	population[j].fitness = tmp;
}
void BubbleSort(individual population[]) {
	for (int i = 0; i < POPSIZE - 1; i++) {
		for (int j = POPSIZE - 1; j > i; j--) {
			if (population[j].fitness < population[j - 1].fitness) {
				swap(population, j, j - 1);
			}
		}
	}
}
void quickSort(individual population[], int l, int r)
{
	if (l< r)
	{
		int i = l, j = r; double x = population[l].fitness;
		while (i < j)
		{
			while (i < j && population[j].fitness >= x) // 从右向左找第一个小于x的数  
				j--;
			if (i < j)
				population[i++].fitness = population[j].fitness;
			while (i < j && population[i].fitness< x) // 从左向右找第一个大于等于x的数  
				i++;
			if (i < j)
				population[j--].fitness = population[i].fitness;
		}
		population[i].fitness = x;
		quickSort(population, l, i - 1); // 递归调用  
		quickSort(population, i + 1, r);
	}
}
//-------new selector end--------------------------
void crossoveroperator() {//交叉算法
	int i;
	int index[POPSIZE];
	int point, temp;
	double p;
	for (i = 0; i<POPSIZE; i++) {
		index[i] = i;
	}
	for (i = 0; i<POPSIZE; i++) {
		point = rand() % (POPSIZE - i);
		temp = index[i];
		index[i] = index[point + i];
		index[point + i] = temp;
	}
	srand((unsigned)time(NULL));
	for (i = 0; i<POPSIZE - 1; i += 2) {
		p = rand() % 1000 / 1000.0;
		if (p<pc) {
			//cout << p << " " << pc << " cross start" << endl;
			point = rand() % (chromlength - 1) + 1;
			vector<int> repeatNode;
			for (unsigned int j = 0; j<population[index[i]].chrom.size(); j++) {
				for (unsigned int k = 0; k<population[index[i + 1]].chrom.size(); k++) {
					if (population[index[i]].chrom[j] == population[index[i + 1]].chrom[k]) {
						repeatNode.push_back(population[index[i]].chrom[j]);
						break;
					}
				}
			}
			if (repeatNode.size()>2) {             //当重复节点大于1的时候才进行交叉
				int firstNode = 0;                    //选择交叉的第一个点
				int secondNode = 0;                   //选择交叉的第二个点
				int crossoverPosition[4];             //记录重复节点出现在染色体中的位置
				for (int i = 0; i<4; i++) { crossoverPosition[i] = 0; }
				//int PositionTemp;                     //临时储存重复节点出现在染色体中的位置
				vector<int> chromTemp1;                //临时储存染色体
				vector<int> chromTemp2;                //临时储存染色体
				firstNode = repeatNode[rand() % repeatNode.size()];
				do {
					secondNode = repeatNode[rand() % repeatNode.size()];
				} while (secondNode == firstNode);

				for (unsigned int j = 0; j<population[index[i]].chrom.size(); j++) {
					if (population[index[i]].chrom[j] == firstNode) { crossoverPosition[0] = j; }
					if (population[index[i]].chrom[j] == secondNode) { crossoverPosition[1] = j; }
				}
				for (unsigned int j = 0; j<population[index[i + 1]].chrom.size(); j++) {
					if (population[index[i + 1]].chrom[j] == firstNode) { crossoverPosition[2] = j; }
					if (population[index[i + 1]].chrom[j] == secondNode) { crossoverPosition[3] = j; }
				}
				/*
				if (crossoverPosition[1]<crossoverPosition[0]) {
				PositionTemp = crossoverPosition[0];
				crossoverPosition[0] = crossoverPosition[1];
				crossoverPosition[1] = PositionTemp;
				}
				if (crossoverPosition[3]<crossoverPosition[2]) {
				PositionTemp = crossoverPosition[2];
				crossoverPosition[2] = crossoverPosition[3];
				crossoverPosition[3] = PositionTemp;
				}
				*/
				if ((crossoverPosition[1]<crossoverPosition[0]) == (crossoverPosition[3]<crossoverPosition[2]))
				{
					chromTemp1 = population[index[i]].chrom;
					population[index[i]].chrom.clear();

					//生成第一条
					for (int j = 0; j<crossoverPosition[0]; j++) {
						population[index[i]].chrom.push_back(chromTemp1[j]);
					}
					for (int k = crossoverPosition[2]; k <= crossoverPosition[3]; k++) {
						population[index[i]].chrom.push_back(population[index[i + 1]].chrom[k]);
					}
					for (unsigned int k = crossoverPosition[1] + 1; k<chromTemp1.size(); k++) {
						population[index[i]].chrom.push_back(chromTemp1[k]);
					}
					//生成第二条
					chromTemp2 = population[index[i + 1]].chrom;
					population[index[i + 1]].chrom.clear();
					for (int j = 0; j<crossoverPosition[2]; j++) {
						population[index[i + 1]].chrom.push_back(chromTemp2[j]);
					}
					for (int k = crossoverPosition[0]; k <= crossoverPosition[1]; k++) {
						population[index[i + 1]].chrom.push_back(chromTemp1[k]);
					}
					for (unsigned int k = crossoverPosition[3] + 1; k<chromTemp2.size(); k++) {
						population[index[i + 1]].chrom.push_back(chromTemp2[k]);
					}
				}
			}


		}
	}
	//for debug
	//	displayChroms("after cross");
	deleteCloseCycles();//Delete all closed cycles
}
void mutationoperator() {//变异操作
	double p; int k1, k2, k3;
	srand((unsigned)time(NULL));
	for (int i = 0; i<POPSIZE; i++) {
		p = rand() % 1000 / 1000.0;
		if (p < pm) {
			//	cout <<p << " "<< pm <<  " mutation start" << endl;
			//	srand((unsigned)time(NULL));
			int ks = rand() % (population[i].chrom.size() - 0 - 3) + 1;//(0,chrom.size()-3] ,此处是下标，并非节点编号本身
			int kt = ks + 2;
			//防止变异时所取的随机基因位与中间变量相同
			//	do {
			k1 = rand() % (NodeEnd - NodeStart - 1) + 1;
			k2 = rand() % (NodeEnd - NodeStart - 1) + 1;
			k3 = rand() % (NodeEnd - NodeStart - 1) + 1;
			//	} while ( ks == k1 || ks == k2 || kt ==k1 || kt ==k2 || k1==k2 || ks == k3 || kt == k3 || k3 == k1 || k3 == k2);
			population[i].chrom.erase(population[i].chrom.begin() + ks + 1, population[i].chrom.begin() + kt);//删除ks与kt之间的元素,保留ks，kt

																											  //displayChroms("--after erase");// --for debug
																											  //此处n从1开始，否则写入的染色体会将中间值k重复两遍
			kt = kt - 1;//erase后索引发生了变化，从ks开始减了1个;

						//k3-kt
			int Length_k3_kt = allPath[k3][population[i].chrom[kt]].pathLenght;
			//--for debug 
			/*
			cout << "k-kt:";
			for (int n = 0; n < Length_k_kt; n++) {
			cout << allPath[k][population[i].chrom[kt]].path[n] << " ";
			}
			cout << endl;
			*/
			//---debug end	
			for (int n = 1; n < Length_k3_kt; n++) {
				population[i].chrom.insert(population[i].chrom.begin() + kt,
					allPath[k3][population[i].chrom[kt]].path[Length_k3_kt - n - 1]);//将新的k-kt的路径写在kt前面
			}
			//k2-k3
			int Length_k2_k3 = allPath[k2][k3].pathLenght;
			//--for debug 
			/*
			cout << "k-kt:";
			for (int n = 0; n < Length_k_kt; n++) {
			cout << allPath[k][population[i].chrom[kt]].path[n] << " ";
			}
			cout << endl;
			*/
			//---debug end	
			for (int n = 1; n < Length_k2_k3; n++) {
				population[i].chrom.insert(population[i].chrom.begin() + kt,
					allPath[k2][k3].path[Length_k2_k3 - n - 1]);//将新的k-kt的路径写在kt前面
			}
			//k1-k2
			int Length_k1_k2 = allPath[k1][k2].pathLenght;
			//--for debug 
			/*
			cout << "k-kt:";
			for (int n = 0; n < Length_k_kt; n++) {
			cout << allPath[k][population[i].chrom[kt]].path[n] << " ";
			}
			cout << endl;
			*/
			//---debug end	
			for (int n = 1; n < Length_k1_k2; n++) {
				population[i].chrom.insert(population[i].chrom.begin() + kt,
					allPath[k1][k2].path[Length_k1_k2 - n - 1]);//将新的k-kt的路径写在kt前面
			}

			//ks-k1
			int Length_ks_k1 = allPath[population[i].chrom[ks]][k1].pathLenght;
			//---------for debug---
			/*
			cout << "ks-k:";
			for (int m = 0; m < Length_ks_k; m++) {//Length_ks_k-1是因为避免与原来基因位（erase左开右闭未删除）重复
			cout << allPath[population[i].chrom[ks]][k].path[m] << " ";
			}
			cout << endl;
			*/
			//---debug end---
			for (int m = 1; m < Length_ks_k1 - 1; m++) {//Length_ks_k-1是因为避免与原来基因位（erase左[右)未删除）重复
				population[i].chrom.insert(population[i].chrom.begin() + kt,
					allPath[population[i].chrom[ks]][k1].path[Length_ks_k1 - m - 1]);// 将新的ks-k的路径写在kt前面
			}
			//displayChroms("after mutation");//for debug
			//	deleteCloseCycles();//Delete all closed cycles
			//displayChroms("AfterMutationAndDeleteCycles:");
		}
	}
	deleteCloseCycles();//Delete all closed cycles
}
void outputtextreport() {//数据输出
	double sum;
	double average;
	sum = 0.0;
	for (int i = 0; i<POPSIZE; i++) {
		sum += population[i].fitness;
	}
	average = sum / POPSIZE;

	printf("当前世代=%d\n当前世代平均函数值=%f\n当前世代最高函数值=%f\n最佳个体=%d\n", generation, average, currentbest.fitness, best_index);
	//---for debug---
	/*	cout << "chrom:";
	for (unsigned int i = 0; i < currentbest.chrom.size(); i++) {
	cout <<currentbest.chrom[i] << "-->";
	}
	cout << endl;
	cout << endl;
	*/
	//---for debug end---
}
void deleteCloseCycles() {//去除环，形参为第i条染色体
	for (int i = 0; i < POPSIZE; i++) {
		unsigned int dupIndex_end;
		for (unsigned int ii = 0; ii < population[i].chrom.size(); ii++) {
			dupIndex_end = ii;
			for (unsigned int j = ii + 1; j < population[i].chrom.size(); j++) {
				if (population[i].chrom[ii] == population[i].chrom[j]) {
					dupIndex_end = j;
				}
			}
			if (dupIndex_end > ii) {
				population[i].chrom.erase(population[i].chrom.begin() + ii + 1, population[i].chrom.begin() + dupIndex_end + 1);
			}
		}
	}
}
void displayChroms(std::string debugName) {
	cout << "--For debug:" << debugName << endl;
	for (int i = 0; i < POPSIZE; i++) {
		for (unsigned int ii = 0; ii <population[i].chrom.size(); ii++) {
			cout << population[i].chrom[ii] << " ";
		}
		cout << endl;
	}
	cout << endl;
}
int getRealCost() {
	int costTmp = 0;
	for (unsigned int i = 0; i < currentbest.chrom.size() - 1; i++) {
		costTmp += LinkUnitPriceReal[currentbest.chrom[i]][currentbest.chrom[i + 1]];
	}
	return costTmp;
}

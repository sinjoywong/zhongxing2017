#include "includes.h"
using namespace std;
int generation, maxgeneration;      //世代数,最大世代数
int best_index, worst_index;
int functionmode = maximization;
double pc, pm;          //交叉率，变异率
struct individual bestindividual,worstindividual, currentbest;  //最佳个体,最差个体
struct individual population[POPSIZE];
int chromlength;
extern int NodeNum_Network, LinkNum;
extern double time_length;
extern clock_t start, finish;
clock_t finish3;
extern Path **allPath;
extern int NodeStart, NodeEnd;
void input() {//数据输入
	printf("初始化全局变量:\n");
	//种群大小includes.h
	//最大世代数(100-300)
	maxgeneration = 20;
	//交叉率(0.2-0.99)
	pc = 0.99;
	//变异率(0.001-0.1)
	pm = 1;
}
void generateinitialpopulation() { //种群初始化
	/*
		srand((unsigned)time(NULL));
	for (int i = 0; i < POPSIZE; i++) {
		//srand((unsigned)time(NULL));
		int k = rand() % (NodeEnd - NodeStart - 1) + 1;
		for (int m = 0; m < allPath[NodeStart][k].pathLenght; m++) {
			population[i].chrom.push_back(allPath[NodeStart][k].path[m]);
		}
		//此处n从1开始，否则写入的染色体会将中间值k重复两遍
		for (int n = 1; n < allPath[k][NodeEnd].pathLenght; n++) {
			population[i].chrom.push_back(allPath[k][NodeEnd].path[n]);
		}
		//去除环
		vector<int>::iterator iter = population[i].chrom.begin();
		while (iter != population[i].chrom.end()) {
			if (iter != find(population[i].chrom.begin(), iter, *iter)) {

			}
			iter++;
		}
		*/
		population[0].chrom.push_back(0);
		population[0].chrom.push_back(2);
		population[0].chrom.push_back(3);
		population[0].chrom.push_back(7);
		population[0].chrom.push_back(8);
		population[0].chrom.push_back(14);
		population[0].chrom.push_back(13);
		population[0].chrom.push_back(17);

		//---------for debug---
//		cout << "{OriginalPopulation[" << i << "]:" << "k=" << k << "} ";
		for (unsigned int ii = 0; ii != population[0].chrom.size(); ii++) {
			std::cout << population[0].chrom[ii] << " ";
		}
		std::cout << endl;
		//--------debug end----
//	}
}
void generatenextpopulation() { //生成下一代
	//selectoperator();
	//crossoveroperator();
	mutationoperator();
}
void evaluatepopulation() {  //评价个体，求最佳个体
	calculateobjectvalue();
	calculatefitnessvalue();
	findbestandworstindividual();
	for (int i = 0; i < POPSIZE; i++) {
	//cout << "population[" << i << "] cost:" << population[i].value << " fitness:" << population[i].fitness << endl;
	}
}
//todo:【还没有加入对路径点的约束】
void calculateobjectvalue() { //计算函数值,
	for (int i = 0; i<POPSIZE; i++) {
		for (unsigned int j = 0; j != population[i].chrom.size() - 1; j++) {
		population[i].value +=  allPath[population[i].chrom[j]][population[i].chrom[j+1]].pathCost ;
		}
	}
}
void calculatefitnessvalue() {//计算适应度
	double temp;
	for (int i = 0; i<POPSIZE; i++) {
		if (functionmode == maximization) {
			if ((population[i].value + cmin)>0.0) {
				temp = cmin + population[i].value;
			}
			else {
				temp = 0.0;
			}
		}
		else if (functionmode == minimization) {
			if (population[i].value<cmax) {
				temp = cmax - population[i].value;
			}
			else { temp = 0.0; }
		}
		population[i].fitness = temp;
		//population[i].fitness = population[i].value;
	}
}
void findbestandworstindividual() { //求最佳个体和最差个体
	double sum = 0.0;
	bestindividual = population[0];
	worstindividual = population[0];
	for (int i = 1; i<POPSIZE; i++) {
		if (population[i].fitness>bestindividual.fitness) {
			bestindividual = population[i];
			best_index = i;
		}
		else if (population[i].fitness<worstindividual.fitness) {
			worstindividual = population[i];
			worst_index = i;
		}
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
}
void performevolution() {//演示评价结果
	if (bestindividual.fitness>currentbest.fitness) {
		currentbest = population[best_index];
	}
	else {
		population[worst_index] = currentbest;
	}
	//加入计时器
	finish = clock();
	time_length = (double)(finish - start) / CLOCKS_PER_SEC;
	//  std::cout << "GA157:time_length=" << time_length << std::endl;
}
void selectoperator() {//比例选择算法
	int i, index;
	double p, sum = 0.0;
	double cfitness[POPSIZE];

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
void crossoveroperator() {//交叉算法
	int i, j;
	int index[POPSIZE];
	int point, temp;
	double p;
	char ch;
	for (i = 0; i<POPSIZE; i++) {
		index[i] = i;
	}
	for (i = 0; i<POPSIZE; i++) {
		point = rand() % (POPSIZE - i);
		temp = index[i];
		index[i] = index[point + i];
		index[point + i] = temp;
	}

	for (i = 0; i<POPSIZE - 1; i += 2) {
		p = rand() % 1000 / 1000.0;
		if (p<pc)
		{
			point = rand() % (chromlength - 1) + 1;
			vector<int> repeatNode;
			for (unsigned int j = 0; j<population[index[i]].chrom.size(); j++)
			{
				for (unsigned int k = 0; k<population[index[i + 1]].chrom.size(); k++)
				{
					if (population[index[i]].chrom[j] == population[index[i + 1]].chrom[k])
					{
						repeatNode.push_back(population[index[i]].chrom[j]);
						break;
					}
				}
			}
			if (repeatNode.size()>1)                   //当重复节点大于1的时候才进行交叉
			{
				int firstNode = 0;                    //选择交叉的第一个点
				int secondNode = 0;                   //选择交叉的第二个点
				int crossoverPosition[4];             //记录重复节点出现在染色体中的位置
				int PositionTemp;                     //临时储存重复节点出现在染色体中的位置
				vector<int> chromTemp1;                //临时储存染色体
				vector<int> chromTemp2;                //临时储存染色体
				firstNode = repeatNode[rand() % repeatNode.size()];
				do
				{
					secondNode = repeatNode[rand() % repeatNode.size()];
				} while (secondNode == firstNode);

				for (unsigned int j = 0; j<population[index[i]].chrom.size(); j++)
				{
					if (population[index[i]].chrom[j] == firstNode) { crossoverPosition[0] = j; }
					if (population[index[i]].chrom[j] == secondNode) { crossoverPosition[1] = j; }
				}

				for (unsigned int j = 0; j<population[index[i + 1]].chrom.size(); j++)
				{
					if (population[index[i + 1]].chrom[j] == firstNode) { crossoverPosition[2] = j; }
					if (population[index[i + 1]].chrom[j] == secondNode) { crossoverPosition[3] = j; }
				}
				if (crossoverPosition[1]<crossoverPosition[0])
				{
					PositionTemp = crossoverPosition[0];
					crossoverPosition[0] = crossoverPosition[1];
					crossoverPosition[1] = PositionTemp;
				}
				if (crossoverPosition[3]<crossoverPosition[2])
				{
					PositionTemp = crossoverPosition[2];
					crossoverPosition[2] = crossoverPosition[3];
					crossoverPosition[3] = PositionTemp;
				}
				chromTemp1 = population[index[i]].chrom;
				population[index[i]].chrom.clear();

				//生成第一条
				for (int j = 0; j<crossoverPosition[0]; j++)
				{
					population[index[i]].chrom.push_back(chromTemp1[j]);
				}
				for (int k = crossoverPosition[2]; k <= crossoverPosition[3]; k++)
				{
					population[index[i]].chrom.push_back(population[index[i + 1]].chrom[k]);
				}
				for (unsigned int k = crossoverPosition[1] + 1; k<chromTemp1.size(); k++)
				{
					population[index[i]].chrom.push_back(chromTemp1[k]);
				}
				//生成第二条
				chromTemp2 = population[index[i + 1]].chrom;
				population[index[i + 1]].chrom.clear();
				for (int j = 0; j<crossoverPosition[2]; j++)
				{
					population[index[i + 1]].chrom.push_back(chromTemp2[j]);
				}
				for (int k = crossoverPosition[0]; k <= crossoverPosition[1]; k++)
				{
					population[index[i + 1]].chrom.push_back(chromTemp1[k]);
				}
				for (unsigned int k = crossoverPosition[3] + 1; k<chromTemp2.size(); k++)
				{
					population[index[i + 1]].chrom.push_back(chromTemp2[k]);
				}
			}
		}
	}
}
void mutationoperator() {//变异操作
	double p;
	int tmp;
	bool swapped ;
	for (int i = 0; i<POPSIZE; i++) {
	
				srand((unsigned)time(NULL));
		//	p = rand() % 1000 / 1000.0;
			p = 0.1 ;
			if (p < pm) {
				//	srand((unsigned)time(NULL));
				int ks = rand() % (population[i].chrom.size() - 0 - 3) + 1;//(0,chrom.size()-3] ,此处是下标，并非节点编号本身
				int kt = ks + 2;
				int k = rand() % (NodeEnd - NodeStart - 1) + 1;//(NodeStart,NodeEnd)
				//-for debug
			//	ks = 1;
			//	k = 1;
			//	kt = 2;
				cout << "POP" << i << " chrom ks[" << ks << "]=" << population[i].chrom[ks]<<" " << "k=" << k << " " << " chrom kt[" << kt << "]=" << population[i].chrom[kt] << endl;

				population[i].chrom.erase(population[i].chrom.begin() + ks+1, population[i].chrom.begin() + kt);//删除ks与kt之间的元素,保留ks，kt
				cout << "after erase:";
				for (unsigned int i = 0; i < population[0].chrom.size(); i++) {
					cout << population[0].chrom[i] << " ";
				}
				cout << endl;

				//此处n从1开始，否则写入的染色体会将中间值k重复两遍
				kt = kt - 1;//erase后索引发生了变化，从ks开始减了1个;
				int Length_k_kt = allPath[k][population[i].chrom[kt]].pathLenght;

				//--for debug 
				cout << "k-kt:";
				for (int n = 0; n < Length_k_kt; n++) {
					cout << allPath[k][population[i].chrom[kt]].path[n] << " ";
				}
				cout << endl;
				//---debug end	

				for (int n = 1; n < Length_k_kt; n++) {
					population[i].chrom.insert(population[i].chrom.begin() + kt, 
						allPath[k][population[i].chrom[kt]].path[Length_k_kt - n-1]);//将新的k-kt的路径写在kt前面
				}

				int Length_ks_k = allPath[population[i].chrom[ks]][k].pathLenght;

				//---------for debug---
				cout << "ks-k:";
				for (int m = 0; m < Length_ks_k ; m++) {//Length_ks_k-1是因为避免与原来基因位（erase左开右闭未删除）重复
					cout << allPath[population[i].chrom[ks]][k].path[m] << " ";
				}
				cout << endl;
				//---debug end---

				for (int m =1 ; m < Length_ks_k -1; m++) {//Length_ks_k-1是因为避免与原来基因位（erase左开右闭未删除）重复
					population[i].chrom.insert(population[i].chrom.begin()+kt,
						allPath[population[i].chrom[ks]][k].path[Length_ks_k -m-1]);// 将新的ks-k的路径写在kt前面
				}
				
				//---for debug---
				cout << "AfterMutation:";
				for (unsigned int ii = 0; ii != population[i].chrom.size(); ii++) {
					 cout << population[i].chrom[ii] << " ";
				}
				cout << endl;
				//--------debug end----
			
			}
		}
	}

void outputtextreport() {//数据输出
	double sum;
	double average;
	sum = 0.0;
	for (int i = 0; i<POPSIZE; i++) {
		sum += population[i].value;
	}
	average = sum / POPSIZE;
	//printf("当前世代=%d\n当前世代平均函数值=%f\n当前世代最高函数值=%f\n", generation, average, population[best_index].value);
}
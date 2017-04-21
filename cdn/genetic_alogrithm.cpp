#include "includes.h"

int generation, maxgeneration;      //世代数,最大世代数
int best_index, worst_index;
int functionmode = maximization;
double pc, pm;          //交叉率，变异率
struct individual bestindividual,worstindividual, currentbest;  //最佳个体,最差个体
struct individual population[POPSIZE];
int chromlength;
extern int NodeNum_Network, NodeNum_Demand, LinkNum, ServerUnitPrice;
extern std::vector<number> vn;//网络节点中出现次数最多的集合,vn[1].value vn[2].value ...
extern double time_length;
extern clock_t start, finish;
clock_t finish3;

void input() {//数据输入
	printf("初始化全局变量:\n");
	//种群大小includes.h
	//最大世代数(100-300)
	maxgeneration = 20;
	//交叉率(0.2-0.99)
	pc = 0.99;
	//变异率(0.001-0.1)
	pm = 0.5;
}
void generateinitialpopulation() { //种群初始化
	//-----------改动----------
	/*
	for (i=0;i<POPSIZE/2; i++) {
	for(int k=0; k<NodeNum_Network/2;k++)
	{
	j = rand()%NodeNum_Network;
	population[i].chrom[j]=1;
	}
	//  population[i].chrom[chromlength]='\0';
	}
	*/
	for (int i = 0; i<POPSIZE / 2; i++) {
		for (int j = 0; j<NodeNum_Network; j++) {
			population[i].chrom[j] = (rand() % 10<8) ? 0 : 1;
		}
	}
	int j;
	for (int i = POPSIZE / 2; i<POPSIZE; i++) {
		for (int k = 0; k<NodeNum_Demand; k++) {
			//j = rand()%(NodeNum_Demand/2);
			//  j = rand()%(NodeNum_Network/3);
			j = rand() % (NodeNum_Demand);
			population[i].chrom[vn[j].value] = 1;
		}
	}
	/*
	for(int i=0;i<NodeNum_Network;i++) {
	population[POPSIZE - 1].chrom[i] = 1;
	}
	*/
}
void generatenextpopulation() { //生成下一代
	selectoperator();
	crossoveroperator();
	mutationoperator();
}
void evaluatepopulation() {  //评价个体，求最佳个体
	calculateobjectvalue();
	calculatefitnessvalue();
	findbestandworstindividual();
}
void calculateobjectvalue() { //计算函数值
	std::vector<std::vector <int> > Server(POPSIZE);//for debug 
	for (int i = 0; i<POPSIZE; i++) {
		population[i].value = result(i);
		//------------------------------for debug-----------------------------------------------
		for (int j = 0; j < NodeNum_Network; j++) {
			if (population[i].chrom[j] == 1) {
				Server[i].push_back(j);
			}
		}
	}
		for (int i = 0; i < POPSIZE; i++) {
			std::cout << "[" << i << "] ";
			for (int j = 0; j < Server.size(); j++) {
				std::cout << Server[i][j]<< " " ;
			}
			std::cout << std::endl;
		}
		//------------------------------debug end-----------------------
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
	for (i = 0; i<POPSIZE; i++)
	{
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
		if (p<pc) {
			point = rand() % (chromlength - 1) + 1;
			for (j = point; j<chromlength; j++) {
				ch = population[index[i]].chrom[j];
				population[index[i]].chrom[j] = population[index[i + 1]].chrom[j];
				population[index[i + 1]].chrom[j] = ch;
			}
		}
	}
}
void mutationoperator() {//变异操作
	double p;
	for (int i = 0; i<POPSIZE; i++) {
		for (int j = 0; j<chromlength; j++) {
			p = rand() % 1000 / 1000.0;
			if (p<pm) {
				//population[i].chrom[j]=(population[i].chrom[j]=='0')?'1':'0';
				population[i].chrom[j] = (population[i].chrom[j] == 0) ? 1 : 0;
			}
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
	printf("当前世代=%d\n当前世代平均函数值=%f\n当前世代最高函数值=%f\n", generation, average, population[best_index].value);
}
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
	for (int i = 0; i < POPSIZE; i++) {
		//srand((unsigned)time(NULL));
		int k = rand() % (17 - 1 - 0) + 1;
		cout << "k=" << k << endl;
		for (int m = 0; m < allPath[0][k].pathLenght; m++) {
			population[i].chrom.push_back(allPath[0][k].path[m]);
		}
		//此处n从1开始，否则写入的染色体会将中间值k重复两遍
		for (int n = 1; n < allPath[k][17].pathLenght; n++) {
			population[i].chrom.push_back(allPath[k][17].path[n]);
		}
		//---------for debug---
		for (unsigned int ii = 0; ii != population[i].chrom.size(); ii++) {
			cout << population[i].chrom[ii] << endl;
		}
		//--------debug end----
	}

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
	for (int i = 0; i < POPSIZE; i++) {
	cout << "population[" << i << "] cost:" << population[i].value << " fitness:" << population[i].fitness << endl;
	}
}
void calculateobjectvalue() { //计算函数值
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
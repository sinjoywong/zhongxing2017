#ifndef SDK_GCC_COPY_INCLUDES_H
#define SDK_GCC_COPY_INCLUDES_H
#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <stdio.h>
#include <algorithm>
#include "time.h"
#include "stdafx.h"
#define MAX_VALUE 1000
#define POPSIZE 200//population number

struct MGraph {
	double *edges[MAX_VALUE];
	int iVertexCount, iEdgeCount;
};
struct Path {
	int path[100];  //链路
	bool cross;       //是否连通
	//int pathCost;    //链路单价
	double  pathCost;    //链路单价
	int pathLenght;   //链路节点数
};
struct success {
	Path successPath[1000]; //暂定一百吧，之后可以改
	int successPathNum;
	double  successAllCost;
	int time;             //用来记录时间，发现时间不够就输出
};
struct individual {
	//int *chrom;
	std::vector<int> chrom;
	double value;//函数值
	double fitness;      //适应度
};

//-----Floyd
void PrintResult(const MGraph& mGraph, int **iArrPath);
void Floyd(double **LinkUnitPrice,int &NodeNum_Network, int &LinkNum);
void Deallocate_Arrays();
void generateinitialpopulation();
void generatenextpopulation();
void evaluatepopulation();
void calculatefitnessvalue();
void findbestandworstindividual();
void performevolution();
void selectoperator();
void crossoveroperator();
void mutationoperator();
void input();
void outputtextreport();
void deleteCloseCycles();
void displayChroms(std::string debugName);
double getRealCost(int **LinkUnitPriceReal, individual currentbest);
void weightsetting();
#endif //SDK_GCC_COPY_INCLUDES_H

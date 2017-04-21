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
#define MAX_VALUE 10000
#define MAX_VERTEX_COUNT 1000
#define RESULT_PATH_NUM 1000
#define POPSIZE 15 //population number
#define maximization 1
#define minimization 2
#define cmax 100
#define cmin 0

#define DEMANDNODEUSBLENUM 30  //定义循环时采用的节点数

struct MGraph {
	int *edges[MAX_VALUE];
	int iVertexCount, iEdageCount;
};
struct Path {
	int path[100];  //链路
	bool cross;       //是否连通
	int pathCost;    //链路单价
	int pathWitch;  //链路使用带宽
	int pathLenght;   //链路节点数
};
struct success {
	Path successPath[1000]; //暂定一百吧，之后可以改
	int successPathNum;
	int successAllCost;
	int time;             //用来记录时间，发现时间不够就输出
};
struct individual {
	int *chrom;
	double value;//函数值
	double fitness;      //适应度
};
//------找出vector中出现次数最多的几个元素--------
struct number {
	int value;
	int count;
};
int find(const std::vector<number> vec_init, int value);
bool cmp(const number &a, const number &b);
//-----Floyd
void PrintResult(const MGraph& mGraph, int **iArrPath);
void Floyd(int &NodeNum_Network, int &LinkNum);
void changePath(int &firstNode, int &secondNode, int **LinkMaxTemp, Path **allPathTemp, int **LinkUnitPriceTemp);
int pathMaxWitch(const Path& path);
int pathMaxWitch(const Path& path, int **LinkMaxTemp);
void Deallocate_Arrays();
void Print_to_file();
void Allocate_Arrays();
//int result(int& pop_no);
void Allocate_result();
void Deallocate_result();
double result(int& pop_no);

void generateinitialpopulation();
void generatenextpopulation();
void evaluatepopulation();
void calculateobjectvalue();
void calculatefitnessvalue();
void findbestandworstindividual();
void performevolution();
void selectoperator();
void crossoveroperator();
void mutationoperator();
void input();
void outputtextreport();
#endif //SDK_GCC_COPY_INCLUDES_H

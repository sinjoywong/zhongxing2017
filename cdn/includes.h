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
#define POPSIZE 10//population number
#define maximization 1
#define minimization 2
#define cmax 100
#define cmin 0

struct MGraph {
	int *edges[MAX_VALUE];
	int iVertexCount, iEdgeCount;
};
struct Path {
	int path[100];  //��·
	bool cross;       //�Ƿ���ͨ
	//int pathCost;    //��·����
	double  pathCost;    //��·����
	int pathLenght;   //��·�ڵ���
};
struct success {
	Path successPath[1000]; //�ݶ�һ�ٰɣ�֮����Ը�
	int successPathNum;
	//int successAllCost;
	double  successAllCost;
	int time;             //������¼ʱ�䣬����ʱ�䲻�������
};
struct individual {
	//int *chrom;
	std::vector<int> chrom;
	double value;//����ֵ
	double fitness;      //��Ӧ��
};

//-----Floyd
void PrintResult(const MGraph& mGraph, int **iArrPath);
void Floyd(double **LinkUnitPrice,int &NodeNum_Network, int &LinkNum);
void Deallocate_Arrays();
void Allocate_result();
void Deallocate_result();
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
void deleteCloseCycles();


void displayChroms(std::string debugName);
double getRealCost(int **LinkUnitPriceReal, individual currentbest);
#endif //SDK_GCC_COPY_INCLUDES_H

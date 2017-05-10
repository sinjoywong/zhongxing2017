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
	int path[100];  //��·
	bool cross;       //�Ƿ���ͨ
	//int pathCost;    //��·����
	double  pathCost;    //��·����
	int pathLenght;   //��·�ڵ���
};
struct success {
	Path successPath[1000]; //�ݶ�һ�ٰɣ�֮����Ը�
	int successPathNum;
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

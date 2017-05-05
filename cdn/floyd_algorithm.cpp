#include "includes.h"
using namespace std;

extern double **LinkUnitPrice;
extern Path **allPath;
extern int NodeNum_Network, LinkNum;
extern struct individual population[POPSIZE];
Path **allPathTemp;

int pathLenght = 0;

void Floyd(double **LinkUnitPrice,int &NodeNum_Network, int &LinkNum) {
	int ** iArrPath;
		iArrPath = new int*[MAX_VALUE];
		for (int i = 0; i < MAX_VALUE; i++) {
			iArrPath[i] = new int[MAX_VALUE];
			memset(iArrPath[i], 0, MAX_VALUE * sizeof(int));
		}
	MGraph mGraph;
	for (int i = 0; i < MAX_VALUE; i++) {
		mGraph.edges[i] = new int[MAX_VALUE];
	}
	mGraph.iVertexCount = NodeNum_Network;
	mGraph.iEdgeCount = LinkNum;

	for (int iRow = 0; iRow < mGraph.iVertexCount; iRow++) {
		for (int iCol = 0; iCol < mGraph.iVertexCount; iCol++) {
			mGraph.edges[iRow][iCol] = LinkUnitPrice[iRow][iCol];
		}
	}
	for (int i = 0; i < mGraph.iVertexCount; i++) {
		for (int j = 0; j <= mGraph.iVertexCount; j++) {
			iArrPath[i][j] = i;
		}
	}//初始化路径表
	//Floyd algorithm
	for (int k = 0; k < mGraph.iVertexCount; k++) {
		for (int i = 0; i < mGraph.iVertexCount; i++) {
			for (int j = 0; j < mGraph.iVertexCount; j++) {
				if (mGraph.edges[i][k] + mGraph.edges[k][j] < mGraph.edges[i][j]) {
					mGraph.edges[i][j] = mGraph.edges[i][k] + mGraph.edges[k][j];
					iArrPath[i][j] = iArrPath[k][j];
				}
			}
		}
	}
	PrintResult(mGraph, iArrPath);
	//---------改动1.delete iArrPath--------
	for (int i = 0; i < MAX_VALUE; i++) {
		delete[] iArrPath[i];
		iArrPath[i] = NULL;
	}
	delete[] iArrPath;
	iArrPath = NULL;
}

void PrintResult(const MGraph& mGraph, int **iArrPath) {

	allPath = new Path *[NodeNum_Network];
	for (int i = 0; i < NodeNum_Network; i++) {
		allPath[i] = new Path[NodeNum_Network];
	}
	for (int i = 0; i < mGraph.iVertexCount; i++) {
		for (int j = 0; j < mGraph.iVertexCount; j++) {
			if (i != j) {
				cout << i << "->" << j << "\t\t";//start point and end point

				if (mGraph.edges[i][j] == MAX_VALUE) {
					cout << "无连通路径" << "\n" << endl;
					allPath[i][j].cross = false;
					allPath[i][j].pathCost = 1000000;
					allPath[i][j].pathLenght = 0;
				}
				else {
					cout << "mGraph.edges "<<i <<" "<< j <<" " << mGraph.edges[i][j] << "\t\t"; //from i to j, the cost
					allPath[i][j].pathCost = mGraph.edges[i][j];
					allPath[i][j].cross = true;
				//	int pathLenght = 0;
					std::stack<int> stackVertices;
					int k = j;
					do {
						k = iArrPath[i][k];
						stackVertices.push(k);
					} while (k != i);
					cout << stackVertices.top();//start point in node
					allPath[i][j].path[pathLenght] = stackVertices.top();
					pathLenght++;
					stackVertices.pop();

					unsigned int nLength = stackVertices.size();
					for (unsigned int nIndex = 0; nIndex < nLength; nIndex++) {
						cout << " -> " << stackVertices.top();// middle nodes in path
						allPath[i][j].path[pathLenght] = stackVertices.top();//link node
						pathLenght++;
						stackVertices.pop();
					}
					allPath[i][j].path[pathLenght] = j;
					pathLenght++;
					allPath[i][j].pathLenght = pathLenght;

					cout << " -> " << j << "  " << pathLenght << endl;//end node in path and path lenght
					pathLenght = 0;
				}
			}
		}
	}
	//--for debugging 
	/*
	cout << "0 to 17 " << endl;
	//for (int i = 0; i <allPath[0][17].pathLenght; i++) {
	for (int i = 0; i <allPath[0][0].pathLenght; i++) {
		//cout << allPath[0][17].path[i]<< endl;
		cout << allPath[0][0].path[i]<< endl;
	}
		cout << "pathCost=" << allPath[0][17].pathCost<<endl;
		*/
	//--end debug
}

//形参为种群信息及第pop_no个种群
void Allocate_result() {
	//--------------1.将yuanNode改为动态数组---------------
	allPathTemp = new Path *[NodeNum_Network];
	for (int i = 0; i < NodeNum_Network; i++) {
		allPathTemp[i] = new Path[NodeNum_Network];
		memset(allPathTemp[i], 0, NodeNum_Network * sizeof(int));
	}
}
void Deallocate_result() {
	for (int i = 0; i < NodeNum_Network; i++) {
		delete[] allPathTemp[i];
		allPathTemp[i] = NULL;
	}
	delete[] allPathTemp;
	allPathTemp = NULL;
}
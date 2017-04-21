#include "includes.h"
using namespace std;

extern int **LinkUnitPrice;
extern int **LinkMax;
extern Path **allPath;
extern int NodeNum_Network, NodeNum_Demand, LinkNum, ServerUnitPrice;
extern int **DemandMax;
//extern vector<int> ServerNum;
extern vector<vector <int> > ServerNum;//返回的服务器编号,4行即4个染色体,每行为染色体中1的位置
extern success sucessFinish;
extern vector<number> vn;//网络节点中出现次数最多的集合,vn[1].value vn[2].value ...
extern struct individual population[POPSIZE];
extern int** DemandNodeUseble; 
int * yuanNode;
int ** belong;
int ** LinkMaxTemp;
int ** LinkUnitPriceTemp;
Path **allPathTemp;
int **DemandMaxTemp;

void Floyd(int &NodeNum_Network, int &LinkNum) {
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
	mGraph.iEdageCount = LinkNum;

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

int pathMaxWitch(const Path& path) {
	//该函数计算Path带宽
	int temp = 1000;
	for (int k = 0; k<path.pathLenght - 1; k++) {
		int firstNode = path.path[k];
		int secondNode = path.path[k + 1];
		if (temp > LinkMax[firstNode][secondNode]) {
			temp = LinkMax[firstNode][secondNode];
		}
	}
	return temp;
}

//计算路径带宽
int pathMaxWitch(const Path& path, int **LinkMaxTemp) {
	//该函数计算Path带宽
	int temp = 1000;
	for (int k = 0; k<path.pathLenght - 1; k++) {
		int firstNode = path.path[k];
		int secondNode = path.path[k + 1];
		if (temp > LinkMaxTemp[firstNode][secondNode]) {
			temp = LinkMaxTemp[firstNode][secondNode];
		}
	}
	return temp;
}

//获得新的最短路线

//获得新的最短路线
void changePath(int &firstNode, int &secondNode, int **LinkMaxTemp, Path **allPathTemp, int **LinkUnitPriceTemp) {

	int lujingdanjiaTemp = 1000;
	int lujingdanjia;
	int lujingdaikuan1, lujingdaikuan2;
	int lujingdaikuan = 0;
	int NodeTemp = 0;

	//连否则可以直接相连或之前已生成最短路径但带宽可能需要更新
	lujingdaikuan = pathMaxWitch(allPathTemp[firstNode][secondNode], LinkMaxTemp);
	allPathTemp[firstNode][secondNode].pathWitch = lujingdaikuan;
	if (!lujingdaikuan)
	{
		for (int ii = 0; ii<DEMANDNODEUSBLENUM; ii++)
		{
			NodeTemp = DemandNodeUseble[secondNode][ii];
			if ((NodeTemp != firstNode) && (NodeTemp != secondNode))
			{
				//生成新路径的带宽单价
				lujingdanjia = LinkUnitPriceTemp[firstNode][NodeTemp] + LinkUnitPriceTemp[NodeTemp][secondNode];

				//生成新路径带宽
				lujingdaikuan1 = pathMaxWitch(allPathTemp[firstNode][NodeTemp], LinkMaxTemp);
				lujingdaikuan2 = pathMaxWitch(allPathTemp[NodeTemp][secondNode], LinkMaxTemp);
				lujingdaikuan = lujingdaikuan1;
				if (lujingdaikuan1  >  lujingdaikuan2)
				{
					lujingdaikuan = lujingdaikuan2;
				}

				//如果带宽单价满足要求则选取最短的一个
				if ((lujingdaikuan>0) && (lujingdanjia<lujingdanjiaTemp))
				{
					int lenght1 = allPath[firstNode][NodeTemp].pathLenght;
					int lenght2 = allPath[NodeTemp][secondNode].pathLenght;
					for (int jj = 0; jj<lenght1; jj++) {
						allPathTemp[firstNode][secondNode].path[jj] = allPathTemp[firstNode][NodeTemp].path[jj];
					}
					for (int jj = lenght1; jj<lenght1 + lenght2 - 1; jj++) {
						allPathTemp[firstNode][secondNode].path[jj] = allPathTemp[NodeTemp][secondNode].path[jj - lenght1 + 1];
					}
					allPathTemp[firstNode][secondNode].pathLenght = lenght1 + lenght2 - 1;
					allPathTemp[firstNode][secondNode].pathCost = lujingdanjia;
					allPathTemp[firstNode][secondNode].pathWitch = lujingdaikuan; //更新源点到该消费节点的带宽
					LinkUnitPriceTemp[firstNode][secondNode] = lujingdanjia;
					lujingdanjiaTemp = lujingdanjia;
				}
			}
		}
	}
}


void PrintResult(const MGraph& mGraph, int **iArrPath) {
	allPath = new Path *[NodeNum_Network];
	for (int i = 0; i < NodeNum_Network; i++) {
		allPath[i] = new Path[NodeNum_Network];
	}

	for (int i = 0; i < mGraph.iVertexCount; i++) {
		for (int j = 0; j < mGraph.iVertexCount; j++) {
			if (i != j) {

				if (mGraph.edges[i][j] == MAX_VALUE) {

					allPath[i][j].cross = false;
					allPath[i][j].pathCost = 1000000;
					allPath[i][j].pathWitch = 0;
					allPath[i][j].pathLenght = 0;
					LinkMax[i][j] = 0;
				}
				else {
					allPath[i][j].pathCost = mGraph.edges[i][j];
					LinkUnitPrice[i][j] = mGraph.edges[i][j];
					allPath[i][j].cross = true;
					allPath[i][j].pathWitch = 0;
					int pathLenght = 0;
					std::stack<int> stackVertices;
					int k = j;
					do {
						k = iArrPath[i][k];
						stackVertices.push(k);
					} while (k != i);
					allPath[i][j].path[pathLenght] = stackVertices.top();
					pathLenght++;
					stackVertices.pop();

					unsigned int nLength = stackVertices.size();
					for (unsigned int nIndex = 0; nIndex < nLength; nIndex++) {
						allPath[i][j].path[pathLenght] = stackVertices.top();
						pathLenght++;
						stackVertices.pop();
					}

					allPath[i][j].path[pathLenght] = j;
					pathLenght++;
					allPath[i][j].pathLenght = pathLenght;

					pathLenght = 0;

				}
			}
		}
	}
	//为所有路径更新带宽
	for (int i = 0; i < mGraph.iVertexCount; i++) {
		for (int j = 0; j < mGraph.iVertexCount; j++) {
			if (i != j && allPath[i][j].cross) {
				LinkMax[i][j] = pathMaxWitch(allPath[i][j]);
			}
		}
	}

	//该函数将初始化DemandNodeUsble数组，该数组记录距离消费节点最近的DEMANDNODEUSBLENUM个点
	number* numberTemp;
	numberTemp = new number[NodeNum_Network];
	for (int i = 0; i < NodeNum_Demand; i++)
	{
		//将allPath路径花费信息拷贝进去
		int demandNode = DemandMax[i][0];
		for (int j = 0; j<NodeNum_Network; j++)
		{
			numberTemp[j].value = j;   //记录节点编号
			numberTemp[j].count = allPath[j][demandNode].pathCost; //记录花费
		}

		sort(numberTemp, numberTemp + NodeNum_Network, cmp);

		for (int j = 0; j<DEMANDNODEUSBLENUM; j++)
		{
			DemandNodeUseble[demandNode][j] = numberTemp[NodeNum_Network - j - 2].value;
			cout << DemandNodeUseble[demandNode][j] << "  ";;
		}
		cout << endl;

	}


}

//形参为种群信息及第pop_no个种群
void Allocate_result() {
	//--------------1.将yuanNode改为动态数组---------------
	yuanNode = new int[NodeNum_Network];
	memset(yuanNode, 0, NodeNum_Network * sizeof(int));

	LinkUnitPriceTemp = new int *[NodeNum_Network];
	//构建临时网络链路带宽限制矩阵
	LinkMaxTemp = new int *[NodeNum_Network];
	allPathTemp = new Path *[NodeNum_Network];

	for (int i = 0; i < NodeNum_Network; i++) {
		LinkMaxTemp[i] = new int[NodeNum_Network];
		memset(LinkMaxTemp[i], 0, NodeNum_Network * sizeof(int));
		LinkUnitPriceTemp[i] = new int[NodeNum_Network];
		memset(LinkUnitPriceTemp[i], 0, NodeNum_Network * sizeof(int));
		allPathTemp[i] = new Path[NodeNum_Network];
		memset(allPathTemp[i], 0, NodeNum_Network * sizeof(int));
	}

	DemandMaxTemp = new int *[MAX_VERTEX_COUNT];
	// belong = new int*[MAX_VERTEX_COUNT];
	for (int i = 0; i<MAX_VERTEX_COUNT; i++) {
		DemandMaxTemp[i] = new int[3];
		memset(DemandMaxTemp[i], 0, sizeof(3));
		//   belong[i] = new int [MAX_VERTEX_COUNT];
		//  memset(belong[i],0,MAX_VERTEX_COUNT* sizeof(int));

	}
	belong = new int*[MAX_VALUE];
	for (int i = 0; i<MAX_VALUE; i++) {
		belong[i] = new int[MAX_VALUE];
		memset(belong[i], 0, MAX_VALUE * sizeof(int));
	}

}

void Deallocate_result() {
	for (int i = 0; i < NodeNum_Network; i++) {
		delete[] LinkMaxTemp[i];
		delete[] LinkUnitPriceTemp[i];
		delete[] allPathTemp[i];
		LinkMaxTemp[i] = NULL;
		LinkUnitPriceTemp[i] = NULL;
		allPathTemp[i] = NULL;
	}
	delete[] LinkMaxTemp;
	delete[] LinkUnitPriceTemp;
	delete[] allPathTemp;

	LinkMaxTemp = NULL;
	LinkUnitPriceTemp = NULL;
	allPathTemp = NULL;

	for (int i = 0; i<MAX_VERTEX_COUNT; i++) {
		delete[] belong[i];
		delete[] DemandMaxTemp[i];
	}
	delete[] belong;
	delete[] DemandMaxTemp;

	belong = NULL;
	DemandMaxTemp = NULL;
}

double result(int& pop_no) {
	//构建临时网络链路矩阵
	for (int i = 0; i < NodeNum_Network; i++) {
		for (int j = 0; j<NodeNum_Network; j++) {
			LinkMaxTemp[i][j] = LinkMax[i][j];
			allPathTemp[i][j] = allPath[i][j];
			LinkUnitPriceTemp[i][j] = LinkUnitPrice[i][j];
		}
	}
	//从染色体中获得服务器位置
	ServerNum[pop_no].clear();
	for (int i = 0; i < NodeNum_Network; i++) {
		if (population[pop_no].chrom[i] == 1)
			ServerNum[pop_no].push_back(i);
	}
	yuanNode[0] = ServerNum[pop_no].size();
	for (unsigned int i = 1; i <= ServerNum[pop_no].size(); i++) {
		//第pop_no条染色体中的服务器位置
		yuanNode[i] = ServerNum[pop_no][i - 1];//此处应是ServerNum[pop_no][i-1],因为是第i个yuanNode针对第i-1个ServerNum
	}
	int yuanNodeNum = yuanNode[0];//服务器节点数目

	Path resultPath[RESULT_PATH_NUM];  //存储链路
	bool thereIsHaveResult = true;      //判断是否有解
	int resultPathNum = 0;          //存储链路个数
	int firstNode;
	int secondNode;
	int NodeNum_DemandTemp = 0;
	for (int i = 0; i < NodeNum_Demand; i++) {
		bool IsDemandNeed = true;
		for (int j = 0; j < yuanNodeNum; j++) {
			if (DemandMax[i][0] == yuanNode[j + 1]) {
				IsDemandNeed = false;
				Path temp;
				temp.path[0] = DemandMax[i][0];
				temp.path[1] = i;
				temp.pathLenght = 2;
				temp.pathCost = 0;
				temp.pathWitch = DemandMax[i][1];
				resultPath[resultPathNum] = temp;
				resultPathNum += 1;
			}
		}
		if (IsDemandNeed) {
			for (int j = 0; j < 2; j++) {
				DemandMaxTemp[NodeNum_DemandTemp][j] = DemandMax[i][j];
			}
			DemandMaxTemp[NodeNum_DemandTemp][2] = i;
			NodeNum_DemandTemp++;
		}
	}

	if (yuanNodeNum <= 0) {
		thereIsHaveResult = false;
		return 0.00001;
	}

	for (int i = 0; i < NodeNum_DemandTemp; i++) {
		int DemandPathNum = 0;  //记录为该需求节点添加了多少路线，到时候如果添加新服务器则删除这些路线
		int DemandPathCost = 0; //记录为该需求节点花费了多少钱，太多则添加服务器
								//为消费节点加入路径
		int widthDmandTemp = DemandMaxTemp[i][1]; //消费节点链接节点所需带宽
		Path linsiPath;      //临时存储路径
		int linsiPathWitch;      //计算临时路径的带宽
								 //	int controlNode;        //记录限制路径带宽的节点编号
		secondNode = DemandMaxTemp[i][0];  //目标节点
		int temp = 1000;
		for (int j = 0; j < yuanNodeNum; j++) {
			//现在需要保证服务器不在消费节点相邻节点上  没有考录路径不同的情况
			firstNode = yuanNode[j + 1];    //源节点
			secondNode = DemandMaxTemp[i][0];  //目标节点
			changePath(firstNode, secondNode, LinkMaxTemp, allPathTemp, LinkUnitPriceTemp);
			belong[i][j + 2] = allPathTemp[firstNode][secondNode].pathCost;
			if (temp > belong[i][j + 2]) {
				temp = belong[i][j + 2];
				belong[i][0] = yuanNode[j + 1];
				belong[i][1] = j + 2;  //最短路径所在位置
			}
		}
		linsiPath = allPathTemp[belong[i][0]][DemandMaxTemp[i][0]];
		linsiPathWitch = linsiPath.pathWitch;

		while (widthDmandTemp && linsiPathWitch) {
			/*      for(int count=0; count<linsiPath.pathLenght; count++) {
			cout<<linsiPath.path[count]<<"  ";
			}
			*/
			linsiPath.pathWitch = pathMaxWitch(linsiPath, LinkMaxTemp);

			//  cout<<linsiPath.pathWitch<<"  "<<linsiPath.pathCost<<endl;
			if (linsiPath.pathWitch <= 0) { break; }
			if (widthDmandTemp>linsiPathWitch) {
				widthDmandTemp = widthDmandTemp - linsiPathWitch;

				linsiPath.path[linsiPath.pathLenght] = DemandMaxTemp[i][2];
				linsiPath.pathLenght += 1;
				resultPath[resultPathNum] = linsiPath;
				resultPathNum += 1;
				DemandPathNum += 1;
				DemandPathCost += (linsiPath.pathCost) * (linsiPath.pathWitch);
				//删除临时带宽矩阵中以利用的带宽
				for (int k = 0; k < linsiPath.pathLenght - 1; k++) {
					int firstNode = linsiPath.path[k];
					int secondNode = linsiPath.path[k + 1];
					LinkMaxTemp[firstNode][secondNode] = LinkMaxTemp[firstNode][secondNode] - linsiPathWitch;
				}
				//生成新的临时路径
				int lujingdanjia;
				int lujingdaikuan = 0;
				linsiPath.pathWitch = 0;
				firstNode = belong[i][0];
				secondNode = DemandMaxTemp[i][0];
				//找出第二短的路径 没有考虑原源点没有生成临时路径的可能
				changePath(firstNode, secondNode, LinkMaxTemp, allPathTemp, LinkUnitPriceTemp);
				linsiPath = allPathTemp[firstNode][secondNode];
				belong[i][belong[i][1]] = linsiPath.pathCost; // 更新原源点与消费节点的最短路径 更换源节点后有用
															  //判断是否要更换路径源点 即现有源节点与消费节点的路径单价不一定最便宜 需要考录其它节点的影响
				for (int kk = 0; kk < yuanNodeNum; kk++) {
					firstNode = yuanNode[kk + 1];
					secondNode = DemandMaxTemp[i][0];
					lujingdaikuan = pathMaxWitch(allPathTemp[firstNode][secondNode], LinkMaxTemp);
					lujingdanjia = LinkUnitPriceTemp[firstNode][secondNode];
					//如果其他源节点单价更低这更换节点 同时更新临时路径 以及belong矩阵
					//如果之前路径导致噶路径现在带宽为零则不会再进行更换该节点，实际上应该在带宽为零的时候考录换线
					//有时间拷贝一下
					if (lujingdaikuan == 0) {
						changePath(firstNode, secondNode, LinkMaxTemp, allPathTemp, LinkUnitPriceTemp);
						belong[i][kk + 2] = allPathTemp[firstNode][secondNode].pathCost;
					}
					if ((linsiPath.pathCost > (allPathTemp[firstNode][secondNode].pathCost) &&
						allPathTemp[firstNode][secondNode].pathWitch) || (linsiPath.pathWitch == 0)) {
						//更换源点
						belong[i][0] = firstNode;
						belong[i][1] = kk;
						allPathTemp[firstNode][secondNode].pathWitch = pathMaxWitch(allPathTemp[firstNode][secondNode],
							LinkMaxTemp);
						linsiPath = allPathTemp[firstNode][secondNode];
					}
				}
				linsiPathWitch = linsiPath.pathWitch;
				////生成临时路径完成
			}
			else {
				linsiPath.pathWitch = widthDmandTemp;
				linsiPath.path[linsiPath.pathLenght] = DemandMaxTemp[i][2];
				linsiPath.pathLenght += 1;
				resultPath[resultPathNum] = linsiPath;
				resultPathNum += 1;
				DemandPathNum += 1;
				DemandPathCost += (linsiPath.pathCost) * (linsiPath.pathWitch);
				//删除临时带宽矩阵中以利用的带宽
				for (int k = 0; k<linsiPath.pathLenght - 1; k++) {
					int firstNode = linsiPath.path[k];
					int secondNode = linsiPath.path[k + 1];
					LinkMaxTemp[firstNode][secondNode] = LinkMaxTemp[firstNode][secondNode] - widthDmandTemp;
				}
				widthDmandTemp = 0;
			}
		}
		//如果条件为真则while条件跳出条件为linsiPathWitch == 0，即找不到可用的路径则整个路径搜索失败 终止寻路程序
		/*
		if (linsiPathWitch <= 0) {
		thereIsHaveResult = false;
		for (int i = 0; i < NodeNum_Network; i++) {
		}
		return 0.00001;
		}
		*/
		//如果找不到合适的路径就添加服务器节点
		if ((linsiPathWitch <= 0) || (DemandPathCost >= ServerUnitPrice)) {
			int addPathSverNode = DemandMaxTemp[i][0];
			yuanNode[yuanNodeNum + 1] = addPathSverNode;
			yuanNodeNum++;
			yuanNode[0] = yuanNodeNum;
			population[pop_no].chrom[addPathSverNode] = 1;

			//把消耗掉的带宽加回去
			//  for(DemandPathNum; DemandPathNum>0;DemandPathNum--)
			while (DemandPathNum>0) {
				resultPathNum--;
				for (int k = 0; k<resultPath[resultPathNum].pathLenght - 1; k++) {
					int firstNode = resultPath[resultPathNum].path[k];
					int secondNode = resultPath[resultPathNum].path[k + 1];
					LinkMaxTemp[firstNode][secondNode] = LinkMaxTemp[firstNode][secondNode] + resultPath[resultPathNum].pathWitch;
				}
				DemandPathNum--;
			}
			Path addPath;
			addPath.path[0] = addPathSverNode;
			addPath.path[1] = DemandMaxTemp[i][2];
			addPath.pathLenght = 2;
			addPath.pathCost = 0;
			addPath.pathWitch = DemandMaxTemp[i][1];
			resultPath[resultPathNum] = addPath;
			resultPathNum += 1;
		}
	}
	double allCost = 0;
	for (int count = 0; count < resultPathNum; count++) {
		/*
		for(int kk=0; kk < resultPath[count].pathLenght; kk++) {
		cout<<"-> " << resultPath[count].path[kk]<<"  ";
		}
		cout<<"cost " << resultPath[count].pathCost<<" width "<<resultPath[count].pathWitch<<endl;
		*/
		allCost += resultPath[count].pathCost * resultPath[count].pathWitch;
	}
	yuanNodeNum = 0;
	int deleteTemp[1000];
	int deleteNode = 0;    //需要一个局部变量
						   //计算所用使用的服务器节点数，同时更新该染色体中，删除没有用到的节点
	for (int i = 0; i < NodeNum_Network; i++) { deleteTemp[i] = 0; }
	for (int i = 0; i < resultPathNum; i++) {
		deleteNode = resultPath[i].path[0];
		if (deleteTemp[deleteNode] == 0) {
			yuanNodeNum++;
			deleteTemp[deleteNode] = 1;
		}
	}
	allCost += ServerUnitPrice * yuanNodeNum;
	//写入最优解
	if (allCost < sucessFinish.successAllCost) {
		for (int count = 0; count < resultPathNum; count++) {
			sucessFinish.successPath[count] = resultPath[count];
		}
		sucessFinish.successAllCost = allCost;
		sucessFinish.successPathNum = resultPathNum;
	}
	//return allCost;
	allCost = 100.0 / allCost;
	return allCost;
}





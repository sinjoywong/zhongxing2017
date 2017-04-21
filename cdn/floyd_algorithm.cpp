#include "includes.h"
using namespace std;

extern int **LinkUnitPrice;
extern int **LinkMax;
extern Path **allPath;
extern int NodeNum_Network, NodeNum_Demand, LinkNum, ServerUnitPrice;
extern int **DemandMax;
//extern vector<int> ServerNum;
extern vector<vector <int> > ServerNum;//���صķ��������,4�м�4��Ⱦɫ��,ÿ��ΪȾɫ����1��λ��
extern success sucessFinish;
extern vector<number> vn;//����ڵ��г��ִ������ļ���,vn[1].value vn[2].value ...
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
	}//��ʼ��·����
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
	//---------�Ķ�1.delete iArrPath--------
	for (int i = 0; i < MAX_VALUE; i++) {
		delete[] iArrPath[i];
		iArrPath[i] = NULL;
	}
	delete[] iArrPath;
	iArrPath = NULL;
}

int pathMaxWitch(const Path& path) {
	//�ú�������Path����
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

//����·������
int pathMaxWitch(const Path& path, int **LinkMaxTemp) {
	//�ú�������Path����
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

//����µ����·��

//����µ����·��
void changePath(int &firstNode, int &secondNode, int **LinkMaxTemp, Path **allPathTemp, int **LinkUnitPriceTemp) {

	int lujingdanjiaTemp = 1000;
	int lujingdanjia;
	int lujingdaikuan1, lujingdaikuan2;
	int lujingdaikuan = 0;
	int NodeTemp = 0;

	//���������ֱ��������֮ǰ���������·�������������Ҫ����
	lujingdaikuan = pathMaxWitch(allPathTemp[firstNode][secondNode], LinkMaxTemp);
	allPathTemp[firstNode][secondNode].pathWitch = lujingdaikuan;
	if (!lujingdaikuan)
	{
		for (int ii = 0; ii<DEMANDNODEUSBLENUM; ii++)
		{
			NodeTemp = DemandNodeUseble[secondNode][ii];
			if ((NodeTemp != firstNode) && (NodeTemp != secondNode))
			{
				//������·���Ĵ�����
				lujingdanjia = LinkUnitPriceTemp[firstNode][NodeTemp] + LinkUnitPriceTemp[NodeTemp][secondNode];

				//������·������
				lujingdaikuan1 = pathMaxWitch(allPathTemp[firstNode][NodeTemp], LinkMaxTemp);
				lujingdaikuan2 = pathMaxWitch(allPathTemp[NodeTemp][secondNode], LinkMaxTemp);
				lujingdaikuan = lujingdaikuan1;
				if (lujingdaikuan1  >  lujingdaikuan2)
				{
					lujingdaikuan = lujingdaikuan2;
				}

				//�������������Ҫ����ѡȡ��̵�һ��
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
					allPathTemp[firstNode][secondNode].pathWitch = lujingdaikuan; //����Դ�㵽�����ѽڵ�Ĵ���
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
	//Ϊ����·�����´���
	for (int i = 0; i < mGraph.iVertexCount; i++) {
		for (int j = 0; j < mGraph.iVertexCount; j++) {
			if (i != j && allPath[i][j].cross) {
				LinkMax[i][j] = pathMaxWitch(allPath[i][j]);
			}
		}
	}

	//�ú�������ʼ��DemandNodeUsble���飬�������¼�������ѽڵ������DEMANDNODEUSBLENUM����
	number* numberTemp;
	numberTemp = new number[NodeNum_Network];
	for (int i = 0; i < NodeNum_Demand; i++)
	{
		//��allPath·��������Ϣ������ȥ
		int demandNode = DemandMax[i][0];
		for (int j = 0; j<NodeNum_Network; j++)
		{
			numberTemp[j].value = j;   //��¼�ڵ���
			numberTemp[j].count = allPath[j][demandNode].pathCost; //��¼����
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

//�β�Ϊ��Ⱥ��Ϣ����pop_no����Ⱥ
void Allocate_result() {
	//--------------1.��yuanNode��Ϊ��̬����---------------
	yuanNode = new int[NodeNum_Network];
	memset(yuanNode, 0, NodeNum_Network * sizeof(int));

	LinkUnitPriceTemp = new int *[NodeNum_Network];
	//������ʱ������·�������ƾ���
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
	//������ʱ������·����
	for (int i = 0; i < NodeNum_Network; i++) {
		for (int j = 0; j<NodeNum_Network; j++) {
			LinkMaxTemp[i][j] = LinkMax[i][j];
			allPathTemp[i][j] = allPath[i][j];
			LinkUnitPriceTemp[i][j] = LinkUnitPrice[i][j];
		}
	}
	//��Ⱦɫ���л�÷�����λ��
	ServerNum[pop_no].clear();
	for (int i = 0; i < NodeNum_Network; i++) {
		if (population[pop_no].chrom[i] == 1)
			ServerNum[pop_no].push_back(i);
	}
	yuanNode[0] = ServerNum[pop_no].size();
	for (unsigned int i = 1; i <= ServerNum[pop_no].size(); i++) {
		//��pop_no��Ⱦɫ���еķ�����λ��
		yuanNode[i] = ServerNum[pop_no][i - 1];//�˴�Ӧ��ServerNum[pop_no][i-1],��Ϊ�ǵ�i��yuanNode��Ե�i-1��ServerNum
	}
	int yuanNodeNum = yuanNode[0];//�������ڵ���Ŀ

	Path resultPath[RESULT_PATH_NUM];  //�洢��·
	bool thereIsHaveResult = true;      //�ж��Ƿ��н�
	int resultPathNum = 0;          //�洢��·����
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
		int DemandPathNum = 0;  //��¼Ϊ������ڵ�����˶���·�ߣ���ʱ���������·�������ɾ����Щ·��
		int DemandPathCost = 0; //��¼Ϊ������ڵ㻨���˶���Ǯ��̫������ӷ�����
								//Ϊ���ѽڵ����·��
		int widthDmandTemp = DemandMaxTemp[i][1]; //���ѽڵ����ӽڵ��������
		Path linsiPath;      //��ʱ�洢·��
		int linsiPathWitch;      //������ʱ·���Ĵ���
								 //	int controlNode;        //��¼����·������Ľڵ���
		secondNode = DemandMaxTemp[i][0];  //Ŀ��ڵ�
		int temp = 1000;
		for (int j = 0; j < yuanNodeNum; j++) {
			//������Ҫ��֤�������������ѽڵ����ڽڵ���  û�п�¼·����ͬ�����
			firstNode = yuanNode[j + 1];    //Դ�ڵ�
			secondNode = DemandMaxTemp[i][0];  //Ŀ��ڵ�
			changePath(firstNode, secondNode, LinkMaxTemp, allPathTemp, LinkUnitPriceTemp);
			belong[i][j + 2] = allPathTemp[firstNode][secondNode].pathCost;
			if (temp > belong[i][j + 2]) {
				temp = belong[i][j + 2];
				belong[i][0] = yuanNode[j + 1];
				belong[i][1] = j + 2;  //���·������λ��
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
				//ɾ����ʱ��������������õĴ���
				for (int k = 0; k < linsiPath.pathLenght - 1; k++) {
					int firstNode = linsiPath.path[k];
					int secondNode = linsiPath.path[k + 1];
					LinkMaxTemp[firstNode][secondNode] = LinkMaxTemp[firstNode][secondNode] - linsiPathWitch;
				}
				//�����µ���ʱ·��
				int lujingdanjia;
				int lujingdaikuan = 0;
				linsiPath.pathWitch = 0;
				firstNode = belong[i][0];
				secondNode = DemandMaxTemp[i][0];
				//�ҳ��ڶ��̵�·�� û�п���ԭԴ��û��������ʱ·���Ŀ���
				changePath(firstNode, secondNode, LinkMaxTemp, allPathTemp, LinkUnitPriceTemp);
				linsiPath = allPathTemp[firstNode][secondNode];
				belong[i][belong[i][1]] = linsiPath.pathCost; // ����ԭԴ�������ѽڵ�����·�� ����Դ�ڵ������
															  //�ж��Ƿ�Ҫ����·��Դ�� ������Դ�ڵ������ѽڵ��·�����۲�һ������� ��Ҫ��¼�����ڵ��Ӱ��
				for (int kk = 0; kk < yuanNodeNum; kk++) {
					firstNode = yuanNode[kk + 1];
					secondNode = DemandMaxTemp[i][0];
					lujingdaikuan = pathMaxWitch(allPathTemp[firstNode][secondNode], LinkMaxTemp);
					lujingdanjia = LinkUnitPriceTemp[firstNode][secondNode];
					//�������Դ�ڵ㵥�۸���������ڵ� ͬʱ������ʱ·�� �Լ�belong����
					//���֮ǰ·�����¸�·�����ڴ���Ϊ���򲻻��ٽ��и����ýڵ㣬ʵ����Ӧ���ڴ���Ϊ���ʱ��¼����
					//��ʱ�俽��һ��
					if (lujingdaikuan == 0) {
						changePath(firstNode, secondNode, LinkMaxTemp, allPathTemp, LinkUnitPriceTemp);
						belong[i][kk + 2] = allPathTemp[firstNode][secondNode].pathCost;
					}
					if ((linsiPath.pathCost > (allPathTemp[firstNode][secondNode].pathCost) &&
						allPathTemp[firstNode][secondNode].pathWitch) || (linsiPath.pathWitch == 0)) {
						//����Դ��
						belong[i][0] = firstNode;
						belong[i][1] = kk;
						allPathTemp[firstNode][secondNode].pathWitch = pathMaxWitch(allPathTemp[firstNode][secondNode],
							LinkMaxTemp);
						linsiPath = allPathTemp[firstNode][secondNode];
					}
				}
				linsiPathWitch = linsiPath.pathWitch;
				////������ʱ·�����
			}
			else {
				linsiPath.pathWitch = widthDmandTemp;
				linsiPath.path[linsiPath.pathLenght] = DemandMaxTemp[i][2];
				linsiPath.pathLenght += 1;
				resultPath[resultPathNum] = linsiPath;
				resultPathNum += 1;
				DemandPathNum += 1;
				DemandPathCost += (linsiPath.pathCost) * (linsiPath.pathWitch);
				//ɾ����ʱ��������������õĴ���
				for (int k = 0; k<linsiPath.pathLenght - 1; k++) {
					int firstNode = linsiPath.path[k];
					int secondNode = linsiPath.path[k + 1];
					LinkMaxTemp[firstNode][secondNode] = LinkMaxTemp[firstNode][secondNode] - widthDmandTemp;
				}
				widthDmandTemp = 0;
			}
		}
		//�������Ϊ����while������������ΪlinsiPathWitch == 0�����Ҳ������õ�·��������·������ʧ�� ��ֹѰ·����
		/*
		if (linsiPathWitch <= 0) {
		thereIsHaveResult = false;
		for (int i = 0; i < NodeNum_Network; i++) {
		}
		return 0.00001;
		}
		*/
		//����Ҳ������ʵ�·������ӷ������ڵ�
		if ((linsiPathWitch <= 0) || (DemandPathCost >= ServerUnitPrice)) {
			int addPathSverNode = DemandMaxTemp[i][0];
			yuanNode[yuanNodeNum + 1] = addPathSverNode;
			yuanNodeNum++;
			yuanNode[0] = yuanNodeNum;
			population[pop_no].chrom[addPathSverNode] = 1;

			//�����ĵ��Ĵ���ӻ�ȥ
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
	int deleteNode = 0;    //��Ҫһ���ֲ�����
						   //��������ʹ�õķ������ڵ�����ͬʱ���¸�Ⱦɫ���У�ɾ��û���õ��Ľڵ�
	for (int i = 0; i < NodeNum_Network; i++) { deleteTemp[i] = 0; }
	for (int i = 0; i < resultPathNum; i++) {
		deleteNode = resultPath[i].path[0];
		if (deleteTemp[deleteNode] == 0) {
			yuanNodeNum++;
			deleteTemp[deleteNode] = 1;
		}
	}
	allCost += ServerUnitPrice * yuanNodeNum;
	//д�����Ž�
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





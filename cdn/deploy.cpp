#include "deploy.h"
#include "includes.h"
using namespace std;
int NodeNum_Network, LinkNum, NodeNum_Blue, LinkNum_Blue, LinkNum_Red;
double **LinkUnitPrice;//Ϊ�˼�Ȩ�������ɣ��ڴ˴�ʹ��double��
int **LinkUnitPriceReal;//��Ϊ��ͨ���޸ĵ��۵��ڽӾ�����Ѱ��·����������Ҫһ��ԭ���ĵ����ڽӾ���������ʵ�ʻ���
double **LinkGreen, **LinkRed;//��ʼ��Ϊ�㣬����ɫ����ɫ·�����ڽӾ����и�ֵ��ӦȨֵ
double *NodeGreen;//��ʼ��Ϊ�㣬����ɫ�ڵ��λ�ø�ֵȨֵ

Path **allPath;
extern int generation;      //������
extern int best_index, worst_index;
extern int functionmode;
extern int popsize;        //��Ⱥ��С
extern int maxgeneration;  //���������
extern double pc;          //������
extern double pm;          //������
extern struct individual bestindividual, worstindividual, currentbest;  //��Ѹ���
extern struct individual population[POPSIZE];
double time_length;
clock_t start, finish;

double  Weight_GreenLink,Weight_GreenNode,Weight_RedLink;
int NodeStart, NodeEnd;
double realCost;
//----main-----
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename) {
	//�����ʱ������β������performevolution()��
	start = clock();
	finish = start;
	weightsetting();
	get_split_number(topo, line_num);
	input();
	Floyd(LinkUnitPrice,NodeNum_Network, LinkNum);
	generation = 0;
	generateinitialpopulation();
	evaluatepopulation();
	//��ʱ��ͽ�����Ŀ��˫�ؿ��ƣ�����������������������һ������������
	while (time_length < 85) {
		while (generation < maxgeneration) {
			generation++;
			if (generation >= maxgeneration || time_length > 85) {
				goto  jumpout;
			}
			//�����ʱ��
			clock_t finish2 = clock();
			double time_length2 = (double)(finish2 - start) / CLOCKS_PER_SEC;
		//	std::cout << "generation:" << generation << " time_length2=" << time_length2 << " Cost:" << sucessFinish.successAllCost<< std::endl;
			generatenextpopulation();
			evaluatepopulation();
			performevolution();
			outputtextreport();  //for debug
		}
	}
jumpout:
	printf("�����ֵ���ڣ�%f\n", currentbest.fitness);
	//������ʵ����
	realCost = getRealCost(LinkUnitPriceReal, currentbest);
	cout << "real Cost:" << realCost << endl;
	/*
	string outString;
	char charTemp[20];
	//sprintf(charTemp, "%d", sucessFinish.successPathNum);
	_itoa(sucessFinish.successPathNum, charTemp, 10);
	outString = outString + charTemp + "\n\n";
	for (int i = 0; i < sucessFinish.successPathNum; i++) {
		for (int j = 0; j < sucessFinish.successPath[i].pathLenght; j++) {
	//		sprintf(charTemp, "%d", sucessFinish.successPath[i].path[j]);
			_itoa(sucessFinish.successPath[i].path[j], charTemp, 10);
			outString = outString + charTemp + ' ';
		}
//		sprintf(charTemp, "%d", sucessFinish.successPath[i].pathWitch);
			_itoa(sucessFinish.successPath[i].pathWitch, charTemp, 10);
		outString += charTemp;
		outString += '\n';
	}

	//for debug,����ܻ��ѵ��ļ�
	
	//sprintf(charTemp, "%d", sucessFinish.successAllCost);
	_itoa(sucessFinish.successAllCost,charTemp, 10);
	outString += charTemp;
	outString += '\n';

	int allPrice = NodeNum_Demand* ServerUnitPrice;
	_itoa(allPrice, charTemp, 10);
	outString += charTemp;
	//debug end
	
	//����Ҫ���
	//   outString.pop_back();//write_result()�������һ�����з����������ɾȥһ�����з�
	const char *topo_file;
	topo_file = outString.c_str();
	write_result(topo_file, filename);
	*/
	Deallocate_Arrays();
	system("pause");
}
//----------------main end------------------------
void get_split_number(char * topo[MAX_EDGE_NUM], int line_num) {
	vector<int> data;
	vector<int> vec_link;
	vector<int> vec_greenlink;
	vector<int> vec_greenNode;
	vector<int> vec_redlink;
	string temp;
	char *test = topo[0];
	//��õ�һ����Ϣ
	for (int i = 0; test[i] != '\n'; i++) {
		if ((test[i + 1] == ' ') || (test[i + 1] == '\n')) {
			temp += test[i];
			data.push_back(atoi(temp.c_str()));
			temp = "";
		}
		else {
			temp += test[i];
		}
	}

	test = topo[2];
	for (int i = 0; test[i] != '\n'; i++) {
		if ((test[i + 1] == ' ') || (test[i + 1] == '\n')) {
			temp += test[i];
			data.push_back(atoi(temp.c_str()));
			temp = "";
		}
		else {
			temp += test[i];
		}
	}

	NodeNum_Network = data[0];
	LinkNum = data[1];
	NodeNum_Blue = data[2];
	LinkNum_Blue = data[3];
	LinkNum_Red = data[4];

//Allocate
	LinkUnitPrice = new double *[NodeNum_Network];
	LinkUnitPriceReal = new int  *[NodeNum_Network];
	NodeGreen = new double [NodeNum_Network];
	
	LinkGreen = new double *[NodeNum_Network];
	LinkRed = new double *[NodeNum_Network];
	for (int i = 0; i < NodeNum_Network; i++) {
		LinkUnitPrice[i] = new double[NodeNum_Network];
		LinkUnitPriceReal[i] = new int[NodeNum_Network];
		LinkGreen[i] = new double[NodeNum_Network];
		LinkRed[i] = new double[NodeNum_Network];
		memset(LinkUnitPrice[i], 0, NodeNum_Network * sizeof(double));
		memset(LinkUnitPriceReal[i], 0, NodeNum_Network * sizeof(int));
		memset(LinkGreen[i], 0, NodeNum_Network * sizeof(double));
		memset(LinkRed[i], 0, NodeNum_Network * sizeof(double));
	}
		memset(NodeGreen, 0, NodeNum_Network * sizeof(double));//����ɫ�ڵ�ı�ų�ʼ��Ϊ�㣬�ں�ߵ���Ӧ���ֵ���ɫ�ڵ��λ�ø���Ȩ��
	//��ʼ������������ ʹ�ò�ֱ����ͨ����·����Ϊ100000
	for (int i = 0; i < NodeNum_Network; i++) {
		for (int j = 0; j < NodeNum_Network; j++) {
			LinkUnitPrice[i][j] = 100000;
			LinkUnitPriceReal[i][j] = 100000;
		}
	}

// ��㼰�յ�Ľڵ���
	NodeStart = 0;
	NodeEnd = NodeNum_Network - 1;
	//Get link information
	//ǰ2�и�ʽ�̶�,��·��Ϣ�ӵ�3�п�ʼ��
	//�ڶ���ôӵ�3�п�ʼ��������·��Ϣ����
	for (int j = 2; j < LinkNum + 3; j++) {
		char *test = topo[j];
		if (test[0] != '\n') {
			for (int i = 0; test[i] != '\n'; i++) {
				if ((test[i + 1] == ' ') || (test[i + 1] == '\n')) {
					temp += test[i];
					vec_link.push_back(atoi(temp.c_str()));
					temp = "";
				}
				else {
					temp += test[i];
				}
			}
		}
	}
	
	for (int i = 0; i != vec_link.size() / 3; i++) {
		LinkUnitPrice[vec_link[3 * i]][vec_link[3 * i + 1]] = vec_link[3 * i + 2];
		LinkUnitPriceReal[vec_link[3 * i]][vec_link[3 * i + 1]] = vec_link[3 * i + 2];
	}

	for (int i = 0; i < NodeNum_Network; i++) {
		for (int j = i; j < NodeNum_Network; j++) {
			LinkUnitPrice[j][i] = LinkUnitPrice[i][j];
			LinkUnitPriceReal[j][i] = LinkUnitPriceReal[i][j];
		}
	}
	//������greenNode 
	for (int j = LinkNum + 4; j< LinkNum + 4 + NodeNum_Blue; j++) {
		char *test = topo[j];
		for (int i = 0; test[i] != '\0'; i++) {
			if ((test[i + 1] == ' ') || (test[i + 1] == '\0')) {
				temp += test[i];
				vec_greenNode.push_back(atoi(temp.c_str()));
				temp = "";
			}
			else {
				temp += test[i];
			}
		}
	}
	for (unsigned int i = 0; i < vec_greenNode.size(); i++) {
		NodeGreen[vec_greenNode[i]] = Weight_GreenNode;
	}
	//��4�����greenLink
	for (int j = LinkNum + 5 + NodeNum_Blue; j< LinkNum + 5 + NodeNum_Blue + LinkNum_Blue; j++) {
		char *test = topo[j];
		for (int i = 0; test[i] != '\0'; i++) {
			if ((test[i + 1] == ' ') || (test[i + 1] == '\0')) {
				temp += test[i];
				vec_greenlink.push_back(atoi(temp.c_str()));
				temp = "";
			}
			else {
				temp += test[i];
			}
		}
	}
	for (int i = 0; i != vec_greenlink.size() / 2; i++) {
		LinkGreen[vec_greenlink[2 * i]][vec_greenlink[2 * i + 1]] = Weight_GreenLink;
		LinkGreen[vec_greenlink[2 * i + 1]][vec_greenlink[2 * i]] = Weight_GreenLink;
	}
	/*
	//----for debug 
	cout << "greenLink debug:" << endl;
	for (int i=0;i<NodeNum_Network;i++){
		for (int j = 0; j < NodeNum_Network; j++) {
			cout <<"["<< i << " " << j << "] " << LinkGreen[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	//-----debug end
	*/
	//��5,redLink
	for (int j = LinkNum +6  + NodeNum_Blue + LinkNum_Blue; j< line_num; j++) {
		char *test = topo[j];
		for (int i = 0; test[i] != '\0'; i++) {
			if ((test[i + 1] == ' ') || (test[i + 1] == '\0')) {
				temp += test[i];
				vec_redlink.push_back(atoi(temp.c_str()));
				temp = "";
			}
			else {
				temp += test[i];
			}
		}
	}
	for (int i = 0; i != vec_redlink.size() / 2; i++) {
		LinkRed[vec_redlink[2 * i]][vec_redlink[2 * i + 1]] = Weight_RedLink;
		LinkRed[vec_redlink[2 * i + 1]][vec_redlink[2 * i]] = Weight_RedLink;
	}

	//----for debug
	cout << "greenLink debug:" << endl;
	for (int i=0;i<NodeNum_Network;i++){
	for (int j = 0; j < NodeNum_Network; j++) {
	cout <<"["<< i << " " << j << "] " << LinkRed[i][j] << " ";
	}
	cout << endl;
	}
	cout << endl;
	//-----debug end
	
}
void Deallocate_Arrays() {
	for (int i = 0; i < NodeNum_Network; i++) {
		delete[] LinkUnitPrice[i];
		delete[] LinkUnitPriceReal[i];
		delete[] LinkGreen[i];
		LinkUnitPrice[i] = NULL;
		allPath[i] = NULL;
		LinkUnitPriceReal[i] = NULL;
		LinkGreen[i] = NULL;
	}
	delete[] LinkUnitPrice;
	delete[] allPath;
	delete[] NodeGreen;
	delete[] LinkGreen;
	LinkUnitPrice = NULL;
	allPath = NULL;
	NodeGreen = NULL;
	LinkGreen = NULL;
	delete[] LinkUnitPriceReal;
	LinkUnitPriceReal = NULL;
}

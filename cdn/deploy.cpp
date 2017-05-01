#include "deploy.h"
#include "includes.h"
using namespace std;
int NodeNum_Network, LinkNum, NodeNum_Blue, LinkNum_Blue, LinkNum_Red;
int **LinkUnitPrice,**LinkUnitPrice_Ori;//��Ϊ��ͨ���޸ĵ��۵��ڽӾ�����Ѱ��·����������Ҫһ��ԭ���ĵ����ڽӾ���������ʵ�ʻ���
int **LinkGreen;
int **LinkRed;

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
extern Path **allPathTemp;
double time_length;
clock_t start, finish;

vector<int> vec_greenNode;

int Weight_Greenlink;
int NodeStart, NodeEnd;
//----main-----
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename) {
	//�����ʱ������β������performevolution()��
	start = clock();
	finish = start;

	get_split_number(topo, line_num);
	//------ Weight setting---------------
	Weight_Greenlink = 1;
	//--for green link
	LinkUnitPrice[2][4] = 0.1;
	LinkUnitPrice[4][2] = 0.1;
	LinkUnitPrice[14][13] = 0.1;
	LinkUnitPrice[13][14] = 0.1;
	//--for green node
	LinkUnitPrice[3][7] = 0.1;
	LinkUnitPrice[7][3] = 0.1;
	LinkUnitPrice[6][7] = 0.1;
	LinkUnitPrice[7][6] = 0.1;
	LinkUnitPrice[8][7] = 0.1;
	LinkUnitPrice[7][8] = 0.1;
	// for red link
	LinkUnitPrice[11][12] = 999;
	LinkUnitPrice[12][11] = 999;
//----------Weight setting end--------------	
	 
	Allocate_result();

	Floyd(NodeNum_Network, LinkNum);

	generation = 0;
	input();

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
}

//----------------main end------------------------

void get_split_number(char * topo[MAX_EDGE_NUM], int line_num) {
	vector<int> data;
	vector<int> vec_link;
	vector<int> vec_greenlink;
	vector<int> vec_rednode;
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
	LinkUnitPrice = new int *[NodeNum_Network];
	LinkUnitPrice_Ori = new int *[NodeNum_Network];
	for (int i = 0; i < NodeNum_Network; i++) {
		LinkUnitPrice[i] = new int[NodeNum_Network];
		LinkUnitPrice_Ori[i] = new int[NodeNum_Network];
		memset(LinkUnitPrice[i], 0, NodeNum_Network * sizeof(int));
		memset(LinkUnitPrice_Ori[i], 0, NodeNum_Network * sizeof(int));
	}
	//��ʼ������������ ʹ�ò�ֱ����ͨ����·����Ϊ100000 ���i=j����ʽΪ֮�����·����׼��
	for (int i = 0; i < NodeNum_Network; i++) {
		for (int j = 0; j < NodeNum_Network; j++) {
			LinkUnitPrice[i][j] = 1000;
			LinkUnitPrice_Ori[i][j] = 1000;
		}
	}
	for (int i = 0; i != vec_link.size() / 3; i++) {
		LinkUnitPrice[vec_link[3 * i]][vec_link[3 * i + 1]] = vec_link[3 * i + 2];
		LinkUnitPrice_Ori[vec_link[3 * i]][vec_link[3 * i + 1]] = vec_link[3 * i + 2];
	}
/*
	for (int i = 0; i != vec_link.size() / 3; i++) {
		cout << vec_link[3 * i] << ' ' << vec_link[3 * i + 1]<< ' ' << LinkUnitPrice[vec_link[3 * i]][vec_link[3 * i + 1]] << endl;
	}
*/
	for (int i = 0; i < NodeNum_Network; i++) {
		for (int j = i; j < NodeNum_Network; j++) {
			LinkUnitPrice[j][i] = LinkUnitPrice[i][j];
		//	cout << LinkUnitPrice[i][j] << endl;
		}
	}
	//������Get green node information
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
	//��4�����
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

	LinkGreen = new int *[NodeNum_Network];
	for (int i = 0; i < NodeNum_Network; i++) {
		LinkGreen[i] = new int[NodeNum_Network];
		memset(LinkGreen[i], 0, NodeNum_Network * sizeof(int));
	}
	for (int i = 0; i != vec_greenlink.size() / 2; i++) {
		LinkGreen[vec_greenlink[2 * i]][vec_greenlink[2 * i + 1]] = Weight_Greenlink;
	}

	//��5
	for (int j = LinkNum +5  + NodeNum_Blue + LinkNum_Blue; j< line_num; j++) {
		char *test = topo[j];
		for (int i = 0; test[i] != '\0'; i++) {
			if ((test[i + 1] == ' ') || (test[i + 1] == '\0')) {
				temp += test[i];
				vec_rednode.push_back(atoi(temp.c_str()));
				temp = "";
			}
			else {
				temp += test[i];
			}
		}
	}
	//remember to De-allocate arrays
}

void Deallocate_Arrays() {
	for (int i = 0; i < NodeNum_Network; i++) {
		delete[] LinkUnitPrice[i];
		delete[] allPath[i];
		LinkUnitPrice[i] = NULL;
		allPath[i] = NULL;
		delete[] allPathTemp[i];
		allPathTemp[i] = NULL;
	}
	delete[] LinkUnitPrice;
	delete[] allPath;
	LinkUnitPrice = NULL;
	allPath = NULL;
	delete[] allPathTemp;
	allPathTemp = NULL;
	
}

#include "includes.h"
using namespace std;
int generation, maxgeneration;      //������,���������
int best_index, worst_index;
int functionmode = maximization;
double pc, pm;          //�����ʣ�������
struct individual bestindividual,worstindividual, currentbest;  //��Ѹ���,������
struct individual population[POPSIZE];
int chromlength;
extern int NodeNum_Network, LinkNum;
extern double time_length;
extern clock_t start, finish;
clock_t finish3;
extern Path **allPath;
extern int NodeStart, NodeEnd;
extern int **LinkUnitPriceReal;//Ϊ��Ȩ���ɣ���LinkUnitPrice is double, let LinkUnitPriceReal is int
extern double Weight_GreenLink,Weight_GreenNode;
extern double **LinkUnitPrice;
extern double **LinkGreen, **LinkRed;//��ʼ��Ϊ�㣬����ɫ����ɫ·�����ڽӾ����и�ֵ��ӦȨֵ
extern double *NodeGreen;//��ʼ��Ϊ�㣬����ɫ�ڵ��λ�ø�ֵȨֵ
void weightsetting() {
	//------ Weight setting---------------
	Weight_GreenLink = 500;
	Weight_GreenNode = 500;
}
void input() {//��������
	printf("��ʼ��ȫ�ֱ���:\n");
	//���������(100-300)
	maxgeneration = 20;
	//������(0.2-0.99)
	pc = 0.8;
	//������(0.001-0.1)
	pm = 0.3;

	////------ Weight setting---------------
	//Weight_GreenLink = 1;
	//Weight_GreenNode = 1;
	for (int i = 0; i < NodeNum_Network; i++) {
		for (int j = 0; j < NodeNum_Network; j++) {
			LinkUnitPrice[i][j] = 100 * LinkUnitPrice[i][j];
		}
	}
	/*
	//--for green link

	LinkUnitPrice[2][4] = 0;
	LinkUnitPrice[4][2] = 0;
	LinkUnitPrice[14][13] = 0;
	LinkUnitPrice[13][14] = 0;

	//--for green node
	
	LinkUnitPrice[3][7] *= 0.1;
	LinkUnitPrice[7][3] *= 0.1;
	LinkUnitPrice[6][7] *= 0.1;
	LinkUnitPrice[7][6] *= 0.1;
	LinkUnitPrice[8][7] *= 0.1;
	LinkUnitPrice[7][8] *= 0.1;

	LinkUnitPrice[10][12] *= 0.1;
	LinkUnitPrice[12][10] *= 0.1;
	LinkUnitPrice[5][12] *= 0.1;
	LinkUnitPrice[12][5] *= 0.1;
	LinkUnitPrice[6][12] *= 0.1;
	LinkUnitPrice[12][6] *= 0.1;
	LinkUnitPrice[12][13] *= 0.1;
	LinkUnitPrice[13][12] *= 0.1;
	LinkUnitPrice[12][16] *= 0.1;
	LinkUnitPrice[16][12] *= 0.1;
*/
	// for red link
	LinkUnitPrice[11][12] = 999;
	LinkUnitPrice[12][11] = 999;
	//----------Weight setting end--------------	
}
void generateinitialpopulation() { //��Ⱥ��ʼ��
	// srand((unsigned)time(NULL));
	int k1,k2;
	for (int i = 0; i < POPSIZE; i++) {
		int k = rand() % (NodeEnd - NodeStart - 1) + 1;
		//int k1 = rand() % (NodeEnd - NodeStart - 1) + 1;
		do {
		 k1 = rand() % (NodeEnd - NodeStart - 1) + 1;
		 k2 = rand() % (NodeEnd - NodeStart - 1) + 1;
		} while (k1 == k || k2 == k || k1 == k2 );

		cout << "k:" << k << " k1:"<< k1 << endl; 
		for (int m = 0; m < allPath[NodeStart][k].pathLenght; m++) {
			population[i].chrom.push_back(allPath[NodeStart][k].path[m]);
		}
		
		for (int m = 1; m < allPath[k][k1].pathLenght; m++) {
			population[i].chrom.push_back(allPath[k][k1].path[m]);
		}
		for (int m = 1; m < allPath[k1][k2].pathLenght; m++) {
			population[i].chrom.push_back(allPath[k1][k2].path[m]);
		}
		//�˴�n��1��ʼ������д���Ⱦɫ��Ὣ�м�ֵk�ظ�����
		for (int n = 1; n < allPath[k2][NodeEnd].pathLenght; n++) {
			population[i].chrom.push_back(allPath[k2][NodeEnd].path[n]);
		}
		/*
		//�˴�n��1��ʼ������д���Ⱦɫ��Ὣ�м�ֵk�ظ�����
		for (int n = 1; n < allPath[k][NodeEnd].pathLenght; n++) {
			population[i].chrom.push_back(allPath[k][NodeEnd].path[n]);
		}
		*/
	}
	displayChroms("Initialize,BeforeDeleteCycle");

	deleteCloseCycles();	//ȥ����

	displayChroms("Initialize,AfterDeleteCycles");
}
void generatenextpopulation() { //������һ��
	selectoperator();
	crossoveroperator();
	mutationoperator();
}
void evaluatepopulation() {  //���۸��壬����Ѹ���
	calculateobjectvalue();
	calculatefitnessvalue();
	findbestandworstindividual();

	for (int i = 0; i < POPSIZE; i++) {
		cout << "population[" << i << "] ";
		for (unsigned int ii = 0; ii <population[i].chrom.size(); ii++) {
		cout <<  population[i].chrom[ii] << " ";
	}
		cout << " cost:" << population[i].value << " fitness:" << population[i].fitness << endl;
	}
	//displayChroms("evaluatePopulation");
}
//todo:����û�м����·�����Լ����
void calculateobjectvalue() { //���㺯��ֵ,
	for (int i = 0; i<POPSIZE; i++) {
		population[i].value = 0;
		for (unsigned int j = 0; j != population[i].chrom.size() - 1; j++) {
			population[i].value += (allPath[population[i].chrom[j]][population[i].chrom[j + 1]].pathCost
				- LinkGreen[population[i].chrom[j]][population[i].chrom[j + 1]]
				- NodeGreen[population[i].chrom[j]]);
		}
		//population[i].value = 10000  / population[i].value;
		population[i].value = 10000-(population[i].value);
	}
}
void calculatefitnessvalue() {//������Ӧ��
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
		//population[i].fitness = temp;
		population[i].fitness = population[i].value;
	}
}
void findbestandworstindividual() { //����Ѹ����������
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
void performevolution() {//��ʾ���۽��
	if (bestindividual.fitness>currentbest.fitness) {
		currentbest = population[best_index];
	}
	else {
		population[worst_index] = currentbest;
	}
	//�����ʱ��
	finish = clock();
	time_length = (double)(finish - start) / CLOCKS_PER_SEC;
	//  std::cout << "GA157:time_length=" << time_length << std::endl;
}
void selectoperator() {//����ѡ���㷨
	int i, index;
	double p, sum = 0.0;
	double cfitness[POPSIZE];

	struct individual newpopulation[POPSIZE];
	for (i = 0; i<POPSIZE; i++) {
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
void crossoveroperator() {//�����㷨
	int i, j;
	int index[POPSIZE];
	int point, temp;
	double p;
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
		//p = 0.01;//for debug
		if (p<pc) {
			cout << p << " " << pc << " cross start" << endl;
			point = rand() % (chromlength - 1) + 1;
			vector<int> repeatNode;
			for (unsigned int j = 0; j<population[index[i]].chrom.size(); j++) {
				for (unsigned int k = 0; k<population[index[i + 1]].chrom.size(); k++) {
					if (population[index[i]].chrom[j] == population[index[i + 1]].chrom[k]) {
						repeatNode.push_back(population[index[i]].chrom[j]);
						break;
					}
				}
			}
			if (repeatNode.size()>1) {             //���ظ��ڵ����1��ʱ��Ž��н���
				int firstNode = 0;                    //ѡ�񽻲�ĵ�һ����
				int secondNode = 0;                   //ѡ�񽻲�ĵڶ�����
				int crossoverPosition[4];             //��¼�ظ��ڵ������Ⱦɫ���е�λ��
				int PositionTemp;                     //��ʱ�����ظ��ڵ������Ⱦɫ���е�λ��
				vector<int> chromTemp1;                //��ʱ����Ⱦɫ��
				vector<int> chromTemp2;                //��ʱ����Ⱦɫ��
				firstNode = repeatNode[rand() % repeatNode.size()];
				do {
					secondNode = repeatNode[rand() % repeatNode.size()];
				} while (secondNode == firstNode);

				for (unsigned int j = 0; j<population[index[i]].chrom.size(); j++) {
					if (population[index[i]].chrom[j] == firstNode) { crossoverPosition[0] = j; }
					if (population[index[i]].chrom[j] == secondNode) { crossoverPosition[1] = j; }
				}
				for (unsigned int j = 0; j<population[index[i + 1]].chrom.size(); j++) {
					if (population[index[i + 1]].chrom[j] == firstNode) { crossoverPosition[2] = j; }
					if (population[index[i + 1]].chrom[j] == secondNode) { crossoverPosition[3] = j; }
				}
				if (crossoverPosition[1]<crossoverPosition[0]) {
					PositionTemp = crossoverPosition[0];
					crossoverPosition[0] = crossoverPosition[1];
					crossoverPosition[1] = PositionTemp;
				}
				if (crossoverPosition[3]<crossoverPosition[2]) {
					PositionTemp = crossoverPosition[2];
					crossoverPosition[2] = crossoverPosition[3];
					crossoverPosition[3] = PositionTemp;
				}
				chromTemp1 = population[index[i]].chrom;
				population[index[i]].chrom.clear();

				//���ɵ�һ��
				for (int j = 0; j<crossoverPosition[0]; j++) {
					population[index[i]].chrom.push_back(chromTemp1[j]);
				}
				for (int k = crossoverPosition[2]; k <= crossoverPosition[3]; k++) {
					population[index[i]].chrom.push_back(population[index[i + 1]].chrom[k]);
				}
				for (unsigned int k = crossoverPosition[1] + 1; k<chromTemp1.size(); k++) {
					population[index[i]].chrom.push_back(chromTemp1[k]);
				}
				//���ɵڶ���
				chromTemp2 = population[index[i + 1]].chrom;
				population[index[i + 1]].chrom.clear();
				for (int j = 0; j<crossoverPosition[2]; j++) {
					population[index[i + 1]].chrom.push_back(chromTemp2[j]);
				}
				for (int k = crossoverPosition[0]; k <= crossoverPosition[1]; k++) {
					population[index[i + 1]].chrom.push_back(chromTemp1[k]);
				}
				for (unsigned int k = crossoverPosition[3] + 1; k<chromTemp2.size(); k++) {
					population[index[i + 1]].chrom.push_back(chromTemp2[k]);
				}
			}
			//deleteCloseCycles();//Delete all closed cycles
		}
	}
	//for debug
//	displayChroms("after cross");
}
void mutationoperator() {//�������
	double p;
	for (int i = 0; i<POPSIZE; i++) {
		//srand((unsigned)time(NULL));
			p = rand() % 1000 / 1000.0;
			if (p < pm) {
				cout <<p << " "<< pm <<  " mutation start" << endl;
				//	srand((unsigned)time(NULL));
				int ks = rand() % (population[i].chrom.size() - 0 - 3) + 1;//(0,chrom.size()-3] ,�˴����±꣬���ǽڵ��ű���
				int kt = ks + 2;
				int k = rand() % (NodeEnd - NodeStart - 1) + 1;//(NodeStart,NodeEnd)
				int k1,k2;
				//��ֹ����ʱ��ȡ���������λ���м������ͬ,����ʱ���п�����ͬ
				do {
					k = rand() % (NodeEnd - NodeStart - 1) + 1;
					k1 = rand() % (NodeEnd - NodeStart - 1) + 1;
					k2 = rand() % (NodeEnd - NodeStart - 1) + 1;
				} while (k == ks || k == kt || k == k1 || k == k2 || ks == k1 || ks == k2 || kt ==k1 || kt ==k2);
				population[i].chrom.erase(population[i].chrom.begin() + ks + 1, population[i].chrom.begin() + kt);//ɾ��ks��kt֮���Ԫ��,����ks��kt

				// --for debug
				//displayChroms("--after erase");

				//�˴�n��1��ʼ������д���Ⱦɫ��Ὣ�м�ֵk�ظ�����
				kt = kt - 1;//erase�����������˱仯����ks��ʼ����1��;
				int Length_k_kt = allPath[k][population[i].chrom[kt]].pathLenght;

				//--for debug 
				/*
				cout << "k-kt:";
				for (int n = 0; n < Length_k_kt; n++) {
					cout << allPath[k][population[i].chrom[kt]].path[n] << " ";
				}
				cout << endl;
				*/
				//---debug end	
				for (int n = 1; n < Length_k_kt; n++) {
					population[i].chrom.insert(population[i].chrom.begin() + kt,
						allPath[k][population[i].chrom[kt]].path[Length_k_kt - n - 1]);//���µ�k-kt��·��д��ktǰ��
				}
				int Length_ks_k = allPath[population[i].chrom[ks]][k].pathLenght;
				//---------for debug---
				/*
				cout << "ks-k:";
				for (int m = 0; m < Length_ks_k; m++) {//Length_ks_k-1����Ϊ������ԭ������λ��erase���ұ�δɾ�����ظ�
					cout << allPath[population[i].chrom[ks]][k].path[m] << " ";
				}
				cout << endl;
				*/
				//---debug end---
				for (int m = 1; m < Length_ks_k - 1; m++) {//Length_ks_k-1����Ϊ������ԭ������λ��erase���ұ�δɾ�����ظ�
					population[i].chrom.insert(population[i].chrom.begin() + kt,
						allPath[population[i].chrom[ks]][k].path[Length_ks_k - m - 1]);// ���µ�ks-k��·��д��ktǰ��
				}
				//displayChroms("after mutation");//for debug
				deleteCloseCycles();//Delete all closed cycles
			//displayChroms("AfterMutationAndDeleteCycles:");//---for debug---
			}
		}
	}
void outputtextreport() {//�������
	double sum;
	double average;
	sum = 0.0;
	for (int i = 0; i<POPSIZE; i++) {
		sum += population[i].value;
	}
	average = sum / POPSIZE;
	printf("��ǰ����=%d\n��ǰ����ƽ������ֵ=%f\n��ǰ������ߺ���ֵ=%f\n", generation, average, population[best_index].value);
	cout << "chrom:";
	for (unsigned int i = 0; i < population[best_index].chrom.size(); i++) {
		cout << population[best_index].chrom[i] << "->";
	}
	cout << endl;
}
void deleteCloseCycles() {//ȥ�������β�Ϊ��i��Ⱦɫ��
	for (int i = 0; i < POPSIZE; i++) {
		int dupIndex_end;
		for (unsigned int ii = 0; ii < population[i].chrom.size(); ii++) {
			dupIndex_end = ii;
			for (unsigned int j = ii + 1; j < population[i].chrom.size(); j++) {
				if (population[i].chrom[ii] == population[i].chrom[j]) {
					dupIndex_end = j;
				}
			}
			if (dupIndex_end > ii) {
				population[i].chrom.erase(population[i].chrom.begin() + ii + 1, population[i].chrom.begin() + dupIndex_end + 1);
			}
		}
	}
}
//debug function
void displayChroms(std::string debugName) {
	cout << "--For debug:" << debugName << endl;
	for (int i = 0; i < POPSIZE; i++) {
		for (unsigned int ii = 0; ii <population[i].chrom.size(); ii++) {
			cout << population[i].chrom[ii] << " ";
		}
		cout << endl;
	}
	cout << endl;
}
double getRealCost(int**LinkUnitPriceReal, individual currentbest) {
	double costTmp=0;
	for (unsigned int i = 0; i < currentbest.chrom.size()-1; i++) {
	//	for (unsigned int j = i + 1; j < currentbest.chrom.size(); j++) {
		//int j = i + 1;
			costTmp += LinkUnitPriceReal[currentbest.chrom[i]][currentbest.chrom[i+1]];
	//	}
	}
	return costTmp;
}

/*
�û�ȫ�ֱ�������ĵط�
*/
#include "basicservice.h"
#include <QString>

unsigned int aint = 0;
unsigned int bint = 200;
int cint = 0;
unsigned short dushort = 0;
short eshort = 0;
unsigned long long full = 0;
long long gll = 0;
unsigned long hul = 0;
long il = 0;
float jf = 0.0;
double kd = 0.0;

int iiii = 1;
int variii[1000];

//ע���û�ȫ�ֱ���
void initGlobalvar(void)
{

	//��ͨȫ�ֱ���
	var_register("����ͨѶ����",&aint);
	var_register("bint",&bint);
	var_register("cint",&cint);
	var_register("dushort",&dushort);
	var_register("eshort",&eshort);
	var_register("full",&full);
	var_register("gll",&gll);
	var_register("hul",&hul);
	var_register("il",&il);


	//����ȫ�ֱ���
	var_register("jf",&jf,true);
	var_register("kd",&kd,true);
}
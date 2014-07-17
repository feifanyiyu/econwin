/*
���ļ�ʵ���ⲿ�����豸�Ķ���
ÿ���豸Ҫʵ������3��ӿ�
//�豸��ʼ��
void tdevice107_init(struct tdevice107* pdevice,unsigned int id);
//�豸�������ݴ���ص��ӿ�
void tdevice107_in_handler(void *p);
//�豸������ݴ���ص��ӿ�
void tdevice107_out_handler(void *p);
*/
#ifndef DEVICE_H
#define DEVICE_H
#include <QList>
#include "lua.hpp"

extern QList<struct _devicenode> devicenodes;

//�ⲿ�豸�������ã�ÿ���豸�Ľṹ�ʹ�����һ���������ж����ⲿ�豸�����ж��ٶ�������

//����Զ˿ڽ��в����Ĺ���
struct toutportbit{
	unsigned char out0:1;
	unsigned char out1:1;
	unsigned char out2:1;
	unsigned char out3:1;
	unsigned char out4:1;
	unsigned char out5:1;
	unsigned char out6:1;
	unsigned char out7:1;
};

union outport{
	struct toutportbit outportbit;
	unsigned char outportall;
};

struct tinportbit{
	unsigned char in0:1;
	unsigned char in1:1;
	unsigned char in2:1;
	unsigned char in3:1;
	unsigned char in4:1;
	unsigned char in5:1;
	unsigned char in6:1;
	unsigned char in7:1;
};

union inport{
	struct tinportbit inportbit;
	unsigned char inportall;
};

struct port{
	unsigned char type;//�˿�����0:input8 1:input16 2:output8 3:output16 4:DA 6:AD 7
	void* pv;
	bool change;//�˿ڸı��ʶ����Ҫ��lua�ռ��豸��
};

//�豸�ڵ�
struct _devicenode{
	unsigned int id;
	unsigned char moduletype;
	void *pdevice;
};
//107�豸
struct tdevice107
{
   	unsigned char sendbuff[1500];
   	unsigned char recvbuff[1500];
   	unsigned int id;
	unsigned char moduletype;//ģ������

	struct port ports[2];

	union outport  outval;
	union inport   inval;
	unsigned char luaflag;
};


//�豸�ĳ�ʼ������
struct tdevice107* tdevice107_init(unsigned int id,unsigned char luaflag = 0);
int mytdevice107_init(lua_State* L);

//�豸�������ݴ���ص��ӿ�
void tdevice107_in_handler(void *p);

//�豸������ݴ���ص��ӿ�
void tdevice107_out_handler(void *p);

//207�豸
struct tdevice207
{
   	unsigned char sendbuff[1500];
   	unsigned char recvbuff[1500];
   	unsigned int id;
	unsigned char moduletype;//ģ������

	struct port ports[4];

	union outport  outval0;
	union outport  outval1;

	union inport  inval0;
	union inport  inval1;

	unsigned char luaflag;
};


//�豸�ĳ�ʼ������
struct tdevice207*  tdevice207_init(unsigned int id,unsigned char luaflag = 0);
int mytdevice207_init(lua_State* L);

//�豸�������ݴ���ص��ӿ�
void tdevice207_in_handler(void *p);

//�豸������ݴ���ص��ӿ�
void tdevice207_out_handler(void *p);

#endif /* DEVICE_H */
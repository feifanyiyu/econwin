/*
�û������Լ�����ĵط�
*/
#include "basicservice.h"
#include "controltask.h"
#include <stdio.h>
#include "mainloop.h"
#include "device.h"

extern unsigned int bint;

//�ֲ������ͺ���
static bool retval = true;

static TON timer1(200);

static int plctask1step = 0;

//plc����1ִ����
void plctask1run(void);
//plc����2ִ����
void plctask2run(void);
//plc����3ִ����
void plctask3run(void);

//ע���û�����
void init_usertask(void)
{
	//task_register("plc1",0,plctask1run);//ÿ��ͨѶѭ����ִ��
}
extern struct tdevice207* id1;

//plc����1ִ����
void plctask1run(void)
{
	if(timer1.Q)
	{
		timer1.setms(bint);
		retval = false;
		if(plctask1step == 0)
		{
			plctask1step = 1;
			id1->outval0.outportbit.out0 = 1;
			id1->outval0.outportbit.out1 = 0;
			id1->outval0.outportbit.out2 = 0;
			id1->outval0.outportbit.out3 = 0;
			//id1.sendbuff[0] = 0x01;
		}
		else if(plctask1step == 1)
		{
			plctask1step = 2;
			id1->outval0.outportbit.out0 = 0;
			id1->outval0.outportbit.out1 = 1;
			id1->outval0.outportbit.out2 = 0;
			id1->outval0.outportbit.out3 = 0;

			//id1.sendbuff[0] = 0x02;
		}
		else if(plctask1step == 2)
		{
			plctask1step = 3;
			id1->outval0.outportbit.out0 = 0;
			id1->outval0.outportbit.out1 = 0;
			id1->outval0.outportbit.out2 = 1;
			id1->outval0.outportbit.out3 = 0;
			//id1.sendbuff[0] = 0x04;

		}
		else if(plctask1step == 3)
		{
			plctask1step = 0;
			id1->outval0.outportbit.out0 = 0;
			id1->outval0.outportbit.out1 = 0;
			id1->outval0.outportbit.out2 = 0;
			id1->outval0.outportbit.out3 = 1;

			//id1.sendbuff[0] = 0x08;
		}

		//printf("1\n");
	}
	timer1.checkin(retval);
	if(retval == false)
		retval = true;
}


//�ֲ������ͺ���
static bool retval2 = true;
static TON timer2(1000);
static R_TRIG R_trig_timer;
static F_TRIG F_trig_timer;
static void testfunc(void)
{
}


//plc����2ִ����
void plctask2run(void)
{
	if(timer2.Q)
	{
		retval2 = false;
	}
	if(R_trig_timer.Q)
	{
		//printf("2\n");
	}
	if(F_trig_timer.Q)
	{
		//printf("3\n");
	}
	timer2.checkin(retval2);
	R_trig_timer.checkin(timer2.Q);
	F_trig_timer.checkin(timer2.Q);
	if(retval2 == false)
		retval2 = true;
}


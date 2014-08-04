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
#include "device.h"
#include "mainloop.h"

#include <windows.h>
extern lua_State* L;

QList<struct _devicenode> devicenodes;

//�豸��ʼ��
struct tdevice107* tdevice107_init(unsigned int id,unsigned char luaflag)
{
	//�����ڴ�
	struct tdevice107* pdevice = (struct tdevice107*)malloc(sizeof(struct tdevice107));
	//id
	pdevice->id = id;

	//�ڴ��ʼ��
	memset(pdevice->sendbuff,0,1);
	memset(pdevice->recvbuff,0,1);

	//ģ������
	pdevice->moduletype = 1;

	//�˿�����
	pdevice->ports[0].type = 2;
	pdevice->ports[0].pv = (void *)(&(pdevice->outval));
	pdevice->ports[0].change = false;

	//�˿�����
	pdevice->ports[1].type = 0;
	pdevice->ports[1].pv = (void *)(&(pdevice->inval));
	pdevice->ports[1].change = false;

	pdevice->luaflag = luaflag;

	//ע�ᵽȫ��ģ���б�
	struct _devicenode t;
	t.id = id;
	t.moduletype = pdevice->moduletype;
	t.pdevice = (void *)pdevice;
    devicenodes.append(t);

	//ע�ᵽͨѶ�ڵ�ģ���б�
	struct tcommnode comnode;
	comnode.id = id;
	comnode.pcommsendbuff =pdevice->sendbuff;
	comnode.PDOwritelen = 1;
	comnode.PDOwriteaddr = 0;

	comnode.pcommrecvbuff = pdevice->recvbuff;
	comnode.PDOreadlen = 1;
	comnode.PDOreadaddr = 0;
	comnode.pdevice =(void *)pdevice;
	comnode.in_handle = tdevice107_in_handler;
	comnode.out_handle = tdevice107_out_handler;

	commnodes.append(comnode);

	return pdevice;
}
//�ṩ��lua��ע��ӿ�
int mytdevice107_init(lua_State* L)
{
	int id = (int)luaL_checknumber(L,1);
	tdevice107_init(id,1);
	return 0;
}

//�豸�������ݴ���ص��ӿ�
void tdevice107_in_handler(void *p)
{
	struct tdevice107* pdevice = (struct tdevice107*)p;
	//��������ӳ��
	pdevice->inval.inportall = pdevice->recvbuff[0];
	if(pdevice->luaflag == 1)
	{
		lua_getglobal(L,"set107inport");
		lua_pushinteger(L,pdevice->id);
		lua_pushboolean(L,pdevice->inval.inportbit.in0);
		lua_pushboolean(L,pdevice->inval.inportbit.in1);
		lua_pushboolean(L,pdevice->inval.inportbit.in2);
		lua_pushboolean(L,pdevice->inval.inportbit.in3);
		lua_pushboolean(L,pdevice->inval.inportbit.in4);
		lua_pushboolean(L,pdevice->inval.inportbit.in5);
		lua_pushboolean(L,pdevice->inval.inportbit.in6);
		lua_pushboolean(L,pdevice->inval.inportbit.in7);
		lua_pcall(L,9,0,0);
	}
}


//�豸������ݴ���ص��ӿ�
void tdevice107_out_handler(void *p)
{
	struct tdevice107* pdevice = (struct tdevice107*)p; 
	if(pdevice->luaflag == 1)
	{
		if(pdevice->ports[0].change == true)
		{
			pdevice->ports[0].change = false;
			lua_getglobal(L,"set107outport");
			lua_pushinteger(L,pdevice->id);
			lua_pushboolean(L,pdevice->outval.outportbit.out0);
			lua_pushboolean(L,pdevice->outval.outportbit.out1);
			lua_pushboolean(L,pdevice->outval.outportbit.out2);
			lua_pushboolean(L,pdevice->outval.outportbit.out3);
			lua_pushboolean(L,pdevice->outval.outportbit.out4);
			lua_pushboolean(L,pdevice->outval.outportbit.out5);
			lua_pushboolean(L,pdevice->outval.outportbit.out6);
			lua_pushboolean(L,pdevice->outval.outportbit.out7);
			lua_pcall(L,9,0,0);
		}
		else
		{
			lua_getglobal(L,"get107outport");
			lua_pushinteger(L,pdevice->id);
			lua_pcall(L,1,1,0);
			if(lua_isnumber(L,-1))
			{
				pdevice->outval.outportall = lua_tointeger(L,-1);
				lua_pop(L,1);
			}
		}
	}
	
	//��������ӳ��
	pdevice->sendbuff[0] = pdevice->outval.outportall;
}


//�豸�ĳ�ʼ������
struct tdevice207*  tdevice207_init(unsigned int id,unsigned char luaflag)
{
	//�ڴ����
	struct tdevice207* pdevice = (struct tdevice207*)malloc(sizeof(struct tdevice207));

	//id
	pdevice->id = id;
	//��ʼ���ڴ�
	memset(pdevice->sendbuff,0,2);
	memset(pdevice->recvbuff,0,2);

	//ģ������
	pdevice->moduletype = 2;

	//�˿�����
	pdevice->ports[0].type = 0;
	pdevice->ports[0].pv = (void *)(&(pdevice->inval0));
	pdevice->ports[0].change = false;

	//�˿�����
	pdevice->ports[1].type = 0;
	pdevice->ports[1].pv = (void *)(&(pdevice->inval1));
	pdevice->ports[1].change = false;

	//�˿�����
	pdevice->ports[2].type = 2;
	pdevice->ports[2].pv = (void *)(&(pdevice->outval0));
	pdevice->ports[2].change = false;

	//�˿�����
	pdevice->ports[3].type = 2;
	pdevice->ports[3].pv = (void *)(&(pdevice->outval1));
	pdevice->ports[3].change = false;

	pdevice->luaflag = luaflag;

	//ע�ᵽȫ��ģ���б�
	struct _devicenode t;
	t.id = id;
	t.moduletype = pdevice->moduletype;
	t.pdevice = (void *)pdevice;
	devicenodes.append(t);


	//ע�ᵽͨѶ�ڵ�ģ���б�
	struct tcommnode comnode;
	comnode.id = id;
	comnode.pcommsendbuff =pdevice->sendbuff;
	comnode.PDOwritelen = 2;
	comnode.PDOwriteaddr = 0;
	comnode.pcommrecvbuff = pdevice->recvbuff;
	comnode.PDOreadlen = 2;
	comnode.PDOreadaddr = 0;
	comnode.pdevice =(void *)pdevice;
	comnode.in_handle = tdevice207_in_handler;
	comnode.out_handle = tdevice207_out_handler;

	commnodes.append(comnode);

	return pdevice;
}

//�ṩ��lua��ע��ӿ�
int mytdevice207_init(lua_State* L)
{
	int id = (int)luaL_checknumber(L,1);
	tdevice207_init(id,1);
	return 0;
}

//�豸�������ݴ���ص��ӿ�
void tdevice207_in_handler(void *p)
{
	struct tdevice207* pdevice = (struct tdevice207*)p;
	//��������ӳ��
	pdevice->inval0.inportall = pdevice->recvbuff[0];
	pdevice->inval1.inportall = pdevice->recvbuff[1];
	if(pdevice->luaflag == 1)
	{
		lua_getglobal(L,"set207inport");
		lua_pushinteger(L,pdevice->id);
		lua_pushboolean(L,pdevice->inval0.inportbit.in0);
		lua_pushboolean(L,pdevice->inval0.inportbit.in1);
		lua_pushboolean(L,pdevice->inval0.inportbit.in2);
		lua_pushboolean(L,pdevice->inval0.inportbit.in3);
		lua_pushboolean(L,pdevice->inval0.inportbit.in4);
		lua_pushboolean(L,pdevice->inval0.inportbit.in5);
		lua_pushboolean(L,pdevice->inval0.inportbit.in6);
		lua_pushboolean(L,pdevice->inval0.inportbit.in7);
		lua_pcall(L,9,0,0);

		lua_getglobal(L,"set207inport2");
		lua_pushinteger(L,pdevice->id);
		lua_pushboolean(L,pdevice->inval1.inportbit.in0);
		lua_pushboolean(L,pdevice->inval1.inportbit.in1);
		lua_pushboolean(L,pdevice->inval1.inportbit.in2);
		lua_pushboolean(L,pdevice->inval1.inportbit.in3);
		lua_pushboolean(L,pdevice->inval1.inportbit.in4);
		lua_pushboolean(L,pdevice->inval1.inportbit.in5);
		lua_pushboolean(L,pdevice->inval1.inportbit.in6);
		lua_pushboolean(L,pdevice->inval1.inportbit.in7);
		lua_pcall(L,9,0,0);

	}

}

//�豸������ݴ���ص��ӿ�
void tdevice207_out_handler(void *p)
{
	struct tdevice207* pdevice = (struct tdevice207*)p;

	if(pdevice->luaflag == 1)
	{
		if(pdevice->ports[2].change == true)
		{
			pdevice->ports[2].change = false;
			lua_getglobal(L,"set207outport");
			lua_pushinteger(L,pdevice->id);
			lua_pushboolean(L,pdevice->outval0.outportbit.out0);
			lua_pushboolean(L,pdevice->outval0.outportbit.out1);
			lua_pushboolean(L,pdevice->outval0.outportbit.out2);
			lua_pushboolean(L,pdevice->outval0.outportbit.out3);
			lua_pushboolean(L,pdevice->outval0.outportbit.out4);
			lua_pushboolean(L,pdevice->outval0.outportbit.out5);
			lua_pushboolean(L,pdevice->outval0.outportbit.out6);
			lua_pushboolean(L,pdevice->outval0.outportbit.out7);
			lua_pcall(L,9,0,0);
		}
		else
		{
			lua_getglobal(L,"get207outport");
			lua_pushinteger(L,pdevice->id);
			lua_pcall(L,1,1,0);
			if(lua_isnumber(L,-1))
			{
				pdevice->outval0.outportall = lua_tointeger(L,-1);
				lua_pop(L,1);
			}

		}
		if(pdevice->ports[3].change == true)
		{
			pdevice->ports[3].change = false;
			lua_getglobal(L,"set207outport2");
			lua_pushinteger(L,pdevice->id);
			lua_pushboolean(L,pdevice->outval1.outportbit.out0);
			lua_pushboolean(L,pdevice->outval1.outportbit.out1);
			lua_pushboolean(L,pdevice->outval1.outportbit.out2);
			lua_pushboolean(L,pdevice->outval1.outportbit.out3);
			lua_pushboolean(L,pdevice->outval1.outportbit.out4);
			lua_pushboolean(L,pdevice->outval1.outportbit.out5);
			lua_pushboolean(L,pdevice->outval1.outportbit.out6);
			lua_pushboolean(L,pdevice->outval1.outportbit.out7);
			lua_pcall(L,9,0,0);
		}
		else
		{
			lua_getglobal(L,"get207outport2");
			lua_pushinteger(L,pdevice->id);
			lua_pcall(L,1,1,0);
			if(lua_isnumber(L,-1))
			{
				pdevice->outval1.outportall = lua_tointeger(L,-1);
				lua_pop(L,1);
			}

		}
	}

	//��������ӳ��
	pdevice->sendbuff[0] = pdevice->outval0.outportall;
	pdevice->sendbuff[1] = pdevice->outval1.outportall;
}

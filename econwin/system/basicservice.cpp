/*
���ļ��ṩһЩPLC����Ļ�������
��ʱ��
�����غ��½��ش���
ȫ�ֱ���
*/
#include "basicservice.h"
#include <QString>
#include <iostream>
#include <Windows.h>
#include <QMessageBox>
#include <QFile>
#include <QApplication>
#include "DBBackend.h"
#include "lua.hpp"

extern DB mydb;
extern lua_State* L;


TON::TON(int timems)
{
	PT = timems;//ms
	Q = false;//����ź�
	INPUT = false;//�����ź�
	oldIN = false;
}

TON::~TON()
{
	// TODO Auto-generated destructor stub
}

void TON::setms(int ms)
{
	PT = ms;//ms
}

void TON::checkin(bool in)//���нӿ�
{
	INPUT = in;
	if( (!oldIN) && INPUT)//����
	{
		//����ʱ���
		t.start();
	}
	else if(oldIN && INPUT)//һֱΪ��
	{
		if(t.elapsed()> PT)
		{
		     Q = true;
		}
	}
	else
	{
		Q = false;
	}
	oldIN = INPUT;
}




R_TRIG::R_TRIG()
{
	Q = false;//����ź�
	INPUT = false;//�����ź�
	oldIN = false;
}


R_TRIG::~R_TRIG()
{
	// TODO Auto-generated destructor stub
}


void R_TRIG::checkin(bool in)//���нӿ�
{
	INPUT = in;
	if( (!oldIN) && INPUT)//����
	{
		Q = true;
	}
	else
	{
		Q = false;
	}
	oldIN = INPUT;
}


F_TRIG::F_TRIG()
{
	Q = false;//����ź�
	INPUT = false;//�����ź�
	oldIN = false;
}


F_TRIG::~F_TRIG()
{
	// TODO Auto-generated destructor stub
}


void F_TRIG::checkin(bool in)//���нӿ�
{
	INPUT = in;
	if(oldIN && (!INPUT))//�½�
	{
		Q = true;
	}
	else
	{
		Q = false;
	}
	oldIN = INPUT;
}

QList<struct varUInt> varUIntlist;
QList<struct varInt> varIntlist;
QList<struct varUShort> varUShortlist;
QList<struct varShort> varShortlist;
QList<struct varULongLong> varULongLonglist;
QList<struct varLongLong> varLongLonglist;
QList<struct varULong> varULonglist;
QList<struct varLong> varLonglist;
QList<struct varFloat> varFloatlist;
QList<struct varDouble> varDoublelist;
QList<struct varDouble> varLuavarlist;
QList<QTime *> timerlist;

//�ṩ��lua�ı���ע��ӿ�
int myvar_register(lua_State* L)
{
	const char *id = luaL_checkstring(L,1);
	double defaultval = luaL_checknumber(L,2);
	bool persist = luaL_checknumber(L,3);
	double *p = (double*)malloc(sizeof(double));
	*p = defaultval;

	//�����ڴ����
	struct varDouble t;
	double val= 0;
	
	//id��str
	t.id = id;
	
	t.p = p;

	//persist false��ͨ���� true ���ֱ���
	if(persist != 0)
	{

		//��sql�в����Ƿ��б��������û���������Ŀ�����߽�����ֵ���뵽�ڴ����
		if(mydb.GetValue(id,val))
		{
			*p = val;
			t.sqlval = val;
		}
		else
		{
			//printf("newval\n");
			mydb.NewId(id,*p,"");
			t.sqlval = *p;
		}
	}
	else
	{
		t.sqlval = *p;
	}

	//�ڴ��б�
	varLuavarlist.append(t);
	lua_pushnumber(L,*p);
	return 1;
}

//����ע��
void var_register(const QString& id,unsigned int* p,bool persist)
{
	//�����ڴ����
	struct varUInt t;
	unsigned int val= 0;
	
	//id��str
	t.id = id;
	
	t.p = p;
	//persist false��ͨ���� true ���ֱ���
	if(persist)
	{

		//��sql�в����Ƿ��б��������û���������Ŀ�����߽�����ֵ���뵽�ڴ����
		if(mydb.GetValue(id,val))
		{
			*p = val;
			t.sqlval = val;
		}
		else
		{
			printf("newval\n");
			mydb.NewId(id,*p,"");
			t.sqlval = *p;
		}
	}
	else
	{
		t.sqlval = *p;
	}

	//�ڴ��б�
	varUIntlist.append(t);

}

//����ע��
void var_register(const QString& id,int* p,bool persist)
{
	//�����ڴ����
	struct varInt t;
	int val= 0;
	
	//id��str
	t.id = id;
	
	t.p = p;

	//persist false��ͨ���� true ���ֱ���
	if(persist)
	{

		//��sql�в����Ƿ��б��������û���������Ŀ�����߽�����ֵ���뵽�ڴ����
		if(mydb.GetValue(id,val))
		{
			*p = val;
			t.sqlval = val;
		}
		else
		{
			printf("newval\n");
			mydb.NewId(id,*p,"");
			t.sqlval = *p;
		}
	}
	else
	{
		t.sqlval = *p;
	}

	//�ڴ��б�
	varIntlist.append(t);

}

void var_register(const  QString& id,unsigned short* p,bool persist)
{
	//�����ڴ����
	struct varUShort t;
	unsigned short val= 0;
	
	//id��str
	t.id = id;
	
	t.p = p;

	//persist false��ͨ���� true ���ֱ���
	if(persist)
	{

		//��sql�в����Ƿ��б��������û���������Ŀ�����߽�����ֵ���뵽�ڴ����
		if(mydb.GetValue(id,val))
		{
			*p = val;
			t.sqlval = val;
		}
		else
		{
			printf("newval\n");
			mydb.NewId(id,*p,"");
			t.sqlval = *p;
		}
	}
	else
	{
		t.sqlval = *p;
	}
	//�ڴ��б�
	varUShortlist.append(t);

}

void var_register(const  QString& id,short* p,bool persist)
{
	//�����ڴ����
	struct varShort t;
	short val= 0;
	
	//id��str
	t.id = id;
	
	t.p = p;

	//persist false��ͨ���� true ���ֱ���
	if(persist)
	{

		//��sql�в����Ƿ��б��������û���������Ŀ�����߽�����ֵ���뵽�ڴ����
		if(mydb.GetValue(id,val))
		{
			*p = val;
			t.sqlval = val;
		}
		else
		{
			printf("newval\n");
			mydb.NewId(id,*p,"");
			t.sqlval = *p;
		}
	}
	else
	{
		t.sqlval = *p;
	}

	//�ڴ��б�
	varShortlist.append(t);

}

void var_register(const  QString& id,unsigned long long* p,bool persist)
{
	//�����ڴ����
	struct varULongLong t;
	unsigned long long val= 0;
	
	//id��str
	t.id = id;
	
	t.p = p;

	//persist false��ͨ���� true ���ֱ���
	if(persist)
	{

		//��sql�в����Ƿ��б��������û���������Ŀ�����߽�����ֵ���뵽�ڴ����
		if(mydb.GetValue(id,val))
		{
			*p = val;
			t.sqlval = val;
		}
		else
		{
			printf("newval\n");
			mydb.NewId(id,*p,"");
			t.sqlval = *p;
		}
	}
	else
	{
		t.sqlval = *p;
	}

	//�ڴ��б�
	varULongLonglist.append(t);

}

void var_register(const  QString& id,long long* p,bool persist)
{
	//�����ڴ����
	struct varLongLong t;
	long long  val= 0;
	
	//id��str
	t.id = id;
	
	t.p = p;

	//persist false��ͨ���� true ���ֱ���
	if(persist)
	{

		//��sql�в����Ƿ��б��������û���������Ŀ�����߽�����ֵ���뵽�ڴ����
		if(mydb.GetValue(id,val))
		{
			*p = val;
			t.sqlval = val;
		}
		else
		{
			printf("newval\n");
			mydb.NewId(id,*p,"");
			t.sqlval = *p;
		}
	}
	else
	{
		t.sqlval = *p;
	}

	//�ڴ��б�
	varLongLonglist.append(t);

}

void var_register(const  QString& id,unsigned long* p,bool persist)
{
	//�����ڴ����
	struct varULong t;
	unsigned long val= 0;
	
	//id��str
	t.id = id;
	
	t.p = p;

	//persist false��ͨ���� true ���ֱ���
	if(persist)
	{

		//��sql�в����Ƿ��б��������û���������Ŀ�����߽�����ֵ���뵽�ڴ����
		if(mydb.GetValue(id,val))
		{
			*p = val;
			t.sqlval = val;
		}
		else
		{
			printf("newval\n");
			mydb.NewId(id,*p,"");
			t.sqlval = *p;
		}
	}
	else
	{
		t.sqlval = *p;
	}

	//�ڴ��б�
	varULonglist.append(t);

}

void var_register(const  QString& id,long* p,bool persist)
{
	//�����ڴ����
	struct varLong t;
	long val= 0;
	
	//id��str
	t.id = id;
	
	t.p = p;

	//persist false��ͨ���� true ���ֱ���
	if(persist)
	{

		//��sql�в����Ƿ��б��������û���������Ŀ�����߽�����ֵ���뵽�ڴ����
		if(mydb.GetValue(id,val))
		{
			*p = val;
			t.sqlval = val;
		}
		else
		{
			printf("newval\n");
			mydb.NewId(id,*p,"");
			t.sqlval = *p;
		}
	}
	else
	{
		t.sqlval = *p;
	}

	//�ڴ��б�
	varLonglist.append(t);

}

void var_register(const  QString& id,float* p,bool persist)
{
	//�����ڴ����
	struct varFloat t;
	float val= 0;
	
	//id��str
	t.id = id;
	
	t.p = p;

	//persist false��ͨ���� true ���ֱ���
	if(persist)
	{

		//��sql�в����Ƿ��б��������û���������Ŀ�����߽�����ֵ���뵽�ڴ����
		if(mydb.GetValue(id,val))
		{
			*p = val;
			t.sqlval = val;
		}
		else
		{
			printf("newval\n");
			mydb.NewId(id,*p,"");
			t.sqlval = *p;
		}
	}
	else
	{
		t.sqlval = *p;
	}

	//�ڴ��б�
	varFloatlist.append(t);

}

void var_register(const  QString& id,double* p,bool persist)
{
	//�����ڴ����
	struct varDouble t;
	double val= 0;
	
	//id��str
	t.id = id;
	
	t.p = p;

	//persist false��ͨ���� true ���ֱ���
	if(persist)
	{

		//��sql�в����Ƿ��б��������û���������Ŀ�����߽�����ֵ���뵽�ڴ����
		if(mydb.GetValue(id,val))
		{
			*p = val;
			t.sqlval = val;
		}
		else
		{
			printf("newval\n");
			mydb.NewId(id,*p,"");
			t.sqlval = *p;
		}
	}
	else
	{
		t.sqlval = *p;
	}

	//�ڴ��б�
	varDoublelist.append(t);

}

//c++ => lua
void flushctolua(void)
{
	QList<struct varDouble>::iterator iD;
	for(iD = varLuavarlist.begin();iD != varLuavarlist.end();++ iD)
	{
		lua_pushnumber(L,*(*iD).p);
		lua_setglobal(L,(*iD).id.toAscii());
	}
}

//lua=> c++
void flushluatoc(void)
{
	QList<struct varDouble>::iterator iD;
	for(iD = varLuavarlist.begin();iD != varLuavarlist.end();++ iD)
	{
		lua_getglobal(L,(*iD).id.toAscii());
		if(lua_isnumber(L,-1))
		{
			*(*iD).p = lua_tonumber(L,-1);
		}
		lua_pop(L,1);
	}
}

int mynewtimer(lua_State* L)
{
	QTime* pt = new QTime;
	timerlist.append(pt);
	lua_pushnumber(L,timerlist.count()-1);
	//printf("mynewtimer\n");
	return 1;
}
int mystarttimer(lua_State* L)
{
	int id = (int)luaL_checknumber(L,1);
	timerlist[id]->start();
	//printf("mystarttimer\n");
	return 0;
}
int mytimerelapsed(lua_State* L)
{
	int id = (int)luaL_checknumber(L,1);
	lua_pushnumber(L,timerlist[id]->elapsed());
	return 1;
}




/*
���ļ��ṩһЩPLC����Ļ�������
��ʱ��
�����غ��½��ش���
ȫ�ֱ���
*/
#ifndef BASICSERVICE_H
#define BASICSERVICE_H
#include <QTime>
#include <QList>
#include <QString>
#include "lua.hpp"


class TON
{
	//	Q_OBJECT
public:
	TON(int timems);
	virtual ~TON();
	void setms(int ms);
    
	int PT;//ms
	bool Q;//����ź�
	bool INPUT;//�����ź�
	void checkin(bool in);
	//���нӿ�

private:
	QTime t;//����������ʱ���
	bool oldIN;//��һ�ε�����ֵ
};


class R_TRIG
{
public:
	R_TRIG();
	virtual ~R_TRIG();
    
	bool Q;//����ź�
	bool INPUT;//�����ź�
	void checkin(bool in);//���нӿ�
private:
	bool oldIN;//��һ�ε�����ֵ
};

class F_TRIG
{
public:
	F_TRIG();
	virtual ~F_TRIG();
	bool Q;//����ź�
	bool INPUT;//�����ź�r
	void checkin(bool in);//���нӿ�
private:
	bool oldIN;//��һ�ε�����ֵ
};

struct varUInt{
	QString id;
	unsigned int* p;
	unsigned int sqlval;
};

struct varInt{
	QString id;
	int* p;
	int sqlval;
};

struct varUShort{
	QString id;
	unsigned short* p;
	unsigned short sqlval;
};

struct varShort{
	QString id;
	short* p;
	short sqlval;
};

struct varULongLong{
	QString id;
	unsigned long long* p;
	unsigned long long sqlval;
};

struct varLongLong{
	QString id;
	long long* p;
	long long sqlval;
};

struct varULong{
	QString id;
	unsigned long* p;
	unsigned long sqlval;
};

struct varLong{
	QString id;
	long * p;
	long sqlval;
};

struct varFloat{
	QString id;
	float* p;
	float sqlval;
};

struct varDouble{
	QString id;
	double * p;
	double sqlval;
};

extern QList<struct varUInt> varUIntlist;
extern QList<struct varInt> varIntlist;
extern QList<struct varUShort> varUShortlist;
extern QList<struct varShort> varShortlist;
extern QList<struct varULongLong> varULongLonglist;
extern QList<struct varLongLong> varLongLonglist;
extern QList<struct varULong> varULonglist;
extern QList<struct varLong> varLonglist;
extern QList<struct varFloat> varFloatlist;
extern QList<struct varDouble> varDoublelist;
extern QList<struct varDouble> varLuavarlist;
extern QList<QTime *> timerlist;


//����ע��
void var_register(const  QString& id,unsigned int* p,bool persist = false);
void var_register(const  QString& id,int* p,bool persist = false);
void var_register(const  QString& id,unsigned short* p,bool persist = false);
void var_register(const  QString& id,short* p,bool persist = false);
void var_register(const  QString& id,unsigned long long* p,bool persist = false);
void var_register(const  QString& id,long long* p,bool persist = false);
void var_register(const  QString& id,unsigned long* p,bool persist = false);
void var_register(const  QString& id,long* p,bool persist = false);
void var_register(const  QString& id,float* p,bool persist = false);
void var_register(const  QString& id,double* p,bool persist = false);

int myvar_register(lua_State* L);

//c++ => lua
void flushctolua(void);
//lua=> c++
void flushluatoc(void);

int mynewtimer(lua_State* L);
int mystarttimer(lua_State* L);
int mytimerelapsed(lua_State* L);

#endif // BASICSERVICE_H

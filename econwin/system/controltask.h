/*
���ļ��ṩ�û�����ע��Ľӿ�

*/
#ifndef CONTROLTASK_H
#define CONTROLTASK_H

#include <QTime>
#include <QTimer>
#include <QString>
#include <QList>
#include "lua.hpp"

struct controltask
{
		QString taskname;
		unsigned int intervalms;//�������м��ʱ�䣬���Ϊ0��,�����ܿ�����У�Ҳ��ÿ��IOˢ�����ڶ�����һ��
		bool runflag;//����ִ�б�ʶ
		void (*taskcallback)(void);
		QTime t;
};

//����ע��
void task_register(const  QString& taskname,unsigned int intervalms,void (*taskcallback)(void));
extern QList<struct controltask> tasklist;

struct luacontroltask
{
		QString taskname;
		unsigned int intervalms;//�������м��ʱ�䣬���Ϊ0��,�����ܿ�����У�Ҳ��ÿ��IOˢ�����ڶ�����һ��
		bool runflag;//����ִ�б�ʶ
		QTime t;
};

//����ע��
void luatask_register(const  QString& taskname,unsigned int intervalms);
extern QList<struct luacontroltask> luatasklist;

int mytaskregister(lua_State* L);
#endif
/*
���ļ�ʵ��ȫ�ֱ��������ݿ��̨����ʵ��

*/

#ifndef DBBACKEND_H_
#define DBBACKEND_H_

#include <qthread>
#include <QTime>
#include <QTimer>
#include <QThread>
#include <QtSql>
#include <QMutex>
#include <QTableView>
#include <QSqlTableModel>
#include <QString>
#include <QApplication>

class DB
{
public:
	DB();
	virtual ~DB();
	bool init();
	void close();

	//ȡ��id��ֵ
	bool GetValue(const  QString& id,int& value);
	bool GetValue(const  QString& id,unsigned int& value);
	bool GetValue(const  QString& id,short& value);
	bool GetValue(const  QString& id,unsigned short& value);
	bool GetValue(const  QString& id,long long& value);
	bool GetValue(const  QString& id,unsigned long long& value);
	bool GetValue(const  QString& id,long& value);
	bool GetValue(const  QString& id,unsigned long& value);
	bool GetValue(const  QString& id,float& value);
	bool GetValue(const  QString& id,double& value);

	//����id��ֵ
	bool SaveValue(const QString& id,const int value);
	bool SaveValue(const QString& id,const unsigned int value);
	bool SaveValue(const QString& id,const short value);
	bool SaveValue(const QString& id,const unsigned short value);
	bool SaveValue(const QString& id,const long long value);
	bool SaveValue(const QString& id,const unsigned long long value);
	bool SaveValue(const QString& id,const long value);
	bool SaveValue(const QString& id,const unsigned long value);
	bool SaveValue(const QString& id,const float value);
	bool SaveValue(const QString& id,const double value);

    //����ID
	bool NewId(const QString& id,const int value,const QString& text);
	bool NewId(const QString& id,const unsigned int value,const QString& text);
	bool NewId(const QString& id,const short value,const QString& text);
	bool NewId(const QString& id,const unsigned short value,const QString& text);
	bool NewId(const QString& id,const long long value,const QString& text);
	bool NewId(const QString& id,const unsigned long long value,const QString& text);
	bool NewId(const QString& id,const long value,const QString& text);
	bool NewId(const QString& id,const unsigned long value,const QString& text);
	bool NewId(const QString& id,const float value,const QString& text);
	bool NewId(const QString& id,const double value,const QString& text);

public:

	//SQL�������
	QList<QString> SQLQueue;
	QList<QString> SQLQueue2;

	//���ݿ�
	QSqlDatabase sqldb;

	//�Ƿ�������ݿ�
	bool sqldbopen;

	bool SQLQueueflag; //0����ǰ��¼д��SQLQueue   1����ǰ��¼д��SQLQueue2
	//���б�ʶ��
	QMutex SQLQueueflagmutex;

	//���������Ҷ����ݿ�ϵͳ���������ʱ������
	QMutex SQLmutex;

	//���иı��ʶ
	bool SQLQueuechg; //0 ����û�иı� 1���иı���
	bool SQLQueuechg2; //0 ����û�иı� 1���иı���
};

//���ݿ��̨
class DBBackend: public QThread
{

	Q_OBJECT

public:

	DBBackend();
	virtual ~DBBackend();

protected:
	virtual void run();

};

#endif /* DBBACKEND_H_ */
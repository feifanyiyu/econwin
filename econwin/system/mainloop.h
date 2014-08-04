/*
���ļ�ʵ�ֺ��ⲿģ���ͨ�š��û����������ִ��
ϵͳ��ʼ���к�����������
���ֳɹ��������ͨ�Ľڵ�ͨѶ
ȫ�����ֳɹ��ſ�����������
ִ��˳����
...
�������
����ִ��
�������
...
*/
#ifndef BACKEND_H
#define BACKEND_H

#include <qthread>
#include <QTime>
#include <QTimer>
#include <QThread>
#include <Qlist>

extern QList<struct tcommnode> commnodes;

//ͨѶģ��ڵ�ṹ
struct tcommnode {
	//commid
	unsigned int id;

	//����������ָ��
	unsigned char *pcommsendbuff;

	//����������ָ��
	unsigned char *pcommrecvbuff;

	//��ʱд�ĳ���
	unsigned char PDOwritelen;
	//��ʱд��Ŀ���ַ
	unsigned char PDOwriteaddr;
	//��ʱ���ĵ�ַ
	unsigned char PDOreadaddr;
	//��ʱ���ĳ���
	unsigned char PDOreadlen;

	//�豸�ṹָ��
	void * pdevice;
	//�������ݺ�ͨ������ӳ�䴦��in,pΪ�豸�ṹָ��
	void (*in_handle)(void * p);
	//�������ݺ�ͨ������ӳ�䴦��out,pΪ�豸�ṹָ��
	void (*out_handle)(void * p);
	//�ڵ��Ƿ����ֳɹ�
	//bool shakehand;
};

class backend: public QThread
{

	Q_OBJECT

public:

	backend();
	virtual ~backend();

protected:
	virtual void run();
};

class backend2: public QThread
{

	Q_OBJECT

public:

	backend2();
	virtual ~backend2();

protected:
	virtual void run();
};



#endif /* BACKEND_H */

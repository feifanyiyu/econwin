/*
���ļ�ʵ��ϵͳ����
*/
#ifndef SYS_H
#define SYS_H

#include <QtGui/QDialog>
#include <QEvent>
#include <QTime>
#include "ui_sys.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

const int CUSTOM_EVENT_BEGIN = QEvent::User; //�Զ�����¼����͵Ŀ�ʼֵ
const int BACKEND_EVENT = CUSTOM_EVENT_BEGIN + 2; //��̨���¼�

//��������������
extern bool econwinrun;

class sys : public QDialog
{
	Q_OBJECT

public:
	sys(QWidget *parent = 0, Qt::WFlags flags = 0);
	~sys();
	void displaycnt(int cnt);

private:
	Ui::sysClass ui;


	QTime ttt;
	//�ɹ����ͽ��ռ���
	unsigned long succeedcnt;


private slots:
	void on_pbluaview_clicked();
	void on_pushButton_5_clicked();
	void on_pushButton_3_clicked();
	void on_pushButton_4_clicked();
	void on_pushButton_clicked();
	void on_pushButton_2_clicked();


};

#endif // SYS_H

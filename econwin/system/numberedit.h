#ifndef NUMBEREDIT_H
#define NUMBEREDIT_H

#include <QtGui/QDialog>
#include <QEvent>

#include "ui_numberedit.h"
#include <Windows.h>
#include <QMutex>
#include <QString>
#include <QWaitCondition>
#include <QPicture>
#include <QPainter>
#include <QTimer>
#include <QList>
#include <QScrollArea>
#include <QScrollBar>


class numberedit : public QDialog
{
	Q_OBJECT

public:
	numberedit(QWidget *parent = 0, Qt::WFlags flags = 0);
	~numberedit();

	int vartype;//�������� 0���� 1������
	QString targetvar;//�󶨱���

	int zhengshuwei;//��������λ��
	int xiaoshuwei;//С������λ��
	int shuru;//�Ƿ�֧������

	int fanweijiancha;//��Χ��� 0����� 1���
	int zuixiaozhi;//��Сֵ
	int zuidazhi;//��Сֵ

	QString fontcolor;//�ı���ɫ
	QString fontfamily;//�ı�����
	int fontpointSize;//�ı���С
	int fontweight;//�ʻ���ϸ
	bool fontitalic;//б��

	int border;//�߿���
	QString bordercolor;//�߿���ɫ
	QString intercolor;//�ڲ���ɫ


private:
	Ui::numbereditClass ui;


protected:
	void closeEvent(QCloseEvent *e);

private slots:
	void on_backpb_clicked();
	void on_pb_wenzicolor_clicked();
	void on_pb_ziti_clicked();
	void on_pb_biankuangyanse_clicked();
	void on_pb_juxingyanse_clicked();
	void on_pb_targetvar_clicked();
};
#endif //NUMBEREDIT_H
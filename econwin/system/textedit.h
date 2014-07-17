#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QtGui/QDialog>
#include <QEvent>

#include "ui_textedit.h"
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


class textedit : public QDialog
{
	Q_OBJECT

public:
	textedit(QWidget *parent = 0, Qt::WFlags flags = 0);
	~textedit();

	QString text;//�ı�
	QString fontcolor;//�ı���ɫ
	QString fontfamily;//�ı�����
	int fontpointSize;//�ı���С
	int fontweight;//�ʻ���ϸ
	bool fontitalic;//б��

	int touming;//0 ͸�� 1 ��͸��

	int border;//�߿���
	QString bordercolor;//�߿���ɫ
	QString intercolor;//�ڲ���ɫ
	

private:
	Ui::texteditClass ui;


protected:
	void closeEvent(QCloseEvent *e);

private slots:
	void on_backpb_clicked();
	void on_pb_wenzicolor_clicked();
	void on_pb_ziti_clicked();
	void on_pb_biankuangyanse_clicked();
	void on_pb_juxingyanse_clicked();
};
#endif //TEXTEDIT_H
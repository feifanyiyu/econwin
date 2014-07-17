#ifndef ECONWIN_H
#define ECONWIN_H

#include <QtGui/QDialog>
#include "ui_econwin.h"
#include <QTimer>
#include <QMap>
#include <QString>
#include <QPainter>
#include <QPicture>
#include <QScrollArea>

struct hmipb{
	QString text;//��ť�ϵ��ı�
	QString fontcolor;//�ı���ɫ
	QString fontfamily;//�ı�����
	int fontpointSize;//�ı���С
	int fontweight;//�ʻ���ϸ
	bool fontitalic;//б��

	int operation;//�������� 1��λ��λ 2��ת 3ҳ����ת
	QString targetvar;//�󶨱���
	int targetpage;//Ŀ��ҳ��

	int colortype;//ָʾ������ 1 ��ָʾ�� 2��ָʾ��
	QString coloroff;//ָʾ�Ʊ���off����ɫ
	QString coloron;//ָʾ�Ʊ���on����ɫ
	QString colorvar;//ָʾ�ư󶨵ı���

	bool clicked;
};

struct hmirect{
	int border;//�߿���
	QString bordercolor;//�߿���ɫ
	QString intercolor;//�ڲ���ɫ
};

struct hmitext{
	QString text;//��ť�ϵ��ı�
	QString fontcolor;//�ı���ɫ
	QString fontfamily;//�ı�����
	int fontpointSize;//�ı���С
	int fontweight;//�ʻ���ϸ
	bool fontitalic;//б��

	int touming;//0 ͸�� 1 ��͸��
	int border;//�߿���
	QString bordercolor;//�߿���ɫ
	QString intercolor;//�ڲ���ɫ
};

struct hminumber{
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
};


struct _lightdata{
	QString text;//�ı�
	QString fontcolor;//�ı���ɫ
	QString fontfamily;//�ı�����
	int fontpointSize;//�ı���С
	int fontweight;//�ʻ���ϸ
	bool fontitalic;//б��

	int border;//�߿���
	QString bordercolor;//�߿���ɫ
	QString intercolor;//�ڲ���ɫ

};

//ָʾ��
struct hmilight{
	QString targetvar;//�󶨱���

	int number;//ָʾ��״̬����1-32

	//ÿ��ָʾ��״̬��Ӧ������
	struct _lightdata lightdata[32];
};

struct _hmiwidget{
	//�ؼ�λ��
	int x;
	int y;
	int w;
	int h;

	int widgettype; // 0label 1button
	void * pdata; //����ؼ�������
};

struct _page{
	int	id;
	QString	name;
	QString	color;
	int	needpasswd;
	QString	passwd;
	int len;
};

struct _hmiproject{
	int len;
	int firstpageid;

	QList <int> idlist;
	QList < struct _page > pagelist; 
	QList < QList<struct _hmiwidget> > hmipagelist; 
};



class MyWidget : public QWidget
 {
     Q_OBJECT

 public:
     MyWidget(QWidget *parent);
	 char chgflag;
	QAction*    Act_movebehind;

 public slots:
     void animate();
	 void movebehind();

 protected:
     void paintEvent(QPaintEvent *event);
	 void mouseMoveEvent( QMouseEvent * event );
	 void mousePressEvent( QMouseEvent * event );
	 void keyPressEvent( QKeyEvent * event );
	 void mouseReleaseEvent( QMouseEvent * event );
 	 void contextMenuEvent(QContextMenuEvent *event);
	 void mouseDoubleClickEvent( QMouseEvent * event ); 
 };

class econwin : public QDialog
{
	Q_OBJECT

public:
	econwin(QWidget *parent = 0, Qt::WFlags flags = 0);
	~econwin();
	QTimer mytimer;
	MyWidget * basewidget;
	bool checkyemianchanshu;

	bool projectneedsave;
	bool intcall;

private:
	Ui::econwinClass ui;

private slots:
	void on_pb_new_clicked();
	void on_pb_shezhishouye_clicked();
	void on_pb_shanchuyemian_clicked();
	void on_pb_zhenjiayemian_clicked();
	void on_pb_zhishideng_clicked();
	void on_pb_shuzhi_clicked();
	void on_pb_wenzhi_clicked();
	void on_pb_juxing_clicked();
	void on_pushButton_read_clicked();
	void on_pushButton_save_clicked();
	void on_pushButton_2_clicked();
	void on_pushButton_clicked();
	void on_pb_yunxing_clicked();
	void mytimeupdate();
	void onTableClicked(int,int);

	void onShuxingTableChanged(int,int);
	void onShuxingTableClicked(int,int);

protected:
	void closeEvent(QCloseEvent *e);
	void keyPressEvent( QKeyEvent * event );

};

class KeyPressEater: public QObject
{
	Q_OBJECT

public:
	KeyPressEater(QObject *parent = 0);
	~	KeyPressEater();

protected:
	bool eventFilter(QObject *obj, QEvent *event);
};




#endif // ECONWIN_H


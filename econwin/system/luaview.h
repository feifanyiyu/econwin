#ifndef LUAVIEW_H
#define LUAVIEW_H

#include <QtGui/QDialog>
#include "ui_luaview.h"
#include <QTimer>
#include <QMap>
#include <QString>
#include <QPainter>
#include <QPicture>
#include <QScrollArea>
#include <QWidget>
#include <QPaintEvent>
#include <QtOpenGL>
#include <QGLWidget>
#include <QPushButton>

//�̳�QWidget����Widget����ʾ�ű�
class Widget : public QWidget
 {
     Q_OBJECT

 public:
     Widget(QWidget *parent);
	 char chgflag;
	 QAction*    Act_editval;

 public slots:
	 void editval();

 protected:
     void paintEvent(QPaintEvent *event);
	 //void mousePressEvent( QMouseEvent * event );
	 void contextMenuEvent(QContextMenuEvent *);
 };


class luaview : public QDialog
{
	Q_OBJECT

public:
	luaview(QWidget *parent = 0, Qt::WFlags flags = 0);
	~luaview();
	QTimer mytimer;
	QPicture BackgroudPic;
	QScrollArea* scrollArea;
	Widget* native;

	QPicture varPic;
	QLabel *Lbvar;
	bool first;


private:
	Ui::luaviewClass ui;

	int filenameindex;


protected:
	void closeEvent(QCloseEvent *e);

private slots:
	void on_backpb_clicked();
	void mytimeupdate();
	void on_cbfilenamelist_currentIndexChanged ( int index );

};

//lua�ű�֧��3�����͵�ֵ
union _uvar{
	int ivar;
	double dvar;
	bool bvar;
};

//
struct _var{
	char vartype;//0 int 1bool 2double
	union _uvar varval;
};

//�������ͱ���ֵӳ��map
extern QMap<QString,struct _var> globalvarmap;

//����������λ��
struct _linevar{
	QString name;
	int loc;
};

//һ��ӵ�еı����Լ���λ��
struct _varlist{
	QList<struct _linevar> varlist;
	int line;
};

//��Ӧ�еı���
struct _filelinevarmap{
	QMap<int,struct _varlist> linevarmap;
};

#endif //LUAVIEW_H
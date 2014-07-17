/*
�û��Լ���������
*/
#include <QString>
#include <QtGui/QApplication>
#include <QMap>
#include <QFile>
#include <iostream>
#include <QTextStream>
#include <math.h>
#include "econwin.h"
#include "sys.h"
#include "lua.hpp"
#include <QTextStream>
#include <QKeyEvent>
#include <QMenu>
#include <QFileDialog>
#include <QInputDialog>
#include <QColorDialog>
#include <QMessageBox>
#include "lua.hpp"
#include "buttonedit.h"
#include "rectedit.h"
#include "textedit.h"
#include "numberedit.h"
#include "lightedit.h"
#include "varview.h"
#include "basicservice.h"
#include "chgvarval.h"

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

QMap<QString,struct _tnode> varmap;

//�����2
lua_State* L2;
//�����1
extern lua_State* L;

//lua��֧�ֶ��̣߳����ö�ռ��
extern QMutex luamutex;

int hmistate = 0;//0 nochoose(��ʼ״̬)  1action(���ƿؼ�)  2choose(��ѡ��Ŀؼ�����)
int hmisubject = 0;//0 arrow 1 button, 2 rect, 3 text, 4 number 5 light 

int hmisubjectstep = 0;//0do nothing 
//���hmistate ==1 ������棬hmisubjectstep����Ϊ 1���������Ͻǵ� 2 ���������½ǵĵ�
//���hmistate ==2 ������棬hmisubjectstep����Ϊ 1�����������Ƶ� 2 �����˿ؼ����ڲ���

int crossflag = 0;//ָʾ������ؼ�״̬ 0 δ����+   1����+

int beginx = 0;
int beginy = 0;
int endx = 0;
int endy = 0;

//ѡ������ĸ���
int choosepointindex = 0;

//����ȥ�ĵ��λ��
int origx,origy;

//��ǰ��ҳ�����ĸ�
int currenthmipageindex = 0;


//��ǰѡ��Ŀؼ����ĸ�
int currenthmiwidgetindex = -1;
bool clickwidget = false;

int currenthmiwidgetindexrun = 0;
bool clickwidgetrun = false;

int oldhmipageindex = 0;
int oldhmiwidgetindex = 0;


//hmi��ǰ��״̬
bool hmirun = false;//Ĭ�Ͻ������״̬

//���������
extern QTextStream cin;  
extern QTextStream cout;
extern QTextStream cerr;
extern econwin * peconwin;
extern QString projecthmifilename;
extern bool projecthmirun;

extern int netcardindex;
//��������������
extern bool econwinrun;


//QList<struct _hmiwidget> hmiwidgetlist;

struct _hmiproject hmiproject;

//�ж����������Ƿ��ص�
bool cross(struct _hmiwidget widget1,struct _hmiwidget widget2)
{
	if((abs((widget1.x + widget1.w/2) - (widget2.x + widget2.w/2)) < ((widget1.w+ widget2.w)/2)) 
		&&(abs((widget1.y + widget1.h/2) - (widget2.y + widget2.h/2)) < ((widget1.h+ widget2.h)/2))) 
		return true;
	else 
		return false;
}

//��Ļ�����ؼ�
MyWidget::MyWidget(QWidget *parent)
: QWidget(parent)
{
	chgflag = true;
	Act_movebehind  = new QAction(tr("���ײ��ƶ�"), this);
	connect(Act_movebehind, SIGNAL(triggered()), this, SLOT(movebehind()));

}
//��Ļ���»���
void MyWidget::animate()
{
	//repaint(0,0,400,400);
	repaint();
} 
//��Ļ���»����¼�����
void MyWidget::paintEvent(QPaintEvent *event)
{
	//������û��ҳ��
	if(currenthmipageindex < 0)
		return;

	//��ƽ׶���ʾ����
	if(hmirun == false)
	{
		QPainter painter;

		painter.begin(this);

		painter.setBrush(QBrush(QColor(hmiproject.pagelist[currenthmipageindex].color)));
		painter.setPen(QPen(Qt::darkGreen, 1));
		painter.drawRect(QRectF(0,0,639,479));

		//���ƿؼ�
		for(int i = 0; i< hmiproject.hmipagelist[currenthmipageindex].count() ;i++ )
		{
			if(hmiproject.hmipagelist[currenthmipageindex][i].widgettype == 1)
			{
				hmipb * ppb = (hmipb *)hmiproject.hmipagelist[currenthmipageindex][i].pdata;

				painter.setBrush(QBrush(QColor(ppb->coloroff)));
				painter.setPen(QPen(Qt::darkGreen, 2));
				painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
				painter.setPen(QPen(QColor(ppb->fontcolor)));

				QFont font = QFont(ppb->fontfamily,ppb->fontpointSize,ppb->fontweight,ppb->fontitalic);
				painter.setFont(font);

				QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,ppb->text);
				painter.drawText(tq,ppb->text);

			}
			else if(hmiproject.hmipagelist[currenthmipageindex][i].widgettype == 2)
			{
				hmirect * prect = (hmirect *)hmiproject.hmipagelist[currenthmipageindex][i].pdata;

				painter.setBrush(QBrush(QColor(prect->intercolor)));
				painter.setPen(QPen(QColor(prect->bordercolor), prect->border));
				painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
				painter.setPen(QPen(Qt::darkGreen, 2));

			}
			else if(hmiproject.hmipagelist[currenthmipageindex][i].widgettype == 3)
			{
				hmitext * ptext = (hmitext *)hmiproject.hmipagelist[currenthmipageindex][i].pdata;

				if(ptext->touming)
				{
					painter.setBrush(QBrush(QColor(ptext->intercolor)));
					painter.setPen(QPen(QColor(ptext->bordercolor),ptext->border));
					painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
					painter.setPen(QPen(QColor(ptext->fontcolor)));
				}

				painter.setPen(QPen(QColor(ptext->fontcolor)));
				QFont font = QFont(ptext->fontfamily,ptext->fontpointSize,ptext->fontweight,ptext->fontitalic);
				painter.setFont(font);

				QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,ptext->text);
				painter.drawText(tq,ptext->text);
			}
			else if(hmiproject.hmipagelist[currenthmipageindex][i].widgettype == 4)
			{
				hminumber * pnumber = (hminumber *)hmiproject.hmipagelist[currenthmipageindex][i].pdata;

				painter.setBrush(QBrush(QColor(pnumber->intercolor)));
				painter.setPen(QPen(QColor(pnumber->bordercolor),pnumber->border));
				painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));

				painter.setPen(QPen(QColor(pnumber->fontcolor)));
				QFont font = QFont(pnumber->fontfamily,pnumber->fontpointSize,pnumber->fontweight,pnumber->fontitalic);
				painter.setFont(font);

				QString number = "";
				for(int ii = 0;ii < pnumber->zhengshuwei;ii++)
				{
					number = number + QString::number(ii+1);
				}

				if(pnumber->xiaoshuwei > 0)
					number = number + ".";
				for(int ii = 0;ii < pnumber->xiaoshuwei;ii++)
				{
					number = number + QString::number(ii+1);
				}

				QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,number);
				painter.drawText(tq,number);
			}
			else if(hmiproject.hmipagelist[currenthmipageindex][i].widgettype == 5)
			{
				hmilight * plight = (hmilight *)hmiproject.hmipagelist[currenthmipageindex][i].pdata;

				painter.setBrush(QBrush(QColor(plight->lightdata[0].intercolor)));
				painter.setPen(QPen(QColor(plight->lightdata[0].bordercolor),plight->lightdata[0].border));
				painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
				painter.setPen(QPen(QColor(plight->lightdata[0].fontcolor)));

				QFont font = QFont(plight->lightdata[0].fontfamily,plight->lightdata[0].fontpointSize,plight->lightdata[0].fontweight,plight->lightdata[0].fontitalic);
				painter.setFont(font);

				QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,plight->lightdata[0].text);
				painter.drawText(tq,plight->lightdata[0].text);
			}

			painter.setPen(QPen(Qt::darkGreen, 2));

			if(currenthmiwidgetindex == i)
			{
				painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x +0,hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +0,4,4));

				painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w /2 -2,hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +0,4,4));

				painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w -4,hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +0,4,4));

				painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w -4,hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h /2 -2,4,4));

				painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w -4,hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h -4,4,4));

				painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w /2 -2,hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h -4,4,4));

				painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x +0,hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h -4,4,4));

				painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x +0,hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h /2 -2,4,4));
			}
		}

		//���ƿؼ������ο�
		if((hmistate == 1) 
			//&& (hmisubject == 1)
			&& (hmisubjectstep == 2))
		{
			painter.setBrush(Qt::NoBrush);
			painter.setPen(QPen(Qt::darkGreen, 1));
			painter.drawRect(QRectF(beginx,beginy,endx-beginx,endy-beginy));
		}
		painter.end();
	}
	//���н׶οؼ���ʾ
	else
	{
		//cout << 1 << endl;
		QPainter painter;

		painter.begin(this);

		painter.setBrush(QBrush(QColor(hmiproject.pagelist[currenthmipageindex].color)));
		painter.setPen(QPen(Qt::darkGreen, 1));
		painter.drawRect(QRectF(0,0,639,479));


		//���ƿؼ�
		for(int i = 0; i< hmiproject.hmipagelist[currenthmipageindex].count() ;i++ )
		{
			if(hmiproject.hmipagelist[currenthmipageindex][i].widgettype == 1)
			{

				hmipb * ppb = (hmipb *)hmiproject.hmipagelist[currenthmipageindex][i].pdata;

				if(ppb->colortype == 2) 
				{
					struct _tnode t = varmap.value(ppb->colorvar);
					if(t.type == 1)
					{
						unsigned int* vp = (unsigned int*)(t.p);
						if((int)(*vp) == 0)
						{
							painter.setBrush(QBrush(QColor(ppb->coloroff)));
						}
						else if((int)(*vp) == 1)
						{
							painter.setBrush(QBrush(QColor(ppb->coloron)));

						}
						painter.setPen(QPen(Qt::darkGreen, 2));
						painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
						painter.setPen(QPen(QColor(ppb->fontcolor)));
						QFont font = QFont(ppb->fontfamily,ppb->fontpointSize,ppb->fontweight,ppb->fontitalic);
						painter.setFont(font);
						QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,ppb->text);
						painter.drawText(tq,ppb->text);
					}
					if(t.type == 2)
					{
						int* vp = (int*)(t.p);
						if((int)(*vp) == 0)
						{
							painter.setBrush(QBrush(QColor(ppb->coloroff)));
						}
						else if((int)(*vp) == 1)
						{
							painter.setBrush(QBrush(QColor(ppb->coloron)));

						}
						painter.setPen(QPen(Qt::darkGreen, 2));
						painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
						painter.setPen(QPen(QColor(ppb->fontcolor)));
						QFont font = QFont(ppb->fontfamily,ppb->fontpointSize,ppb->fontweight,ppb->fontitalic);
						painter.setFont(font);
						QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,ppb->text);
						painter.drawText(tq,ppb->text);
					}
					if(t.type == 3)
					{
						unsigned short* vp = (unsigned short*)(t.p);
						if((int)(*vp) == 0)
						{
							painter.setBrush(QBrush(QColor(ppb->coloroff)));
						}
						else if((int)(*vp) == 1)
						{
							painter.setBrush(QBrush(QColor(ppb->coloron)));

						}
						painter.setPen(QPen(Qt::darkGreen, 2));
						painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
						painter.setPen(QPen(QColor(ppb->fontcolor)));
						QFont font = QFont(ppb->fontfamily,ppb->fontpointSize,ppb->fontweight,ppb->fontitalic);
						painter.setFont(font);
						QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,ppb->text);
						painter.drawText(tq,ppb->text);
					}
					if(t.type == 4)
					{
						short* vp = (short*)(t.p);
						if((int)(*vp) == 0)
						{
							painter.setBrush(QBrush(QColor(ppb->coloroff)));
						}
						else if((int)(*vp) == 1)
						{
							painter.setBrush(QBrush(QColor(ppb->coloron)));

						}
						painter.setPen(QPen(Qt::darkGreen, 2));
						painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
						painter.setPen(QPen(QColor(ppb->fontcolor)));
						QFont font = QFont(ppb->fontfamily,ppb->fontpointSize,ppb->fontweight,ppb->fontitalic);
						painter.setFont(font);
						QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,ppb->text);
						painter.drawText(tq,ppb->text);
					}
					if(t.type == 5)
					{
						unsigned long long * vp = (unsigned long long*)(t.p);
						if((int)(*vp) == 0)
						{
							painter.setBrush(QBrush(QColor(ppb->coloroff)));
						}
						else if((int)(*vp) == 1)
						{
							painter.setBrush(QBrush(QColor(ppb->coloron)));

						}
						painter.setPen(QPen(Qt::darkGreen, 2));
						painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
						painter.setPen(QPen(QColor(ppb->fontcolor)));
						QFont font = QFont(ppb->fontfamily,ppb->fontpointSize,ppb->fontweight,ppb->fontitalic);
						painter.setFont(font);
						QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,ppb->text);
						painter.drawText(tq,ppb->text);
					}
					if(t.type == 6)
					{
						long long * vp = (long long*)(t.p);
						if((int)(*vp) == 0)
						{
							painter.setBrush(QBrush(QColor(ppb->coloroff)));
						}
						else if((int)(*vp) == 1)
						{
							painter.setBrush(QBrush(QColor(ppb->coloron)));

						}
						painter.setPen(QPen(Qt::darkGreen, 2));
						painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
						painter.setPen(QPen(QColor(ppb->fontcolor)));
						QFont font = QFont(ppb->fontfamily,ppb->fontpointSize,ppb->fontweight,ppb->fontitalic);
						painter.setFont(font);
						QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,ppb->text);
						painter.drawText(tq,ppb->text);
					}
					if(t.type == 7)
					{
						unsigned long * vp = (unsigned long*)(t.p);
						if((int)(*vp) == 0)
						{
							painter.setBrush(QBrush(QColor(ppb->coloroff)));
						}
						else if((int)(*vp) == 1)
						{
							painter.setBrush(QBrush(QColor(ppb->coloron)));

						}
						painter.setPen(QPen(Qt::darkGreen, 2));
						painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
						painter.setPen(QPen(QColor(ppb->fontcolor)));
						QFont font = QFont(ppb->fontfamily,ppb->fontpointSize,ppb->fontweight,ppb->fontitalic);
						painter.setFont(font);
						QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,ppb->text);
						painter.drawText(tq,ppb->text);
					}
					if(t.type == 8)
					{
						long * vp = (long*)(t.p);
						if((int)(*vp) == 0)
						{
							painter.setBrush(QBrush(QColor(ppb->coloroff)));
						}
						else if((int)(*vp) == 1)
						{
							painter.setBrush(QBrush(QColor(ppb->coloron)));

						}
						painter.setPen(QPen(Qt::darkGreen, 2));
						painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
						painter.setPen(QPen(QColor(ppb->fontcolor)));
						QFont font = QFont(ppb->fontfamily,ppb->fontpointSize,ppb->fontweight,ppb->fontitalic);
						painter.setFont(font);
						QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,ppb->text);
						painter.drawText(tq,ppb->text);
					}
					if(t.type == 9)
					{
						float * vp = (float*)(t.p);
						if((int)(*vp) == 0)
						{
							painter.setBrush(QBrush(QColor(ppb->coloroff)));
						}
						else if((int)(*vp) == 1)
						{
							painter.setBrush(QBrush(QColor(ppb->coloron)));

						}
						painter.setPen(QPen(Qt::darkGreen, 2));
						painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
						painter.setPen(QPen(QColor(ppb->fontcolor)));
						QFont font = QFont(ppb->fontfamily,ppb->fontpointSize,ppb->fontweight,ppb->fontitalic);
						painter.setFont(font);
						QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,ppb->text);
						painter.drawText(tq,ppb->text);
					}
					if(t.type == 10)
					{
						double * vp = (double*)(t.p);
						if((int)(*vp) == 0)
						{
							painter.setBrush(QBrush(QColor(ppb->coloroff)));
						}
						else if((int)(*vp) == 1)
						{
							painter.setBrush(QBrush(QColor(ppb->coloron)));

						}
						painter.setPen(QPen(Qt::darkGreen, 2));
						painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
						painter.setPen(QPen(QColor(ppb->fontcolor)));
						QFont font = QFont(ppb->fontfamily,ppb->fontpointSize,ppb->fontweight,ppb->fontitalic);
						painter.setFont(font);
						QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,ppb->text);
						painter.drawText(tq,ppb->text);
					}
					if(t.type == 11)
					{
						double * vp = (double*)(t.p);
						if((int)(*vp) == 0)
						{
							painter.setBrush(QBrush(QColor(ppb->coloroff)));
						}
						else if((int)(*vp) == 1)
						{
							painter.setBrush(QBrush(QColor(ppb->coloron)));

						}
						painter.setPen(QPen(Qt::darkGreen, 2));
						painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
						painter.setPen(QPen(QColor(ppb->fontcolor)));
						QFont font = QFont(ppb->fontfamily,ppb->fontpointSize,ppb->fontweight,ppb->fontitalic);
						painter.setFont(font);
						QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,ppb->text);
						painter.drawText(tq,ppb->text);
					}

				}
				else
				{
					if(ppb->clicked == false)
					{
						painter.setBrush(QBrush(QColor(ppb->coloroff)));
					}
					else
					{
						painter.setBrush(QBrush(QColor(ppb->coloron)));
					}
					painter.setPen(QPen(Qt::darkGreen, 2));
					painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
					painter.setPen(QPen(QColor(ppb->fontcolor)));
					QFont font = QFont(ppb->fontfamily,ppb->fontpointSize,ppb->fontweight,ppb->fontitalic);
					painter.setFont(font);
					QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,ppb->text);
					painter.drawText(tq,ppb->text);
				}
			}	
			else if(hmiproject.hmipagelist[currenthmipageindex][i].widgettype == 2)
			{

				hmirect * prect = (hmirect *)hmiproject.hmipagelist[currenthmipageindex][i].pdata;

				painter.setBrush(QBrush(QColor(prect->intercolor)));
				painter.setPen(QPen(QColor(prect->bordercolor), prect->border));
				painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
			}
			else if(hmiproject.hmipagelist[currenthmipageindex][i].widgettype == 3)
			{

				hmitext * ptext = (hmitext *)hmiproject.hmipagelist[currenthmipageindex][i].pdata;

				if(ptext->touming)
				{
					painter.setBrush(QBrush(QColor(ptext->intercolor)));
					painter.setPen(QPen(QColor(ptext->bordercolor),ptext->border));
					painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
					painter.setPen(QPen(QColor(ptext->fontcolor)));
				}

				painter.setPen(QPen(QColor(ptext->fontcolor)));
				QFont font = QFont(ptext->fontfamily,ptext->fontpointSize,ptext->fontweight,ptext->fontitalic);
				painter.setFont(font);

				QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,ptext->text);
				painter.drawText(tq,ptext->text);
			}		

			else if(hmiproject.hmipagelist[currenthmipageindex][i].widgettype == 4)
			{
				hminumber * pnumber = (hminumber *)hmiproject.hmipagelist[currenthmipageindex][i].pdata;
				QString number = "";

				struct _tnode t = varmap.value(pnumber->targetvar);
				double vvvv = 0;
				if(t.type == 1)
				{
					unsigned int* vp = (unsigned int*)(t.p);
					//����
					if(pnumber->vartype == 0)
					{
						if(pnumber->xiaoshuwei > 0)
						{
							int vvv = pow(10,pnumber->xiaoshuwei);
							vvvv = (double)(*vp) / vvv;
						}
						else
						{
							vvvv = *vp;
						}

					}
					//������
					else if(pnumber->vartype == 1)
					{
						vvvv = *vp;
					}
				}
				if(t.type == 2)
				{
					int* vp = (int*)(t.p);
					//����
					if(pnumber->vartype == 0)
					{
						if(pnumber->xiaoshuwei > 0)
						{
							int vvv = pow(10,pnumber->xiaoshuwei);
							vvvv = (double)(*vp) / vvv;
						}
						else
						{
							vvvv = *vp;
						}

					}
					//������
					else if(pnumber->vartype == 1)
					{
						vvvv = *vp;
					}
				}
				if(t.type == 3)
				{
					unsigned short* vp = (unsigned short*)(t.p);
					//����
					if(pnumber->vartype == 0)
					{
						if(pnumber->xiaoshuwei > 0)
						{
							int vvv = pow(10,pnumber->xiaoshuwei);
							vvvv = (double)(*vp) / vvv;
						}
						else
						{
							vvvv = *vp;
						}

					}
					//������
					else if(pnumber->vartype == 1)
					{
						vvvv = *vp;
					}
				}
				if(t.type == 4)
				{
					short* vp = (short*)(t.p);
					//����
					if(pnumber->vartype == 0)
					{
						if(pnumber->xiaoshuwei > 0)
						{
							int vvv = pow(10,pnumber->xiaoshuwei);
							vvvv = (double)(*vp) / vvv;
						}
						else
						{
							vvvv = *vp;
						}

					}
					//������
					else if(pnumber->vartype == 1)
					{
						vvvv = *vp;
					}
				}
				if(t.type == 5)
				{
					unsigned long long * vp = (unsigned long long*)(t.p);
					//����
					if(pnumber->vartype == 0)
					{
						if(pnumber->xiaoshuwei > 0)
						{
							int vvv = pow(10,pnumber->xiaoshuwei);
							vvvv = (double)(*vp) / vvv;
						}
						else
						{
							vvvv = *vp;
						}

					}
					//������
					else if(pnumber->vartype == 1)
					{
						vvvv = *vp;
					}
				}
				if(t.type == 6)
				{
					long long * vp = (long long*)(t.p);
					//����
					if(pnumber->vartype == 0)
					{
						if(pnumber->xiaoshuwei > 0)
						{
							int vvv = pow(10,pnumber->xiaoshuwei);
							vvvv = (double)(*vp) / vvv;
						}
						else
						{
							vvvv = *vp;
						}

					}
					//������
					else if(pnumber->vartype == 1)
					{
						vvvv = *vp;
					}
				}
				if(t.type == 7)
				{
					unsigned long * vp = (unsigned long*)(t.p);
					//����
					if(pnumber->vartype == 0)
					{
						if(pnumber->xiaoshuwei > 0)
						{
							int vvv = pow(10,pnumber->xiaoshuwei);
							vvvv = (double)(*vp) / vvv;
						}
						else
						{
							vvvv = *vp;
						}

					}
					//������
					else if(pnumber->vartype == 1)
					{
						vvvv = *vp;
					}
				}
				if(t.type == 8)
				{
					long * vp = (long*)(t.p);
					//����
					if(pnumber->vartype == 0)
					{
						if(pnumber->xiaoshuwei > 0)
						{
							int vvv = pow(10,pnumber->xiaoshuwei);
							vvvv = (double)(*vp) / vvv;
						}
						else
						{
							vvvv = *vp;
						}

					}
					//������
					else if(pnumber->vartype == 1)
					{
						vvvv = *vp;
					}
				}
				if(t.type == 9)
				{
					float * vp = (float*)(t.p);

					//����
					if(pnumber->vartype == 0)
					{
						if(pnumber->xiaoshuwei > 0)
						{
							int vvv = pow(10,pnumber->xiaoshuwei);
							vvvv = (double)(*vp) / vvv;
						}
						else
						{
							vvvv = *vp;
						}

					}
					//������
					else if(pnumber->vartype == 1)
					{
						vvvv = *vp;
					}
				}
				if(t.type == 10)
				{
					double * vp = (double*)(t.p);
					//����
					if(pnumber->vartype == 0)
					{
						if(pnumber->xiaoshuwei > 0)
						{
							int vvv = pow(10,pnumber->xiaoshuwei);
							vvvv = *vp / vvv;
						}
						else
						{
							vvvv = *vp;
						}

					}
					//������
					else if(pnumber->vartype == 1)
					{
						vvvv = *vp;
					}
				}
				if(t.type == 11)
				{
					double * vp = (double*)(t.p);

					//����
					if(pnumber->vartype == 0)
					{
						if(pnumber->xiaoshuwei > 0)
						{
							int vvv = pow(10,pnumber->xiaoshuwei);
							vvvv = *vp / vvv;
						}
						else
						{
							vvvv = *vp;
						}

					}
					//������
					else if(pnumber->vartype == 1)
					{
						vvvv = *vp;
					}
				}

				number = QString::number(vvvv,'f',pnumber->xiaoshuwei);

				painter.setBrush(QBrush(QColor(pnumber->intercolor)));
				painter.setPen(QPen(QColor(pnumber->bordercolor),pnumber->border));
				painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
				painter.setPen(QPen(QColor(pnumber->fontcolor)));

				painter.setPen(QPen(QColor(pnumber->fontcolor)));
				QFont font = QFont(pnumber->fontfamily,pnumber->fontpointSize,pnumber->fontweight,pnumber->fontitalic);
				painter.setFont(font);

				QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,number);
				painter.drawText(tq,number);
			}

			else if(hmiproject.hmipagelist[currenthmipageindex][i].widgettype == 5)
			{
				hmilight * plight = (hmilight *)hmiproject.hmipagelist[currenthmipageindex][i].pdata;

				struct _tnode t = varmap.value(plight->targetvar);
				int vvvv = 0;
				if(t.type == 1)
				{
					unsigned int* vp = (unsigned int*)(t.p);
					vvvv = *vp;
				}
				if(t.type == 2)
				{
					int* vp = (int*)(t.p);
					vvvv = *vp;
				}
				if(t.type == 3)
				{
					unsigned short* vp = (unsigned short*)(t.p);
					vvvv = *vp;
				}
				if(t.type == 4)
				{
					short* vp = (short*)(t.p);
					vvvv = *vp;
				}
				if(t.type == 5)
				{
					unsigned long long * vp = (unsigned long long*)(t.p);
					vvvv = *vp;
				}
				if(t.type == 6)
				{
					long long * vp = (long long*)(t.p);
					vvvv = *vp;
				}
				if(t.type == 7)
				{
					unsigned long * vp = (unsigned long*)(t.p);
					vvvv = *vp;
				}
				if(t.type == 8)
				{
					long * vp = (long*)(t.p);
					vvvv = *vp;
				}
				if(t.type == 9)
				{
					float * vp = (float*)(t.p);
					vvvv = *vp;

				}
				if(t.type == 10)
				{
					double * vp = (double*)(t.p);
					vvvv = *vp;
				}
				if(t.type == 11)
				{
					double * vp = (double*)(t.p);
					vvvv = *vp;
				}

				if((vvvv >= 0) &&(vvvv < plight->number))
				{
					painter.setBrush(QBrush(QColor(plight->lightdata[vvvv].intercolor)));
					painter.setPen(QPen(QColor(plight->lightdata[vvvv].bordercolor),plight->lightdata[vvvv].border));
					painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
					painter.setPen(QPen(QColor(plight->lightdata[vvvv].fontcolor)));

					QFont font = QFont(plight->lightdata[vvvv].fontfamily,plight->lightdata[vvvv].fontpointSize,plight->lightdata[vvvv].fontweight,plight->lightdata[vvvv].fontitalic);
					painter.setFont(font);

					QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,plight->lightdata[vvvv].text);
					painter.drawText(tq,plight->lightdata[vvvv].text);

				}
				else
				{
					painter.setBrush(QBrush(QColor(plight->lightdata[0].intercolor)));
					painter.setPen(QPen(QColor(plight->lightdata[0].bordercolor),plight->lightdata[0].border));
					painter.drawRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h));
					painter.setPen(QPen(QColor(plight->lightdata[0].fontcolor)));

					QFont font = QFont(plight->lightdata[0].fontfamily,plight->lightdata[0].fontpointSize,plight->lightdata[0].fontweight,plight->lightdata[0].fontitalic);
					painter.setFont(font);

					QRectF tq = painter.boundingRect(QRectF(hmiproject.hmipagelist[currenthmipageindex][i].x,hmiproject.hmipagelist[currenthmipageindex][i].y,hmiproject.hmipagelist[currenthmipageindex][i].w,hmiproject.hmipagelist[currenthmipageindex][i].h),Qt::AlignCenter,tr("���"));
					painter.drawText(tq,tr("���"));
				}

			}
		}

		painter.end();
	}
}
//��Ļ������ƶ��¼�����
void MyWidget::mouseMoveEvent( QMouseEvent * event )
{
	//������û��ҳ��
	if(currenthmipageindex < 0)
		return;

	if(hmirun == true)
		return ;

	//�����ؼ�������л�
	if((hmistate == 1) 
		//&& (hmisubject == 1)
		&& (hmisubjectstep != 0))
	{
		if(crossflag == 0)
		{
			crossflag = 1;
			setCursor(Qt::CrossCursor);
		}
	}

	//�����ؼ�����������꣬���ƽ���ӵĿؼ����
	if((hmistate == 1) 
		//&& (hmisubject == 1)
		&& (hmisubjectstep == 2))
	{
		endx = event->x();
		endy = event->y();
		repaint();
	}


	//ѡ��ؼ�״̬���棬�ƶ������Ƶ�
	if((hmistate == 2) && (hmisubjectstep==1))
	{
		int x,y;
		int changedx,changedy;
		if(choosepointindex == 1)
		{
			x = event->x();
			y = event->y();
			changedx = x-origx;
			changedy = y-origy;
			origx = x;
			origy = y;

			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x += changedx;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w -= changedx;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y += changedy;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h -= changedy;
			repaint();

		}
		else if(choosepointindex == 2)
		{
			//x = event->globalX();
			y = event->y();
			//changedx = x-origx;
			changedy = y-origy;
			//origx = x;
			origy = y;

			//hmiwidgetlist[currenthmiwidgetindex].x += changedx;
			//hmiwidgetlist[currenthmiwidgetindex].w -= changedx;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y += changedy;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h -= changedy;

			repaint();
		}
		else if(choosepointindex == 3)
		{
			x = event->x();
			y = event->y();
			changedx = x-origx;
			changedy = y-origy;
			origx = x;
			origy = y;

			//hmiwidgetlist[currenthmiwidgetindex].x += changedx;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w += changedx;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y += changedy;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h -= changedy;

			repaint();
		}
		else if(choosepointindex == 4)
		{
			x = event->x();
			//y = event->globalY();
			changedx = x-origx;
			//changedy = y-origy;
			origx = x;
			//origy = y;
			//hmiwidgetlist[currenthmiwidgetindex].x += changedx;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w += changedx;
			//hmiwidgetlist[currenthmiwidgetindex].y += changedy;
			//hmiwidgetlist[currenthmiwidgetindex].h -= changedy;

			repaint();
		}
		else if(choosepointindex == 5)
		{
			x = event->x();
			y = event->y();
			changedx = x-origx;
			changedy = y-origy;
			origx = x;
			origy = y;
			//hmiwidgetlist[currenthmiwidgetindex].x += changedx;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w += changedx;
			//hmiwidgetlist[currenthmiwidgetindex].y += changedy;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h += changedy;

			repaint();
		}
		else if(choosepointindex == 6)
		{
			//x = event->globalX();
			y = event->y();
			//changedx = x-origx;
			changedy = y-origy;
			//origx = x;
			origy = y;
			//hmiwidgetlist[currenthmiwidgetindex].x += changedx;
			//hmiwidgetlist[currenthmiwidgetindex].w += changedx;
			//hmiwidgetlist[currenthmiwidgetindex].y += changedy;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h += changedy;

			repaint();
		}
		else if(choosepointindex == 7)
		{
			x = event->x();
			y = event->y();
			changedx = x-origx;
			changedy = y-origy;
			origx = x;
			origy = y;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x += changedx;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w -= changedx;
			//hmiwidgetlist[currenthmiwidgetindex].y += changedy;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h += changedy;

			repaint();
		}
		else if(choosepointindex == 8)
		{
			x = event->x();
			//y = event->globalY();
			changedx = x-origx;
			//changedy = y-origy;
			origx = x;
			//origy = y;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x += changedx;
			hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w -= changedx;
			//hmiwidgetlist[currenthmiwidgetindex].y += changedy;
			//hmiwidgetlist[currenthmiwidgetindex].h -= changedy;

			repaint();
		}
		peconwin->projectneedsave = true;

		if(peconwin->projectneedsave)
			peconwin->setWindowTitle(tr("ecomwin��") + projecthmifilename + "*");
		else
			peconwin->setWindowTitle(tr("ecomwin��") + projecthmifilename);

	}
	//ѡ��ؼ�״̬���棬�����ƶ��ؼ�
	else if((hmistate == 2) && (hmisubjectstep==2))
	{
		int x,y;
		int changedx,changedy;

		x = event->x();
		changedx = x-origx;
		origx = x;
		y = event->y();
		changedy = y-origy;
		origy = y;


		hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x += changedx;
		hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y += changedy;
		repaint();
		peconwin->projectneedsave = true;

		if(peconwin->projectneedsave)
			peconwin->setWindowTitle(tr("ecomwin��") + projecthmifilename + "*");
		else
			peconwin->setWindowTitle(tr("ecomwin��") + projecthmifilename);

	}

	//ѡ��ؼ�״̬���棬����ڿؼ��ϻ���ʱ���仯
	else if((hmistate == 2) && (hmisubjectstep==0) )//&& (currenthmiwidgetindex == index))
	{

		hmistate = 2;

		int x1,x2,y1,y2;
		int x,y;
		x = event->x();
		y = event->y();

		x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + 0;
		x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + 4;
		y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + 0;
		y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +4;

		if(	(x<=x2) 
			&&(x>=x1)
			&&(y<=y2)
			&&(y>=y1))
		{
			setCursor(Qt::SizeFDiagCursor);
			return;
		}

		x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w /2 -2;
		x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w /2 +2;
		y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +0;
		y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +4;

		if(	(x<=x2) 
			&&(x>=x1)
			&&(y<=y2)
			&&(y>=y1))
		{
			setCursor(Qt::SizeVerCursor);
			return;
		}

		x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w-4;
		x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w;
		y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +0;
		y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +4;

		if(	(x<=x2) 
			&&(x>=x1)
			&&(y<=y2)
			&&(y>=y1))
		{
			setCursor(Qt::SizeBDiagCursor);
			return;
		}

		x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w-4;
		x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w;
		y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h /2 -2;
		y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h /2 +2;

		if(	(x<=x2) 
			&&(x>=x1)
			&&(y<=y2)
			&&(y>=y1))
		{
			setCursor(Qt::SizeHorCursor);
			return;
		}

		x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w-4;
		x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w;
		y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h -4;
		y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h;

		if(	(x<=x2) 
			&&(x>=x1)
			&&(y<=y2)
			&&(y>=y1))
		{
			setCursor(Qt::SizeFDiagCursor);
			return;
		}

		x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w /2 -2;
		x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w /2 +2;
		y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h -4;
		y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h;

		if(	(x<=x2) 
			&&(x>=x1)
			&&(y<=y2)
			&&(y>=y1))
		{
			setCursor(Qt::SizeVerCursor);
			return;
		}

		x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + 0;
		x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + 4;
		y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h -4;
		y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h;

		if(	(x<=x2) 
			&&(x>=x1)
			&&(y<=y2)
			&&(y>=y1))
		{
			setCursor(Qt::SizeBDiagCursor);
			return;
		}

		x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + 0;
		x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + 4;
		y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h /2 -2;
		y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h /2 +2;

		if(	(x<=x2) 
			&&(x>=x1)
			&&(y<=y2)
			&&(y>=y1))
		{
			setCursor(Qt::SizeHorCursor);
			return;
		}
		setCursor(Qt::ArrowCursor);

	}

}
void MyWidget::mousePressEvent( QMouseEvent * event ) 
{
	//������û��ҳ��
	if(currenthmipageindex < 0)
		return;

	//���״̬����
	if(hmirun == false)
	{
		//����״̬��,�����ؼ����Ͻ�
		if((hmistate == 1) 
			//&& (hmisubject == 1)
			&& (hmisubjectstep == 1))
		{
			beginx = event->x();
			beginy = event->y();
			hmisubjectstep = 2;
		}
		//�����ؼ����½�
		else if((hmistate == 1) 
			//&& (hmisubject == 1)
			&& (hmisubjectstep == 2))
		{
			if(hmisubject == 1)
			{

				hmipb* nbutton = new hmipb;
				nbutton->text = QString(tr("��ť"));
				nbutton->fontcolor = "#ffffff";
				nbutton->clicked = false;

				nbutton->fontfamily = QString(tr("����"));
				nbutton->fontpointSize = 12;
				nbutton->fontweight= 50;
				nbutton->fontitalic = false;

				nbutton->operation = 1;
				nbutton->targetvar = "";
				nbutton->targetpage = 0;
				nbutton->colortype = 1;
				nbutton->coloron = "#00ff00";
				nbutton->coloroff = "#a0a0a4";
				nbutton->colorvar = "";

				if((endx > beginx) && (endy > beginy))
				{
					struct _hmiwidget hmiwidget;
					hmiwidget.widgettype = 1;
					hmiwidget.x = beginx;
					hmiwidget.y = beginy;
					hmiwidget.w = endx-beginx;
					hmiwidget.h = endy-beginy;
					hmiwidget.pdata = nbutton;

					hmiproject.hmipagelist[currenthmipageindex].append(hmiwidget);
					currenthmiwidgetindex = hmiproject.hmipagelist[currenthmipageindex].count()-1;

				}
			}
			else if(hmisubject == 2)
			{
				hmirect* nrect = new hmirect;
				nrect->border = 1;
				nrect->bordercolor = "#00ff00";
				nrect->intercolor = "#a0a0a4";

				if((endx > beginx) && (endy > beginy))
				{
					struct _hmiwidget hmiwidget;
					hmiwidget.widgettype = 2;
					hmiwidget.x = beginx;
					hmiwidget.y = beginy;
					hmiwidget.w = endx-beginx;
					hmiwidget.h = endy-beginy;
					hmiwidget.pdata = nrect;

					hmiproject.hmipagelist[currenthmipageindex].append(hmiwidget);
					currenthmiwidgetindex = hmiproject.hmipagelist[currenthmipageindex].count()-1;

				}
			}
			else if(hmisubject == 3)
			{
				hmitext* ntext = new hmitext;

				ntext->text = QString(tr("����"));
				ntext->fontcolor = "#00ff00";
				ntext->fontfamily = QString(tr("����"));
				ntext->fontpointSize = 12;
				ntext->fontweight= 50;
				ntext->fontitalic = false;


				ntext->touming = 0;
				ntext->border = 1;
				ntext->bordercolor = "#00ff00";
				ntext->intercolor = "#a0a0a4";

				if((endx > beginx) && (endy > beginy))
				{
					struct _hmiwidget hmiwidget;
					hmiwidget.widgettype = 3;
					hmiwidget.x = beginx;
					hmiwidget.y = beginy;
					hmiwidget.w = endx-beginx;
					hmiwidget.h = endy-beginy;
					hmiwidget.pdata = ntext;

					hmiproject.hmipagelist[currenthmipageindex].append(hmiwidget);
					currenthmiwidgetindex = hmiproject.hmipagelist[currenthmipageindex].count()-1;

				}
			}
			else if(hmisubject == 4)
			{
				hminumber* nnumber = new hminumber;

				nnumber->vartype = 0;

				nnumber->targetvar = "";
				nnumber->zhengshuwei = 1;
				nnumber->xiaoshuwei = 0;
				nnumber->shuru = 0;

				nnumber->fanweijiancha = 0;//��Χ��� 0����� 1���
				nnumber->zuixiaozhi = 0;//��Сֵ
				nnumber->zuidazhi = 0;//��Сֵ

				nnumber->fontcolor = "#00ff00";
				nnumber->fontfamily = QString(tr("����"));
				nnumber->fontpointSize = 12;
				nnumber->fontweight= 50;
				nnumber->fontitalic = false;

				nnumber->border = 2;
				nnumber->bordercolor = "#00ff00";
				nnumber->intercolor = "#a0a0a4";

				if((endx > beginx) && (endy > beginy))
				{
					struct _hmiwidget hmiwidget;
					hmiwidget.widgettype = 4;
					hmiwidget.x = beginx;
					hmiwidget.y = beginy;
					hmiwidget.w = endx-beginx;
					hmiwidget.h = endy-beginy;
					hmiwidget.pdata = nnumber;

					hmiproject.hmipagelist[currenthmipageindex].append(hmiwidget);
					currenthmiwidgetindex = hmiproject.hmipagelist[currenthmipageindex].count()-1;

				}
			}
			else if(hmisubject == 5)
			{
				hmilight* nlight = new hmilight;

				nlight->targetvar = "";
				nlight->number = 1;

				for(int ii= 0;ii <32;ii++ )
				{
					nlight->lightdata[ii].text = tr("ָʾ��");
					nlight->lightdata[ii].fontcolor = "#00ff00";
					nlight->lightdata[ii].fontfamily = QString(tr("����"));
					nlight->lightdata[ii].fontpointSize = 12;
					nlight->lightdata[ii].fontweight= 50;
					nlight->lightdata[ii].fontitalic = false;

					nlight->lightdata[ii].border = 2;
					nlight->lightdata[ii].bordercolor = "#00ff00";
					nlight->lightdata[ii].intercolor = "#a0a0a4";
				}

				if((endx > beginx) && (endy > beginy))
				{
					struct _hmiwidget hmiwidget;
					hmiwidget.widgettype = 5;
					hmiwidget.x = beginx;
					hmiwidget.y = beginy;
					hmiwidget.w = endx-beginx;
					hmiwidget.h = endy-beginy;
					hmiwidget.pdata = nlight;

					hmiproject.hmipagelist[currenthmipageindex].append(hmiwidget);
					currenthmiwidgetindex = hmiproject.hmipagelist[currenthmipageindex].count()-1;

				}
			}
			hmistate = 2;
			hmisubject = 0;
			hmisubjectstep = 0;
			crossflag = 0;
			setCursor(Qt::ArrowCursor);

			repaint();

			peconwin->projectneedsave = true;

			if(peconwin->projectneedsave)
				peconwin->setWindowTitle(tr("ecomwin��") + projecthmifilename + "*");
			else
				peconwin->setWindowTitle(tr("ecomwin��") + projecthmifilename);

		}
		//��ǰѡ������ĸ��ؼ����ؼ��ľ��岿λ��ɶ
		else
		{
			//�Ƿ������ؼ�
			clickwidget = false;
			origx = event->x();
			origy = event->y();

			//��õ�ǰ��������ĸ��ؼ�
			for(int i = hmiproject.hmipagelist[currenthmipageindex].count()-1; i>= 0 ;i-- )
			{
				if((origx  <= hmiproject.hmipagelist[currenthmipageindex][i].x + hmiproject.hmipagelist[currenthmipageindex][i].w)
					&& (origx  >= hmiproject.hmipagelist[currenthmipageindex][i].x)
					&& (origy  <= hmiproject.hmipagelist[currenthmipageindex][i].y + hmiproject.hmipagelist[currenthmipageindex][i].h)
					&& (origy  >= hmiproject.hmipagelist[currenthmipageindex][i].y)
					)
				{
					//��ǰ����
					currenthmiwidgetindex = i;
					clickwidget = true;
					break;
				}
			}


			if(clickwidget == true)
				hmistate = 2;
			else
				return;

			int x1,x2,y1,y2;
			int x,y;
			x = event->x();
			y = event->y();

			x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + 0;
			x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + 4;
			y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + 0;
			y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + 4;

			if(	(x<=x2) 
				&&(x>=x1)
				&&(y<=y2)
				&&(y>=y1))
			{
				hmisubjectstep = 1;
				choosepointindex = 1;
				return;
			}

			x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w /2 -2;
			x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w /2 +2;
			y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + 0;
			y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + 4;

			if(	(x<=x2) 
				&&(x>=x1)
				&&(y<=y2)
				&&(y>=y1))
			{
				hmisubjectstep = 1;
				choosepointindex = 2;
				return;
			}

			x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w-4;
			x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w;
			y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + 0;
			y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + 4;

			if(	(x<=x2) 
				&&(x>=x1)
				&&(y<=y2)
				&&(y>=y1))
			{
				hmisubjectstep = 1;
				choosepointindex = 3;
				return;
			}

			x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x +hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w-4;
			x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w;
			y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h /2 -2;
			y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h /2 +2;

			if(	(x<=x2) 
				&&(x>=x1)
				&&(y<=y2)
				&&(y>=y1))
			{
				hmisubjectstep = 1;
				choosepointindex = 4;
				return;
			}

			x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w-4;
			x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w;
			y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h -4;
			y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h;

			if(	(x<=x2) 
				&&(x>=x1)
				&&(y<=y2)
				&&(y>=y1))
			{
				hmisubjectstep = 1;
				choosepointindex = 5;
				return;
			}

			x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w /2 -2;
			x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].w /2 +2;
			y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h -4;
			y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h;

			if(	(x<=x2) 
				&&(x>=x1)
				&&(y<=y2)
				&&(y>=y1))
			{
				hmisubjectstep = 1;
				choosepointindex = 6;
				return;
			}

			x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + 0;
			x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + 4;
			y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h -4;
			y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h;

			if(	(x<=x2) 
				&&(x>=x1)
				&&(y<=y2)
				&&(y>=y1))
			{
				hmisubjectstep = 1;
				choosepointindex = 7;
				return;
			}

			x1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + 0;
			x2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].x + 4;
			y1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h /2 -2;
			y2 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].y + hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].h /2 +2;

			if(	(x<=x2) 
				&&(x>=x1)
				&&(y<=y2)
				&&(y>=y1))
			{
				hmisubjectstep = 1;
				choosepointindex = 8;
				return;
			}

			if(clickwidget == true)
				hmisubjectstep=2;

			repaint();
		}
	}
	//����״̬�������������
	else
	{
		//�Ƿ������ؼ�
		clickwidgetrun = false;
		origx = event->x();
		origy = event->y();

		//��õ�ǰ��������ĸ��ؼ�
		for(int i = hmiproject.hmipagelist[currenthmipageindex].count()-1; i>= 0 ;i-- )
		{
			if((origx  <= hmiproject.hmipagelist[currenthmipageindex][i].x + hmiproject.hmipagelist[currenthmipageindex][i].w)
				&& (origx  >= hmiproject.hmipagelist[currenthmipageindex][i].x)
				&& (origy  <= hmiproject.hmipagelist[currenthmipageindex][i].y + hmiproject.hmipagelist[currenthmipageindex][i].h)
				&& (origy  >= hmiproject.hmipagelist[currenthmipageindex][i].y)
				)
			{
				//��ǰ����
				currenthmiwidgetindexrun = i;
				clickwidgetrun = true;
				break;
			}
		}

		if(!clickwidgetrun)
		{
			return;
		}

		if(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindexrun].widgettype == 1 )
		{
			hmipb * ppb = (hmipb *)hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindexrun].pdata;
			ppb->clicked = true;

			if(ppb->operation == 1) 
			{
				struct _tnode t = varmap.value(ppb->targetvar);
				if(t.type == 1)
				{
					unsigned int* vp = (unsigned int*)(t.p);
					(*vp) = 1;
				}
				if(t.type == 2)
				{
					int* vp = (int*)(t.p);
					(*vp) = 1;
				}
				if(t.type == 3)
				{
					unsigned short* vp = (unsigned short*)(t.p);
					(*vp) = 1;
				}
				if(t.type == 4)
				{
					short* vp = (short*)(t.p);
					(*vp) = 1;
				}
				if(t.type == 5)
				{
					unsigned long long * vp = (unsigned long long*)(t.p);
					(*vp) = 1;
				}
				if(t.type == 6)
				{
					long long * vp = (long long*)(t.p);
					(*vp) = 1;
				}
				if(t.type == 7)
				{
					unsigned long * vp = (unsigned long*)(t.p);
					(*vp) = 1;
				}
				if(t.type == 8)
				{
					long * vp = (long*)(t.p);
					(*vp) = 1;
				}
				if(t.type == 9)
				{
					float * vp = (float*)(t.p);
					(*vp) = 1;
				}
				if(t.type == 10)
				{
					double * vp = (double*)(t.p);
					(*vp) = 1;
				}
				if(t.type == 11)
				{
					double * vp = (double*)(t.p);
					luamutex.lock();
					(*vp) = 1;
					luamutex.unlock();
				}
			}
			else if(ppb->operation == 2) 
			{
				struct _tnode t = varmap.value(ppb->targetvar);
				if(t.type == 1)
				{
					unsigned int* vp = (unsigned int*)(t.p);
					if((*vp) == 1)
					{
						(*vp) = 0;
					}
					else
					{
						(*vp) = 1;
					}
				}
				if(t.type == 2)
				{
					int* vp = (int*)(t.p);
					if((*vp) == 1)
					{
						(*vp) = 0;
					}
					else
					{
						(*vp) = 1;
					}
				}
				if(t.type == 3)
				{
					unsigned short* vp = (unsigned short*)(t.p);
					if((*vp) == 1)
					{
						(*vp) = 0;
					}
					else
					{
						(*vp) = 1;
					}
				}
				if(t.type == 4)
				{
					short* vp = (short*)(t.p);
					if((*vp) == 1)
					{
						(*vp) = 0;
					}
					else
					{
						(*vp) = 1;
					}
				}
				if(t.type == 5)
				{
					unsigned long long * vp = (unsigned long long*)(t.p);
					if((*vp) == 1)
					{
						(*vp) = 0;
					}
					else
					{
						(*vp) = 1;
					}
				}
				if(t.type == 6)
				{
					long long * vp = (long long*)(t.p);
					if((*vp) == 1)
					{
						(*vp) = 0;
					}
					else
					{
						(*vp) = 1;
					}
				}
				if(t.type == 7)
				{
					unsigned long * vp = (unsigned long*)(t.p);
					if((*vp) == 1)
					{
						(*vp) = 0;
					}
					else
					{
						(*vp) = 1;
					}
				}
				if(t.type == 8)
				{
					long * vp = (long*)(t.p);
					if((*vp) == 1)
					{
						(*vp) = 0;
					}
					else
					{
						(*vp) = 1;
					}
				}
				if(t.type == 9)
				{
					float * vp = (float*)(t.p);
					if((*vp) == 1)
					{
						(*vp) = 0;
					}
					else
					{
						(*vp) = 1;
					}
				}
				if(t.type == 10)
				{
					double * vp = (double*)(t.p);
					if((*vp) == 1)
					{
						(*vp) = 0;
					}
					else
					{
						(*vp) = 1;
					}
				}
				if(t.type == 11)
				{
					double * vp = (double*)(t.p);
					if((*vp) == 1)
					{
						luamutex.lock();
						(*vp) = 0;
						luamutex.unlock();
					}
					else
					{
						luamutex.lock();
						(*vp) = 1;
						luamutex.unlock();
					}
				}
			}
			else if(ppb->operation == 3) 
			{
				//ҳ��ת��
				if(hmiproject.idlist.indexOf(ppb->targetpage) != -1)
				{
					currenthmipageindex = hmiproject.idlist.indexOf(ppb->targetpage);
					clickwidgetrun = false;
					ppb->clicked = false;
				}
			}
		}
		else if(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindexrun].widgettype == 4 )
		{
			hminumber * pnumber = (hminumber *)hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindexrun].pdata;

			//���������Ұ󶨵ı����ǿ�
			if((pnumber->shuru == 1) && (pnumber->targetvar != QString(""))) 
			{
				struct _tnode t = varmap.value(pnumber->targetvar);
				double vvvv = 0;
				double vv = 0;
				if(t.type == 1)
				{
					unsigned int* vp = (unsigned int*)(t.p);
					vvvv = *vp;
				}
				if(t.type == 2)
				{
					int* vp = (int*)(t.p);
					vvvv = *vp;

				}
				if(t.type == 3)
				{
					unsigned short* vp = (unsigned short*)(t.p);
					vvvv = *vp;

				}
				if(t.type == 4)
				{
					short* vp = (short*)(t.p);
					vvvv = *vp;

				}
				if(t.type == 5)
				{
					unsigned long long * vp = (unsigned long long*)(t.p);
					vvvv = *vp;

				}
				if(t.type == 6)
				{
					long long * vp = (long long*)(t.p);
					vvvv = *vp;

				}
				if(t.type == 7)
				{
					unsigned long * vp = (unsigned long*)(t.p);
					vvvv = *vp;

				}
				if(t.type == 8)
				{
					long * vp = (long*)(t.p);
					vvvv = *vp;

				}
				if(t.type == 9)
				{
					float * vp = (float*)(t.p);
					vvvv = *vp;

				}
				if(t.type == 10)
				{
					double * vp = (double*)(t.p);
					vvvv = *vp;

				}
				if(t.type == 11)
				{
					double * vp = (double*)(t.p);
					vvvv = *vp;
				}

				bool ok;

				if(pnumber->vartype == 0)
				{
					if(pnumber->xiaoshuwei > 0)
					{
						int vvv = pow(10,pnumber->xiaoshuwei);
						vvvv = vvvv / vvv;
						double vvvvmin =  pnumber->zuixiaozhi / vvv;
						double vvvvmax =  pnumber->zuidazhi / vvv;

						vv = QInputDialog::getDouble(this,tr("����ֵ"),tr("����ֵ"),vvvv,vvvvmin,vvvvmax,pnumber->xiaoshuwei,&ok);

					}
					else
					{
						vv = QInputDialog::getDouble(this,tr("����ֵ"),tr("����ֵ"),vvvv,pnumber->zuixiaozhi,pnumber->zuidazhi,pnumber->xiaoshuwei,&ok);
					}
				}
				else
				{
					vv = QInputDialog::getDouble(this,tr("����ֵ"),tr("����ֵ"),vvvv,pnumber->zuixiaozhi,pnumber->zuidazhi,pnumber->xiaoshuwei,&ok);
				}

				if(ok)
				{
					if(pnumber->vartype == 0)
					{
						int vvv = pow(10,pnumber->xiaoshuwei);
						vvvv = vv * vvv;
					}
					else
					{
						vvvv = vv;
					}

					if(t.type == 1)
					{
						unsigned int* vp = (unsigned int*)(t.p);
						*vp = vvvv;
					}
					if(t.type == 2)
					{
						int* vp = (int*)(t.p);
						*vp = vvvv;

					}
					if(t.type == 3)
					{
						unsigned short* vp = (unsigned short*)(t.p);
						*vp = vvvv;

					}
					if(t.type == 4)
					{
						short* vp = (short*)(t.p);
						*vp = vvvv;

					}
					if(t.type == 5)
					{
						unsigned long long * vp = (unsigned long long*)(t.p);
						*vp = vvvv;

					}
					if(t.type == 6)
					{
						long long * vp = (long long*)(t.p);
						*vp = vvvv;

					}
					if(t.type == 7)
					{
						unsigned long * vp = (unsigned long*)(t.p);
						*vp = vvvv;

					}
					if(t.type == 8)
					{
						long * vp = (long*)(t.p);
						*vp = vvvv;

					}
					if(t.type == 9)
					{
						float * vp = (float*)(t.p);
						*vp = vvvv;

					}
					if(t.type == 10)
					{
						double * vp = (double*)(t.p);
						*vp = vvvv;

					}
					if(t.type == 11)
					{
						double * vp = (double*)(t.p);
						luamutex.lock();
						*vp = vvvv;
						luamutex.unlock();

					}

				}

			}
		}
		repaint();
	}
}

void MyWidget::mouseReleaseEvent( QMouseEvent * event )
{
	//������û��ҳ��
	if(currenthmipageindex < 0)
		return;

	//���״̬��������ͷ�
	if(hmirun == false)
	{
		if(hmistate == 2)
		{
			if(hmisubjectstep == 2)
				hmisubjectstep = 0;

			if(hmisubjectstep == 1)
				hmisubjectstep = 0;
		}
	}
	//����״̬��������ͷ�
	else
	{
		if(!clickwidgetrun)
		{
			return;
		}
		//�������
		clickwidgetrun = false;

		if(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindexrun].widgettype == 1 )
		{
			hmipb * ppb = (hmipb *)hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindexrun].pdata;
			ppb->clicked = false;


			if(ppb->operation == 1) 
			{
				struct _tnode t = varmap.value(ppb->targetvar);
				if(t.type == 1)
				{
					unsigned int* vp = (unsigned int*)(t.p);
					(*vp) = 0;
				}
				if(t.type == 2)
				{
					int* vp = (int*)(t.p);
					(*vp) = 0;
				}
				if(t.type == 3)
				{
					unsigned short* vp = (unsigned short*)(t.p);
					(*vp) = 0;
				}
				if(t.type == 4)
				{
					short* vp = (short*)(t.p);
					(*vp) = 0;
				}
				if(t.type == 5)
				{
					unsigned long long * vp = (unsigned long long*)(t.p);
					(*vp) = 0;
				}
				if(t.type == 6)
				{
					long long * vp = (long long*)(t.p);
					(*vp) = 0;
				}
				if(t.type == 7)
				{
					unsigned long * vp = (unsigned long*)(t.p);
					(*vp) = 0;
				}
				if(t.type == 8)
				{
					long * vp = (long*)(t.p);
					(*vp) = 0;
				}
				if(t.type == 9)
				{
					float * vp = (float*)(t.p);
					(*vp) = 0;
				}
				if(t.type == 10)
				{
					double * vp = (double*)(t.p);
					(*vp) = 0;
				}
				if(t.type == 11)
				{
					double * vp = (double*)(t.p);
					luamutex.lock();
					(*vp) = 0;
					luamutex.unlock();
				}
			}
			repaint();
		}

	}
}

void MyWidget::keyPressEvent( QKeyEvent * event )
{
	if(event->key() == Qt::Key_Escape)
	{

	}
}

//�Ҽ��˵�
void MyWidget::contextMenuEvent(QContextMenuEvent *event)
{
	if(hmirun == false)
	{
		if(clickwidget == true)
		{
			QMenu *menu=new QMenu(this);

			menu->addAction(Act_movebehind);

			menu->exec(QCursor::pos());
		}
	}
}
void MyWidget::mouseDoubleClickEvent( QMouseEvent * event )
{
	//������û��ҳ��
	if(currenthmipageindex < 0)
		return;

	if(hmirun == false)
	{
		//�Ƿ������ؼ�
		clickwidget = false;
		origx = event->x();
		origy = event->y();

		//��õ�ǰ��������ĸ��ؼ�
		for(int i = hmiproject.hmipagelist[currenthmipageindex].count()-1; i>= 0 ;i-- )
		{
			if((origx  <= hmiproject.hmipagelist[currenthmipageindex][i].x + hmiproject.hmipagelist[currenthmipageindex][i].w)
				&& (origx  >= hmiproject.hmipagelist[currenthmipageindex][i].x)
				&& (origy  <= hmiproject.hmipagelist[currenthmipageindex][i].y + hmiproject.hmipagelist[currenthmipageindex][i].h)
				&& (origy  >= hmiproject.hmipagelist[currenthmipageindex][i].y)
				)
			{
				//��ǰ����
				currenthmiwidgetindex = i;
				clickwidget = true;
				break;
			}
		}


		//�����ؼ����Ա༭ҳ��
		if(clickwidget == true)
		{
			if(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].widgettype == 1 )
			{

				buttonedit Dlg;
				Dlg.exec();
			}
			if(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].widgettype == 2 )
			{

				rectedit Dlg;
				Dlg.exec();
			}
			if(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].widgettype == 3 )
			{
				textedit Dlg;
				Dlg.exec();
			}
			if(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].widgettype == 4 )
			{
				numberedit Dlg;
				Dlg.exec();
			}
			if(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].widgettype == 5 )
			{
				lightedit Dlg;
				Dlg.exec();
			}
		}
		else
			return;
	}
}


void MyWidget::movebehind()
{
	//������û��ҳ��
	if(currenthmipageindex < 0)
		return;

	struct _hmiwidget hmiwidget1;
	struct _hmiwidget hmiwidget2;
	int index1 = 0;
	int index2 = 0;

	for(int i = currenthmiwidgetindex-1; i>=0 ;i-- )
	{
		//�����������ɿؼ�
		if(cross(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex],hmiproject.hmipagelist[currenthmipageindex][i]))
		{
			hmiwidget1 = hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex];
			hmiwidget2 = hmiproject.hmipagelist[currenthmipageindex][i];

			index1 = i;
			index2 = currenthmiwidgetindex;

			currenthmiwidgetindex = index1;

			hmiproject.hmipagelist[currenthmipageindex][index1] = hmiwidget1;
			hmiproject.hmipagelist[currenthmipageindex][index2] = hmiwidget2;
			repaint();

			//cout << "haha" << endl;
			//cout << index1 << "," << index2 << endl;
			peconwin->projectneedsave = true;

			if(peconwin->projectneedsave)
				peconwin->setWindowTitle(tr("ecomwin��") + projecthmifilename + "*");
			else
				peconwin->setWindowTitle(tr("ecomwin��") + projecthmifilename);

			break;
		}
		else
		{
			cout << "hehe" << endl;
		}
	}
}

econwin::econwin(QWidget *parent, Qt::WFlags flags)
: QDialog(parent, flags)
{
	L2 = luaL_newstate();
	luaL_openlibs(L2);//�򿪱�׼��

	ui.setupUi(this);

	//ȡ�����е�ȫ�ֱ�����
	struct _tnode t;

	QList<struct varUInt>::iterator iUI;
	for(iUI = varUIntlist.begin();iUI != varUIntlist.end();++ iUI)
	{
		t.type = 1;
		t.p = ((void *)(*iUI).p);
		varmap.insert((*iUI).id,t);
	}
	QList<struct varInt>::iterator iI;
	for(iI = varIntlist.begin();iI != varIntlist.end();++ iI)
	{
		t.type = 2;
		t.p = ((void *)(*iI).p);
		varmap.insert((*iI).id,t);
	}
	QList<struct varUShort>::iterator iUS;
	for(iUS = varUShortlist.begin();iUS != varUShortlist.end();++ iUS)
	{
		t.type = 3;
		t.p = ((void *)(*iUS).p);
		varmap.insert((*iUS).id,t);
	}
	QList<struct varShort>::iterator iS;
	for(iS = varShortlist.begin();iS != varShortlist.end();++ iS)
	{
		t.type = 4;
		t.p = ((void *)(*iS).p);
		varmap.insert((*iS).id,t);
	}
	QList<struct varULongLong>::iterator iULL;
	for(iULL = varULongLonglist.begin();iULL != varULongLonglist.end();++ iULL)
	{
		t.type = 5;
		t.p = ((void *)(*iULL).p);
		varmap.insert((*iULL).id,t);
	}
	QList<struct varLongLong>::iterator iLL;
	for(iLL = varLongLonglist.begin();iLL != varLongLonglist.end();++ iLL)
	{
		t.type = 6;
		t.p = ((void *)(*iLL).p);
		varmap.insert((*iLL).id,t);
	}
	QList<struct varULong>::iterator iUL;
	for(iUL = varULonglist.begin();iUL != varULonglist.end();++ iUL)
	{
		t.type = 7;
		t.p = ((void *)(*iUL).p);
		varmap.insert((*iUL).id,t);
	}
	QList<struct varLong>::iterator iL;
	for(iL = varLonglist.begin();iL != varLonglist.end();++ iL)
	{
		t.type = 8;
		t.p = ((void *)(*iL).p);
		varmap.insert((*iL).id,t);
	}
	QList<struct varFloat>::iterator iF;
	for(iF = varFloatlist.begin();iF != varFloatlist.end();++ iF)
	{
		t.type = 9;
		t.p = ((void *)(*iF).p);
		varmap.insert((*iF).id,t);
	}
	QList<struct varDouble>::iterator iD;
	for(iD = varDoublelist.begin();iD != varDoublelist.end();++ iD)
	{
		t.type = 10;
		t.p = ((void *)(*iD).p);
		varmap.insert((*iD).id,t);
	}

	for(iD = varLuavarlist.begin();iD != varLuavarlist.end();++ iD)
	{
		//lua double
		t.type = 11;
		t.p = ((void *)(*iD).p);
		varmap.insert((*iD).id,t);
	}


	if(projecthmifilename == "")
	{
		//Ĭ������һ��ҳ��
		struct _page page;
		page.id = 1;
		page.name = tr("ҳ��1");
		page.color = "#c0c0c0";
		page.needpasswd = 0;
		page.passwd = "";
		page.len = 0;

		hmiproject.pagelist.append(page);

		//id�б�
		hmiproject.idlist.append(1);

		//һ��ҳ��
		hmiproject.len = 1;
		hmiproject.firstpageid = 1;

		QList<struct _hmiwidget> emptypage;

		//һ����ҳ��
		hmiproject.hmipagelist.append(emptypage);

		basewidget = new MyWidget(this);
		basewidget->setGeometry(200+30,140,640,480);
		basewidget->animate();
		basewidget->setMouseTracking(true);

		int firstpageindex = hmiproject.idlist.indexOf(hmiproject.firstpageid);

		//ˢ�µ�ҳ����
		ui.yemiantableWidget->setRowCount(hmiproject.pagelist.count());
		for(int i = 0;i < hmiproject.pagelist.count();i++)
		{
			//��ҳҪ����ʶ
			if(i == firstpageindex)
			{
				ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name +tr("(*)")));
			}
			else
			{
				ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name));
			}
			ui.yemiantableWidget->item(i,0)->setFlags(0);
			ui.yemiantableWidget->item(i,0)->setForeground(Qt::black);
		}

		ui.yemiantableWidget->setColumnWidth(0, 180);
		ui.yemianshuxingtableWidget->setColumnWidth(0, 60);

		//��һ��ҳ�������
		ui.yemianshuxingtableWidget->setRowCount(5);
		ui.yemianshuxingtableWidget->setItem(0,0, new QTableWidgetItem("ID"));
		ui.yemianshuxingtableWidget->item(0,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(0,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(0,1, new QTableWidgetItem(QString::number(hmiproject.pagelist[0].id)));
		ui.yemianshuxingtableWidget->item(0,1)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(1,0, new QTableWidgetItem(tr("ҳ������")));
		ui.yemianshuxingtableWidget->item(1,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(1,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(1,1, new QTableWidgetItem((hmiproject.pagelist[0].name)));
		ui.yemianshuxingtableWidget->item(1,1)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(2,0, new QTableWidgetItem(tr("������ɫ")));
		ui.yemianshuxingtableWidget->item(2,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(2,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(2,1, new QTableWidgetItem(("")));
		ui.yemianshuxingtableWidget->item(2,1)->setFlags(0);
		ui.yemianshuxingtableWidget->item(2,1)->setBackground(QColor(hmiproject.pagelist[0].color));

		ui.yemianshuxingtableWidget->setItem(3,0, new QTableWidgetItem(tr("��Ҫ����")));
		ui.yemianshuxingtableWidget->item(3,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(3,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(3,1, new QTableWidgetItem(QString::number(hmiproject.pagelist[0].needpasswd)));
		ui.yemianshuxingtableWidget->item(3,1)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(4,0, new QTableWidgetItem(tr("����")));
		ui.yemianshuxingtableWidget->item(4,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(4,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(4,1, new QTableWidgetItem(hmiproject.pagelist[0].passwd));
		ui.yemianshuxingtableWidget->item(4,1)->setForeground(Qt::black);

		connect(ui.yemiantableWidget,SIGNAL(cellClicked( int, int)),this,SLOT(onTableClicked(int,int)));

		connect(ui.yemianshuxingtableWidget,SIGNAL(cellChanged( int, int)),this,SLOT(onShuxingTableChanged(int,int)));
		connect(ui.yemianshuxingtableWidget,SIGNAL(cellClicked( int, int)),this,SLOT(onShuxingTableClicked(int,int)));


		connect(&mytimer, SIGNAL(timeout()), this, SLOT(mytimeupdate()));

		mytimer.start(200); //100ms��ʱ

		checkyemianchanshu = true;

		projectneedsave = true;
		projecthmifilename = tr("δ����");
		projecthmirun = false;

		if(projectneedsave)
			this->setWindowTitle(tr("ecomwin��") + projecthmifilename + "*");
		else
			this->setWindowTitle(tr("ecomwin��") + projecthmifilename);
	}
	else
	{

		//Ĭ������һ��ҳ��
		struct _page page;
		page.id = 1;
		page.name = tr("ҳ��1");
		page.color = "#c0c0c0";
		page.needpasswd = 0;
		page.passwd = "";
		page.len = 0;

		hmiproject.pagelist.append(page);

		//id�б�
		hmiproject.idlist.append(1);

		//һ��ҳ��
		hmiproject.len = 1;
		hmiproject.firstpageid = 1;

		QList<struct _hmiwidget> emptypage;

		//һ����ҳ��
		hmiproject.hmipagelist.append(emptypage);

		basewidget = new MyWidget(this);
		basewidget->setGeometry(200+30,140,640,480);
		basewidget->animate();
		basewidget->setMouseTracking(true);

		int firstpageindex = hmiproject.idlist.indexOf(hmiproject.firstpageid);

		//ˢ�µ�ҳ����
		ui.yemiantableWidget->setRowCount(hmiproject.pagelist.count());
		for(int i = 0;i < hmiproject.pagelist.count();i++)
		{
			//��ҳҪ����ʶ
			if(i == firstpageindex)
			{
				ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name +tr("(*)")));
			}
			else
			{
				ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name));
			}
			ui.yemiantableWidget->item(i,0)->setFlags(0);
			ui.yemiantableWidget->item(i,0)->setForeground(Qt::black);
		}

		ui.yemiantableWidget->setColumnWidth(0, 180);
		ui.yemianshuxingtableWidget->setColumnWidth(0, 60);

		//��һ��ҳ�������
		ui.yemianshuxingtableWidget->setRowCount(5);
		ui.yemianshuxingtableWidget->setItem(0,0, new QTableWidgetItem("ID"));
		ui.yemianshuxingtableWidget->item(0,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(0,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(0,1, new QTableWidgetItem(QString::number(hmiproject.pagelist[0].id)));
		ui.yemianshuxingtableWidget->item(0,1)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(1,0, new QTableWidgetItem(tr("ҳ������")));
		ui.yemianshuxingtableWidget->item(1,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(1,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(1,1, new QTableWidgetItem((hmiproject.pagelist[0].name)));
		ui.yemianshuxingtableWidget->item(1,1)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(2,0, new QTableWidgetItem(tr("������ɫ")));
		ui.yemianshuxingtableWidget->item(2,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(2,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(2,1, new QTableWidgetItem(("")));
		ui.yemianshuxingtableWidget->item(2,1)->setFlags(0);
		ui.yemianshuxingtableWidget->item(2,1)->setBackground(QColor(hmiproject.pagelist[0].color));

		ui.yemianshuxingtableWidget->setItem(3,0, new QTableWidgetItem(tr("��Ҫ����")));
		ui.yemianshuxingtableWidget->item(3,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(3,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(3,1, new QTableWidgetItem(QString::number(hmiproject.pagelist[0].needpasswd)));
		ui.yemianshuxingtableWidget->item(3,1)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(4,0, new QTableWidgetItem(tr("����")));
		ui.yemianshuxingtableWidget->item(4,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(4,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(4,1, new QTableWidgetItem(hmiproject.pagelist[0].passwd));
		ui.yemianshuxingtableWidget->item(4,1)->setForeground(Qt::black);

		connect(ui.yemiantableWidget,SIGNAL(cellClicked( int, int)),this,SLOT(onTableClicked(int,int)));

		connect(ui.yemianshuxingtableWidget,SIGNAL(cellChanged( int, int)),this,SLOT(onShuxingTableChanged(int,int)));
		connect(ui.yemianshuxingtableWidget,SIGNAL(cellClicked( int, int)),this,SLOT(onShuxingTableClicked(int,int)));


		connect(&mytimer, SIGNAL(timeout()), this, SLOT(mytimeupdate()));

		mytimer.start(200); //100ms��ʱ

		checkyemianchanshu = true;

		intcall = true;
		on_pushButton_read_clicked();


		if(projecthmirun == false)
		{
			hmirun = false;

			ui.pb_yunxing->setText(tr("�л�����"));

			//�ָ�ҳ��
			currenthmipageindex = oldhmipageindex;
			currenthmiwidgetindex = oldhmiwidgetindex;

			//��ť�Լ��ؼ���ʾ

			ui.frame->show();
			ui.pushButton_2->show();
			ui.pb_juxing->show();
			ui.pb_wenzhi->show();
			ui.pb_shuzhi->show();
			ui.pb_zhishideng->show();
			ui.pushButton_read->show();
			ui.pushButton_save->show();
			ui.yemiantableWidget->show();
			ui.yemianshuxingtableWidget->show();
			ui.pb_shanchuyemian->show();
			ui.pb_shezhishouye->show();
			ui.pb_zhenjiayemian->show();
			ui.pb_new->show();

			repaint();
		}
		else
		{
			hmirun = true;
			ui.pb_yunxing->setText(tr("�������"));

			//����ҳ��Ϳؼ�
			oldhmipageindex = currenthmipageindex;
			oldhmiwidgetindex = currenthmiwidgetindex;

			currenthmipageindex = hmiproject.idlist.indexOf(hmiproject.firstpageid);

			//��ť�Լ��ؼ�����

			ui.frame->hide();
			ui.pushButton_2->hide();
			ui.pb_juxing->hide();
			ui.pb_wenzhi->hide();
			ui.pb_shuzhi->hide();
			ui.pb_zhishideng->hide();
			ui.pushButton_read->hide();
			ui.pushButton_save->hide();
			ui.yemiantableWidget->hide();
			ui.yemianshuxingtableWidget->hide();
			ui.pb_shanchuyemian->hide();
			ui.pb_shezhishouye->hide();
			ui.pb_zhenjiayemian->hide();
			ui.pb_new->hide();

			repaint();
		}

	}

}

econwin::~econwin()
{
	mytimer.disconnect(this);
	lua_close(L2);
}


void econwin::on_pushButton_clicked()
{
	sys Dlg;
	Dlg.exec();
}

void econwin::closeEvent(QCloseEvent *e)
{
	mytimer.disconnect(this);


	QString fileName = QCoreApplication::applicationDirPath() + "/para.lua";

	QFile file(fileName);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream out(&file);

	out << "netcardindex = " << QString::number(netcardindex) << "\n";
	if(econwinrun)
		out << "econwinrun = " << 1 << QString("\n");
	else
		out << "econwinrun=" << 0 << QString("\n");

	if((projecthmifilename != tr("δ����"))
		&&(projecthmifilename != tr(""))
		)
	{
		out << "projectname = [[" << projecthmifilename << QString("]]\n");
		if(projecthmirun)
		{
			out << "hmirun = " << 1 << QString("\n");
		}
		else
		{
			out << "hmirun = " << 0 << QString("\n");
		}
	}
	else
	{
		out << "projectname = " << "[[]]"<< QString("\n");
		out << "hmirun = " << 0 << QString("\n");
	}

	file.flush();
	file.close();

	if(projectneedsave)
	{
		QMessageBox msgBox;
		msgBox.setText("<font size = 4>��ȷ��Ҫ�����ղŵ��޸ģ�</font>");
		msgBox.setWindowTitle("��ȷ��");
		msgBox.setStandardButtons(QMessageBox::Yes| QMessageBox::No );
		QAbstractButton* tb1 = msgBox.button(QMessageBox::Yes);
		tb1->setText("��");
		QAbstractButton* tb2 = msgBox.button(QMessageBox::No);
		tb2->setText("��");

		int ret = msgBox.exec();


		if (ret == QMessageBox::Yes)
		{
			close();
		}
		else
		{
			e->ignore();
		}
	}
}

void econwin::mytimeupdate()
{
	if(hmirun)
	{
		mytimer.stop();
		basewidget->repaint();
		mytimer.start(200);
	}
}



void econwin::on_pushButton_2_clicked()
{	
	if(hmirun == false)
	{
		hmistate = 1;
		hmisubject = 1;
		hmisubjectstep = 1;
	}
}

void econwin::on_pb_juxing_clicked()
{
	if(hmirun == false)
	{
		hmistate = 1;
		hmisubject = 2;
		hmisubjectstep = 1;
	}
}

void econwin::on_pb_wenzhi_clicked()
{
	if(hmirun == false)
	{
		hmistate = 1;
		hmisubject = 3;
		hmisubjectstep = 1;
	}
}

void econwin::on_pb_shuzhi_clicked()
{
	if(hmirun == false)
	{
		hmistate = 1;
		hmisubject = 4;
		hmisubjectstep = 1;
	}
}


void econwin::on_pb_zhishideng_clicked()
{
	if(hmirun == false)
	{
		hmistate = 1;
		hmisubject = 5;
		hmisubjectstep = 1;
	}
}

void econwin::keyPressEvent( QKeyEvent * event )
{	if(hmirun == false)
{
	if(event->key() == Qt::Key_Escape)
	{
		hmistate = 0;
		hmisubject = 0;
		hmisubjectstep = 0;
		basewidget->setCursor(Qt::ArrowCursor);
		crossflag = 0;
	}

	//������û��ҳ��
	if(currenthmipageindex < 0)
		return;

	if(event->key() == Qt::Key_Delete)
	{
		if((currenthmiwidgetindex < hmiproject.hmipagelist[currenthmipageindex].count())
			&&(currenthmiwidgetindex >= 0)
			&& (hmiproject.hmipagelist[currenthmipageindex].count() >= 1))
		{
			if(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].widgettype == 1 )
			{
				delete (hmipb *)hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].pdata;
			}
			else if(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].widgettype == 2 )
			{
				delete (hmirect *)hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].pdata;
			}
			else if(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].widgettype == 3 )
			{
				delete (hmitext *)hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].pdata;
			}
			else if(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].widgettype == 4 )
			{
				delete (hminumber *)hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].pdata;
			}
			else if(hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].widgettype == 5 )
			{
				delete (hmilight *)hmiproject.hmipagelist[currenthmipageindex][currenthmiwidgetindex].pdata;
			}
			hmiproject.hmipagelist[currenthmipageindex].removeAt(currenthmiwidgetindex);
			hmistate = 0;
			repaint();

			projectneedsave = true;

			if(projectneedsave)
				this->setWindowTitle(tr("ecomwin��") + projecthmifilename + "*");
			else
				this->setWindowTitle(tr("ecomwin��") + projecthmifilename);

		}
	}
}

}

void econwin::on_pushButton_save_clicked()
{
	QString fileName;
	if( projecthmifilename == QString(tr("δ����")) )
	{
		fileName = QFileDialog::getSaveFileName(this,
			tr("�����ļ�"), QCoreApplication::applicationDirPath()+"/hmi", tr("�����ļ�(*.lua)"));
		if (!fileName.isEmpty())
			projecthmifilename = fileName;
	}
	else
	{
		fileName = projecthmifilename;
	}

	projectneedsave = false;

	if(projectneedsave)
		this->setWindowTitle(tr("ecomwin��") + projecthmifilename + "*");
	else
		this->setWindowTitle(tr("ecomwin��") + projecthmifilename);

	if (!fileName.isEmpty())
	{
		QFile file(fileName);

		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(&file);
		out << tr("--��Ҫ�༭�����ļ����Զ����ɵ�") << "\n";
		out << "hmiproject={" << "\n";
		//ҳ������
		out << "	len = "<< QString::number(hmiproject.len) << "," << "\n";
		out << "	firstpageid = "<< QString::number(hmiproject.firstpageid) << "," << "\n";
		//����дҳ������
		for(int iii = 0;iii <  hmiproject.len;iii++)
		{
			out << "	["<<QString::number(iii) <<"] = {" << "\n";
			//ҳ�������
			out << "		id = " <<  QString::number(hmiproject.idlist[iii]) << "," << "\n";
			out << "		name = [[" <<  hmiproject.pagelist[iii].name << "]]," << "\n";
			out << "		color = '" <<  hmiproject.pagelist[iii].color << "'," << "\n";
			out << "		needpasswd = " <<  QString::number(hmiproject.pagelist[iii].needpasswd) << "," << "\n";
			out << "		passwd = [[" <<  hmiproject.pagelist[iii].passwd << "]]," << "\n";

			//ҳ���еĿؼ�����
			out << "		len = " <<  QString::number(hmiproject.hmipagelist[iii].count()) << "," << "\n";

			//����дҳ��ؼ�����
			for(int i = 0;i < hmiproject.hmipagelist[iii].count();i++)
			{
				out << "		[" <<  QString::number(i) << "]" << "={\n";
				out << "			x = " <<  QString::number(hmiproject.hmipagelist[iii][i].x) << ",\n";
				out << "			y = " <<  QString::number(hmiproject.hmipagelist[iii][i].y) << ",\n";
				out << "			w = " <<  QString::number(hmiproject.hmipagelist[iii][i].w) << ",\n";
				out << "			h = " <<  QString::number(hmiproject.hmipagelist[iii][i].h) << ",\n";
				//��ť����������
				if(hmiproject.hmipagelist[iii][i].widgettype == 1)
				{
					out << "			widgettype = 1,\n";
					out << "			data = {\n";
					hmipb * ppb = (hmipb *)hmiproject.hmipagelist[iii][i].pdata;
					out << "				text = [[" << ppb->text <<"]],\n";
					out << "				fontcolor = '" << ppb->fontcolor <<"',\n";
					out << "				fontfamily = '" << ppb->fontfamily <<"',\n";
					out << "				fontpointSize = " << QString::number(ppb->fontpointSize) <<",\n";
					out << "				fontweight = " << QString::number(ppb->fontweight) <<",\n";
					out << "				fontitalic = " << QString::number(ppb->fontitalic) <<",\n";
					out << "				operation = " << QString::number(ppb->operation) <<",\n";
					out << "				targetpage = " << ppb->targetpage <<",\n";
					out << "				targetvar = '" << ppb->targetvar <<"',\n";
					out << "				colortype = " << QString::number(ppb->colortype) <<",\n";
					out << "				coloroff = '" << ppb->coloroff <<"',\n";
					out << "				coloron = '" << ppb->coloron <<"',\n";
					out << "				colorvar = '" << ppb->colorvar <<"'\n";
					out << "			}\n";
				}
				//���ε���������
				else if(hmiproject.hmipagelist[iii][i].widgettype == 2)
				{
					out << "			widgettype = 2,\n";
					out << "			data = {\n";
					hmirect * prect = (hmirect *)hmiproject.hmipagelist[iii][i].pdata;
					out << "				border = " << QString::number(prect->border) <<",\n";
					out << "				bordercolor = '" << prect->bordercolor <<"',\n";
					out << "				intercolor = '" << prect->intercolor <<"'\n";
					out << "			}\n";
				}
				//���ֵ���������
				else if(hmiproject.hmipagelist[iii][i].widgettype == 3)
				{
					out << "			widgettype = 3,\n";
					out << "			data = {\n";
					hmitext * ptext = (hmitext *)hmiproject.hmipagelist[iii][i].pdata;
					out << "				text = [[" << ptext->text <<"]],\n";
					out << "				fontcolor = '" << ptext->fontcolor <<"',\n";
					out << "				fontfamily = '" << ptext->fontfamily <<"',\n";
					out << "				fontpointSize = " << QString::number(ptext->fontpointSize) <<",\n";
					out << "				fontweight = " << QString::number(ptext->fontweight) <<",\n";
					out << "				fontitalic = " << QString::number(ptext->fontitalic) <<",\n";
					out << "				touming = " << QString::number(ptext->touming) <<",\n";
					out << "				border = " << QString::number(ptext->border) <<",\n";
					out << "				bordercolor = '" << ptext->bordercolor <<"',\n";
					out << "				intercolor = '" << ptext->intercolor <<"'\n";
					out << "			}\n";
				}
				//���ֵ���������
				else if(hmiproject.hmipagelist[iii][i].widgettype == 4)
				{
					out << "			widgettype = 4,\n";
					out << "			data = {\n";
					hminumber * pnumber = (hminumber *)hmiproject.hmipagelist[iii][i].pdata;
					out << "				vartype = " << QString::number(pnumber->vartype) <<",\n";
					out << "				targetvar = '" << pnumber->targetvar <<"',\n";
					out << "				zhengshuwei = " << QString::number(pnumber->zhengshuwei) <<",\n";
					out << "				xiaoshuwei = " << QString::number(pnumber->xiaoshuwei) <<",\n";
					out << "				shuru = " << QString::number(pnumber->shuru) <<",\n";
					out << "				fanweijiancha = " << QString::number(pnumber->fanweijiancha) <<",\n";
					out << "				zuixiaozhi = " << QString::number(pnumber->zuixiaozhi) <<",\n";
					out << "				zuidazhi = " << QString::number(pnumber->zuidazhi) <<",\n";
					out << "				fontcolor = '" << pnumber->fontcolor <<"',\n";
					out << "				fontfamily = '" << pnumber->fontfamily <<"',\n";
					out << "				fontpointSize = " << QString::number(pnumber->fontpointSize) <<",\n";
					out << "				fontweight = " << QString::number(pnumber->fontweight) <<",\n";
					out << "				fontitalic = " << QString::number(pnumber->fontitalic) <<",\n";
					out << "				border = " << QString::number(pnumber->border) <<",\n";
					out << "				bordercolor = '" << pnumber->bordercolor <<"',\n";
					out << "				intercolor = '" << pnumber->intercolor <<"'\n";
					out << "			}\n";
				}
				//ָʾ�Ƶ���������
				else if(hmiproject.hmipagelist[iii][i].widgettype == 5)
				{
					out << "			widgettype = 5,\n";
					out << "			data = {\n";
					hmilight * plight = (hmilight *)hmiproject.hmipagelist[iii][i].pdata;
					out << "				targetvar = '" << plight->targetvar <<"',\n";
					out << "				number = " << QString::number(plight->number) <<",\n"; 

					for(int ii = 0;ii < plight->number;ii++)
					{
						out << "				[" <<  QString::number(ii) << "]" << "={\n";
						out << "					text = [[" << plight->lightdata[ii].text <<"]],\n";
						out << "					fontcolor = '" << plight->lightdata[ii].fontcolor <<"',\n";
						out << "					fontfamily = '" << plight->lightdata[ii].fontfamily <<"',\n";
						out << "					fontpointSize = " << QString::number(plight->lightdata[ii].fontpointSize) <<",\n";
						out << "					fontweight = " << QString::number(plight->lightdata[ii].fontweight) <<",\n";
						out << "					fontitalic = " << QString::number(plight->lightdata[ii].fontitalic) <<",\n";
						out << "					border = " << QString::number(plight->lightdata[ii].border) <<",\n";
						out << "					bordercolor = '" << plight->lightdata[ii].bordercolor <<"',\n";
						out << "					intercolor = '" << plight->lightdata[ii].intercolor <<"'\n";
						if(ii != plight->number -1)
						{
							out << "				},\n";
						}
						else
						{
							out << "				}\n";
						}

					}

					out << "			}\n";
				}
				out << "		}";

				if(i != hmiproject.hmipagelist[iii].count() -1)
				{
					out << ",\n";
				}
				else
				{
					out << "\n";
				}
			}

			if(iii != hmiproject.len -1)
			{
				out << "	},\n";

			}
			else
			{
				out << "	}\n";
			}

		}

		out << "}\n";

		file.flush();
		file.close();
	}
	else
	{
		return;
	}

}
void econwin::on_pb_yunxing_clicked()
{
	if(hmirun)
	{
		hmirun = false;
		projecthmirun = false;

		ui.pb_yunxing->setText(tr("�л�����"));

		//�ָ�ҳ��
		currenthmipageindex = oldhmipageindex;
		currenthmiwidgetindex = oldhmiwidgetindex;

		//��ť�Լ��ؼ���ʾ

		ui.frame->show();
		ui.pushButton_2->show();
		ui.pb_juxing->show();
		ui.pb_wenzhi->show();
		ui.pb_shuzhi->show();
		ui.pb_zhishideng->show();
		ui.pushButton_read->show();
		ui.pushButton_save->show();
		ui.yemiantableWidget->show();
		ui.yemianshuxingtableWidget->show();
		ui.pb_shanchuyemian->show();
		ui.pb_shezhishouye->show();
		ui.pb_zhenjiayemian->show();
		ui.pb_new->show();

		repaint();
	}
	else
	{
		hmirun = true;
		projecthmirun = true;
		ui.pb_yunxing->setText(tr("�������"));

		//����ҳ��Ϳؼ�
		oldhmipageindex = currenthmipageindex;
		oldhmiwidgetindex = currenthmiwidgetindex;

		currenthmipageindex = hmiproject.idlist.indexOf(hmiproject.firstpageid);

		//��ť�Լ��ؼ�����

		ui.frame->hide();
		ui.pushButton_2->hide();
		ui.pb_juxing->hide();
		ui.pb_wenzhi->hide();
		ui.pb_shuzhi->hide();
		ui.pb_zhishideng->hide();
		ui.pushButton_read->hide();
		ui.pushButton_save->hide();
		ui.yemiantableWidget->hide();
		ui.yemianshuxingtableWidget->hide();
		ui.pb_shanchuyemian->hide();
		ui.pb_shezhishouye->hide();
		ui.pb_zhenjiayemian->hide();
		ui.pb_new->hide();
		repaint();
	}
}

void econwin::on_pushButton_read_clicked()
{
	QString fileName;
	if(intcall)
	{
		fileName = projecthmifilename;
		intcall = false;

	}
	else
	{
		fileName = QFileDialog::getOpenFileName(this,
			tr("���ļ�"), QCoreApplication::applicationDirPath()+"/hmi", tr("�����ļ�(*.lua)"));

	}
	if (!fileName.isEmpty())
	{

		//ִ���ļ�
		if(luaL_loadfile(L2,fileName.toAscii().data()) || lua_pcall(L2,0,0,0) )
		{
			printf("�����ļ���ʽ������%s",lua_tostring(L2,-1));
		}
		else
		{
			projectneedsave = false;
			projecthmifilename = fileName;

			if(projectneedsave)
				this->setWindowTitle(tr("ecomwin��") + projecthmifilename + "*");
			else
				this->setWindowTitle(tr("ecomwin��") + projecthmifilename);


			//����ɾ��ҳ��Ϳؼ�
			hmiproject.idlist.clear();
			hmiproject.pagelist.clear();

			for(int i = 0;i < hmiproject.hmipagelist.count();i++)
			{
				if(hmiproject.hmipagelist[i].count() > 0)
				{

					for(int ii = 0;ii < hmiproject.hmipagelist[i].count();ii++)
					{
						if(hmiproject.hmipagelist[i][ii].widgettype == 1 )
						{
							delete (hmipb *)hmiproject.hmipagelist[i][ii].pdata;
						}
						else if(hmiproject.hmipagelist[i][ii].widgettype == 2 )
						{
							delete (hmirect *)hmiproject.hmipagelist[i][ii].pdata;
						}
						else if(hmiproject.hmipagelist[i][ii].widgettype == 3 )
						{
							delete (hmitext *)hmiproject.hmipagelist[i][ii].pdata;
						}
						else if(hmiproject.hmipagelist[i][ii].widgettype == 4 )
						{
							delete (hminumber *)hmiproject.hmipagelist[i][ii].pdata;
						}
						else if(hmiproject.hmipagelist[i][ii].widgettype == 5 )
						{
							delete (hmilight *)hmiproject.hmipagelist[i][ii].pdata;
						}
					}
					hmiproject.hmipagelist[i].clear();

				}
			}
			hmiproject.hmipagelist.clear();
		}

		lua_getglobal(L2,"hmiproject");
		if(lua_istable(L2,-1))
		{
			int pagelen = 0;

			//ȡ�ó��ȣ�ҳ�������
			lua_pushstring(L2,"len");
			lua_gettable(L2,-2);
			if(lua_isnumber(L2,-1))
			{
				pagelen = lua_tointeger(L2,-1);
			}
			//��������
			lua_pop(L2,1);

			hmiproject.len = pagelen;

			//ȡ����ҳid
			lua_pushstring(L2,"firstpageid");
			lua_gettable(L2,-2);
			if(lua_isnumber(L2,-1))
			{
				hmiproject.firstpageid = lua_tointeger(L2,-1);
			}
			//��������
			lua_pop(L2,1);

			for(int iii = 0;iii < pagelen;iii++)
			{

				//ҳ��table
				lua_pushinteger(L2,iii);
				lua_gettable(L2,-2);
				if(lua_istable(L2,-1))
				{
					struct _page page;

					//ҳ��������
					lua_pushstring(L2,"id");
					lua_gettable(L2,-2);
					if(lua_isnumber(L2,-1))
					{
						page.id = lua_tointeger(L2,-1);
					}
					//����
					lua_pop(L2,1);

					lua_pushstring(L2,"name");
					lua_gettable(L2,-2);
					if(lua_isstring(L2,-1))
					{
						page.name = lua_tostring(L2,-1);
					}
					//����
					lua_pop(L2,1);

					lua_pushstring(L2,"color");
					lua_gettable(L2,-2);
					if(lua_isstring(L2,-1))
					{
						page.color = lua_tostring(L2,-1);
					}
					//����
					lua_pop(L2,1);

					lua_pushstring(L2,"needpasswd");
					lua_gettable(L2,-2);
					if(lua_isnumber(L2,-1))
					{
						page.needpasswd = lua_tointeger(L2,-1);
					}
					//����
					lua_pop(L2,1);

					lua_pushstring(L2,"passwd");
					lua_gettable(L2,-2);
					if(lua_isstring(L2,-1))
					{
						page.passwd = lua_tostring(L2,-1);
					}
					//����
					lua_pop(L2,1);

					int len = 0;
					//ȡ�ó��ȣ�ҳ���еĿؼ�����
					lua_pushstring(L2,"len");
					lua_gettable(L2,-2);
					if(lua_isnumber(L2,-1))
					{
						len = lua_tointeger(L2,-1);
					}
					//��������
					lua_pop(L2,1);

					hmiproject.idlist.append(page.id);
					hmiproject.pagelist.append(page);
					QList<struct _hmiwidget> twlist;
					hmiproject.hmipagelist.append(twlist);


					//�������еĿؼ������ڴ�������list
					for(int i = 0;i < len;i++)
					{
						//ȡ�ÿؼ�table
						lua_pushinteger(L2,i);
						lua_gettable(L2,-2);
						if(lua_istable(L2,-1))
						{
							struct _hmiwidget hmiwidget;

							//ȡ�ÿؼ��ĸ�������
							lua_pushstring(L2,"x");
							lua_gettable(L2,-2);
							if(lua_isnumber(L2,-1))
							{
								hmiwidget.x = lua_tointeger(L2,-1);
							}
							//�����ؼ�����
							lua_pop(L2,1);

							lua_pushstring(L2,"y");
							lua_gettable(L2,-2);
							if(lua_isnumber(L2,-1))
							{
								hmiwidget.y = lua_tointeger(L2,-1);
							}
							//�����ؼ�����
							lua_pop(L2,1);

							lua_pushstring(L2,"w");
							lua_gettable(L2,-2);
							if(lua_isnumber(L2,-1))
							{
								hmiwidget.w = lua_tointeger(L2,-1);
							}
							//�����ؼ�����
							lua_pop(L2,1);

							lua_pushstring(L2,"h");
							lua_gettable(L2,-2);
							if(lua_isnumber(L2,-1))
							{
								hmiwidget.h = lua_tointeger(L2,-1);
							}
							//�����ؼ�����
							lua_pop(L2,1);

							lua_pushstring(L2,"widgettype");
							lua_gettable(L2,-2);
							if(lua_isnumber(L2,-1))
							{
								hmiwidget.widgettype = lua_tointeger(L2,-1);
							}
							//�����ؼ�����
							lua_pop(L2,1);

							if(hmiwidget.widgettype == 1)
							{
								lua_pushstring(L2,"data");
								lua_gettable(L2,-2);

								if(lua_isnil(L2,-1))
								{
									//cout << "nil" << endl;;
								}
								else if(lua_istable(L2,-1))
								{

									hmipb* nbutton = new hmipb;

									lua_pushstring(L2,"text");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										nbutton->text = lua_tostring(L2,-1);
										//cout << QString(lua_tostring(L2,-1)) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"fontcolor");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										nbutton->fontcolor = lua_tostring(L2,-1);
										//cout << QString(lua_tostring(L2,-1)) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"fontfamily");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										nbutton->fontfamily = lua_tostring(L2,-1);
										//cout << QString(lua_tostring(L2,-1)) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"fontpointSize");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nbutton->fontpointSize = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"fontweight");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nbutton->fontweight = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"fontitalic");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nbutton->fontitalic = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);


									lua_pushstring(L2,"operation");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nbutton->operation = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"targetvar");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										nbutton->targetvar = lua_tostring(L2,-1);
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"targetpage");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nbutton->targetpage = lua_tointeger(L2,-1);
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"colortype");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nbutton->colortype = lua_tointeger(L2,-1);
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"coloron");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										nbutton->coloron = lua_tostring(L2,-1);
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"coloroff");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										nbutton->coloroff = lua_tostring(L2,-1);
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"colorvar");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										nbutton->colorvar = lua_tostring(L2,-1);
									}
									//����data����
									lua_pop(L2,1);
									nbutton->clicked = false;

									hmiwidget.pdata = nbutton;
								}

								//����data��
								lua_pop(L2,1);
							}
							else if(hmiwidget.widgettype == 2)
							{
								lua_pushstring(L2,"data");
								lua_gettable(L2,-2);

								if(lua_isnil(L2,-1))
								{
									//cout << "nil" << endl;;
								}
								else if(lua_istable(L2,-1))
								{

									hmirect* nrect = new hmirect;

									lua_pushstring(L2,"border");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nrect->border = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"bordercolor");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										nrect->bordercolor = lua_tostring(L2,-1);
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"intercolor");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										nrect->intercolor = lua_tostring(L2,-1);
									}
									//����data����
									lua_pop(L2,1);

									hmiwidget.pdata = nrect;
								}

								//����data��
								lua_pop(L2,1);
							}

							else if(hmiwidget.widgettype == 3)
							{
								lua_pushstring(L2,"data");
								lua_gettable(L2,-2);

								if(lua_isnil(L2,-1))
								{
									//cout << "nil" << endl;;
								}
								else if(lua_istable(L2,-1))
								{

									hmitext* ntext = new hmitext;

									lua_pushstring(L2,"text");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										ntext->text = lua_tostring(L2,-1);
										//cout << QString(lua_tostring(L2,-1)) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"fontcolor");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										ntext->fontcolor = lua_tostring(L2,-1);
										//cout << QString(lua_tostring(L2,-1)) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"fontfamily");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										ntext->fontfamily = lua_tostring(L2,-1);
										//cout << QString(lua_tostring(L2,-1)) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"fontpointSize");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										ntext->fontpointSize = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"fontweight");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										ntext->fontweight = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"fontitalic");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										ntext->fontitalic = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"border");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										ntext->border = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"touming");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										ntext->touming = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"bordercolor");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										ntext->bordercolor = lua_tostring(L2,-1);
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"intercolor");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										ntext->intercolor = lua_tostring(L2,-1);
									}
									//����data����
									lua_pop(L2,1);

									hmiwidget.pdata = ntext;
								}

								//����data��
								lua_pop(L2,1);
							}


							else if(hmiwidget.widgettype == 4)
							{
								lua_pushstring(L2,"data");
								lua_gettable(L2,-2);

								if(lua_isnil(L2,-1))
								{
									//cout << "nil" << endl;;
								}
								else if(lua_istable(L2,-1))
								{

									hminumber* nnumber = new hminumber;

									lua_pushstring(L2,"vartype");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nnumber->vartype = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);


									lua_pushstring(L2,"targetvar");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										nnumber->targetvar = lua_tostring(L2,-1);
										//cout << QString(lua_tostring(L2,-1)) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"zhengshuwei");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nnumber->zhengshuwei = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);
									lua_pushstring(L2,"xiaoshuwei");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nnumber->xiaoshuwei = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);
									lua_pushstring(L2,"shuru");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nnumber->shuru = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);
									lua_pushstring(L2,"fanweijiancha");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nnumber->fanweijiancha = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);
									lua_pushstring(L2,"zuixiaozhi");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nnumber->zuixiaozhi = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);
									lua_pushstring(L2,"zuidazhi");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nnumber->zuidazhi = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"fontcolor");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										nnumber->fontcolor = lua_tostring(L2,-1);
										//cout << QString(lua_tostring(L2,-1)) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"fontfamily");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										nnumber->fontfamily = lua_tostring(L2,-1);
										//cout << QString(lua_tostring(L2,-1)) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"fontpointSize");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nnumber->fontpointSize = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"fontweight");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nnumber->fontweight = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"fontitalic");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nnumber->fontitalic = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"border");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nnumber->border = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"bordercolor");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										nnumber->bordercolor = lua_tostring(L2,-1);
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"intercolor");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										nnumber->intercolor = lua_tostring(L2,-1);
									}
									//����data����
									lua_pop(L2,1);

									hmiwidget.pdata = nnumber;
								}

								//����data��
								lua_pop(L2,1);
							}



							else if(hmiwidget.widgettype == 5)
							{
								lua_pushstring(L2,"data");
								lua_gettable(L2,-2);

								if(lua_isnil(L2,-1))
								{
									//cout << "nil" << endl;;
								}
								else if(lua_istable(L2,-1))
								{

									hmilight* nlight = new hmilight;

									lua_pushstring(L2,"targetvar");
									lua_gettable(L2,-2);
									if(lua_isstring(L2,-1))
									{
										nlight->targetvar = lua_tostring(L2,-1);
										//cout << QString(lua_tostring(L2,-1)) << endl;
									}
									//����data����
									lua_pop(L2,1);

									lua_pushstring(L2,"number");
									lua_gettable(L2,-2);
									if(lua_isnumber(L2,-1))
									{
										nlight->number = lua_tointeger(L2,-1);
										//cout << lua_tointeger(L2,-1) << endl;
									}
									//����data����
									lua_pop(L2,1);

									for(int ii = 0;ii < nlight->number;ii++)
									{
										lua_pushinteger(L2,ii);
										lua_gettable(L2,-2);
										if(lua_istable(L2,-1))
										{
											lua_pushstring(L2,"text");
											lua_gettable(L2,-2);
											if(lua_isstring(L2,-1))
											{
												nlight->lightdata[ii].text = lua_tostring(L2,-1);
												//cout << QString(lua_tostring(L2,-1)) << endl;
											}
											//����lightdata����
											lua_pop(L2,1);


											lua_pushstring(L2,"fontcolor");
											lua_gettable(L2,-2);
											if(lua_isstring(L2,-1))
											{
												nlight->lightdata[ii].fontcolor = lua_tostring(L2,-1);
												//cout << QString(lua_tostring(L2,-1)) << endl;
											}
											//����lightdata����
											lua_pop(L2,1);

											lua_pushstring(L2,"fontfamily");
											lua_gettable(L2,-2);
											if(lua_isstring(L2,-1))
											{
												nlight->lightdata[ii].fontfamily = lua_tostring(L2,-1);
												//cout << QString(lua_tostring(L2,-1)) << endl;
											}
											//����lightdata����
											lua_pop(L2,1);

											lua_pushstring(L2,"fontpointSize");
											lua_gettable(L2,-2);
											if(lua_isnumber(L2,-1))
											{
												nlight->lightdata[ii].fontpointSize = lua_tointeger(L2,-1);
												//cout << lua_tointeger(L2,-1) << endl;
											}
											//����lightdata����
											lua_pop(L2,1);

											lua_pushstring(L2,"fontweight");
											lua_gettable(L2,-2);
											if(lua_isnumber(L2,-1))
											{
												nlight->lightdata[ii].fontweight = lua_tointeger(L2,-1);
												//cout << lua_tointeger(L2,-1) << endl;
											}
											//����lightdata����
											lua_pop(L2,1);

											lua_pushstring(L2,"fontitalic");
											lua_gettable(L2,-2);
											if(lua_isnumber(L2,-1))
											{
												nlight->lightdata[ii].fontitalic = lua_tointeger(L2,-1);
												//cout << lua_tointeger(L2,-1) << endl;
											}
											//����lightdata����
											lua_pop(L2,1);

											lua_pushstring(L2,"border");
											lua_gettable(L2,-2);
											if(lua_isnumber(L2,-1))
											{
												nlight->lightdata[ii].border = lua_tointeger(L2,-1);
												//cout << lua_tointeger(L2,-1) << endl;
											}
											//����lightdata����
											lua_pop(L2,1);

											lua_pushstring(L2,"bordercolor");
											lua_gettable(L2,-2);
											if(lua_isstring(L2,-1))
											{
												nlight->lightdata[ii].bordercolor = lua_tostring(L2,-1);
											}
											//����lightdata����
											lua_pop(L2,1);

											lua_pushstring(L2,"intercolor");
											lua_gettable(L2,-2);
											if(lua_isstring(L2,-1))
											{
												nlight->lightdata[ii].intercolor = lua_tostring(L2,-1);
											}
											//����lightdata����
											lua_pop(L2,1);
										}
										//����lightdata��
										lua_pop(L2,1);
									}

									for(int ii = nlight->number;ii < 32;ii++)
									{
										nlight->lightdata[ii].text = tr("ָʾ��");
										nlight->lightdata[ii].fontcolor = "#00ff00";
										nlight->lightdata[ii].fontfamily = QString(tr("����"));
										nlight->lightdata[ii].fontpointSize = 12;
										nlight->lightdata[ii].fontweight= 50;
										nlight->lightdata[ii].fontitalic = false;

										nlight->lightdata[ii].border = 2;
										nlight->lightdata[ii].bordercolor = "#00ff00";
										nlight->lightdata[ii].intercolor = "#a0a0a4";
									}
									hmiwidget.pdata = nlight;
								}
								//����data��
								lua_pop(L2,1);
							}
							hmiproject.hmipagelist[iii].append(hmiwidget);
						}

						//�����ؼ���
						lua_pop(L2,1);
					}
				}
				//����ҳ��table
				lua_pop(L2,1);
			}
		}
		//����project
		lua_pop(L2,1);

		hmistate = 0;
		hmisubject = 0;
		hmisubjectstep = 0;

		int firstpageindex = hmiproject.idlist.indexOf(hmiproject.firstpageid);

		//ˢ�µ�ҳ����
		ui.yemiantableWidget->setRowCount(hmiproject.pagelist.count());
		for(int i = 0;i < hmiproject.pagelist.count();i++)
		{
			//��ҳҪ����ʶ
			if(i == firstpageindex)
			{
				ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name +tr("(*)")));
			}
			else
			{
				ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name));
			}
			ui.yemiantableWidget->item(i,0)->setFlags(0);
			ui.yemiantableWidget->item(i,0)->setForeground(Qt::black);
		}

		//����ҳ�����Ե�ʱ��رղ����޸ļ��
		checkyemianchanshu = false;

		//��һ��ҳ�������
		ui.yemianshuxingtableWidget->setRowCount(5);
		ui.yemianshuxingtableWidget->setItem(0,0, new QTableWidgetItem("ID"));
		ui.yemianshuxingtableWidget->item(0,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(0,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(0,1, new QTableWidgetItem(QString::number(hmiproject.pagelist[0].id)));
		ui.yemianshuxingtableWidget->item(0,1)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(1,0, new QTableWidgetItem(tr("ҳ������")));
		ui.yemianshuxingtableWidget->item(1,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(1,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(1,1, new QTableWidgetItem((hmiproject.pagelist[0].name)));
		ui.yemianshuxingtableWidget->item(1,1)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(2,0, new QTableWidgetItem(tr("������ɫ")));
		ui.yemianshuxingtableWidget->item(2,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(2,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(2,1, new QTableWidgetItem(("")));
		ui.yemianshuxingtableWidget->item(2,1)->setFlags(0);
		ui.yemianshuxingtableWidget->item(2,1)->setBackground(QColor(hmiproject.pagelist[0].color));

		ui.yemianshuxingtableWidget->setItem(3,0, new QTableWidgetItem(tr("��Ҫ����")));
		ui.yemianshuxingtableWidget->item(3,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(3,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(3,1, new QTableWidgetItem(QString::number(hmiproject.pagelist[0].needpasswd)));
		ui.yemianshuxingtableWidget->item(3,1)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(4,0, new QTableWidgetItem(tr("����")));
		ui.yemianshuxingtableWidget->item(4,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(4,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(4,1, new QTableWidgetItem(hmiproject.pagelist[0].passwd));
		ui.yemianshuxingtableWidget->item(4,1)->setForeground(Qt::black);

		checkyemianchanshu = true;

		currenthmipageindex = 0;
		currenthmiwidgetindex = -1;
		hmistate = 0;
		repaint();
	}
	else
	{
		return;
	}

}


//ҳ���л�
void econwin::onTableClicked(int row,int col)
{
	hmistate = 0;
	hmisubject = 0;
	hmisubjectstep = 0;

	if((col == 0) && (hmirun == false))
	{
		//cout << "onTableClicked:" << row << endl;
		currenthmipageindex = row;
		currenthmiwidgetindex = -1;
		repaint();

		checkyemianchanshu = false;

		ui.yemianshuxingtableWidget->setRowCount(5);
		ui.yemianshuxingtableWidget->setItem(0,0, new QTableWidgetItem("ID"));
		ui.yemianshuxingtableWidget->item(0,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(0,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(0,1, new QTableWidgetItem(QString::number(hmiproject.pagelist[currenthmipageindex].id)));
		ui.yemianshuxingtableWidget->item(0,1)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(1,0, new QTableWidgetItem(tr("ҳ������")));
		ui.yemianshuxingtableWidget->item(1,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(1,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(1,1, new QTableWidgetItem((hmiproject.pagelist[currenthmipageindex].name)));
		ui.yemianshuxingtableWidget->item(1,1)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(2,0, new QTableWidgetItem(tr("������ɫ")));
		ui.yemianshuxingtableWidget->item(2,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(2,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(2,1, new QTableWidgetItem(("")));
		ui.yemianshuxingtableWidget->item(2,1)->setFlags(0);
		ui.yemianshuxingtableWidget->item(2,1)->setBackground(QColor(hmiproject.pagelist[currenthmipageindex].color));

		ui.yemianshuxingtableWidget->setItem(3,0, new QTableWidgetItem(tr("��Ҫ����")));
		ui.yemianshuxingtableWidget->item(3,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(3,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(3,1, new QTableWidgetItem(QString::number(hmiproject.pagelist[currenthmipageindex].needpasswd)));
		ui.yemianshuxingtableWidget->item(3,1)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(4,0, new QTableWidgetItem(tr("����")));
		ui.yemianshuxingtableWidget->item(4,0)->setFlags(0);
		ui.yemianshuxingtableWidget->item(4,0)->setForeground(Qt::black);

		ui.yemianshuxingtableWidget->setItem(4,1, new QTableWidgetItem(hmiproject.pagelist[currenthmipageindex].passwd));
		ui.yemianshuxingtableWidget->item(4,1)->setForeground(Qt::black);

		checkyemianchanshu = true;

	}
}

//����ҳ��
void econwin::on_pb_zhenjiayemian_clicked()
{
	projectneedsave = true;
	this->setWindowTitle(tr("ecomwin��") + projecthmifilename + "*");

	hmistate = 0;
	hmisubject = 0;
	hmisubjectstep = 0;

	int id = 1;
	while(1)
	{
		if(hmiproject.idlist.contains(id))
		{
			id++;
		}
		else
		{
			break;
		}
	}

	//Ĭ������һ��ҳ��
	struct _page page;
	page.id = id;
	page.name = tr("ҳ��")+ QString::number(id);
	page.color = "#c0c0c0";
	page.needpasswd = 0;
	page.passwd = "";
	page.len = 0;


	hmiproject.pagelist.append(page);

	//id�б�
	hmiproject.idlist.append(id);

	//һ��ҳ��
	hmiproject.len ++ ;

	QList<struct _hmiwidget> emptypage;
 
	//һ����ҳ��
	hmiproject.hmipagelist.append(emptypage);

	int firstpageindex = hmiproject.idlist.indexOf(hmiproject.firstpageid);

	//ˢ�µ�ҳ����
	ui.yemiantableWidget->setRowCount(hmiproject.pagelist.count());
	for(int i = 0;i < hmiproject.pagelist.count();i++)
	{
		//��ҳҪ����ʶ
		if(i == firstpageindex)
		{
			ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name +tr("(*)")));
		}
		else
		{
			ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name));
		}
		ui.yemiantableWidget->item(i,0)->setFlags(0);
		ui.yemiantableWidget->item(i,0)->setForeground(Qt::black);
	}

	currenthmipageindex = hmiproject.idlist.indexOf(id);
	currenthmiwidgetindex = -1;
	ui.yemiantableWidget->setCurrentCell(currenthmipageindex,0);

	checkyemianchanshu = false;

	ui.yemianshuxingtableWidget->setRowCount(5);
	ui.yemianshuxingtableWidget->setItem(0,0, new QTableWidgetItem("ID"));
	ui.yemianshuxingtableWidget->item(0,0)->setFlags(0);
	ui.yemianshuxingtableWidget->item(0,0)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(0,1, new QTableWidgetItem(QString::number(hmiproject.pagelist[currenthmipageindex].id)));
	ui.yemianshuxingtableWidget->item(0,1)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(1,0, new QTableWidgetItem(tr("ҳ������")));
	ui.yemianshuxingtableWidget->item(1,0)->setFlags(0);
	ui.yemianshuxingtableWidget->item(1,0)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(1,1, new QTableWidgetItem((hmiproject.pagelist[currenthmipageindex].name)));
	ui.yemianshuxingtableWidget->item(1,1)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(2,0, new QTableWidgetItem(tr("������ɫ")));
	ui.yemianshuxingtableWidget->item(2,0)->setFlags(0);
	ui.yemianshuxingtableWidget->item(2,0)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(2,1, new QTableWidgetItem(("")));
	ui.yemianshuxingtableWidget->item(2,1)->setFlags(0);
	ui.yemianshuxingtableWidget->item(2,1)->setBackground(QColor(hmiproject.pagelist[currenthmipageindex].color));

	ui.yemianshuxingtableWidget->setItem(3,0, new QTableWidgetItem(tr("��Ҫ����")));
	ui.yemianshuxingtableWidget->item(3,0)->setFlags(0);
	ui.yemianshuxingtableWidget->item(3,0)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(3,1, new QTableWidgetItem(QString::number(hmiproject.pagelist[currenthmipageindex].needpasswd)));
	ui.yemianshuxingtableWidget->item(3,1)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(4,0, new QTableWidgetItem(tr("����")));
	ui.yemianshuxingtableWidget->item(4,0)->setFlags(0);
	ui.yemianshuxingtableWidget->item(4,0)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(4,1, new QTableWidgetItem(hmiproject.pagelist[currenthmipageindex].passwd));
	ui.yemianshuxingtableWidget->item(4,1)->setForeground(Qt::black);

	checkyemianchanshu = true;

	repaint();

	projectneedsave = true;

	if(projectneedsave)
		this->setWindowTitle(tr("ecomwin��") + projecthmifilename + "*");
	else
		this->setWindowTitle(tr("ecomwin��") + projecthmifilename);

}

//ɾ��ҳ��
void econwin::on_pb_shanchuyemian_clicked()
{
	//ɾ���ؼ�
	for(int ii = 0;ii < hmiproject.hmipagelist[currenthmipageindex].count();ii++)
	{
		if(hmiproject.hmipagelist[currenthmipageindex][ii].widgettype == 1 )
		{
			delete (hmipb *)hmiproject.hmipagelist[currenthmipageindex][ii].pdata;
		}
		else if(hmiproject.hmipagelist[currenthmipageindex][ii].widgettype == 2 )
		{
			delete (hmirect *)hmiproject.hmipagelist[currenthmipageindex][ii].pdata;
		}
		else if(hmiproject.hmipagelist[currenthmipageindex][ii].widgettype == 3 )
		{
			delete (hmitext *)hmiproject.hmipagelist[currenthmipageindex][ii].pdata;
		}
		else if(hmiproject.hmipagelist[currenthmipageindex][ii].widgettype == 4 )
		{
			delete (hminumber *)hmiproject.hmipagelist[currenthmipageindex][ii].pdata;
		}
		else if(hmiproject.hmipagelist[currenthmipageindex][ii].widgettype == 5 )
		{
			delete (hmilight *)hmiproject.hmipagelist[currenthmipageindex][ii].pdata;
		}
	}

	//ɾ��ҳ��
	hmiproject.hmipagelist[currenthmipageindex].clear();
	hmiproject.hmipagelist.removeAt(currenthmipageindex);
	hmiproject.pagelist.removeAt(currenthmipageindex);
	hmiproject.idlist.removeAt(currenthmipageindex);

	//ҳ������
	hmiproject.len -- ;

	hmistate = 0;
	hmisubject = 0;
	hmisubjectstep = 0;
	currenthmiwidgetindex = -1;

	//�л���ǰҳ��
	if(currenthmipageindex >= hmiproject.len)
	{
		if((currenthmipageindex -1) >= 0)
		{
			currenthmipageindex --;
		}
		else
		{
			currenthmipageindex = -1;
		}
	}

	int firstpageindex = hmiproject.idlist.indexOf(hmiproject.firstpageid);

	//ˢ�µ�ҳ����
	ui.yemiantableWidget->setRowCount(hmiproject.pagelist.count());
	for(int i = 0;i < hmiproject.pagelist.count();i++)
	{
		//��ҳҪ����ʶ
		if(i == firstpageindex)
		{
			ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name +tr("(*)")));
		}
		else
		{
			ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name));
		}
		ui.yemiantableWidget->item(i,0)->setFlags(0);
		ui.yemiantableWidget->item(i,0)->setForeground(Qt::black);
	}

	currenthmiwidgetindex = -1;
	ui.yemiantableWidget->setCurrentCell(currenthmipageindex,0);

	if(currenthmipageindex < 0)
	{
		checkyemianchanshu = false;
		ui.yemianshuxingtableWidget->setRowCount(0);

		ui.yemianshuxingtableWidget->clear();
		checkyemianchanshu = true;
		return;
	}


	checkyemianchanshu = false;

	ui.yemianshuxingtableWidget->setRowCount(5);
	ui.yemianshuxingtableWidget->setItem(0,0, new QTableWidgetItem("ID"));
	ui.yemianshuxingtableWidget->item(0,0)->setFlags(0);
	ui.yemianshuxingtableWidget->item(0,0)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(0,1, new QTableWidgetItem(QString::number(hmiproject.pagelist[currenthmipageindex].id)));
	ui.yemianshuxingtableWidget->item(0,1)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(1,0, new QTableWidgetItem(tr("ҳ������")));
	ui.yemianshuxingtableWidget->item(1,0)->setFlags(0);
	ui.yemianshuxingtableWidget->item(1,0)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(1,1, new QTableWidgetItem((hmiproject.pagelist[currenthmipageindex].name)));
	ui.yemianshuxingtableWidget->item(1,1)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(2,0, new QTableWidgetItem(tr("������ɫ")));
	ui.yemianshuxingtableWidget->item(2,0)->setFlags(0);
	ui.yemianshuxingtableWidget->item(2,0)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(2,1, new QTableWidgetItem(("")));
	ui.yemianshuxingtableWidget->item(2,1)->setFlags(0);
	ui.yemianshuxingtableWidget->item(2,1)->setBackground(QColor(hmiproject.pagelist[currenthmipageindex].color));

	ui.yemianshuxingtableWidget->setItem(3,0, new QTableWidgetItem(tr("��Ҫ����")));
	ui.yemianshuxingtableWidget->item(3,0)->setFlags(0);
	ui.yemianshuxingtableWidget->item(3,0)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(3,1, new QTableWidgetItem(QString::number(hmiproject.pagelist[currenthmipageindex].needpasswd)));
	ui.yemianshuxingtableWidget->item(3,1)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(4,0, new QTableWidgetItem(tr("����")));
	ui.yemianshuxingtableWidget->item(4,0)->setFlags(0);
	ui.yemianshuxingtableWidget->item(4,0)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(4,1, new QTableWidgetItem(hmiproject.pagelist[currenthmipageindex].passwd));
	ui.yemianshuxingtableWidget->item(4,1)->setForeground(Qt::black);

	checkyemianchanshu = true;

	repaint();

	projectneedsave = true;

	if(projectneedsave)
		this->setWindowTitle(tr("ecomwin��") + projecthmifilename + "*");
	else
		this->setWindowTitle(tr("ecomwin��") + projecthmifilename);


}

//����Ϊ��ҳ
void econwin::on_pb_shezhishouye_clicked()
{
	hmiproject.firstpageid = hmiproject.idlist[currenthmipageindex];
	int firstpageindex = hmiproject.idlist.indexOf(hmiproject.firstpageid);

	for(int i = 0;i < hmiproject.pagelist.count();i++)
	{
		//��ҳҪ����ʶ
		if(i == firstpageindex)
		{
			ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name +tr("(*)")));
		}
		else
		{
			ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name));
		}
		ui.yemiantableWidget->item(i,0)->setFlags(0);
		ui.yemiantableWidget->item(i,0)->setForeground(Qt::black);
	}
	projectneedsave = true;

	if(projectneedsave)
		this->setWindowTitle(tr("ecomwin��") + projecthmifilename + "*");
	else
		this->setWindowTitle(tr("ecomwin��") + projecthmifilename);

}


//�����޸ļ��
void econwin::onShuxingTableChanged(int row,int col)
{
	
	if(checkyemianchanshu)
	{
		projectneedsave = true;

		if(projectneedsave)
			this->setWindowTitle(tr("ecomwin��") + projecthmifilename + "*");
		else
			this->setWindowTitle(tr("ecomwin��") + projecthmifilename);

		//id
		if((row == 0) && (col == 1))
		{
			if(ui.yemianshuxingtableWidget->item(row,col)->text().toInt() <= 0)
			{
				ui.yemianshuxingtableWidget->item(row,col)->setText(QString::number(hmiproject.pagelist[currenthmipageindex].id));
			}
			else
			{
				hmiproject.pagelist[currenthmipageindex].id = ui.yemianshuxingtableWidget->item(row,col)->text().toInt();
				hmiproject.idlist[currenthmipageindex] = hmiproject.pagelist[currenthmipageindex].id;

				int firstpageindex = hmiproject.idlist.indexOf(hmiproject.firstpageid);

				for(int i = 0;i < hmiproject.pagelist.count();i++)
				{
					//��ҳҪ����ʶ
					if(i == firstpageindex)
					{
						ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name +tr("(*)")));
					}
					else
					{
						ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name));
					}
					ui.yemiantableWidget->item(i,0)->setFlags(0);
					ui.yemiantableWidget->item(i,0)->setForeground(Qt::black);
				}

			}
		}
		//name
		else if((row == 1)  && (col == 1))
		{
			//�մ�������
			if(ui.yemianshuxingtableWidget->item(row,col)->text() == QString(""))
			{
				ui.yemianshuxingtableWidget->item(row,col)->setText(hmiproject.pagelist[currenthmipageindex].name);
			}
			else
			{
				//�޸�name
				hmiproject.pagelist[currenthmipageindex].name = ui.yemianshuxingtableWidget->item(row,col)->text(); 
			}
		}
		//needpasswd
		else if(( row == 3) && (col == 1))
		{
			if(ui.yemianshuxingtableWidget->item(row,col)->text().toInt() == 0)
			{
				//�޸�needpasswd
				hmiproject.pagelist[currenthmipageindex].needpasswd = 0;
			}
			else if(ui.yemianshuxingtableWidget->item(row,col)->text().toInt() == 1)
			{
				//�޸�needpasswd
				hmiproject.pagelist[currenthmipageindex].needpasswd = 1;
			}
			else
			{
				ui.yemianshuxingtableWidget->item(row,col)->setText(QString::number(hmiproject.pagelist[currenthmipageindex].needpasswd));
			}
		}
		//passwd
		else if(( row == 4) && (col == 1))
		{
			//�޸�passwd
			hmiproject.pagelist[currenthmipageindex].passwd = ui.yemianshuxingtableWidget->item(row,col)->text(); 
		}
	}
}

//�������������
void econwin::onShuxingTableClicked(int row,int col)
{
	if(col == 1 && row == 2)
	{
		QColor choosecolor = QColorDialog::getColor(QColor(hmiproject.pagelist[currenthmipageindex].color));
		if(choosecolor.isValid())
		{
			hmiproject.pagelist[currenthmipageindex].color = choosecolor.name();
			ui.yemianshuxingtableWidget->item(2,1)->setBackground(QColor(hmiproject.pagelist[currenthmipageindex].color));
			projectneedsave = true;

			if(projectneedsave)
				this->setWindowTitle(tr("ecomwin��") + projecthmifilename + "*");
			else
				this->setWindowTitle(tr("ecomwin��") + projecthmifilename);

		}
	}
}






void econwin::on_pb_new_clicked()
{

	if(projectneedsave)
	{
		QMessageBox msgBox;
		msgBox.setText("<font size = 4>��ȷ��Ҫ�����ղŵ��޸ģ�</font>");
		msgBox.setWindowTitle("��ȷ��");
		msgBox.setStandardButtons(QMessageBox::Yes| QMessageBox::No );
		QAbstractButton* tb1 = msgBox.button(QMessageBox::Yes);
		tb1->setText("��");
		QAbstractButton* tb2 = msgBox.button(QMessageBox::No);
		tb2->setText("��");

		int ret = msgBox.exec();


		if (ret == QMessageBox::Yes)
		{
			;
		}
		else
		{
			return;
		}
	}

	//����ɾ��ҳ��Ϳؼ�
	hmiproject.len = 0;
	hmiproject.idlist.clear();
	hmiproject.pagelist.clear();

	for(int i = 0;i < hmiproject.hmipagelist.count();i++)
	{
		if(hmiproject.hmipagelist[i].count() > 0)
		{

			for(int ii = 0;ii < hmiproject.hmipagelist[i].count();ii++)
			{
				if(hmiproject.hmipagelist[i][ii].widgettype == 1 )
				{
					delete (hmipb *)hmiproject.hmipagelist[i][ii].pdata;
				}
				else if(hmiproject.hmipagelist[i][ii].widgettype == 2 )
				{
					delete (hmirect *)hmiproject.hmipagelist[i][ii].pdata;
				}
				else if(hmiproject.hmipagelist[i][ii].widgettype == 3 )
				{
					delete (hmitext *)hmiproject.hmipagelist[i][ii].pdata;
				}
				else if(hmiproject.hmipagelist[i][ii].widgettype == 4 )
				{
					delete (hminumber *)hmiproject.hmipagelist[i][ii].pdata;
				}
				else if(hmiproject.hmipagelist[i][ii].widgettype == 5 )
				{
					delete (hmilight *)hmiproject.hmipagelist[i][ii].pdata;
				}
			}
			hmiproject.hmipagelist[i].clear();

		}
	}
	hmiproject.hmipagelist.clear();

	//Ĭ������һ��ҳ��
	struct _page page;
	page.id = 1;
	page.name = tr("ҳ��1");
	page.color = "#c0c0c0";
	page.needpasswd = 0;
	page.passwd = "";
	page.len = 0;

	hmiproject.pagelist.append(page);

	//id�б�
	hmiproject.idlist.append(1);

	//һ��ҳ��
	hmiproject.len = 1;
	hmiproject.firstpageid = 1;

	QList<struct _hmiwidget> emptypage;

	//һ����ҳ��
	hmiproject.hmipagelist.append(emptypage);

	int firstpageindex = hmiproject.idlist.indexOf(hmiproject.firstpageid);

	//ˢ�µ�ҳ����
	ui.yemiantableWidget->setRowCount(hmiproject.pagelist.count());
	for(int i = 0;i < hmiproject.pagelist.count();i++)
	{
		//��ҳҪ����ʶ
		if(i == firstpageindex)
		{
			ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name +tr("(*)")));
		}
		else
		{
			ui.yemiantableWidget->setItem(i,0, new QTableWidgetItem(QString::number(hmiproject.pagelist[i].id) + ":" + hmiproject.pagelist[i].name));
		}
		ui.yemiantableWidget->item(i,0)->setFlags(0);
		ui.yemiantableWidget->item(i,0)->setForeground(Qt::black);
	}

	ui.yemiantableWidget->setColumnWidth(0, 180);
	ui.yemianshuxingtableWidget->setColumnWidth(0, 60);

	//��һ��ҳ�������
	ui.yemianshuxingtableWidget->setRowCount(5);
	ui.yemianshuxingtableWidget->setItem(0,0, new QTableWidgetItem("ID"));
	ui.yemianshuxingtableWidget->item(0,0)->setFlags(0);
	ui.yemianshuxingtableWidget->item(0,0)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(0,1, new QTableWidgetItem(QString::number(hmiproject.pagelist[0].id)));
	ui.yemianshuxingtableWidget->item(0,1)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(1,0, new QTableWidgetItem(tr("ҳ������")));
	ui.yemianshuxingtableWidget->item(1,0)->setFlags(0);
	ui.yemianshuxingtableWidget->item(1,0)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(1,1, new QTableWidgetItem((hmiproject.pagelist[0].name)));
	ui.yemianshuxingtableWidget->item(1,1)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(2,0, new QTableWidgetItem(tr("������ɫ")));
	ui.yemianshuxingtableWidget->item(2,0)->setFlags(0);
	ui.yemianshuxingtableWidget->item(2,0)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(2,1, new QTableWidgetItem(("")));
	ui.yemianshuxingtableWidget->item(2,1)->setFlags(0);
	ui.yemianshuxingtableWidget->item(2,1)->setBackground(QColor(hmiproject.pagelist[0].color));

	ui.yemianshuxingtableWidget->setItem(3,0, new QTableWidgetItem(tr("��Ҫ����")));
	ui.yemianshuxingtableWidget->item(3,0)->setFlags(0);
	ui.yemianshuxingtableWidget->item(3,0)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(3,1, new QTableWidgetItem(QString::number(hmiproject.pagelist[0].needpasswd)));
	ui.yemianshuxingtableWidget->item(3,1)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(4,0, new QTableWidgetItem(tr("����")));
	ui.yemianshuxingtableWidget->item(4,0)->setFlags(0);
	ui.yemianshuxingtableWidget->item(4,0)->setForeground(Qt::black);

	ui.yemianshuxingtableWidget->setItem(4,1, new QTableWidgetItem(hmiproject.pagelist[0].passwd));
	ui.yemianshuxingtableWidget->item(4,1)->setForeground(Qt::black);

	checkyemianchanshu = true;

	projectneedsave = true;
	projecthmifilename = tr("δ����");
	projecthmirun = false;
	hmistate = 0;
	hmisubject = 0;
	hmisubjectstep = 0;
	currenthmiwidgetindex = -1;

	if(projectneedsave)
		this->setWindowTitle(tr("ecomwin��") + projecthmifilename + "*");
	else
		this->setWindowTitle(tr("ecomwin��") + projecthmifilename);

}
/*
���ļ�ʵ���û���������۲����
*/
#include "taskview.h"
#include <QTableWidgetItem>
#include "basicservice.h"
#include "controltask.h"

extern QList<struct controltask> tasklist;

//�����źŵİ�ť
pb::pb(QWidget *parent) :
QPushButton(parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(sendsignal()));
}

//�����ź�
void pb::sendsignal()
{
	if(type == 0 )
	{
		if(tasklist[num].runflag == true)
		{
			tasklist[num].runflag = false;
		}
		else
		{
			tasklist[num].runflag = true;
		}
	}
	else if(type == 1)
	{
		if(luatasklist[num].runflag == true)
		{
			luatasklist[num].runflag = false;
		}
		else
		{
			luatasklist[num].runflag = true;
		}
	}
}

//���ñ��
void pb::setnum(int n)
{
	num = n;
}

pb::~pb()
{

}

taskview::taskview(QWidget *parent,Qt::WFlags flags)
: QDialog(parent, flags)
{
	ui.setupUi(this);
	ui.tasktable->setRowCount(tasklist.count()+luatasklist.count());
	QTableWidgetItem *newItem;

	pb* pbwidget;


	int col,row;
	col = 0;
	row = 0;
	int cindex = 0;
	int luaindex = 0;

	QList<struct controltask>::iterator itask;
	for(itask = tasklist.begin();itask != tasklist.end();++ itask)
	{

		//newItem; = new QTableWidgetItem((*iUI).id);
		ui.tasktable->setItem(col,0, new QTableWidgetItem((*itask).taskname));
		ui.tasktable->setItem(col,1, new QTableWidgetItem(QString::number((*itask).intervalms)));
		if((*itask).runflag == true)
		{
			ui.tasktable->setItem(col,2, new QTableWidgetItem("����"));
			pbwidget = new pb(this);//�ڱ������Ӱ�ť
			pbwidget->setText("ֹͣ����");
			pbwidget->setnum(cindex);
			pbwidget->type = 0;
			ui.tasktable->setCellWidget(col , 3 ,pbwidget);
			buttonlist.append(pbwidget);
		}
		else
		{
			ui.tasktable->setItem(col,2, new QTableWidgetItem("ֹͣ"));
			pbwidget = new pb(this);//�ڱ������Ӱ�ť
			pbwidget->setText("��������");
			pbwidget->setnum(cindex);
			pbwidget->type = 0;
			ui.tasktable->setCellWidget(col , 3 ,pbwidget);
			buttonlist.append(pbwidget);
		}

		ui.tasktable->item(col,0)->setFlags(0);
		ui.tasktable->item(col,1)->setFlags(0);
		ui.tasktable->item(col,2)->setFlags(0);
		ui.tasktable->item(col,0)->setForeground(Qt::black);
		ui.tasktable->item(col,1)->setForeground(Qt::black);
		ui.tasktable->item(col,2)->setForeground(Qt::black);

		col ++;
		cindex ++;
	}
	QList<struct luacontroltask>::iterator iluatask;
	for(iluatask = luatasklist.begin();iluatask != luatasklist.end();++ iluatask)
	{

		//newItem; = new QTableWidgetItem((*iUI).id);
		ui.tasktable->setItem(col,0, new QTableWidgetItem((*iluatask).taskname));
		ui.tasktable->setItem(col,1, new QTableWidgetItem(QString::number((*iluatask).intervalms)));
		if((*iluatask).runflag == true)
		{
			ui.tasktable->setItem(col,2, new QTableWidgetItem("����"));
			pbwidget = new pb(this);//�ڱ������Ӱ�ť
			pbwidget->setText("ֹͣ����");
			pbwidget->setnum(luaindex);
			pbwidget->type = 1;
			ui.tasktable->setCellWidget(col , 3 ,pbwidget);
			buttonlist.append(pbwidget);
		}
		else
		{
			ui.tasktable->setItem(col,2, new QTableWidgetItem("ֹͣ"));
			pbwidget = new pb(this);//�ڱ������Ӱ�ť
			pbwidget->setText("��������");
			pbwidget->setnum(luaindex);
			pbwidget->type = 1;
			ui.tasktable->setCellWidget(col , 3 ,pbwidget);
			buttonlist.append(pbwidget);
		}

		ui.tasktable->item(col,0)->setFlags(0);
		ui.tasktable->item(col,1)->setFlags(0);
		ui.tasktable->item(col,2)->setFlags(0);
		ui.tasktable->item(col,0)->setForeground(Qt::black);
		ui.tasktable->item(col,1)->setForeground(Qt::black);
		ui.tasktable->item(col,2)->setForeground(Qt::black);

		col ++;
		luaindex ++;
	}

	connect(&mytimer, SIGNAL(timeout()), this, SLOT(mytimeupdate()));
	mytimer.start(200); //100ms��ʱ

	//ui.vartable->setRowCount(2);
}

taskview::~taskview()
{
}

void taskview::on_backpb_clicked()
{
	//mytimer.stop();
	mytimer.disconnect(this);
	close();
}
void taskview::closeEvent(QCloseEvent *e)
{
	mytimer.disconnect(this);
	close();
}

void taskview::mytimeupdate()
{
	int col;
	col = 0;


	QList<struct controltask>::iterator itask;
	for(itask = tasklist.begin();itask != tasklist.end();++ itask)
	{

		if((*itask).runflag == true)
		{
			ui.tasktable->item(col,2)->setText("����");
			buttonlist[col]->setText("ֹͣ����");
			//(pb*)(ui.tasktable->cellWidget(col,3))->setText("ֹͣ����");

		}
		else
		{
			ui.tasktable->item(col,2)->setText("ֹͣ");
			buttonlist[col]->setText("��������");
			//(pb*)(ui.tasktable->cellWidget(col,3))->setText("��������");
		}


		col ++;
	}
	QList<struct luacontroltask>::iterator iluatask;
	for(iluatask = luatasklist.begin();iluatask != luatasklist.end();++ iluatask)
	{

		if((*iluatask).runflag == true)
		{
			ui.tasktable->item(col,2)->setText("����");
			buttonlist[col]->setText("ֹͣ����");
			//(pb*)(ui.tasktable->cellWidget(col,3))->setText("ֹͣ����");

		}
		else
		{
			ui.tasktable->item(col,2)->setText("ֹͣ");
			buttonlist[col]->setText("��������");
			//(pb*)(ui.tasktable->cellWidget(col,3))->setText("��������");
		}


		col ++;
	}

}


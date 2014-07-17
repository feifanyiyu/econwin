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
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include "pcap.h"
#include <QMutex>
#include <QWaitCondition>
#include <QTime>
#include <QTextStream> 
#include "mainloop.h"
#include "sys.h"
#include "device.h"
#include "controltask.h"
#include "basicservice.h"
#include "lua.hpp"
#include "econwin.h"
#include "luaview.h"

extern lua_State* L;

extern bool threadrun;
extern bool comopen;

//�Ƿ�ȫ�����ֹ�
bool allshake = false;

//���ְ����ȣ������˫��Լ����������֮���Զ����һ������Ϊ����ʱ���ܻ����ܶ������
static int shakehandlen = 1500;

//�¼���Ϣ���ݶ���
extern pcap_t *adhandle;

//lua��֧�ֶ��̣߳����ö�ռ��
QMutex luamutex;


QMutex waitmutex;
QWaitCondition etherwait;
int cnt = 0;

static unsigned int failcnt = 0;//ͨѶʧ�ܼ���
static unsigned int successcnt = 0;//ͨѶ�ɹ�����
static unsigned char senddata[1500];

static unsigned int nodeindex = 0;

/* prototype of the packet handler */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);


extern unsigned int aint;

//ͨ�Žڵ�����
QList<struct tcommnode> commnodes;
extern QMap<QString,struct _var> globalvarmap;

extern QList<struct _varlist> linevarlist;

extern QTextStream cin;  
extern QTextStream cout;  
extern QTextStream cerr;
extern bool freshgloballuavar;

extern int beginline,endline;
extern int oldbeginline,oldendline;
extern int currentfileindex;

backend::backend()
{
	// TODO Auto-generated constructor stub

}

backend::~backend()
{
	// TODO Auto-generated destructor stub
}


//�߳���ѭ��
void backend::run()
{
	int interval = 1;	// 1ms

	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	int res;
	bool ret = false;
	int loopcnt = 0;

	unsigned int nodenum = commnodes.count();

	aint = 0;


	//��ʼ��PLC����ʱ��
	{
		//��������
		QList<struct controltask>::iterator itask;
		for(itask = tasklist.begin();itask != tasklist.end();++ itask)
		{
			(*itask).t.start();
		}
	}

	while (threadrun == true)
	{
		if( econwinrun == true)
		{
			for(nodeindex = 0;nodeindex < nodenum;nodeindex++)
			{
				if(nodeindex == 0)
				{
					//����C PLC����
					QList<struct controltask>::iterator itask;
					for(itask = tasklist.begin();itask != tasklist.end();++ itask)
					{
						//�����������м��ʱ��������м��ʱ��Ϊ0
						if(((*itask).t.elapsed()> (*itask).intervalms) ||
							((*itask).intervalms == 0))
						{
							//����ִ����
							if((((*itask).taskcallback != (void (*)(void))0)) && ((*itask).runflag == true) && (allshake == true))
							{
								(*itask).taskcallback();
								(*itask).t.start();
							}
						}
					}

					luamutex.lock();

					//ͬ������c=>lua
					flushctolua();
					//����LUA PLC����
					QList<struct luacontroltask>::iterator iluatask;
					for(iluatask = luatasklist.begin();iluatask != luatasklist.end();++ iluatask)
					{
						//�����������м��ʱ��������м��ʱ��Ϊ0
						if(((*iluatask).t.elapsed()> (*iluatask).intervalms) ||
							((*iluatask).intervalms == 0))
						{
							//����ִ����
							if(((*iluatask).runflag == true) && (allshake == true))
							{
								//������
								lua_getglobal(L,(*iluatask).taskname.toAscii());
								lua_pcall(L,0,0,0);
								(*iluatask).t.start();
							}
						}
					}

					//ͬ������lua=>c
					flushluatoc();

					//ͬ��lua�����е�ȫ�ֱ���
					if(freshgloballuavar == true)
					{
						freshgloballuavar = false;

						//�Ƿ�ı����ˢ�·�Χ
						if((beginline == oldbeginline) && (endline == oldendline))
						{
						}
						else
						{
							oldbeginline = beginline;
							oldendline = endline;
							//����������ֵ
							lua_getglobal(L,"freshviewedvar");
							lua_pushinteger(L,currentfileindex+1);
							lua_pushinteger(L,beginline);
							lua_pushinteger(L,endline);
							lua_pcall(L,3,0,0);
						}

						//����������ֵ
						lua_getglobal(L,"freshviewedvarval");
						lua_pcall(L,0,0,0);

						//����ˢ�µ�Qt viewvarvallen

						lua_getglobal(L,"viewvarvallen");
						if(lua_isnumber(L,-1))
						{
							int viewvarvallen = lua_tonumber(L,-1);
							for(int i =1;i <= viewvarvallen;i++)
							{
								lua_getglobal(L,"viewedvarval");
								lua_pushinteger(L,i);
								lua_gettable(L,-2);
								if(lua_istable(L,-1))
								{
									QString name;
									lua_pushstring(L,"name");
									lua_gettable(L,-2);
									if(lua_isstring(L,-1))
									{
										name = QString(lua_tostring(L,-1));
									}

									lua_pop(L,1);

									lua_pushstring(L,"val");
									lua_gettable(L,-2);

									struct _var tvar = globalvarmap.value(name);
									if(tvar.vartype == 0)
									{
										if(lua_isnumber(L,-1))
										{
											tvar.varval.ivar = lua_tointeger(L,-1);
											globalvarmap[name] = tvar;

											//cout << name <<"," << tvar.varval.ivar << endl;
										}
									}
									else if(tvar.vartype == 1)
									{
										if(lua_isboolean(L,-1))
										{
											tvar.varval.bvar = lua_toboolean(L,-1);
											globalvarmap[name] = tvar;
											//cout << name <<"," << tvar.varval.bvar << endl;
										}
									}
									else if(tvar.vartype == 2)
									{
										if(lua_isnumber(L,-1))
										{
											tvar.varval.dvar = lua_tonumber(L,-1);
											globalvarmap[name] = tvar;
											//cout << name <<"," << tvar.varval.dvar << endl;
										}
									}

									lua_pop(L,1);

								}
								lua_pop(L,1);
								lua_pop(L,1);
							}
						}
						lua_pop(L,1);

					}

					/*if(((loopcnt++)%1000) == 0)
					{
					//������
					lua_getglobal(L,"viewvar");
					lua_pcall(L,0,0,0);
					}
					*/
					luamutex.unlock();
				}
				if( (--(commnodes[nodeindex].loopcnt)) == 0)
				{
					commnodes[nodeindex].loopcnt = commnodes[nodeindex].loopset;

					//ͨ�Žڵ���ʱ�����
					if(commnodes[nodeindex].intervalms)
					{
						msleep(commnodes[nodeindex].intervalms);
					}

					//��������
					// ��������̫���ϣ�����MAC��Ŀ�ĵ�ַΪ 0:0:0:0:0:id
					senddata[0] = 0x00;
					senddata[1] = 0x00;
					senddata[2] = 0x00;
					senddata[3] = 0x00;
					senddata[4] = 0x00;
					senddata[5] = commnodes[nodeindex].id;

					//����MACԴ��ַΪ 0:0:0:0:0:240
					senddata[6] = 0x00;
					senddata[7] = 0x00;
					senddata[8] = 0x00;
					senddata[9] = 0x00;
					senddata[10] = 0x00;
					senddata[11] = 240;

					//Э��ͷ
					senddata[12] = 0x88;
					senddata[13] = 0xa1;


					luamutex.lock();
					//�ص��������ã�ӳ�䴦��
					commnodes[nodeindex].out_handle(commnodes[nodeindex].pdevice);
					luamutex.unlock();


					//�������ݰ�
					if(commnodes[nodeindex].shakehand == true)
					{
						//cmd
						senddata[14] = 0x02;//��ͨ��ͨ������

						//����
						memcpy(senddata+15,commnodes[nodeindex].pcommsendbuff,commnodes[nodeindex].commsendbufflen);

						//���������ݰ����Ȱ����÷���
						if (pcap_sendpacket(adhandle, senddata,commnodes[nodeindex].sendpacklen /* size */) != 0)
						{
							printf("\nError sending the packet: \n", pcap_geterr(adhandle));
						}
					}
					else
					{
						//cmd
						senddata[14] = 0x01;//��������

						//����
						memcpy(senddata+15,commnodes[nodeindex].pcommsendbuff,shakehandlen-15);

						//��һ��������Ϊ100�ֽ�
						if (pcap_sendpacket(adhandle, senddata,shakehandlen /* size */) != 0)
						{
							printf("\nError sending the packet: \n", pcap_geterr(adhandle));
						}
					}


					//�ȷ�������
					waitmutex.lock();
					ret = etherwait.wait(&waitmutex,11);
					waitmutex.unlock();
					//�������ݰ�����
					if( ret == false)
					{
						aint = aint + 1;
						//printf("outtime");
					}
					//��ȷ���ݰ�����
					else
					{
						luamutex.lock();
						//�ص��������ã�ӳ�䴦��
						commnodes[nodeindex].in_handle(commnodes[nodeindex].pdevice);
						luamutex.unlock();

						//��ȷ���ֱ�ʶ
						commnodes[nodeindex].shakehand = true;
						//�Ƿ�ȫ�������ֹ�
						if(allshake == false)
						{
							bool f = true;
							for(int mynodeindex = 0;mynodeindex < nodenum;mynodeindex++)
							{
								if(commnodes[mynodeindex].shakehand == false)
								{	f = false;
								break;
								}
							}
							if(f == true)
							{
								//printf("���ֳɹ�\n");
								//���������û���������
								allshake = true;
							}
						}
					}
				}
			}
		}

		else
		{
			msleep(100);
		}
	}
errgoto:
	return;
}

/* Callback function invoked by libpcap for every incoming packet */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	int res;
	struct tm *ltime;
	char timestr[16];
	//struct pcap_pkthdr *header;
	//const u_char *pkt_data;
	time_t local_tv_sec;

	//���ֺ�����ݰ�У��
	if(commnodes[nodeindex].shakehand == true)
	{
		if((pkt_data[12] == 0x88)
			&& (pkt_data[13] == 0xa1)
			&& (pkt_data[14] == 0x02)

			&& (pkt_data[0] == 0)
			&& (pkt_data[1] == 0)
			&& (pkt_data[2] == 0)
			&& (pkt_data[3] == 0)
			&& (pkt_data[4] == 0)
			&& (pkt_data[5] == 240)

			&& (pkt_data[6] == 0)
			&& (pkt_data[7] == 0)
			&& (pkt_data[8] == 0)
			&& (pkt_data[9] == 0)
			&& (pkt_data[10] == 0)
			&& (pkt_data[11] == commnodes[nodeindex].id)
			&& (header->len == commnodes[nodeindex].recvpacklen))
		{
			memcpy(commnodes[nodeindex].pcommrecvbuff,pkt_data+15,commnodes[nodeindex].recvpacklen-15);

			waitmutex.lock();
			etherwait.wakeAll();
			waitmutex.unlock();
			cnt ++;
		}
	}
	else
	{

		if((pkt_data[12] == 0x88)
			&& (pkt_data[13] == 0xa1)
			&& (pkt_data[14] == 0x01)

			&& (pkt_data[0] == 0)
			&& (pkt_data[1] == 0)
			&& (pkt_data[2] == 0)
			&& (pkt_data[3] == 0)
			&& (pkt_data[4] == 0)
			&& (pkt_data[5] == 240)

			&& (pkt_data[6] == 0)
			&& (pkt_data[7] == 0)
			&& (pkt_data[8] == 0)
			&& (pkt_data[9] == 0)
			&& (pkt_data[10] == 0)
			&& (pkt_data[11] == commnodes[nodeindex].id)
			&& (header->len == shakehandlen))
		{
			memcpy(commnodes[nodeindex].pcommrecvbuff,pkt_data+15,shakehandlen-15);

			waitmutex.lock();
			etherwait.wakeAll();
			waitmutex.unlock();
			cnt ++;
		}
	}
}


backend2::backend2()
{
	// TODO Auto-generated constructor stub

}

backend2::~backend2()
{
	// TODO Auto-generated destructor stub
}

//�߳���ѭ��
void backend2::run()
{
	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	int res;
	int cnt = 0;

	while (threadrun == true)
	{
		if( econwinrun == true)
		{
			/* ��ʼ���� */
			pcap_loop(adhandle, 0, packet_handler, NULL);
		}

		else
		{
			msleep(100);
		}
	}
}
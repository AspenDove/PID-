#pragma once
#include<iostream>
#include<windows.h>
#include <cstdlib> 
#include <string>
using namespace std;

extern HANDLE hComm;//����������
extern OVERLAPPED OverLapped;
extern COMSTAT Comstat;
extern DWORD dwCommEvents;
typedef struct
{
	int iSpeed;
	int iCurrent;
	int iTemperature;
}rec;

bool OpenPort(LPCSTR port);    //�򿪴���  

bool SetupDCB(int rate_arg, HANDLE Comm);  //����DCB  
bool SetupTimeout(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD
	ReadTotalConstant, DWORD WriteTotalMultiplier, DWORD WriteTotalConstant);   //���ó�ʱ 

rec ReciveString(HANDLE Comm);   //�����ַ�  
bool WriteHEX(CHAR* WriteBuffer, DWORD dwSend, HANDLE Comm);

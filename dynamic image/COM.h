#pragma once
#include<iostream>
#include<windows.h>
#include <cstdlib> 
#include <string>
using namespace std;

extern HANDLE hComm;//打开两个串口
extern OVERLAPPED OverLapped;
extern COMSTAT Comstat;
extern DWORD dwCommEvents;
typedef struct
{
	int iSpeed;
	int iCurrent;
	int iTemperature;
}rec;

bool OpenPort(LPCSTR port);    //打开串口  

bool SetupDCB(int rate_arg, HANDLE Comm);  //设置DCB  
bool SetupTimeout(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD
	ReadTotalConstant, DWORD WriteTotalMultiplier, DWORD WriteTotalConstant);   //设置超时 

rec ReciveString(HANDLE Comm);   //接收字符  
bool WriteHEX(CHAR* WriteBuffer, DWORD dwSend, HANDLE Comm);

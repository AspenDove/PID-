#include<iostream>
#include<windows.h>
#include <cstdlib> 
#include <string>
#include "com.h"
#define Rcount 10
HANDLE hComm;//打开两个串口
OVERLAPPED OverLapped;
COMSTAT Comstat;
DWORD dwCommEvents;

/*串口开启函数*/
bool OpenPort(LPCSTR port)
{
	hComm = CreateFileA(port,          //指定串口
		GENERIC_READ | GENERIC_WRITE,   //允许读写
		0,                              //以独占方式打开
		0,                              //无安全属性 
		OPEN_EXISTING,                  //通讯设备已存在
		FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL,           //代表异步操作     
		0);                             //不指定模式
	return hComm != INVALID_HANDLE_VALUE;
}

//配置串口基本设置
bool SetupDCB(int rate_arg, HANDLE Comm)
{
	SetupComm(Comm, 0, 1);//设置（读、写）缓冲区大小为1024
	DCB dcb;
	/*
	将s中当前位置后面的n = sizeof(dcb)个字节用 0 替换并返回&dcb
	memset：作用是在一段内存块中填充某个给定的值，
	它是对较大的结构体或数组进行清零操作的一种最快方法
	*/
	memset(&dcb, 0, sizeof(dcb));//
	if(!GetCommState(Comm, &dcb))//获取当前DCB配置  
	{
		return FALSE;
	}
	dcb.DCBlength = sizeof(dcb);//DCB块大小
	/* ---------- Serial Port Config ------- */
	dcb.BaudRate = rate_arg;    //波特率
	dcb.Parity = NOPARITY;      //奇偶校验0-4：分别表示不校验、奇校验，偶校验、标号、空格
	dcb.fParity = 0;            //不允许奇偶校验  
	dcb.StopBits = ONESTOPBIT;  //1停止位
	dcb.ByteSize = 8;           //数据位，以字节表示4-8
	dcb.fOutxCtsFlow = 0;       //CTS输出流控制
	dcb.fOutxDsrFlow = 0;       //DSR输出流控制
	dcb.fDtrControl = DTR_CONTROL_DISABLE;//无DTR流控制类型
	dcb.fDsrSensitivity = 0;              //对DSR信号线不敏感
	dcb.fRtsControl = RTS_CONTROL_DISABLE; //无RTS流控制 
	dcb.fOutX = 0;                         //XON/XOFF输出流控制
	dcb.fInX = 0;                          //XON/XOFF输入流控制
	/* ---------- 容错机制 ------- */
	dcb.fErrorChar = 0;                   //允许错误替换
	dcb.fBinary = 1;                      //二进制模式，不检测EOF
	dcb.fNull = 0;                        //允许剥离，去掉NULL字符
	dcb.fAbortOnError = 0;                //有错误时终止读写操作
	dcb.wReserved = 0;
	dcb.XonLim = 2;                       //XON发送字符之前缓冲区中允许接收的最小字节数
	dcb.XoffLim = 4;                      //XOff发送字符之前缓冲区中允许的最小可用字节数
	dcb.XonChar = 0x13;                   //发送和接受XON字符
	dcb.XoffChar = 0x19;                  //发送和接受XOFF字符
	dcb.EvtChar = 0;                      //接收到的事件字符
	return SetCommState(Comm, &dcb);
}
//串口超时设置
bool SetupTimeout(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD
	ReadTotalConstant, DWORD WriteTotalMultiplier, DWORD WriteTotalConstant)
{
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = ReadInterval;                //读间隔时间超时 
	timeouts.ReadTotalTimeoutConstant = ReadTotalConstant;      //读时间常量
	timeouts.ReadTotalTimeoutMultiplier = ReadTotalMultiplier;  //读时间系数
	timeouts.WriteTotalTimeoutConstant = WriteTotalConstant;    //写时间常量    
	timeouts.WriteTotalTimeoutMultiplier = WriteTotalMultiplier;//写时间系数
	return SetCommTimeouts(hComm, &timeouts);
}

/*****************************************************
函数名：ReciveString
功  能：外界蓝牙的数据，实现从接收缓冲区读取字符串信息，
		并调用键盘操作
参  数：Comm对应的串口句柄
备  注：数据读出是以char数组的形式
*****************************************************/
rec ReciveString(HANDLE Comm)
{
	rec x;
	DWORD BytesRead = 8;
	static char str[sizeof(rec)] = { 0 };
	BOOL bResult = ReadFile(Comm,  //通信设备（此处为串口）句柄，由CreateFile()返回值得到  
		str,                  //指向接收缓冲区  
		8,               //指明要从串口中读取的字节数  
		&BytesRead,   //  
		&OverLapped);        //OVERLAPPED结构 //&OverLapped 
	//PurgeComm(Comm, PURGE_RXCLEAR);//清除缓冲区数据，避免前一次数据对本次数据产生影响
	unsigned iSpeed = str[2];
	iSpeed <<= 8;
	iSpeed += str[3];

	unsigned iCurrent = str[4];
	iCurrent <<= 8;
	iCurrent += str[5];

	unsigned iTemperature = str[6];
	iTemperature <<= 8;
	iTemperature += str[7];

	return{ (int)iSpeed,(int)iCurrent,(int)iTemperature };
}

//检测一个字符是不是十六进制字符，若是返回相应的值，否则返回0x10；
char HexChar(char c)
{
	if((c >= '0') && (c <= '9'))
		return c - 0x30;
	else if((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;
	else if((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;
	else
		return 0x10;
}

//将一个字符串作为十六进制串转化为一个字节数组，
//字节间可用空格分隔，返回转换后的字节数组长度，同时字节数组长度自动设置。
int Str2Hex(string str, BYTE *data)//CByteArray &data
{
	int t, t1;
	int rlen = 0, len = str.length();
	//data.SetSize(len / 2);
	for(int i = 0; i < len;)
	{
		char l, h = str[i];
		if(h == ' ')
		{
			i++;
			continue;
		}
		i++;
		if(i >= len)break;
		l = str[i];
		t = HexChar(h);
		t1 = HexChar(l);
		if((t == 16) || (t1 == 16))
			break;
		else t = t * 16 + t1;
		i++;
		data[rlen] = (char)t;
		rlen++;
	}
	return rlen;
}

/*****************************************************
函数名：WriteHEX
功  能：向键盘模拟串口发送键盘、鼠标指令（以16进制的形式）
参  数：WriteBuffer：string类型的字符串，作为将要发送的指令
		dwSend     ：待发送的字符串的长度
		Comm       ：串口的句柄
*****************************************************/
bool WriteHEX(CHAR* WriteBuffer, DWORD dwSend, HANDLE Comm)    //BYTE *szWriteBuffer
{
	bool bWrite = TRUE;
	bool bResult = TRUE;
	DWORD BytesSent = 0;
	HANDLE hWriteEvent = NULL;
	ResetEvent(hWriteEvent);
	//将字符串转化为16进制再发送，数组长度固定为5
	//BYTE data[5] = { 0 };
	//Str2Hex(WriteBuffer, data);

	//数据加载缓冲区进行发送
	if(bWrite)
	{
		OverLapped.Offset = 0;
		OverLapped.OffsetHigh = 0;
		bResult = WriteFile(Comm,    //通信设备句柄，CreateFile()返回值得到  
			WriteBuffer,                    //指向写入数据缓冲区  
			dwSend,                  //设置要写的字节数  
			&BytesSent,              // 实际写入的字节数
			&OverLapped);            //指向异步I/O数据  
	}
	return TRUE;
}

/*int main()
{
	char COM[5] = {0};

	cout << "请输入蓝牙串口号和模拟键盘串口号（以空格分割）：" << endl;
	cin.get(COM, 5);

	if (OpenPort(COM))                                      //后期设置串口号输入
		cout << "串口成功打开" << endl;

	if (SetupDCB(128000, hComm))                            //这里后期可以设置串口波特率
		cout << "串口参数表设置成功" << endl;

	if (SetupTimeout(0, 0, 0, 0, 0))                                                 //不设置读、写超时
		cout << "完成超时设置" << endl;
	PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);  //清空串口1缓冲区
	cout << endl;
	cout << "正在等待数据...."<< endl;
	ReciveString(hComm);
	return EXIT_SUCCESS;
}*/

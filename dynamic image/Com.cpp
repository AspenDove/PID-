#include<iostream>
#include<windows.h>
#include <cstdlib> 
#include <string>
#include "com.h"
#define Rcount 10
HANDLE hComm;//����������
OVERLAPPED OverLapped;
COMSTAT Comstat;
DWORD dwCommEvents;

/*���ڿ�������*/
bool OpenPort(LPCSTR port)
{
	hComm = CreateFileA(port,          //ָ������
		GENERIC_READ | GENERIC_WRITE,   //�����д
		0,                              //�Զ�ռ��ʽ��
		0,                              //�ް�ȫ���� 
		OPEN_EXISTING,                  //ͨѶ�豸�Ѵ���
		FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL,           //�����첽����     
		0);                             //��ָ��ģʽ
	return hComm != INVALID_HANDLE_VALUE;
}

//���ô��ڻ�������
bool SetupDCB(int rate_arg, HANDLE Comm)
{
	SetupComm(Comm, 0, 1);//���ã�����д����������СΪ1024
	DCB dcb;
	/*
	��s�е�ǰλ�ú����n = sizeof(dcb)���ֽ��� 0 �滻������&dcb
	memset����������һ���ڴ�������ĳ��������ֵ��
	���ǶԽϴ�Ľṹ�������������������һ����췽��
	*/
	memset(&dcb, 0, sizeof(dcb));//
	if(!GetCommState(Comm, &dcb))//��ȡ��ǰDCB����  
	{
		return FALSE;
	}
	dcb.DCBlength = sizeof(dcb);//DCB���С
	/* ---------- Serial Port Config ------- */
	dcb.BaudRate = rate_arg;    //������
	dcb.Parity = NOPARITY;      //��żУ��0-4���ֱ��ʾ��У�顢��У�飬żУ�顢��š��ո�
	dcb.fParity = 0;            //��������żУ��  
	dcb.StopBits = ONESTOPBIT;  //1ֹͣλ
	dcb.ByteSize = 8;           //����λ�����ֽڱ�ʾ4-8
	dcb.fOutxCtsFlow = 0;       //CTS���������
	dcb.fOutxDsrFlow = 0;       //DSR���������
	dcb.fDtrControl = DTR_CONTROL_DISABLE;//��DTR����������
	dcb.fDsrSensitivity = 0;              //��DSR�ź��߲�����
	dcb.fRtsControl = RTS_CONTROL_DISABLE; //��RTS������ 
	dcb.fOutX = 0;                         //XON/XOFF���������
	dcb.fInX = 0;                          //XON/XOFF����������
	/* ---------- �ݴ���� ------- */
	dcb.fErrorChar = 0;                   //��������滻
	dcb.fBinary = 1;                      //������ģʽ�������EOF
	dcb.fNull = 0;                        //������룬ȥ��NULL�ַ�
	dcb.fAbortOnError = 0;                //�д���ʱ��ֹ��д����
	dcb.wReserved = 0;
	dcb.XonLim = 2;                       //XON�����ַ�֮ǰ��������������յ���С�ֽ���
	dcb.XoffLim = 4;                      //XOff�����ַ�֮ǰ���������������С�����ֽ���
	dcb.XonChar = 0x13;                   //���ͺͽ���XON�ַ�
	dcb.XoffChar = 0x19;                  //���ͺͽ���XOFF�ַ�
	dcb.EvtChar = 0;                      //���յ����¼��ַ�
	return SetCommState(Comm, &dcb);
}
//���ڳ�ʱ����
bool SetupTimeout(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD
	ReadTotalConstant, DWORD WriteTotalMultiplier, DWORD WriteTotalConstant)
{
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = ReadInterval;                //�����ʱ�䳬ʱ 
	timeouts.ReadTotalTimeoutConstant = ReadTotalConstant;      //��ʱ�䳣��
	timeouts.ReadTotalTimeoutMultiplier = ReadTotalMultiplier;  //��ʱ��ϵ��
	timeouts.WriteTotalTimeoutConstant = WriteTotalConstant;    //дʱ�䳣��    
	timeouts.WriteTotalTimeoutMultiplier = WriteTotalMultiplier;//дʱ��ϵ��
	return SetCommTimeouts(hComm, &timeouts);
}

/*****************************************************
��������ReciveString
��  �ܣ�������������ݣ�ʵ�ִӽ��ջ�������ȡ�ַ�����Ϣ��
		�����ü��̲���
��  ����Comm��Ӧ�Ĵ��ھ��
��  ע�����ݶ�������char�������ʽ
*****************************************************/
rec ReciveString(HANDLE Comm)
{
	rec x;
	DWORD BytesRead = 8;
	static char str[sizeof(rec)] = { 0 };
	BOOL bResult = ReadFile(Comm,  //ͨ���豸���˴�Ϊ���ڣ��������CreateFile()����ֵ�õ�  
		str,                  //ָ����ջ�����  
		8,               //ָ��Ҫ�Ӵ����ж�ȡ���ֽ���  
		&BytesRead,   //  
		&OverLapped);        //OVERLAPPED�ṹ //&OverLapped 
	//PurgeComm(Comm, PURGE_RXCLEAR);//������������ݣ�����ǰһ�����ݶԱ������ݲ���Ӱ��
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

//���һ���ַ��ǲ���ʮ�������ַ������Ƿ�����Ӧ��ֵ�����򷵻�0x10��
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

//��һ���ַ�����Ϊʮ�����ƴ�ת��Ϊһ���ֽ����飬
//�ֽڼ���ÿո�ָ�������ת������ֽ����鳤�ȣ�ͬʱ�ֽ����鳤���Զ����á�
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
��������WriteHEX
��  �ܣ������ģ�⴮�ڷ��ͼ��̡����ָ���16���Ƶ���ʽ��
��  ����WriteBuffer��string���͵��ַ�������Ϊ��Ҫ���͵�ָ��
		dwSend     �������͵��ַ����ĳ���
		Comm       �����ڵľ��
*****************************************************/
bool WriteHEX(CHAR* WriteBuffer, DWORD dwSend, HANDLE Comm)    //BYTE *szWriteBuffer
{
	bool bWrite = TRUE;
	bool bResult = TRUE;
	DWORD BytesSent = 0;
	HANDLE hWriteEvent = NULL;
	ResetEvent(hWriteEvent);
	//���ַ���ת��Ϊ16�����ٷ��ͣ����鳤�ȹ̶�Ϊ5
	//BYTE data[5] = { 0 };
	//Str2Hex(WriteBuffer, data);

	//���ݼ��ػ��������з���
	if(bWrite)
	{
		OverLapped.Offset = 0;
		OverLapped.OffsetHigh = 0;
		bResult = WriteFile(Comm,    //ͨ���豸�����CreateFile()����ֵ�õ�  
			WriteBuffer,                    //ָ��д�����ݻ�����  
			dwSend,                  //����Ҫд���ֽ���  
			&BytesSent,              // ʵ��д����ֽ���
			&OverLapped);            //ָ���첽I/O����  
	}
	return TRUE;
}

/*int main()
{
	char COM[5] = {0};

	cout << "�������������ںź�ģ����̴��ںţ��Կո�ָ��" << endl;
	cin.get(COM, 5);

	if (OpenPort(COM))                                      //�������ô��ں�����
		cout << "���ڳɹ���" << endl;

	if (SetupDCB(128000, hComm))                            //������ڿ������ô��ڲ�����
		cout << "���ڲ��������óɹ�" << endl;

	if (SetupTimeout(0, 0, 0, 0, 0))                                                 //�����ö���д��ʱ
		cout << "��ɳ�ʱ����" << endl;
	PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);  //��մ���1������
	cout << endl;
	cout << "���ڵȴ�����...."<< endl;
	ReciveString(hComm);
	return EXIT_SUCCESS;
}*/

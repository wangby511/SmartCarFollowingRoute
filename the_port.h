//#pragma once
//#include <Windows.h>
//#include <iostream>
//using namespace std;
//
//class ComPort
//{
//public:
//	HANDLE hCom;
//	DWORD dwError;
//	BOOL bComOpened;
//	OVERLAPPED wrOverlapped;
//	ComPort(LPCSTR portName)
//	{
//	hCom = NULL;
//	dwError = NULL;
//	bComOpened = NULL;
//	hCom = CreateFile(portName, //文件名，这里是串口号
//		GENERIC_READ | GENERIC_WRITE, //充许读和写
//		0, //独占方式
//		NULL,
//		OPEN_EXISTING, //打开而不是创建
//		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //异步I/O结构
//		NULL
//		);
//	if(hCom == INVALID_HANDLE_VALUE){
//	dwError = GetLastError(); //取得错误代码
//	std::cout<<portName<<" Opened failed. Error code: "<<dwError<<std::endl;
//	}
//	else{
//	bComOpened = TRUE;
//	std::cout<<portName<<" Opened successful."<<std::endl;
//	}
//
//	if(!SetupComm(hCom,2048,2048)){ //输入缓冲区和输出缓冲区的大小都是1024
//	std::cout<<"SetupComm fail! Close Comm!"<<endl;
//	CloseHandle(hCom);
//	}
//	else{
//	std::cout<<"SetupComm OK!"<<endl;
//	}
//
//	COMMTIMEOUTS TimeOuts;
//	memset(&TimeOuts,0,sizeof(TimeOuts));
//	// read timeout
//	TimeOuts.ReadIntervalTimeout=1000;
//	TimeOuts.ReadTotalTimeoutMultiplier=500;
//	TimeOuts.ReadTotalTimeoutConstant=5000;
//	// write timeout
//	TimeOuts.WriteTotalTimeoutMultiplier=500;
//	TimeOuts.WriteTotalTimeoutConstant=2000;
//	SetCommTimeouts(hCom,&TimeOuts); //set timeout
//	DCB dcb;
//	if (!GetCommState(hCom,&dcb))
//	{
//		std::cout<<"GetCommState fail! Comm close"<<endl;
//		CloseHandle(hCom);
//	}
//	else
//	{
//		std::cout<<"GetCommState OK!"<<endl;
//	}
//	dcb.DCBlength = sizeof(dcb);
//	if (!BuildCommDCB("9600,n,8,1",&dcb))//填充DCB的数据传输率...
//	{
//		std::cout<<"BuileCOmmDCB fail,Comm close!"<<endl;
//			CloseHandle(hCom);
//	}
//	if(SetCommState(hCom,&dcb))
//	{
//		std::cout<<"SetCommState OK!"<<endl;
//		}
//	ZeroMemory(&wrOverlapped,sizeof(wrOverlapped));
//	if (wrOverlapped.hEvent != NULL)
//	{
//		ResetEvent(wrOverlapped.hEvent);
//		wrOverlapped.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
//	}
//	PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR); //clear port buffer
//	txIndex=0;
//	rxIndex=0;
//}
//
//virtual ~ComPort(void)
//{
//	if(bComOpened)
//	CloseHandle(hCom);
//}
//
//unsigned char rxIndex;
//unsigned char lpInBuffer[1024];
//
//int Sync(void){
//	int i=1000;
//	while(rxIndex!=txIndex && txIndex!=0 && (--i)){
//		int size=Read();
//		for(int i=0;i<size;i++)
//			if(((lpInBuffer[i]&0x80)==0) && (lpInBuffer[i]!=0))
//				rxIndex=lpInBuffer[i];
//	}
//	if(i<1)
//		return -1;
//	return rxIndex;
//}
//
//
//int Read(void){
//	DWORD dwBytesRead=1024;
//	COMSTAT ComStat;
//	DWORD dwErrorFlags;
//	OVERLAPPED m_osRead;
//	memset(&m_osRead,0,sizeof(OVERLAPPED));
//	m_osRead.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
//	ClearCommError(hCom,&dwErrorFlags,&ComStat);
//	dwBytesRead=min(dwBytesRead,(DWORD)ComStat.cbInQue);
//	if(!dwBytesRead)
//		return FALSE;
//	BOOL bReadStatus;
//	bReadStatus=ReadFile(hCom,lpInBuffer,
//	dwBytesRead,&dwBytesRead,&m_osRead);
//	if(!bReadStatus) //如果ReadFile函数返回FALSE
//	{
//		if(GetLastError()==ERROR_IO_PENDING) // ERROR_IO_PENDING == 997
//		{
//			WaitForSingleObject(m_osRead.hEvent,2000);
//			//使用WaitForSingleObject函数等待,直到读操作完成或延时已达到2秒钟
//			//当串口读操作进行完毕后，m_osRead的hEvent事件会变为有信号
//			PurgeComm(hCom, PURGE_TXABORT|
//				PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
//			return dwBytesRead;
//		}
//		return 0;
//	}
//	PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
//	return dwBytesRead;
//}
//
//unsigned char txIndex;
//
//int Send(unsigned char command){
//	//command |= 0x80;
//	++txIndex;
//	txIndex &=~0x80;
//	DWORD dwError;
//	DWORD dwWantSend = 3;//2;
//	DWORD dwRealSend = 0;
////	char* pReadBuf = NULL;
//	char pSendBuffer[]={'O','N',command};//,txIndex};
//	if (ClearCommError(hCom,&dwError,NULL))
//	{
//	PurgeComm(hCom,PURGE_TXABORT | PURGE_TXCLEAR);
//	//std::cout<<"PurgeComm OK!"<<endl;
//	}
//	if (!WriteFile(hCom,pSendBuffer,dwWantSend,&dwRealSend,&wrOverlapped))
//	{
//		dwError=GetLastError();
//	if ( dwError == ERROR_IO_PENDING){
//		while (!GetOverlappedResult(hCom,&wrOverlapped,&dwRealSend,FALSE))
//		{
//			if (GetLastError() == ERROR_IO_INCOMPLETE){
//				//std::cout<<"Sending..."<<endl;
//				continue;
//			}
//			else
//			{
//				std::cout<<"Send failed. Error code: "<<dwError<<endl;
//				ClearCommError(hCom,&dwError,NULL);
//				break;
//			}
//		}
//	}
//	}
//	return dwRealSend;
//	}
//};
#pragma once
#include <Windows.h>
#include <iostream>
using namespace std;

class ComPort
{
public:
	HANDLE hCom;
	DWORD dwError;
	BOOL bComOpened;
	OVERLAPPED wrOverlapped;
	ComPort(LPCSTR portName)
	{
	hCom = NULL;
	dwError = NULL;
	bComOpened = NULL;
	hCom = CreateFile(portName, //文件名，这里是串口号
		GENERIC_READ | GENERIC_WRITE, //充许读和写
		0, //独占方式
		NULL,
		OPEN_EXISTING, //打开而不是创建
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, //异步I/O结构
		NULL
		);
	if(hCom == INVALID_HANDLE_VALUE){
	dwError = GetLastError(); //取得错误代码
	std::cout<<portName<<" Opened failed. Error code: "<<dwError<<std::endl;
	}
	else{
	bComOpened = TRUE;
	std::cout<<portName<<" Opened successful."<<std::endl;
	}

	if(!SetupComm(hCom,2048,2048)){ //输入缓冲区和输出缓冲区的大小都是1024
	std::cout<<"SetupComm fail! Close Comm!"<<endl;
	CloseHandle(hCom);
	}
	else{
	std::cout<<"SetupComm OK!"<<endl;
	}

	COMMTIMEOUTS TimeOuts;
	memset(&TimeOuts,0,sizeof(TimeOuts));
	// read timeout
	TimeOuts.ReadIntervalTimeout=1000;
	TimeOuts.ReadTotalTimeoutMultiplier=500;
	TimeOuts.ReadTotalTimeoutConstant=5000;
	// write timeout
	TimeOuts.WriteTotalTimeoutMultiplier=500;
	TimeOuts.WriteTotalTimeoutConstant=2000;
	SetCommTimeouts(hCom,&TimeOuts); //set timeout
	DCB dcb;
	if (!GetCommState(hCom,&dcb))
	{
		std::cout<<"GetCommState fail! Comm close"<<endl;
		CloseHandle(hCom);
	}
	else
	{
		std::cout<<"GetCommState OK!"<<endl;
	}
	dcb.DCBlength = sizeof(dcb);
	if (!BuildCommDCB("9600,n,8,1",&dcb))//填充DCB的数据传输率...
	{
		std::cout<<"BuileCOmmDCB fail,Comm close!"<<endl;
			CloseHandle(hCom);
	}
	if(SetCommState(hCom,&dcb))
	{
		std::cout<<"SetCommState OK!"<<endl;
		}
	ZeroMemory(&wrOverlapped,sizeof(wrOverlapped));
	if (wrOverlapped.hEvent != NULL)
	{
		ResetEvent(wrOverlapped.hEvent);
		wrOverlapped.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	}
	PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR); //clear port buffer
	txIndex=0;
	rxIndex=0;
}

virtual ~ComPort(void)
{
	if(bComOpened)
	CloseHandle(hCom);
}

unsigned char rxIndex;
unsigned char lpInBuffer[1024];

int Sync(void){
	int i=1000;
	while(rxIndex!=txIndex && txIndex!=0 && (--i)){
		int size=Read();
		for(int i=0;i<size;i++)
			if(((lpInBuffer[i]&0x80)==0) && (lpInBuffer[i]!=0))
				rxIndex=lpInBuffer[i];
	}
	if(i<1)
		return -1;
	return rxIndex;
}


int Read(void){
	DWORD dwBytesRead=1024;
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	OVERLAPPED m_osRead;
	memset(&m_osRead,0,sizeof(OVERLAPPED));
	m_osRead.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	ClearCommError(hCom,&dwErrorFlags,&ComStat);
	dwBytesRead=min(dwBytesRead,(DWORD)ComStat.cbInQue);
	if(!dwBytesRead)
		return FALSE;
	BOOL bReadStatus;
	bReadStatus=ReadFile(hCom,lpInBuffer,
	dwBytesRead,&dwBytesRead,&m_osRead);
	if(!bReadStatus) //如果ReadFile函数返回FALSE
	{
		if(GetLastError()==ERROR_IO_PENDING) // ERROR_IO_PENDING == 997
		{
			WaitForSingleObject(m_osRead.hEvent,2000);
			//使用WaitForSingleObject函数等待,直到读操作完成或延时已达到2秒钟
			//当串口读操作进行完毕后，m_osRead的hEvent事件会变为有信号
			PurgeComm(hCom, PURGE_TXABORT|
				PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
			return dwBytesRead;
		}
		return 0;
	}
	PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	return dwBytesRead;
}

unsigned char txIndex;

int Send(unsigned char command){
	//command |= 0x80;
	++txIndex;
	txIndex &=~0x80;
	DWORD dwError;
	DWORD dwWantSend = 3;//2;
	DWORD dwRealSend = 0;
//	char* pReadBuf = NULL;
	char pSendBuffer[]={'O','N',command};//,txIndex};
	if (ClearCommError(hCom,&dwError,NULL))
	{
	PurgeComm(hCom,PURGE_TXABORT | PURGE_TXCLEAR);
	//std::cout<<"PurgeComm OK!"<<endl;
	}
	if (!WriteFile(hCom,pSendBuffer,dwWantSend,&dwRealSend,&wrOverlapped))
	{
		dwError=GetLastError();
	if ( dwError == ERROR_IO_PENDING){
		while (!GetOverlappedResult(hCom,&wrOverlapped,&dwRealSend,FALSE))
		{
			if (GetLastError() == ERROR_IO_INCOMPLETE){
				//std::cout<<"Sending..."<<endl;
				continue;
			}
			else
			{
				std::cout<<"Send failed. Error code: "<<dwError<<endl;
				ClearCommError(hCom,&dwError,NULL);
				break;
			}
		}
	}
	}
	return dwRealSend;
	}
};

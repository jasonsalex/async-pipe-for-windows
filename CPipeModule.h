#pragma once
#include <windows.h>
#include <string>
#include "CMemBuffer.h"
#include "CRingBuffer.h"
#include "CFastMutex.h"
#include "CFastLogger.h"

#define INVALID_VALUE -1

struct CPipePackge{
	static const char hander[8];
	hfjy::CMemBuffer::CMemBufferClass buf;
	std::wstring pipeName;
};


class CPipeIOEvent {
public:
	CPipeIOEvent(const std::wstring name = L"")
	{
		flagName = name;
	}

	void WritePackge(const char *buf, const int len)
	{
		CMutexAutoLock lock(&mutex);

		this->writeBuf.Write(CPipePackge::hander, sizeof(CPipePackge::hander));
		this->writeBuf.Write(&len, sizeof(len));
		this->writeBuf.Write(buf, len);

		//LOG(DEBUG) << "write packge, write buf size:"<< writeBuf.GetReadableSize() << ", current size:" << len << ", data:" << buf;
	}

	bool ReadPackge(char *buf, const int len)
	{
		CMutexAutoLock lock(&mutex);
		auto ret = writeBuf.Read(buf, len);

		//LOG(DEBUG) << "read packge, read buf size:" << writeBuf.GetReadableSize() << ", current size:" << len << ", data:" << buf;

		return ret;
	}

	int GetPackgeSize()
	{
		CMutexAutoLock lock(&mutex);
		return writeBuf.GetReadableSize();
	}

	virtual void ReadPackge(CPipePackge * packge) = 0;
	virtual void ConnectStatus(bool status) = 0;

protected:
	hfjy::CRingBuffer::CRingBufferClass writeBuf;
	CFastMutex mutex;
	std::wstring flagName;
};



class CPipeModule
{
public:
	friend class CPipeEvent;

	CPipeModule();
	~CPipeModule();

	bool Listen(const std::wstring name, const int pipeSize = 1024);
	bool Connect(const std::wstring name, const int timeOut = 5000);
	bool Disconnect();
	bool IsConnect() { return isConnect; }
	void SetIOEvent(CPipeIOEvent* pipeRead) { pipeEvent = pipeRead; readPackge->pipeName = pipeName;}

private:
	//创建等待线程
	static DWORD WINAPI WaitForSingleThread(LPVOID pM);

private:
	bool CreateIOThread();
	int ReviceData();
	bool WriteData();

private:
	int pipeBufSize = 1024;
	
	OVERLAPPED	ovlpd;
	std::wstring pipeName;

	CPipeIOEvent *pipeEvent = nullptr;

	HANDLE	hNamedPipe = 0;
	HANDLE  hEvent = 0;
	HANDLE  hIOThread = 0;
	HANDLE	hWaitThread = 0;

	CPipePackge* readPackge;
	hfjy::CRingBuffer::CRingBufferClass readRingBuf;
	int readLength = 0;
	bool isConnect = false;
};
#include "CPipeModule.h"
#include "CRingBuffer.h"
#include <memory>  

const char CPipePackge::hander[8] = "HFJYPKG";

CPipeModule::CPipeModule()
{
	memset(&ovlpd, 0, sizeof(OVERLAPPED));
	readPackge = new CPipePackge;
}


CPipeModule::~CPipeModule()
{
	TerminateThread(hWaitThread, 0);
	TerminateThread(hIOThread, 0);
	WaitForSingleObject(hWaitThread, INFINITE);
	WaitForSingleObject(hIOThread, INFINITE);

	CloseHandle(hNamedPipe);
	CloseHandle(hEvent);
	CloseHandle(hWaitThread);
	CloseHandle(hIOThread);

	if (isConnect)
	{
		Disconnect();
	}

	delete readPackge;
}


//创建服务端
bool CPipeModule::Listen(const std::wstring name, const int pipeSize)
{
	pipeName = name;
	pipeBufSize = pipeSize;

	//这里创建的是双向模式且使用重叠模式的命名管道

	if(hNamedPipe != 0)CloseHandle(hNamedPipe);

	hNamedPipe = CreateNamedPipeW(pipeName.c_str(),
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
		PIPE_UNLIMITED_INSTANCES,
		pipeBufSize, 
		pipeBufSize,
		0,
		NULL);

	if(INVALID_HANDLE_VALUE == hNamedPipe)
	{
		LOG(ERROR)<< "create pipe is ERROR, pipe name:" << pipeName.c_str();
		return false;
	}

	//LOG(DEBUG) << "create pipe is ok, pipe name:" << pipeName;


	//添加事件以等待客户端连接命名管道
	//该事件为手动重置事件，且初始化状态为无信号状态

	if(hEvent != 0)CloseHandle(hEvent);

	hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
	
	if (!hEvent)
	{
		CloseHandle(hNamedPipe);
		LOG(ERROR)<< "create pipe event is ERROR, pipe name:" << pipeName.c_str();
		return false;
	}

	//LOG(DEBUG) << "create event is ok, pipe name:" << pipeName;

	ovlpd.hEvent = hEvent;

	//等待客户端连接
	if (!ConnectNamedPipe(hNamedPipe, &ovlpd))
	{
		if (ERROR_IO_PENDING != GetLastError())
		{
			CloseHandle(hNamedPipe);
			CloseHandle(hEvent);

			LOG(ERROR) << "wait for pipe client connectd is failed, pipe name:" << pipeName.c_str();
			return false;
		}
	}

	//LOG(DEBUG) << "connect pipe is ok, pipe name:" << pipeName;

	if (hWaitThread != 0) {
		TerminateThread(hWaitThread, 0);
		WaitForSingleObject(hWaitThread, INFINITE);
		CloseHandle(hWaitThread);
	}

	hWaitThread = CreateThread(NULL, 0, &WaitForSingleThread, this, 0, NULL);

	if (!hWaitThread)
	{
		CloseHandle(hNamedPipe);
		CloseHandle(hEvent);
		LOG(ERROR) << "create wait single thread is ERROR, pipe name:" << pipeName.c_str();
		return false;
	}

	//LOG(DEBUG) << "create wait single thread is ok, pipe name:" << pipeName << ", thread id:" << hWaitThread;

	return true;
}


//创建客户端
bool CPipeModule::Connect(const std::wstring name, const int timeOut)
{
	pipeName = name;
	
	if (WaitNamedPipeW(pipeName.c_str(), timeOut) == 0)
	{
		LOG(ERROR)<<"WaitNamedPipe failed with error, timeOut:"<< timeOut << "pipe name:" << pipeName.c_str();
		return false;
	}


	//创建读写管道
	hNamedPipe = CreateFileW(
		pipeName.c_str(),   // pipe name 
		GENERIC_READ |  // read and write access 
		GENERIC_WRITE,
		0,              // no sharing 
		NULL,           // default security attributes
		OPEN_EXISTING,  // opens existing pipe 
		0,              // default attributes 
		NULL);          // no template file 


	if (INVALID_HANDLE_VALUE == hNamedPipe) {
		LOG(ERROR) << "create pipe file is ERROR, pipe name:" << pipeName.c_str();
		return false;
	}

	bool fSuccess = SetNamedPipeHandleState(
		hNamedPipe,    // pipe handle 
		PIPE_READMODE_BYTE|PIPE_TYPE_BYTE,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 


	if (fSuccess == false)
	{
		LOG(ERROR) << "SetNamedPipeHandleState failed, pipe name:" << pipeName.c_str();
		return false;
	}

	return 	this->CreateIOThread();
}

bool CPipeModule::Disconnect()
{
	bool ret = DisconnectNamedPipe(hNamedPipe);

	if (isConnect)
	{
		isConnect = false;

		if (this->pipeEvent != nullptr)
		{
			this->pipeEvent->ConnectStatus(isConnect);
		}
	}

	return ret;
}

//创建等待线程
DWORD CPipeModule::WaitForSingleThread(LPVOID pM)
{
	CPipeModule * pipeModule = (CPipeModule*)pM;

	DWORD result = WaitForSingleObject(pipeModule->hEvent, INFINITE);
	
	if (WAIT_FAILED == result)
	{
		CloseHandle(pipeModule->hNamedPipe);
		CloseHandle(pipeModule->hEvent);
		LOG(ERROR) << "wait for pipe object is failed, pipe name:" << pipeModule->pipeName.c_str();
	}

	//LOG(DEBUG) << "wait for single object, pipe name:" << pipeModule->pipeName;
	pipeModule->CreateIOThread();
	
	return 0;
}

//创建读写线程
bool CPipeModule::CreateIOThread()
{

	auto callback = [](LPVOID pM)->DWORD{
		CPipeModule * pipeModule = (CPipeModule*)pM;

		do {

			if (pipeModule->ReviceData() == INVALID_VALUE || pipeModule->IsConnect() == false)
			{
				if (pipeModule->isConnect)
					pipeModule->Disconnect();

				break;
			}

			pipeModule->WriteData();
			Sleep(1);

		} while(1);

		return 0;
	};

	isConnect = true;

	if (this->pipeEvent != nullptr)
	{
		this->pipeEvent->ConnectStatus(isConnect);
	}

	if (hIOThread != 0) {
		TerminateThread(hIOThread, 0);
		WaitForSingleObject(hIOThread, INFINITE);
		CloseHandle(hIOThread);
	}

	hIOThread = CreateThread(NULL, 0, callback, this, 0, NULL);
	
	if (!hIOThread)
	{
		CloseHandle(hNamedPipe);
		CloseHandle(hEvent);
		CloseHandle(hWaitThread);

		LOG(ERROR) << "create pipe io thread is ERROR, pipe name:" << pipeName.c_str();
		return false;
	}

	//LOG(DEBUG) << "create read and write thread is ok, pipe name:" << pipeName << ", read thread id:" << hIOThread;

}



//管道读写操作
int  CPipeModule::ReviceData()
{
	DWORD dwRead, totalRead;

	do
	{
		if (PeekNamedPipe(hNamedPipe, 0, pipeBufSize, &dwRead, &totalRead, 0))
		{
			if (totalRead > 0)
			{
				std::shared_ptr<char> pReadBuf(new char[totalRead]);

				if (ReadFile(hNamedPipe, pReadBuf.get(), totalRead, &dwRead, 0))
				{
					readRingBuf.Write(pReadBuf.get(), dwRead);
					//循环处理缓冲区数据
					while (readRingBuf.GetReadableSize() >= sizeof(CPipePackge::hander) + sizeof(readLength))
					{
						if (readLength == 0)
						{
							char hander[sizeof(readPackge->hander)];

							readRingBuf.Read(hander, sizeof(readPackge->hander));

							if (memcmp(readPackge->hander, hander, sizeof(readPackge->hander)) == 0)
							{
								readRingBuf.Read(&readLength, sizeof(readLength));
							}

							//LOG(DEBUG) << "start revice data from pipe, hander:" << hander << ", data length:" << readLength << ", pipe name:" << pipeName;
						}

						if (readRingBuf.GetReadableSize() >= readLength)
						{
							readPackge->buf.SetSize(readLength);
							readRingBuf.Read(readPackge->buf.GetData(), readLength);

							if (pipeEvent != nullptr)
							{
								pipeEvent->ReadPackge(readPackge);

								//LOG(DEBUG) << "revice data from pipe, data length:" << readPackge->buf.GetSize() << ", pipe name:" << pipeName;

								readPackge->buf.SetSize(0);
							}
							else
							{
								LOG(WARNING) << "drop recvice data for pipe, pipe name:" << pipeName.c_str();
							}

							readLength = 0;
						}
						else
						{
							break;
						}
					}
				}
				else
				{
					LOG(ERROR) << "recvice data from pipe is error, pipe name:" << pipeName.c_str();
					return INVALID_VALUE;
				}
			}
		}
		else
		{
			LOG(ERROR) << "peek recvice data from pipe is error, pipe name:" << pipeName.c_str();
			return INVALID_VALUE;
		}

	} while (totalRead > 0); //把缓冲区 的数据一定要读完，不然写入数据会被阻塞

	return true;
}


bool CPipeModule::WriteData()
{
	DWORD dwWrite;

	if (pipeEvent != nullptr)
	{
		do {
				int writeSize = pipeEvent->GetPackgeSize();
				std::shared_ptr<char> buf(new char[writeSize]);
				pipeEvent->ReadPackge(buf.get(), writeSize);

				if (writeSize > 0)
				{
					if (WriteFile(hNamedPipe, buf.get(), writeSize, &dwWrite, 0))
					{

						if (dwWrite != writeSize)
						{
							//LOG(ERROR) << "write data to pipe is error, pipe size:" << dwWrite << ", write size:" << writeSize << ", pipe name:" << pipeName;
							return false;
						}
						else
						{
							//LOG(DEBUG) << "write data to pipe is ok, write size:" << writeSize << ", pipe name:" << pipeName;
						}
					}
					else
					{
						LOG(ERROR) << "write data from pipe is ERROR, pipe name:" << pipeName.c_str();
						return false;
					}
				}

		} while (pipeEvent->GetPackgeSize() > 0);
	}
	else
	{
		LOG(WARNING) << "not use pipe event delegate, pipe name:"<< pipeName.c_str();
		return false;
	}

	return true;
}



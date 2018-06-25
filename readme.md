# explain
* Using named pipes to simulate socket asynchronous event model

# demo
* Each instance needs to inherit the IO module
* Related read and write operations and events require such operations.
```
class DPipeEvent :public CPipeIOEvent
{
public:
	DPipeEvent(const std::wstring name = L"")
	{
		this->flagName = name;
	}
	int count = 0;

	void ReadPackge(CPipePackge * packge)override
	{

		if (count < 20)
		{
			printf("read pack data:%d, size:%d, pipe:%s, names:%s \n", packge->buf.GetData(), packge->buf.GetSize(), packge->pipeName.c_str(), this->flagName.c_str());

			//std::string s("read packge reponse:");
			//Sleep(100);

			//char i[4];
			//s += itoa(count++, i, 10);

			//this->WritePackge(s.c_str(), s.length());

			this->WritePackge((const char *)packge->buf.GetData(), packge->buf.GetSize());

		}
	}


	void ConnectStatus(bool status)override
	{
		printf("connect status, %d", status);
	}
};
```

* service code
```
	auto *pip = new CPipeModule;
	auto *pipeEvent = new DPipeEvent(L"Server-Pipe1");
	pip->SetIOEvent(pipeEvent);
	TS_ASSERT(pip->Listen(L"\\\\.\\pipe\\VcamPipe"));

	//char test[] = "hahahhahahahahahahah";
	//pip->Disconnect();

	
	for (auto i = 0; i < 10; i++)
	{
		//pipeEvent->WritePackge(test, sizeof(test));

	}
	

	CreateThread(NULL, 0, [](LPVOID pM)->DWORD {
	
		Sleep(10000);
		((CPipeModule*)pM)->Disconnect();
		delete (CPipeModule*)pM;
		return 0;
	}, pip, 0, NULL);
```

* client code 
```
	auto *pip = new CPipeModule;
	auto *pipeEvent = new DPipeEvent(L"Client-Pipe1");
	pip->SetIOEvent(pipeEvent);
	TS_ASSERT(pip->Connect(L"\\\\.\\pipe\\VcamPipe"));

	for (int i = 0; i < 100; i++)
	{
		//srand(i%10);
		auto size = 1000;//rand() % 1000;
		char *test= new char[size];

		printf("rand size:%d\n", size);
		//char *test = new char[5000];
		pipeEvent->WritePackge(test, size);
		
		delete test;

		Sleep(1);
	}
```
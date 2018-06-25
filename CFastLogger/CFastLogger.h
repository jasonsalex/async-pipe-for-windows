#pragma once

// ����easyloggin++.cc Ҳ���뵽��Ŀ��, ���������Ҫ��ȫ�ֶ���
// #define ELPP_THREAD_SAFE 1 
//****INITIALIZE_EASYLOGGINGPP; main����ǰ���

#define GLOG_NO_ABBREVIATED_SEVERITIES
#define GOOGLE_GLOG_DLL_DECL
#include <glog/logging.h>
using namespace google;

#if !defined(IS_DEV_LOG)
#		define IS_DEV_LOG 0
#endif

#if IS_DEV_LOG	// ʹ�õ���log fDebug��fTrace ��Ҫ������ IS_DEV_LOG ����
	
	#define fDebug() LOG(DEBUG)
	#define fTrace() LOG(TRACE)
#endif


//#define fInfo() LOG(INFO)
//#define fWarn() LOG(WARNING)

class fInfo
{
public:
	fInfo(){};
	template <typename T>
	inline fInfo& operator<<(const T& log) {
		try {
			LOG(INFO) << log;
			return *this;
			// TODO: �ڴ˴����� return ���
		}
		catch (std::exception &e) {
			//OutputDebugStringW(e.what());
#if IS_DEV_LOG
			std::cout << "WriteLog: " << e.what() << std::endl;
#endif
		}
		catch (...) {
			OutputDebugStringW(L"@WriteLog excepted.......");
		}	
	}
};

class fWarn
{
public:
	fWarn() {};
	template <typename T>
	inline fWarn& operator<<(const T& log) {
	try{
		LOG(WARNING) << log;
		return *this;
	}
	catch (std::exception &e) {
		//OutputDebugString(e.what());
#if IS_DEV_LOG
		std::cout << "WriteLog: " << e.what() << std::endl;
#endif
	}
	catch (...) {
		OutputDebugStringW(L"@WriteLog excepted.......");
	}
		// TODO: �ڴ˴����� return ���
	}
};

class fError
{
public:
	fError() {};
	template <typename T>
	inline fError& operator<<(const T& log) {
		try {
			LOG(ERROR) << log;
			return *this;
		}
		catch (std::exception &e) {
			//OutputDebugString(e.what());
#if IS_DEV_LOG
			std::cout << "WriteLog: " << e.what() << std::endl;
#endif
		}
		catch (...) {
			OutputDebugStringW(L"@WriteLog excepted.......");
		}
		// TODO: �ڴ˴����� return ���
	}
};

namespace hfjy
{
	namespace FastLogger
	{		
		
		class CFastLoggerClass
		{
		public:
			void Initialization(std::string strPath, std::string threadname);
		};	
	}
}

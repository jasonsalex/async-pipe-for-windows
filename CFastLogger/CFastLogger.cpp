#include "CFastLogger.h"


namespace hfjy
{
	namespace FastLogger
	{
		void CFastLoggerClass::Initialization(std::string strPath, std::string threadname)
		{
			#if !defined(IS_DEV_LOG)
				//el::base::consts::kDefaultLogFile = std::string(strPath).c_str();
				el::Helpers::setThreadName(threadname);
				el::Configurations defaultConf;
			
				defaultConf.setToDefault();
				defaultConf.set(el::Level::Global, el::ConfigurationType::Format, "%datetime %levshort [%logger][%thread] %msg");
				defaultConf.set(el::Level::Debug, el::ConfigurationType::Format, "%datetime %levshort [%logger][%thread] %msg");
				defaultConf.set(el::Level::Trace, el::ConfigurationType::Format, "%datetime %levshort [%logger][%thread][%func] %msg");
				defaultConf.set(el::Level::Warning, el::ConfigurationType::Format, "%datetime %levshort [%logger][%thread][%func] %msg");
				defaultConf.set(el::Level::Global, el::ConfigurationType::Filename, strPath + ".log");
				defaultConf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, "false");
				el::Loggers::reconfigureLogger("default", defaultConf);

				fInfo() << "创建日志文件:" + defaultConf.get(el::Level::Global, el::ConfigurationType::Filename)->value();
			#endif
		}	
		
	}
}
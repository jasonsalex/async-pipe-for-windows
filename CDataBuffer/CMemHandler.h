#ifndef _CMEMHANDLER_H
#define _CMEMHANDLER_H

#include <stdint.h>
#include <memory>

#define ALLOC_MEM_ERROR -1

// Memory Alloc
namespace hfjy
{
	namespace CMemHandler
	{
		class CMemHandlerClass
		{
		public:
			CMemHandlerClass(int nAlign = 0);
			virtual ~CMemHandlerClass();
			void* Malloc(int32_t nSize);
			void Attach(void* buf, int32_t nSize, int nAlign = 0);
			void* Realloc(int32_t nSize);
			void* Detach();
			int32_t DataSize();
			void* Data();
			int Align();
			void Swap(CMemHandlerClass& other);
		private:
			void Release();
			int32_t m_size;
			void* m_buf;
			int m_align;
		};
	}
}

#endif // !_CMEMHANDLER_H

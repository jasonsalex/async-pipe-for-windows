#ifndef _CMEMBUFFER_H
#define _CMEMBUFFER_H

#include "CMemHandler.h"
#include <stdio.h>

namespace hfjy
{
	namespace CMemBuffer
	{
		typedef enum SeekOrigin {
			soFromBeginning = SEEK_SET,
			soFromCurrent = SEEK_CUR,
			soFromEnd = SEEK_END
		}SeekOrigin;


		class CMemBufferClass
		{
		public:
			CMemBufferClass(int nAlign = 0);
			virtual ~CMemBufferClass();
			void SetPosition(int32_t nPos);
			int32_t GetPosition();
			int32_t GetSize();
			void* GetData();
			int32_t Read(void* Buffer, uint32_t Count);
			int32_t Seek(const int32_t Offset, SeekOrigin Origin);
			int32_t Write(const void * Buffer, uint32_t Count);
			bool SetCapacity(int32_t NewCapacity);
			bool SetSize(int32_t NewSize);
			void Swap(CMemBufferClass& other);
		private:
			int32_t m_nPosition;
			int32_t m_nSize;
			int32_t m_nCapacity;
			hfjy::CMemHandler::CMemHandlerClass m_buf;
			const int64_t MemoryDelta = 0x2000;
		};
	}
}

#endif // !_CMEMBUFFER_H

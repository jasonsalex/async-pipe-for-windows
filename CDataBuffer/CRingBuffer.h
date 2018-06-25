#ifndef _CRINGBUFFER_H
#define _CRINGBUFFER_H

#include "CMemHandler.h"
#include <stdio.h>

// 环形缓冲区
namespace hfjy
{
	namespace CRingBuffer
	{
		class CRingBufferClass
		{
		public:
			CRingBufferClass();
			~CRingBufferClass();
			void* GetData();
			int32_t GetSize();
			int32_t GetReadPosition();
			int32_t GetWritePosition();
			// 获取可读区域大小
			int32_t GetReadableSize();
			// 获取可写区域大小
			int32_t GetWriteableSize();
			// 顺序读
			int32_t Read(void* Buffer, uint32_t Count);
			// 顺序写
			int32_t Write(const void * Buffer, uint32_t Count);
		private:
			bool SetCapacity(int32_t NewCapacity);
			hfjy::CMemHandler::CMemHandlerClass m_buf;
			int32_t m_nSize;
			int32_t m_nCapacity;
			int32_t m_nWritePosition; // 写入位置
			int32_t m_nReadPosition; // 读取位置
		};
	}
}

#endif // !_CRINGBUFFER_H

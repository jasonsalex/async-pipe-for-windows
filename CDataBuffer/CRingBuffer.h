#ifndef _CRINGBUFFER_H
#define _CRINGBUFFER_H

#include "CMemHandler.h"
#include <stdio.h>

// ���λ�����
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
			// ��ȡ�ɶ������С
			int32_t GetReadableSize();
			// ��ȡ��д�����С
			int32_t GetWriteableSize();
			// ˳���
			int32_t Read(void* Buffer, uint32_t Count);
			// ˳��д
			int32_t Write(const void * Buffer, uint32_t Count);
		private:
			bool SetCapacity(int32_t NewCapacity);
			hfjy::CMemHandler::CMemHandlerClass m_buf;
			int32_t m_nSize;
			int32_t m_nCapacity;
			int32_t m_nWritePosition; // д��λ��
			int32_t m_nReadPosition; // ��ȡλ��
		};
	}
}

#endif // !_CRINGBUFFER_H

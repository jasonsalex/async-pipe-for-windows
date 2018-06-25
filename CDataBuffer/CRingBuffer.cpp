#include "CRingBuffer.h"
#include <algorithm>

namespace hfjy
{
	namespace CRingBuffer
	{
		const int64_t MemoryDelta = 0x2000;

		CRingBufferClass::CRingBufferClass()
			: m_nCapacity(0)
			, m_nSize(0)
			, m_nWritePosition(0)
			, m_nReadPosition(0)
		{
		}

		CRingBufferClass::~CRingBufferClass()
		{
		}

		void * CRingBufferClass::GetData()
		{
			return m_buf.Data();
		}

		int32_t CRingBufferClass::GetSize()
		{
			return m_nSize;
		}

		int32_t CRingBufferClass::GetReadPosition()
		{
			return m_nReadPosition;
		}

		int32_t CRingBufferClass::GetWritePosition()
		{
			return m_nWritePosition;
		}

		int32_t CRingBufferClass::GetReadableSize()
		{
			if (m_nReadPosition <= m_nWritePosition) {
				return m_nWritePosition - m_nReadPosition;
			}
			else {
				return m_nWritePosition + m_nSize - m_nReadPosition;
			}
		}

		int32_t CRingBufferClass::GetWriteableSize()
		{
			if (m_nWritePosition <= m_nReadPosition) {
				return m_nReadPosition - m_nWritePosition;
			}
			else {
				return m_nReadPosition + m_nSize - m_nWritePosition;
			}
		}

		int32_t CRingBufferClass::Read(void * Buffer, uint32_t Count)
		{
			if (m_nWritePosition >= m_nReadPosition) {
				int32_t nNewPos = m_nReadPosition + Count;
				if (nNewPos >= m_nWritePosition) {
					Count = m_nWritePosition - m_nReadPosition;
				}
				if (Count > 0) {
					if (Buffer) {
						memcpy(Buffer, (uint8_t*)m_buf.Data() + m_nReadPosition, (size_t)Count);
					}
					m_nReadPosition += Count;
				}
				return Count;
			}
			else { // 环形读取
				int32_t nNewPos = m_nReadPosition + Count;
				if (nNewPos >= m_nSize + m_nWritePosition) {
					Count = m_nSize + m_nWritePosition - m_nReadPosition;
				}
				int32_t nRead = std::min((int32_t)Count, m_nSize - m_nReadPosition);
				if (nRead > 0) {
					if (Buffer) {
						memcpy(Buffer, (uint8_t*)m_buf.Data() + m_nReadPosition, (size_t)nRead);
					}
					m_nReadPosition += nRead;
				}
				nNewPos -= m_nReadPosition;
				if (m_nReadPosition == m_nSize) {
					m_nReadPosition = 0;
				}
				// 转圈
				if (nNewPos > 0) {
					return nRead + Read(Buffer ? (uint8_t*)Buffer + nRead : Buffer, nNewPos);
				}
				return nRead;
			}
		}

		int32_t CRingBufferClass::Write(const void * Buffer, uint32_t Count)
		{
			if (m_nWritePosition >= m_nReadPosition) {
				if (Count >= m_nSize - m_nWritePosition + m_nReadPosition) { // 没有足够的空间
					int32_t nNewPos = m_nWritePosition + Count;
					if (nNewPos > m_nCapacity) {
						if (SetCapacity(nNewPos) == false) return ALLOC_MEM_ERROR;
					}
					if (m_nSize < nNewPos) {
						m_nSize = nNewPos;
					}
				}
				int32_t nNewCount = Count - (m_nSize - m_nWritePosition);
				if (nNewCount > 0) { // 转圈
					if (Buffer) {
						memcpy((uint8_t*)m_buf.Data(), (uint8_t*)Buffer + (Count - nNewCount), nNewCount);
					}
					else {
						memset((uint8_t*)m_buf.Data(), 0, nNewCount);
					}
				}
				// 补充后面的
				nNewCount = std::min((int32_t)Count, m_nSize - m_nWritePosition);
				if (nNewCount > 0) {
					if (Buffer) {
						memcpy((uint8_t*)m_buf.Data() + m_nWritePosition, Buffer, nNewCount);
					}
					else {
						memset((uint8_t*)m_buf.Data() + m_nWritePosition, 0, nNewCount);
					}
				}
				m_nWritePosition += Count;
				if (m_nWritePosition > m_nSize) {
					m_nWritePosition -= m_nSize;
				}
			}
			else {
				int32_t nNewPos = m_nWritePosition + Count;
				if (nNewPos >= m_nReadPosition) { // 装不下了
					// 从新分配内存, 插入更多空间
					int32_t nReadSize = m_nSize - m_nReadPosition;
					int32_t nSizeOffset = nNewPos - m_nReadPosition + MemoryDelta;
					m_nSize += nSizeOffset;
					if (m_nSize >= m_nCapacity) {
						if (SetCapacity(m_nSize) == false) return ALLOC_MEM_ERROR;
						;
					}
					// 移动读取位置, 使用memmove处理 内存重叠
					memmove((uint8_t*)m_buf.Data() + m_nReadPosition + nSizeOffset, (uint8_t*)m_buf.Data() + m_nReadPosition, nReadSize);
					m_nReadPosition += nSizeOffset;
				}
				// 写入新的内容
				if (Buffer) {
					memcpy((uint8_t*)m_buf.Data() + m_nWritePosition, Buffer, Count);
				}
				else {
					memset((uint8_t*)m_buf.Data() + m_nWritePosition, 0, Count);
				}
				m_nWritePosition += Count;
			}
			return Count;
		}

		bool CRingBufferClass::SetCapacity(int32_t NewCapacity)
		{
			NewCapacity = (NewCapacity + (MemoryDelta - 1)) & ~(MemoryDelta - 1);
			m_nCapacity = NewCapacity;
			return m_buf.Realloc(m_nCapacity) != NULL ? true : false;
		}
	}
}

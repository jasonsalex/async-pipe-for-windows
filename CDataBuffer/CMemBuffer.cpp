#include "CMemBuffer.h"

namespace hfjy
{
	namespace CMemBuffer
	{
		CMemBufferClass::CMemBufferClass(int nAlign)
			: m_buf(nAlign)
			, m_nSize(0)
			, m_nPosition(0)
			, m_nCapacity(0)
		{
		}

		CMemBufferClass::~CMemBufferClass()
		{
		}

		void CMemBufferClass::SetPosition(int32_t nPos)
		{
			Seek(nPos, soFromBeginning);
		}

		int32_t CMemBufferClass::GetPosition()
		{
			return m_nPosition;
		}

		int32_t CMemBufferClass::GetSize()
		{
			return m_nSize;
		}

		void * CMemBufferClass::GetData()
		{
			return m_buf.Data();
		}

		int32_t CMemBufferClass::Read(void * Buffer, uint32_t Count)
		{
			if (Count <= 0) {
				return 0;
			}
			if (m_nPosition >= 0 && Count >= 0) {
				int64_t nRemainSize = m_nSize - m_nPosition;
				if (nRemainSize >= 0) {
					if (nRemainSize > Count) {
						nRemainSize = Count;
					}
					memcpy(Buffer, (uint8_t*)m_buf.Data() + m_nPosition, (size_t)nRemainSize);
					m_nPosition += nRemainSize;
					return (int32_t)nRemainSize;
				}
			}
			return 0;
		}

		int32_t CMemBufferClass::Seek(const int32_t Offset, SeekOrigin Origin)
		{
			int32_t nPosition = m_nPosition;
			switch (Origin) {
			case soFromBeginning:
				nPosition = Offset;
				break;
			case soFromCurrent:
				nPosition += Offset;
				break;
			case soFromEnd:
				nPosition = m_nSize + Offset;
				break;
			default:
				return -1;
			}
			if (nPosition > m_nSize) {
				return -1;
			}
			m_nPosition = nPosition;
			return m_nPosition;
		}

		int32_t CMemBufferClass::Write(const void * Buffer, uint32_t Count)
		{
			if (m_nPosition >= 0 && Count >= 0) {
				int64_t Pos = m_nPosition + Count;
				if (Pos > 0) {
					if (Pos > m_nSize) {
						if (Pos > m_nCapacity) {
							if (SetCapacity(Pos) == false) return ALLOC_MEM_ERROR;
						}
						m_nSize = Pos;
					}
					if (Buffer) {
						memcpy((uint8_t*)m_buf.Data() + m_nPosition, Buffer, Count);
					}
					else {
						memset((uint8_t*)m_buf.Data() + m_nPosition, 0, Count);
					}
					m_nPosition = Pos;
					return Count;
				}
			}
			return 0;
		}

		bool CMemBufferClass::SetCapacity(int32_t NewCapacity)
		{
			NewCapacity = (NewCapacity + (MemoryDelta - 1)) & ~(MemoryDelta - 1);
			m_nCapacity = NewCapacity;
			return m_buf.Realloc(m_nCapacity) != NULL ? true : false;
		}

		bool CMemBufferClass::SetSize(int32_t NewSize)
		{
			if (NewSize >= m_nCapacity) {
				if (SetCapacity(NewSize) == NULL) return false;
			}

			m_nSize = NewSize;
			return true;
		}


		void CMemBufferClass::Swap(CMemBufferClass & other)
		{
			other.m_buf.Swap(m_buf);
			std::swap(other.m_nCapacity, m_nCapacity);
			std::swap(other.m_nSize, m_nSize);
			std::swap(other.m_nPosition, m_nPosition);
		}
	}
}

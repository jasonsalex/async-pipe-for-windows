#include "CMemHandler.h"
#include <iostream>
namespace hfjy
{
	namespace CMemHandler
	{
		CMemHandlerClass::CMemHandlerClass(int nAlign)
			: m_buf(0)
			, m_size(0)
			, m_align(nAlign)
		{
		}

		CMemHandlerClass::~CMemHandlerClass()
		{
			Release();
		}

		void * CMemHandlerClass::Malloc(int32_t nSize)
		{
			Release();
			if (nSize > 0) {
				if (m_align) {
					m_buf = _aligned_malloc(nSize, 16);
				}
				else {
					m_buf = malloc(nSize);
				}
				if (nullptr != m_buf) {
					m_size = nSize;
				}
			}
			return m_buf;
		}

		void CMemHandlerClass::Attach(void * buf, int32_t nSize, int nAlign)
		{
			Release();
			m_buf = buf;
			m_size = nSize;
			m_align = nAlign;
		}

		void * CMemHandlerClass::Detach()
		{
			void * buf = m_buf;
			m_buf = NULL;
			m_size = 0;
			return buf;
		}

		int32_t CMemHandlerClass::DataSize()
		{
			return m_size;
		}

		void * CMemHandlerClass::Data()
		{
			return m_buf;
		}

		int CMemHandlerClass::Align()
		{
			return m_align;
		}

		void CMemHandlerClass::Swap(CMemHandlerClass & other)
		{
			std::swap(other.m_align, m_align);
			std::swap(other.m_size, m_size);
			std::swap(other.m_buf, m_buf);
		}

		void CMemHandlerClass::Release()
		{
			if (m_buf) {
				if (m_align) {
					_aligned_free(m_buf);
				}
				else {
					free(m_buf);
				}
				m_buf = NULL;
				m_size = 0;
			}
		}

		void * CMemHandlerClass::Realloc(int32_t nSize)
		{
			if (m_align) {
				m_buf = _aligned_realloc(m_buf, nSize, m_align);
			}
			else {
				m_buf = realloc(m_buf, nSize);
			}
			if (nullptr != m_buf) {
				m_size = nSize;
			}
			return m_buf;
		}
	}
}
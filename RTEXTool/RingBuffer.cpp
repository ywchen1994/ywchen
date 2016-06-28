#include "StdAfx.h"
#include "RingBuffer.h"


CRingBuffer::CRingBuffer(void)
	: m_HeadRingBuffer(0)
	, m_TailRingBuffer(0)
{
	for(int i = 0 ; i < RingBufferSize ; i++)
	{
		m_DataRingBuffer[RingBufferSize] = 0;
	}
}
CRingBuffer::~CRingBuffer(void)
{
}


bool CRingBuffer::BufferIsFull(void)
{
	//TRACE("head = %d , tail = %d\n",m_HeadRingBuffer, m_TailRingBuffer);
	if(((m_HeadRingBuffer + 1) % RingBufferSize) == m_TailRingBuffer)
		return TRUE;
	else
		return FALSE;
}
bool CRingBuffer::BufferIsEmpty()
{
	if(m_HeadRingBuffer == m_TailRingBuffer)
		return TRUE;
	else
		return FALSE;
}
UINT CRingBuffer::uiBufferSpace(void)
{
	if(m_HeadRingBuffer >= m_TailRingBuffer)
	{
		return(RingBufferSize + m_TailRingBuffer - m_HeadRingBuffer  - 1);
	}
	else
	{
		return (m_TailRingBuffer - m_HeadRingBuffer - 1);
	}
}
UINT CRingBuffer::uiBufferDataSize()
{
	if(m_HeadRingBuffer >= m_TailRingBuffer)
	{
		return(m_HeadRingBuffer - m_TailRingBuffer);
	}
	else
	{
		return (RingBufferSize + m_HeadRingBuffer - m_TailRingBuffer);
	}
}

void CRingBuffer::ADD(unsigned char ucdata)
{
	m_DataRingBuffer[m_HeadRingBuffer] = ucdata;
	m_HeadRingBuffer = ((m_HeadRingBuffer + 1) % RingBufferSize) ;
}
unsigned char CRingBuffer::Extract()
{
	unsigned char ch = 0;
	ch = m_DataRingBuffer[m_TailRingBuffer];
	m_TailRingBuffer = ((m_TailRingBuffer + 1) % RingBufferSize) ;
	return ch;
}

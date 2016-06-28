#pragma once
#define RingBufferSize 1000

class CRingBuffer
{
public:
	CRingBuffer(void);
	~CRingBuffer(void);
protected:
	unsigned char m_DataRingBuffer[RingBufferSize];
	UINT m_HeadRingBuffer;
	UINT m_TailRingBuffer;

public:
	bool BufferIsFull(void);
	bool BufferIsEmpty(void);
	UINT uiBufferSpace(void);
	UINT uiBufferDataSize(void);
	void ADD(unsigned char ucdata);
	unsigned char Extract(void);

};


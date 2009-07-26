#ifndef __POOL_H_INCLUDED__
#define __POOL_H_INCLUDED__

template<typename T>
class Pool
{
public:
	Pool<T>(int size = 0) :m_buffers(NULL), m_size(size), m_next(0), m_allocated(0), m_maxAllocated(0)
	{
		if (m_size > 0)
			m_buffers = new T[m_size];
	}
	bool isNull() const { return m_buffers == NULL; }
	int size() const { return m_size; }
	void init(int size)
	{
		if (m_buffers != NULL)
			delete[] m_buffers;
		m_size = size;
		if (m_size > 0)
			m_buffers = new T[m_size];
		clear();
		clearMaxAlloc();
	}
	T* allocate()
	{
		++m_allocated;
		if (m_next < m_size)
			return &m_buffers[m_next++];
		else
			return new T(); // TBD: leaks memory
	}
	void clear()
	{
	//	for(int i = 0; i < m_size; ++i) no real need to do this.
	//		m_buffers[i] = T();
		m_next = 0;
		calcMax();
		m_allocated = 0;
	}
	void calcMax() const
	{
		if (m_allocated > m_maxAllocated)
			m_maxAllocated = m_allocated;
	}

	void clearMaxAlloc() { m_maxAllocated = 0; }
	int getMaxAlloc() const { return m_maxAllocated; }

private:
	T* m_buffers;
	int m_size;
	int m_next; // the index of the next buffer to allocate
	int m_allocated;

	mutable int m_maxAllocated; // not part of the internal state. just keeps stats. can be mutable

};


#endif
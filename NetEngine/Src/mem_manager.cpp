#include "mem_manager.h"

#include <stdlib.h>
#include <assert.h>


#ifdef _DEBUG
	#include <stdio.h>
	#define MemTrace(...) printf(__VA_ARGS__)
#else
	#define MemTrace(...)
#endif // _DEBUG

namespace knet
{
	static size_t s_MemBlockSize = sizeof(MemBlock);
	/***********************************************
	*
	***********************************************/

	/***********************************************
	 *
	 ***********************************************/
	MemAlloc::MemAlloc()
	{

	}

	MemAlloc::~MemAlloc()
	{
		if (m_poolData)
		{
			free(m_poolData);
			m_poolData = nullptr;
		}
	}

	void* MemAlloc::Alloc(size_t size)
	{
		if (!m_poolData)
			Init();

		MemBlock* ret = nullptr;
		// 没有内存,直接向操作系统申请
		if (!m_header)
		{
			// 这里需要多分配s_MemBlockSize内存用来存储block信息
			ret = (MemBlock*)malloc(size + s_MemBlockSize);

			ret->m_inPool = false;
			ret->m_id = -1;
			ret->m_ref = 1;
			ret->m_alloc = nullptr;
			ret->m_next = nullptr;
		}
		// 还有,可以直接从池中分配
		else
		{
			ret = m_header;
			m_header = m_header->m_next;
			assert(0 == ret->m_ref);
			ret->m_ref++;
		}

		MemTrace("  MemAlloc::Alloc: 0x%llx, m_id=%d, size=%d\n", ret, ret->m_id, (int)size);
		return ((char*)ret + s_MemBlockSize);
	}

	void MemAlloc::Free(void* p)
	{
		char* data = (char*)p;
		// 回退 s_MemBlockSize 指向MemBlock位置
		MemBlock* block = (MemBlock*)(data - s_MemBlockSize);
		assert(block->m_ref > 0);

		// 说明还有引用
		if (--block->m_ref != 0)
			return;

		if (block->m_inPool)
		{
			block->m_next = m_header;
			m_header = block;
		}
		else
		{
			free(block);
		}
	}

	void MemAlloc::Init()
	{
		assert(nullptr == m_poolData);

		size_t poolSize = (m_size + s_MemBlockSize) * m_blockCount;
		m_poolData = (char*)malloc(poolSize);

		m_header = (MemBlock*)m_poolData;
		m_header->m_inPool = true;
		m_header->m_id = 0;
		m_header->m_ref = 0;
		m_header->m_alloc = this;
		m_header->m_next = nullptr;

		MemBlock* header = m_header;
		MemBlock* next = nullptr;
		for (size_t i = 1; i < m_blockCount; ++i)
		{
			next = (MemBlock*)(m_poolData + i * (m_size + s_MemBlockSize));
			next->m_inPool = true;
			next->m_id = (int)i;
			next->m_ref = 0;
			next->m_alloc = this;
			next->m_next = nullptr;
			header->m_next = next;
			header = next;
		}
	}
	/***********************************************
	 *
	 ***********************************************/
	MemManager::MemManager()
	{
		Init(0, 64, &m_mem);
	}

	MemManager::~MemManager()
	{

	}

	void MemManager::Init(int bgn, int end, MemAlloc* mem)
	{
		for (int i = bgn; i <= end; ++i)
		{
			m_szAlloc[i] = mem;
		}
	}

	void* MemManager::Alloc(size_t size)
	{
		if (size <= MAX_MEM_SIZE)
			return m_szAlloc[size]->Alloc(size);

		MemBlock* ret = (MemBlock*)malloc(size + s_MemBlockSize);

		ret->m_inPool = false;
		ret->m_id = -1;
		ret->m_ref = 1;
		ret->m_alloc = nullptr;
		ret->m_next = nullptr;
		MemTrace("MemManager::Alloc: 0x%llx, m_id=%d, size=%d\n", ret, ret->m_id, size);

		return ((char*)ret + s_MemBlockSize);
	}

	void MemManager::Free(void* p)
	{
		char* data = (char*)p;
		// 回退 s_MemBlockSize 指向MemBlock位置
		MemBlock* block = (MemBlock*)(data - s_MemBlockSize);
		MemTrace("MemManager::Free 0x%llx, id=%d\n", block, block->m_id);
		if (block->m_inPool)
		{
			block->m_alloc->Free(p);
		}
		else
		{
			if(--block->m_ref == 0)
				free(block);
		}
	}

} // end of namespace knet
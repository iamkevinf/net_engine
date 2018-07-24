#ifndef __OBJECT_POOL_H__
#define __OBJECT_POOL_H__

#include <stdlib.h>
#include <mutex>
#include <assert.h>

#ifdef _DEBUG
	#include <stdio.h>

	#ifndef MemTrace
		#define MemTrace(...) printf(__VA_ARGS__)
	#endif // MemTrace
#else
	#define MemTrace(...)
#endif // _DEBUG

namespace knet
{
	/***********************************************
	*
	***********************************************/
	class ObjectNode
	{
	public:
		ObjectNode * m_next = nullptr;
		int m_id = 0;
		char m_ref = 0;
		bool m_inPool = false;

	private:
		char c1;
		char c2;
	};

	/***********************************************
	*
	***********************************************/
	template<class Type, size_t PoolSize>
	class ObjectPool
	{
	public:
		ObjectPool()
		{
			InitPool();
		}

		~ObjectPool()
		{
			if (m_poolData)
			{
				delete[] m_poolData;
				m_poolData = nullptr;
			}
		}

		void InitPool()
		{
			assert(nullptr == m_poolData);

			size_t realSize = sizeof(Type) + sizeof(ObjectNode);
			size_t size = PoolSize * realSize;
			m_poolData = new char[size];

			m_header = (ObjectNode*)m_poolData;
			m_header->m_inPool = true;
			m_header->m_id = 0;
			m_header->m_ref = 0;
			m_header->m_next = nullptr;

			ObjectNode* header = m_header;
			ObjectNode* next = nullptr;
			for (size_t i = 1; i < PoolSize; ++i)
			{
				next = (ObjectNode*)(m_poolData + i * realSize);
				next->m_inPool = true;
				next->m_id = (int)i;
				next->m_ref = 0;
				next->m_next = nullptr;
				header->m_next = next;
				header = next;
			}
		}

		void* Alloc(size_t size)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			ObjectNode* ret = nullptr;
			// 没有内存,直接向操作系统申请
			if (nullptr == m_header)
			{
				ret = (ObjectNode*)new char[sizeof(Type) + sizeof(ObjectNode)];
				ret->m_inPool = false;
				ret->m_id = -1;
				ret->m_ref = 1;
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

			MemTrace("ObjectPool::Alloc: 0X%p, m_id=%d, size=%zd\n", ret, ret->m_id, size);
			return ((char*)ret + sizeof(ObjectNode));
		}

		void Free(void* p)
		{
			char* data = (char*)p;
			// 回退 s_MemBlockSize 指向MemBlock位置
			ObjectNode* block = (ObjectNode*)(data - sizeof(ObjectNode));
			assert(block->m_ref > 0);

			if (block->m_inPool)
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				// 说明还有引用
				if (--block->m_ref != 0)
					return;

				block->m_next = m_header;
				m_header = block;
			}
			else
			{
				// 说明还有引用
				if (--block->m_ref != 0)
					return;

				delete[] block;
				block = nullptr;
			}
		}

	private:
		ObjectNode * m_header = nullptr;
		char* m_poolData = nullptr;
		std::mutex m_mutex;
	};

	/***********************************************
	*
	***********************************************/
	template<class Type, size_t PoolSize>
	class ObjectPoolBase
	{
	public:
		void* operator new (size_t size)
		{
			return ObjectPoolInstance().Alloc(size);
		}

		void operator delete(void* p)
		{
			ObjectPoolInstance().Free(p);
		}

		template<typename ...Args>
		static Type* CreateObject(Args ... args)
		{
			Type* obj = new Type(args...);
			return obj;
		}

		static void DestroyObject(Type* obj)
		{
			delete obj;
			obj = nullptr;
		}

	private:
		typedef ObjectPool<Type, PoolSize> ClassTypePool;
		static ClassTypePool& ObjectPoolInstance()
		{
			static ClassTypePool s_objectPool;
			return s_objectPool;
		}
	};


}; // end of namespace knet

#endif // __OBJECT_POOL_H__
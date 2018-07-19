#ifndef __MEM_MANAGER_H__
#define __MEM_MANAGER_H__

namespace knet
{
#ifndef MAX_MEM_SIZE
#define MAX_MEM_SIZE 64
#endif // MAX_MEM_SIZE

	class MemAlloc;

	/***********************************************
	 *
	 ***********************************************/
	class MemBlock
	{
	public:
		MemBlock();
		~MemBlock();
	public:
		// 编号
		int m_id = 0; 
		// 引用次数
		int m_ref = 0;
		// 所属内存池
		MemAlloc* m_alloc = nullptr;
		// 下一个块的位置
		MemBlock* m_next = nullptr;
		// 是否在内存池中
		bool m_inPool = false;

	private:
		// 预留 用来对其
		char c1, c2, c3;
	};

	/***********************************************
	*
	***********************************************/
	class MemAlloc
	{
	public:
		MemAlloc();
		~MemAlloc();

		void* Alloc(size_t size);
		void Free(void* p);

		void Init();

	protected:
		// 内存单元的大小
		size_t m_size = 0;
		// 内存块的数量
		size_t m_blockCount = 0;

		// 池
		char* m_poolData = nullptr;
		// 头
		MemBlock* m_header = nullptr;
	};
	/***********************************************
	*
	***********************************************/
	template<size_t size, size_t blockCount>
	class MemAllocImp : public MemAlloc
	{
	public:
		MemAllocImp():MemAlloc()
		{
			// 操作系统的内存对其数
			size_t n = sizeof(void*);
			// 重算内存对其
			m_size = size / n * n + (size % n ? n : 0);
			m_blockCount = blockCount;
		}
	};

	/***********************************************
	*
	***********************************************/
	class MemManager
	{
	public:
		void* Alloc(size_t size);
		void Free(void* p);

		static MemManager& GetInstance()
		{
			static MemManager s_instance;
			return s_instance;
		}

	private:
		MemManager();
		~MemManager();

		void Init(int bgn, int end, MemAlloc* mem);

	private:
		MemAllocImp<MAX_MEM_SIZE, 10> m_mem;
		MemAlloc* m_szAlloc[MAX_MEM_SIZE+1];
	};

}; // end of namespace knet

#endif // __MEM_MANAGER_H__
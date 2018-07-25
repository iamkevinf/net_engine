#ifndef __MEM_MANAGER_H__
#define __MEM_MANAGER_H__

#include <mutex>

namespace knet
{
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
		// ���
		int m_id = 0; 
		// ���ô���
		int m_ref = 0;
		// �����ڴ��
		MemAlloc* m_alloc = nullptr;
		// ��һ�����λ��
		MemBlock* m_next = nullptr;
		// �Ƿ����ڴ����
		bool m_inPool = false;

	private:
		// Ԥ�� ��������
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
		// �ڴ浥Ԫ�Ĵ�С
		size_t m_size = 0;
		// �ڴ�������
		size_t m_blockCount = 0;

		// ��
		char* m_poolData = nullptr;
		// ͷ
		MemBlock* m_header = nullptr;
		// ��
		std::mutex m_mutex;
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
			// ����ϵͳ���ڴ������
			size_t n = sizeof(void*);
			// �����ڴ����
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
		static const int s_max_mem_size = 1024;
		MemAllocImp<  32, 3000000> m_mem32;
		MemAllocImp<  64, 3000000> m_mem64;
		MemAllocImp< 128, 3000000> m_mem128;
		MemAllocImp< 256, 100000> m_mem256;
		MemAllocImp< 512, 100000> m_mem512;
		MemAllocImp<1024, 100000> m_mem1024;
		MemAlloc* m_szAlloc[s_max_mem_size+1];
	};

}; // end of namespace knet

#endif // __MEM_MANAGER_H__
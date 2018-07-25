#ifndef __CELL_TASK_H__
#define __CELL_TASK_H__

#include <thread>
#include <mutex>
#include <list>
#include <functional>

namespace knet
{
	typedef std::function<void()> CellTaskFunc;
	typedef std::list<CellTaskFunc> CellTaskFuncList;

	/*******************************************************
	 * 
	 *******************************************************/
	class CellTaskService
	{
	public:
		CellTaskService();
		~CellTaskService();

		void AddTask(CellTaskFunc task);

		void Start();

	private:
		void OnRun();

	private:
		CellTaskFuncList m_taskPool;
		CellTaskFuncList m_taskBuff;

		std::mutex m_mutex;
	};

}; // end of namespace knet

#endif // __CELL_TASK_H__
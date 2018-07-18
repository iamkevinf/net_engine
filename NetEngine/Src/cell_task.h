#ifndef __CELL_TASK_H__
#define __CELL_TASK_H__

#include <thread>
#include <mutex>
#include <list>

namespace knet
{
	typedef std::list<class CellTask*> CellTaskList;

	class CellTask
	{
	public:
		CellTask();
		virtual ~CellTask();

		virtual void DoTask();
	};

	/*******************************************************
	 * 
	 *******************************************************/
	class CellTaskService
	{
	public:
		CellTaskService();
		~CellTaskService();

		void AddTask(CellTask* task);

		void Start();

	private:
		void OnRun();

	private:
		CellTaskList m_taskPool;
		CellTaskList m_taskBuff;

		std::mutex m_mutex;
	};

}; // end of namespace knet

#endif // __CELL_TASK_H__
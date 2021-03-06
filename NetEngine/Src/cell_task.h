﻿#ifndef __CELL_TASK_H__
#define __CELL_TASK_H__

#include <thread>
#include <mutex>
#include <list>
#include <functional>

#include "cell_thread.h"

namespace knet
{
	typedef std::function<void()> CellTaskFunc;
	typedef std::list<CellTaskFunc> CellTaskFuncList;

	class Cell;

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
		void Close();

		void SetOwner(Cell* owner) { m_owner = owner; }

	private:
		void OnRun(CellThreadService* thread);

	private:
		CellTaskFuncList m_taskPool;
		CellTaskFuncList m_taskBuff;

		Cell* m_owner = nullptr;

		std::mutex m_mutex;

		CellThreadService m_threadService;
	};

}; // end of namespace knet

#endif // __CELL_TASK_H__
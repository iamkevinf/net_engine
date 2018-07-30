#include "cell_task.h"

#include "cell.h"

#include <functional>   // std::mem_fn
#include <iostream>

namespace knet
{

	/*******************************************************
	*
	*******************************************************/
	CellTaskService::CellTaskService()
	{

	}

	CellTaskService::~CellTaskService()
	{

	}

	void CellTaskService::AddTask(CellTaskFunc task)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_taskBuff.push_back(task);
	}

	void CellTaskService::Start()
	{
		m_isRun = true;
		std::thread thread(std::mem_fn(&CellTaskService::OnRun), this);
		thread.detach();
	}

	void CellTaskService::Close()
	{
		std::cout << "CellTaskService::Close" << m_owner->GetID() << std::endl;
		m_isRun = false;
		m_semaphore.Wait();
	}

	void CellTaskService::OnRun()
	{
		while (m_isRun)
		{
			// lock block
			if (!m_taskBuff.empty())
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				for (auto& task : m_taskBuff)
				{
					m_taskPool.push_back(task);
				}

				m_taskBuff.clear();
			}

			if (m_taskPool.empty())
			{
				std::chrono::milliseconds t(1);
				std::this_thread::sleep_for(t);
				continue;
			}

			for (auto& task : m_taskPool)
				task();

			m_taskPool.clear();
		}

		std::cout << "\tCellTaskService::Closed" << m_owner->GetID() << std::endl;
		m_semaphore.Weakup();
	}

}; // end of namespace knet
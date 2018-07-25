#include "cell_task.h"

#include <functional>   // std::mem_fn

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
		std::thread thread(std::mem_fn(&CellTaskService::OnRun), this);
		thread.detach();
	}

	void CellTaskService::OnRun()
	{
		while (true)
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
	}

}; // end of namespace knet
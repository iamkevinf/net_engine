﻿#include "cell_task.h"

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
		auto onRun = [this](CellThreadService* thread)
		{
			OnRun(thread);
		};

		m_threadService.Start(nullptr, onRun, nullptr);
	}

	void CellTaskService::Close()
	{
		std::cout << "CellTaskService::Close" << m_owner->GetID() << std::endl;
		m_threadService.Close();
	}

	void CellTaskService::OnRun(CellThreadService* thread)
	{
		while (thread->IsRun())
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
	}

}; // end of namespace knet
#include "cell_thread.h"


namespace knet
{

	/*******************************************************
	*
	*******************************************************/
	CellThreadService::CellThreadService()
	{

	}

	CellThreadService::~CellThreadService()
	{

	}

	void CellThreadService::Start(CellThreadFunc onCreate/*= nullptr*/, CellThreadFunc onRun/*= nullptr*/, CellThreadFunc onDestory/*= nullptr*/)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_isRun)
			return;

		m_isRun = true;

		if (onCreate)
			m_onCreate = onCreate;
		if (onRun)
			m_onRun = onRun;
		if (onDestory)
			m_onDestory = onDestory;

		std::thread t(std::mem_fn(&CellThreadService::OnWork), this);
		t.detach();
	}

	void CellThreadService::Close()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (!m_isRun)
			return;

		m_isRun = false;
		m_sem.Wait();
	}

	void CellThreadService::CloseWithoutWait()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (!m_isRun)
			return;

		m_isRun = false;
	}

	void CellThreadService::OnWork()
	{
		if (m_onCreate)
			m_onCreate(this);
		if (m_onRun)
			m_onRun(this);
		if (m_onDestory)
			m_onDestory(this);

		m_sem.Weakup();
	}

}; // end of namespace knet
#ifndef __CELL_THREAD_H__
#define __CELL_THREAD_H__

#include <thread>
#include <mutex>
#include <functional>

#include "semaphore.h"

namespace knet
{
	typedef std::function<void(class CellThreadService*)> CellThreadFunc;

	class CellThreadService
	{
	public:
		CellThreadService();
		~CellThreadService();

		void Start(CellThreadFunc onCreate=nullptr, CellThreadFunc onRun=nullptr, CellThreadFunc onDestory=nullptr);
		void Close();
		void CloseWithoutWait();
		bool IsRun()const { return m_isRun; }

	protected:
		void OnWork();

	private:
		CellThreadFunc m_onCreate = nullptr;
		CellThreadFunc m_onRun = nullptr;
		CellThreadFunc m_onDestory = nullptr;

		std::mutex m_mutex;
		bool m_isRun = false;
		Semaphore m_sem;
	};

}; // end of namespace knet

#endif // __CELL_THREAD_H__
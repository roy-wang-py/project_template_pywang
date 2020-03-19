#ifndef  __TIMER_H_
#define	 __TIMER_H_

#include<functional>
#include<chrono>
#include<thread>
#include<atomic>
#include<memory>
#include<mutex>
#include<condition_variable>
#include "default_config.hxx"
NAMESPACE_PREFIX

	class CTimer
	{
	public:
		CTimer() :m_Expired(true), m_TryToExpire(false)
		{
		}

		CTimer(const CTimer& t)
		{
			m_Expired = t.m_Expired.load();
			m_TryToExpire = t.m_TryToExpire.load();
		}
		~CTimer()
		{
			Expire();
		}

		void StartTimer(int interval, std::function<void()> task)
		{
			if (m_Expired == false) {
				return;
			}
			m_Expired = false;
			std::thread([this, interval, task]() {
				while (!m_TryToExpire) {
					std::this_thread::sleep_for(std::chrono::milliseconds(interval));
					task();
				}
				{
					std::lock_guard<std::mutex> locker(m_Mtx);
					m_Expired = true;
					m_ExpiredCond.notify_one();
				}
			}).detach();
		}
		void Expire()
		{
			if (m_Expired) {
				return;
			}

			if (m_TryToExpire) {
				return;
			}
			m_TryToExpire = true;
			{
				std::unique_lock<std::mutex> locker(m_Mtx);
				m_ExpiredCond.wait(locker, [this] {return m_Expired == true; });
				if (m_Expired == true) {
					m_TryToExpire = false;
				}
			}
		}
		template<typename callable, class... arguments>
		void SyncWait(int after, callable&& f, arguments&&... args)
		{

			std::function<typename std::result_of<callable(arguments...)>::type()> task
			(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
			std::this_thread::sleep_for(std::chrono::milliseconds(after));
			task();
		}
		template<typename callable, class... arguments>
		void AsyncWait(int after, callable&& f, arguments&&... args)
		{
			std::function<typename std::result_of<callable(arguments...)>::type()> task
			(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

			std::thread([after, task]() {
				std::this_thread::sleep_for(std::chrono::milliseconds(after));
				task();
			}).detach();
		}
	private:
		std::atomic<bool> m_Expired;
		std::atomic<bool> m_TryToExpire;
		std::mutex m_Mtx;
		std::condition_variable m_ExpiredCond;
	};
NAMESPACE_SUBFIX

#endif // ! __TIMER_H_

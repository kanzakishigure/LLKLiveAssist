#include "Core/PeriodicTask.h"
#include <iostream>

namespace NAssist
{

	PeriodicTask::PeriodicTask(std::function<void()> task, int interval)
		:m_task(task), m_interval(interval),m_isRunning(false),
		m_io_ctx(),
		m_timer(m_io_ctx,boost::asio::chrono::seconds(interval))
	{

		
		
	}

	

	PeriodicTask::~PeriodicTask()
	{
		if (!m_io_ctx.stopped())
		{
			Stop();
		}
	}
	

	void PeriodicTask::Start()
	{

		m_timer.async_wait(
			boost::beast::bind_front_handler(
				&PeriodicTask::onTimerEnd,
				shared_from_this()));


		m_isRunning = true;
		m_io_ctx.run();
	}

	void PeriodicTask::Stop()
	{
		m_io_ctx.stop();
		m_isRunning = false;
	}

	void PeriodicTask::CommitTask()
	{
		m_io_ctx.run();
	}

	void PeriodicTask::onTimerEnd(const boost::system::error_code& e)
	{
		if (e)
		{
			std::cout << e.what() << std::endl;
			return;
		}
	
		m_task();
		
		m_timer.expires_at(m_timer.expiry()+boost::asio::chrono::seconds(m_interval));
		m_timer.async_wait( boost::beast::bind_front_handler(&PeriodicTask::onTimerEnd, shared_from_this()));
	}

}
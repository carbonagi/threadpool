#include "thread_pool.h"
#include <stop_token>
#include <print>

ThreadPool::ThreadPool(size_t num)
{
	workers_.reserve(num);
	for (size_t i = 0; i < num; ++i)
	{
		workers_.emplace_back(
			[this](std::stop_token stoken) {
				while (true)
				{
					std::function<void()> task;
					{
						std::unique_lock lk{ mutex_ };
						cv_.wait(lk, stoken, [this] {return !tasks_.empty();});
						if (stoken.stop_requested() && tasks_.empty())
							return;
						task = std::move(tasks_.front());
						tasks_.pop();
					}
					try {
						std::invoke(task);
					} catch (const std::exception& e) {
						// 记录日志
						std::println("Exception in thread pool task: {}", e.what());
						// TODO: 允许用户自定义异常处理机制，例如通过回调函数
					}
				}
			}
		);
	}
}

ThreadPool::~ThreadPool()
{
	for (auto& w : workers_)
		w.request_stop();
	// 需要在这里等待所有 worker 线程结束
	// 避免其它成员变量在 worker 线程结束前销毁
	for (auto& w : workers_)
	{
		if (w.joinable())
			w.join();
	}
}

void ThreadPool::submit(std::function<void()> task)
{
	std::unique_lock lk{ mutex_ };
	tasks_.emplace(std::move(task));
	lk.unlock();
	cv_.notify_one();
}



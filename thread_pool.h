#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <utility>
#include <print>

class ThreadPool
{
public:
	explicit ThreadPool(size_t num);
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&) = delete;
	ThreadPool& operator=(ThreadPool&&) = delete;
	~ThreadPool();

	void submit(std::function<void()> task);
private:
	std::vector<std::jthread> workers_;
	std::queue<std::function<void()>> tasks_;
	std::mutex mutex_;
	std::condition_variable_any cv_;
};

inline ThreadPool::ThreadPool(size_t num)
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
					}
					catch (const std::exception& e) {
						// 记录日志
						std::println("Exception in thread pool task: {}", e.what());
						// TODO: 允许用户自定义异常处理机制，例如通过回调函数
					}
				}
			}
		);
	}
}

inline ThreadPool::~ThreadPool()
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

inline void ThreadPool::submit(std::function<void()> task)
{
	std::unique_lock lk{ mutex_ };
	tasks_.emplace(std::move(task));
	lk.unlock();
	cv_.notify_one();
}

#endif // !THREAD_POOL_H

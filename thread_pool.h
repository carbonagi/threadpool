#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <utility>

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
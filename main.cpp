#include <print>
#include <thread>

#include "thread_pool.h"

static void hello()
{
	std::println("Hello, World! at thread {}", std::this_thread::get_id());
}

static void throw_exception()
{
	throw std::runtime_error("This is a test exception");
}

int main()
{
	ThreadPool pool{ 4 };
	for (auto i = 0;i != 10; ++i)
	{
		pool.submit(hello);
	}

	// 提交一个会抛出异常的任务，测试异常处理机制
	pool.submit(throw_exception);
}
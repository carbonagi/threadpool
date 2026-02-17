#include <print>
#include <thread>

#include "thread_pool.h"

static void hello()
{
	std::println("Hello, World! at thread {}", std::this_thread::get_id());
}

int main()
{
	ThreadPool pool{ 4 };
	for (auto i = 0;i != 10; ++i)
	{
		pool.submit(hello);
	}
}
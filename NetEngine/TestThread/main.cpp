#include "mem_alloctor.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

#include <memory>

#include "net_time.h"
#include "object_pool.h"

// 原子操作 也就是不可分割的操作
std::atomic_int g_atomic_sum = 0;

std::mutex g_mutex;

const int tCount = 8;
const int maxAllocCount = 100000;
const int preAllocCount = maxAllocCount / tCount;
int g_sum = 0;


struct MyInt {
	MyInt(int v) :val(v) {
		std::cout << "  Hello: " << val << std::endl;
	}
	~MyInt() {
		std::cout << "  Good Bye: " << val << std::endl;
	}
	int val;
};

template <typename T>
struct Deleter {
	void operator()(T *ptr) {
		++Deleter::count;
		delete ptr;
	}
	static int count;
};

template <typename T>
int Deleter<T>::count = 0;

typedef Deleter<int> IntDeleter;
typedef Deleter<double> DoubleDeleter;
typedef Deleter<MyInt> MyIntDeleter;

int test_shared_ptr5()
{
	{
		std::shared_ptr<int> sharedPtr1(new int(1998), IntDeleter());
		std::shared_ptr<int> sharedPtr2(new int(2011), IntDeleter());
		std::shared_ptr<double> sharedPtr3(new double(3.17), DoubleDeleter());
		std::shared_ptr<MyInt> sharedPtr4(new MyInt(2017), MyIntDeleter());
		std::shared_ptr<MyInt> sharedPtr5(new MyInt(666), MyIntDeleter());
		sharedPtr5.reset();
		getchar();
	}

	std::cout << "Deleted " << IntDeleter().count << " int values." << std::endl;
	std::cout << "Deleted " << DoubleDeleter().count << " double value." << std::endl;
	std::cout << "Deleted " << MyIntDeleter().count << " MyInt value." << std::endl;

	return 0;
}

class TestObjectPool : public knet::ObjectPoolBase<TestObjectPool, 100000>
{
public:
	TestObjectPool(int a)
	{
		m_a = a;
	}

	int GetA()const { return m_a; }

private:
	int m_a = 0;
};

void thread_func(int index)
{
	//char* data[preAllocCount];
	TestObjectPool* data[preAllocCount];
	for (size_t i = 0; i < preAllocCount; ++i)
	{
		//data[i] = new char[rand() % 1024 + 1];
		data[i] = TestObjectPool::CreateObject(6);
	}
	for (size_t i = 0; i < preAllocCount; ++i)
	{
		//delete[] data[i];
		TestObjectPool::DestroyObject(data[i]);
	}


	for (int i = 0; i < 20000000; ++i)
	{
		//g_atomic_sum++;
		//std::lock_guard<std::mutex> guard(g_mutex);
		////g_mutex.lock();
		//	g_sum++;
		////g_mutex.unlock();
	}
}


int main()
{
	//knet::Time t1;
	//std::thread t[tCount];
	//for (int i = 0; i < tCount; ++i)
	//{
	//	t[i] = std::thread(thread_func, i);
	//}
	//double tt1 = t1.GetElapsedTimeMS();
	//for (int i = 0; i < tCount; ++i)
	//{
	//	t[i].join();
	//	//t[i].detach();
	//}
	//double tt2 = t1.GetElapsedTimeMS();
	//std::cout << "In Job  Thread g_sum = " << g_sum << " dTime = " << tt2 - tt1 << std::endl;
	//getchar();
	//return 0;
	//TestObjectPool* a1 = new TestObjectPool(2);
	//delete a1;

	//std::cout << "------------ Not in Object Pool" << std::endl;

	//// Not In Object Pool
	//{
	//	std::shared_ptr<TestObjectPool> s1 = std::make_shared<TestObjectPool>(10);
	//}

	//std::cout << "------------ in Object Pool" << std::endl;
	//// In Object Pool
	//{
	//	std::shared_ptr<TestObjectPool> s1(new TestObjectPool(10));
	//}
	//getchar();
	//return 0;
	//TestObjectPool* testA = TestObjectPool::CreateObject(2);
	//TestObjectPool::DestroyObject(testA);
	//std::cout << testA->GetA() << std::endl;
	//TestObjectPool* testA2 = TestObjectPool::CreateObject(20);
	//TestObjectPool::DestroyObject(testA2);
	//std::cout << testA2->GetA() << std::endl;
	//return 0;

	test_shared_ptr5();
	getchar();
	return 0;

	struct Test
	{
		int m_a = 0;
		Test()
		{
			std::cout << "Test::Test()" << std::endl;
		}

		~Test()
		{
			std::cout << "Test::~Test()" << std::endl;
		}
	};

	std::shared_ptr<Test> a = std::make_shared<Test>();
	a->m_a = 2;
	Test* ori = a.get();
	bool u = a.unique();
	int count = a.use_count();
	a.reset();
	count = a.use_count();

	std::unique_ptr<Test> b = std::make_unique<Test>();
	b->m_a = 100;
	auto cc = b.get();
	std::unique_ptr<Test> c = std::move(b);
	auto ccc = c.get();

	//std::weak_ptr<Test> d = std::make_shared<Test>();
	//d.lock().get()->m_a;

	//getchar();
	return 0;
}
/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-22 13:32:03
**/

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

#include "../src/shared_lock.hpp"

using namespace Mushroom;

SharedLock lock;
bool on = true;
std::chrono::milliseconds t1(150);
std::chrono::milliseconds t2(300);
std::chrono::milliseconds t3(100);
std::chrono::milliseconds t4(2000);
std::chrono::milliseconds t5(1000);

void Lock(int i)
{
	for (; on;) {
		switch (i) {
			case 0:
				lock.LockShared();
				lock.Upgrade();
				// std::this_thread::sleep_for(t1);
				lock.Unlock();
				break;
			case 1:
				lock.Lock();
				// std::this_thread::sleep_for(t2);
				lock.Unlock();
				break;
			case 2:
				lock.LockShared();
				// lock.Upgrade();
				// std::this_thread::sleep_for(t3);
				lock.UnlockShared();
				break;
		}
	}
}

void Show()
{
	for (;;) {
		std::this_thread::sleep_for(t5);
		std::cout << lock.ToString();
		std::cout << on << std::endl;
	}
}

int main(int argc, char **argv)
{
	// std::cout << sizeof(SharedLock) << std::endl;
	std::vector<std::thread> threads;
	for (int i = 0; i != 3; ++i)
		threads.push_back(std::thread(Lock, i % 3));
	threads.push_back(std::thread(Show));
	std::this_thread::sleep_for(t4);
	on = false;
	for (auto &e : threads)
		e.join();
	return 0;
}

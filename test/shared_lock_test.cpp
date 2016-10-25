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
int share = 0;
int upgrade = 0;
int locked = 0;
bool on1 = true;
bool on2 = true;
std::chrono::milliseconds t1(3000);
std::chrono::milliseconds t2(1000);

void Lock(int i)
{
	for (; on1;) {
		switch (i) {
			case 0:
				lock.Lock();
				++locked;
				lock.Unlock();
				break;
			case 1:
				lock.LockShared();
				++share;
				lock.UnlockShared();
				break;
			case 2:
				lock.LockShared();
				lock.Upgrade();
				lock.Downgrade();
				lock.UnlockShared();
				++upgrade;
				break;
		}
	}
}

void Show()
{
	for (; on2;) {
		std::this_thread::sleep_for(t2);
		std::cout << share << " " << upgrade << " " << locked << std::endl;
	}
}

int main(int argc, char **argv)
{
	std::cout << sizeof(SharedLock) << std::endl;
	std::vector<std::thread> threads;
	for (int i = 0; i != 3; ++i)
		threads.push_back(std::thread(Lock, i % 4));
	threads.push_back(std::thread(Show));
	std::this_thread::sleep_for(t1);
	on1 = false;
	std::this_thread::sleep_for(t2);
	on2 = false;
	for (auto &e : threads)
		e.join();
	std::cout << share << " " << upgrade << " " << locked << std::endl;
	return 0;
}

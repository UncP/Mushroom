/**
 *    > Author:   UncP
 *    > Mail:     770778010@qq.com
 *    > Github:   https://www.github.com/UncP/Mushroom
 *    > Description:
 *
 *    > Created Time: 2016-10-30 15:10:45
**/

#include <iostream>
#include <vector>
#include <thread>

#include "../src/latch_manager.hpp"

using namespace Mushroom;

LatchManager latch_manager;
int share = 0;
int upgrade = 0;
int locked = 0;
bool on = true;
std::chrono::milliseconds t1(3000);
std::chrono::milliseconds t2(1000);

void Lock(int i, uint32_t page_no)
{
	for (; on;) {
		switch (i) {
			case 0:
				latch_manager.Lock(page_no);
				++locked;
				latch_manager.Unlock(page_no);
				break;
			case 1:
				latch_manager.LockShared(page_no);
				++share;
				latch_manager.UnlockShared(page_no);
				break;
			case 2:
				latch_manager.LockShared(page_no);
				latch_manager.Upgrade(page_no);
				// latch_manager.Downgrade(page_no);
				latch_manager.Unlock(page_no);
				++upgrade;
				break;
			case 3:
				latch_manager.LockShared(page_no);
				latch_manager.Upgrade(page_no);
				// latch_manager.Downgrade(page_no);
				latch_manager.Unlock(page_no);
				++upgrade;
				break;
		}
		page_no = (page_no + 1) % 1024;
	}
}

void Show()
{
	for (; on;) {
		std::this_thread::sleep_for(t2);
		std::cout << share << " " << upgrade << " " << locked << std::endl;
	}
}

int main(int argc, char **argv)
{
	std::vector<std::thread> threads;
	for (int i = 0; i != 2; ++i)
		threads.push_back(std::thread(Lock, i % 4, 0));
	threads.push_back(std::thread(Show));
	std::this_thread::sleep_for(t1);
	on = false;
	for (auto &e : threads)
		e.join();
	return 0;
}

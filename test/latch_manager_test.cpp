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
std::chrono::milliseconds t1(6000);
std::chrono::milliseconds t2(1000);

void Lock(int i, uint32_t page_no)
{
	Latch *latch = nullptr;
	for (; on;) {
		switch (i) {
			case 0:
				latch = latch_manager.GetLatch(page_no);
				latch->Lock();
				++locked;
				latch->Unlock();
				break;
			case 1:
				latch = latch_manager.GetLatch(page_no);
				latch->LockShared();
				++share;
				latch->UnlockShared();
				break;
			case 2:
				latch = latch_manager.GetLatch(page_no);
				latch->LockShared();
				latch->Upgrade();
				++upgrade;
				latch->Unlock();
				break;
			case 3:
				latch = latch_manager.GetLatch(page_no);
				latch->LockShared();
				latch->Upgrade();
				++upgrade;
				latch->Downgrade();
				latch->UnlockShared();
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
	for (int i = 0; i != 4; ++i)
		threads.push_back(std::thread(Lock, i % 4, i % 2));
	threads.push_back(std::thread(Show));
	std::this_thread::sleep_for(t1);
	on = false;
	for (auto &e : threads)
		e.join();
	// std::cout << latch_manager.ToString();
	return 0;
}

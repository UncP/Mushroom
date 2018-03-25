/**
 *    > Author:        UncP
 *    > Github:  www.github.com/UncP/Mushroom
 *    > License:      BSD-3
 *    > Time:    2017-06-21 10:42:40
**/

#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include <cassert>
#include <cstdio>

#include "../src/art/node.hpp"
#include "../src/art/art.hpp"

const char *file = "../data/10000000";
static const int key_len = 16;
static int total;

using namespace Mushroom;

double Put(ART *art)
{
	int fd = open(file, O_RDONLY);
	assert(fd > 0);
	char buf[8192];
	int cur = 0, ptr = 0, count = 0;
	bool flag = true;

	auto beg = std::chrono::high_resolution_clock::now();
	for (; flag && (ptr = pread(fd, buf, 8192, cur)) > 0; cur += ptr) {
		while (--ptr && buf[ptr] != '\n' && buf[ptr] != '\0') buf[ptr] = '\0';
		if (ptr) buf[ptr++] = '\0';
		else break;
		for (int i = 0; i < ptr; ++i) {
			char *tmp = buf + i;
			i += key_len;
			assert(buf[i] == '\n' || buf[i] == '\0');
			buf[i] = '\0';

			art->Put((const uint8_t *)tmp, key_len, (uint32_t)count);

			if (++count == total) {
				flag = false;
				break;
			}
		}
	}
	close(fd);

	auto end = std::chrono::high_resolution_clock::now();
	auto t = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	return t;
}

double Get(ART *art)
{
	int fd = open(file, O_RDONLY);
	assert(fd > 0);
	char buf[8192];
	int cur = 0, ptr = 0, count = 0;
	bool flag = true;

	auto beg = std::chrono::high_resolution_clock::now();
	for (; flag && (ptr = pread(fd, buf, 8192, cur)) > 0; cur += ptr) {
		while (--ptr && buf[ptr] != '\n' && buf[ptr] != '\0') buf[ptr] = '\0';
		if (ptr) buf[ptr++] = '\0';
		else break;
		for (int i = 0; i < ptr; ++i) {
			char *tmp = buf + i;
			i += key_len;
			assert(buf[i] == '\n' || buf[i] == '\0');
			buf[i] = '\0';

			uint32_t val;
			assert(art->Get((const uint8_t *)tmp, key_len, &val) && int(val) == count);

			if (++count == total) {
				flag = false;
				break;
			}
		}
	}
	close(fd);

	auto end = std::chrono::high_resolution_clock::now();
	auto t = std::chrono::duration<double, std::ratio<1>>(end - beg).count();
	return t;
}

int main(int argc, char **argv)
{
	total = argc == 2 ? atoi(argv[1]) : 1;

	ART art;

	auto t1 = Put(&art);

	auto t2 = Get(&art);

	printf("\033[31mtotal: %d\033[0m\n\033[32mput time: %f  s\033[0m\n", total, t1);
	printf("\033[34mget time: %f  s\033[0m\n", t2);

	return 0;
}

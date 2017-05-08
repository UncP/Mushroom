/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-03-25 14:30:45
**/

#include <random>
#include <ctime>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cassert>

namespace Mushroom {

class MushroomDBTestData
{
	public:
		MushroomDBTestData(time_t seed):seed_(seed) { }

		void Generate(int total, int file_num, int key_len) {
			const std::string choice =
				std::string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-*/");
			std::default_random_engine generator(seed_);
			std::uniform_int_distribution<int> distribution(0, choice.size()-1);

			if ((total % file_num)) {
				printf("fail to generate test data :(\n");
				return ;
			}
			total /= file_num;
			std::ostringstream os;
			os << total;
			if (access("data", F_OK))
				assert(mkdir("data", S_IRUSR | S_IWUSR | S_IXUSR | S_IROTH) >= 0);
			std::string base("data/"+os.str());
			for (int i = 0; i != file_num; ++i) {
				std::ostringstream o;
				o << i;
				std::ofstream out(base+"_"+o.str());
				char key[key_len+1];
				key[key_len] = '\n';
				for (int j = 0; j != total; ++j) {
					for (int k = 0; k != key_len; ++k)
						key[k] = choice[distribution(generator)];
					out.write(key, key_len+1);
				}
				out.close();
			}
		}

	private:
		time_t seed_;
};

} // namespace Mushroom

int main()
{
	using namespace Mushroom;

	MushroomDBTestData data(time(0));
	//           total_key  file_number  key_size
	data.Generate(10000000,     4,        16);
	return 0;
}

import random, os

def generate_data(tot, size, file):
	choice = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMN1234567890-'
	index = len(choice) - 1
	dst = open(file, 'w')
	for i in range(0, tot):
		s = ''
		for j in range(0, size):
			s += choice[random.randint(0, index)]
		dst.write(s)
		if (i + 1) % 9 == 0:
			dst.write('\n')
		else:
			dst.write(' ')
		if i % 100000 == 0:
			print(i)
	dst.close()

if __name__ == '__main__':
	if not os.path.exists('./data'):
		os.mkdir('./data')
	os.chdir('./data')
	# total string
	tot = 1000000
	# size of each string
	size = 10
	generate_data(tot, size, str(tot))
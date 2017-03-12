import random, os

def generate_data_random(tot, size, file):
	choice = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMN1234567890-'
	index = len(choice) - 1
	dst = open(file+'_random', 'w')
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

def generate_data_sequential(tot, size, file):
	dst = open(file+'_sequential', 'w')
	s = ''
	for i in range(0, tot):
		s = str(i+1)
		p = ''
		for k in range(len(s), size):
			p += '0'
		s = p + s
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
	tot = 10000000
	# size of each string
	size = 10
	# generate_data_random(tot, size, str(tot))
	generate_data_sequential(tot, size, str(tot))
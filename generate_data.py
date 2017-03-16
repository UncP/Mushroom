import random, os

def generate_data_random(tot, size, file):
	choice = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
	index = len(choice) - 1
	dst = open(file+'_random', 'w')
	for i in range(0, tot):
		s = ''
		for j in range(0, size):
			s += choice[random.randint(0, index)]
		dst.write(s+'\n')
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
		dst.write(s+'\n')
		if i % 100000 == 0:
			print(i)
	dst.close()

if __name__ == '__main__':
	if not os.path.exists('./data'):
		os.mkdir('./data')
	os.chdir('./data')
	# total string
	tot = 2500000
	# size of each string
	size = 16
	# generate_data_random(tot, size, str(size)+'_'+str(tot))
	for i in range(4):
		generate_data_random(tot, size, str(size)+'_'+str(tot)+'_'+str(i))
	# generate_data_sequential(tot, size, str(size)+'_'+str(tot))
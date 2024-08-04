#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int random_number(int max) {
	if(max < 0) return -1;
	if(max == 0) return 0;

	int fd = open("/dev/urandom", O_RDONLY);
	if(fd == -1) return random() % (max + 1);

	unsigned int random_int;
	ssize_t result = read(fd, &random_int, sizeof(random_int));
	close(fd);

	if(result != sizeof(random_int)) {
		return random() % (max + 1);
	}

	return random_int % (max +1);

}

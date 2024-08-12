#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void trim_quotes(char *str) {
	size_t len = strlen(str); 
	if (len > 0 && str[0] == '"') {
		memmove(str, str + 1, len - 1);
		str[len - 1] = '\0';
		len--;
	}

	if (len > 0 && str[len - 1] == '"') {
		str[len - 1] = '\0';
	}
}


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

	return random_int % (max + 1);

}

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

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

int check_program_flags(uint32_t _flags) {
	uint32_t mask =  0x00000001;
	uint32_t tmp_mask = 0x00000000;
	for(int i = 0; i < 33; i++) {
		if(_flags & mask) {
			switch (i) {
				case 0:
					//select id!
					break;
				case 1:
					//select segue!
					break;
				case 2:
					// Code for i == 2
					break;
				case 3:
					// Code for i == 3
					break;
				case 4:
					// Code for i == 4
					break;
				case 5:
					// Code for i == 5
					break;
				case 6:
					// Code for i == 6
					break;
				case 7:
					// Code for i == 7
					break;
				case 8:
					// Code for i == 8
					break;
				case 9:
					// Code for i == 9
					break;
				case 10:
					// Code for i == 10
					break;
				case 11:
					// Code for i == 11
					break;
				case 12:
					// Code for i == 12
					break;
				case 13:
					// Code for i == 13
					break;
				case 14:
					// Code for i == 14
					break;
				case 15:
					// Code for i == 15
					break;
				case 16:
					// Code for i == 16
					break;
				case 17:
					// Code for i == 17
					break;
				case 18:
					// Code for i == 18
					break;
				case 19:
					// Code for i == 19
					break;
				case 20:
					// Code for i == 20
					break;
				case 21:
					// Code for i == 21
					break;
				case 22:
					// Code for i == 22
					break;
				case 23:
					// Code for i == 23
					break;
				case 24:
					// Code for i == 24
					break;
				case 25:
					// Code for i == 25
					break;
				case 26:
					// Code for i == 26
					break;
				case 27:
					// Code for i == 27
					break;
				case 28:
					// Code for i == 28
					break;
				case 29:
					// Code for i == 29
					break;
				case 30:
					// Code for i == 30
					break;
				case 31:
					// Code for i == 31
					break;
				case 32:
					// Code for i == 32
					break;
				default:
					printf("this shouldnt be possible.");
					break;
			}	
			mask <<= 1;
		}
	}

}

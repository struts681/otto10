#include <stdlib.h>
#include <string.h>

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

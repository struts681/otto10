#define MINIAUDIO_IMPLEMENTATION
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <libconfig.h>
#include "miniaudio.h"

char filepath[] = "/home/amelia/test.mp3";
char *pathptr = filepath;

ma_result ma_status;
ma_engine engine;
ma_sound sound;
ma_uint64 length_pcm_frames;
ma_uint64 current_time_pcm_frames;

float length_seconds;
float current_time_seconds;
float remaining_time;

int main() {
	printf("meow\n");

	ma_status = ma_engine_init(NULL, &engine);
	if(ma_status != MA_SUCCESS) {
			printf("engine init failed : %s\n", ma_status);
			}
	else printf("successfully initialized miniaudio\n");

	ma_status = ma_sound_init_from_file(&engine, &filepath, 0, NULL, NULL, &sound);
	if(ma_status != MA_SUCCESS) {
		printf("sound init failed : %s\n", ma_status);
	}
	
	else printf("successfully initialized sound\n");
	
	ma_sound_start(&sound);

	ma_sound_get_length_in_pcm_frames(&sound, &length_pcm_frames);
	length_seconds = (float)length_pcm_frames / (float)ma_engine_get_sample_rate(&engine);

	do {
		ma_sound_get_cursor_in_pcm_frames(&sound, &current_time_pcm_frames);
		current_time_seconds = (float)current_time_pcm_frames / (float)ma_engine_get_sample_rate(&engine);

		remaining_time = length_seconds - current_time_seconds;
		printf("\rRemaining time: %.2f seconds\e[?25l", remaining_time);

		usleep(10000);
	} while(remaining_time > 4);

	sleep(4);
	
	ma_sound_uninit(&sound);
	ma_engine_uninit(&engine);
	
	return 0;
}


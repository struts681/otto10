//#define MINIAUDIO_IMPLEMENTATION
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "miniaudio.h"





//
double play_song(ma_engine *_engine, ma_sound *_sound, char *_audio_file, int _fade) {

	ma_result _ma_status;
	//init an engine
	_ma_status = ma_engine_init(NULL, _engine);
	if(_ma_status != MA_SUCCESS) {
		perror("engine init failed");
		return -1;
	}
	else printf("successfully initialized miniaudio engine\n");


	//init the sound
	printf("initializing miniaudio sound from %s\n", _audio_file);
	_ma_status = ma_sound_init_from_file(_engine, _audio_file, 0, NULL, NULL, _sound);
	if(_ma_status != MA_SUCCESS) {
		perror("engine init failed\n");
		return -1;
	}
	else printf("successfully initialized a sound from %s\n", _audio_file);
	
	//fade in & start
	ma_sound_set_fade_in_milliseconds(_sound, 0, 1, _fade * 1000);
	ma_sound_start(_sound);

	//get the length in pcm frames, convert to seconds as a float
	ma_uint64 _length_pcm_frames;

	ma_sound_get_length_in_pcm_frames(_sound, &_length_pcm_frames);
	
	double _pcm_frames = (double)_length_pcm_frames;
	double _sample_rate = (double)ma_engine_get_sample_rate(_engine);

	printf("sample rate: %.2f", _sample_rate);
	printf("length in pcm frames: %i", _length_pcm_frames);
	double _length = _pcm_frames / _sample_rate;
	printf("length in seconds: %2.2f", _length);

	return _length;
}


float remaining_time_in_sound(ma_engine *_engine, ma_sound *_sound) {
	

}

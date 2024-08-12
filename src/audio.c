//#define MINIAUDIO_IMPLEMENTATION
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "miniaudio.h"






float remaining_time_in_sound(ma_engine *_engine, ma_sound *_sound) {

	ma_uint64 _cursor_time_frames;
	float _cursor_time_seconds;
	ma_uint64 _total_length_frames;
	float _total_length_seconds;
	float _sample_rate = (float)ma_engine_get_sample_rate(_engine);
	
	ma_sound_get_cursor_in_pcm_frames(_sound, &_cursor_time_frames);

	_cursor_time_seconds = _cursor_time_frames / _sample_rate;

	ma_sound_get_length_in_pcm_frames(_sound, &_total_length_frames);
	_total_length_seconds = _total_length_frames / _sample_rate;


	return _total_length_seconds - _cursor_time_seconds;
}

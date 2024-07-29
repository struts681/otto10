#define MINIAUDIO_IMPLEMENTATION
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <libconfig.h>
#include "miniaudio.h"

#define FADE_DURATION 4 //in seconds as an integer
#define REPETITION_LIMIT 8 //how many songs before we don't care if a repeat is selected
#define DEFAULT_CONFIG_FILE "otto10.conf" //set a default config file if none is specified in the arguments

//config config config
char *otto_config_file;

//will not exist after alpha testing
char soundpath[] = "/home/amelia/test.mp3";

//all the miniaudio stuff - main sound
ma_result ma_status;
ma_engine engine;
ma_sound sound;
ma_uint64 length_pcm_frames;
ma_uint64 current_time_pcm_frames;

//miniaudio stuff - segue sound for in between
ma_engine segue_engine;
ma_sound segue_sound;

//miniaudio stuff - might add a feature to interrupt whats playing for an ID
//like for when democracy now decided to be 2 hours long
//ma_engine spare_engine;
//ma_sound spare_sound;

//for timing what to play
float length_seconds;
float current_time_seconds;
float remaining_time;

//avoid immediate repeats
int last_songs_played[REPETITION_LIMIT];
int last_ids_played[REPETITION_LIMIT];
int last_segues_played[REPETITION_LIMIT];

//flags for IDs and config stuff
uint8_t program_flags = 0x00;

int main(int argc, char *argv[]) {

	printf("meow\n");

	/*----------------------------------------------------------------------------
	 *
	 *
	 *
	 *this section of the code reads a config file and assigns variables and stuff
	 *creates a variable holding a highest tier mask for flags?
	 *
	 *
	 *
	 *----------------------------------------------------------------------------
	 */

	if(argc > 1) {
		otto_config_file = argv[1];
	}
	else otto_config_file = DEFAULT_CONFIG_FILE;

	printf("otto10 starting with config file: %s\n", otto_config_file);


	char musicpath[] = "/home/amelia/music/otto10/";
	//char musicpath[] = (code that reads from the config)
	//char idpath[] = ...;
	//char seguepath[] = ...;

	while(1) {

		/*-------------------------------------------------------------------------------------------------
		 *
		 *
		 *
		 *this section of the code checks the time to determine what flags to put up
		 *should determine a fixed max number of slots for scheduled programming
		 *maybe 7? uint8_t with 1 bit for ID and 7 for programs - adjust the mask according to
		 *what has actually been set.
		 *also checks if there are requests and pulls them into a variable - maybe an int as a unique 
		 *id in the db?
		 *
		 *
		 *
		 * ------------------------------------------------------------------------------------------------
		 */

		/*--------------------------------------------------------------------------------------
		 *
		 *
		 *
		 *this section of the code checks if any flags are up and selects an audio file to play!
		 *also puts the relevant flag back down after selecting a file
		 *note: order of precedence for flags?
		 *also: this section of code selects what the next segue will be
		 *whether a segue will be played or not.
		 *
		 *
		 *--------------------------------------------------------------------------------------
		 */

		/*--------------------------------------------------------------------------------------
		 *
		 *
		 *
		 *
		 *this section of the code plays whatever the program decides is the current audio file!
		 *
		 *
		 *
		 *
		 *--------------------------------------------------------------------------------------
		 */
		//initialize the miniaudio engine!
		ma_status = ma_engine_init(NULL, &engine);
		if(ma_status != MA_SUCCESS) {
			printf("engine init failed : %s\n", ma_status);
			return -1;
		}//check for any failures
		else printf("successfully initialized miniaudio\n");

		//initialize a sound from the audio file selected
		ma_status = ma_sound_init_from_file(&engine, soundpath, 0, NULL, NULL, &sound);
		if(ma_status != MA_SUCCESS) {
			printf("sound init failed : %s\n", ma_status);
			return -1;
		}
		else printf("successfully initialized a sound from %s\n", soundpath);

		//apply a fade in from 0 to 1 of some number of milliseconds(2000)
		ma_sound_set_fade_in_milliseconds(&sound, 0, 1, FADE_DURATION * 1000);
		//start playing it!
		ma_sound_start(&sound);


		//check the total length of whats playing and then convert it to seconds as a float
		ma_sound_get_length_in_pcm_frames(&sound, &length_pcm_frames);
		length_seconds = (float)length_pcm_frames / (float)ma_engine_get_sample_rate(&engine);

		//have to run this at least once to ensure it really does run - comparison for remaining_time probably 
		//sets a minimum length for audio files to play properly. i doubt 2-4 seconds will be an issue
		do {
			//get current time! from the cursor
			ma_sound_get_cursor_in_pcm_frames(&sound, &current_time_pcm_frames);
			current_time_seconds = (float)current_time_pcm_frames / (float)ma_engine_get_sample_rate(&engine);

			remaining_time = length_seconds - current_time_seconds;
			printf("\rRemaining time: %.1f seconds ", remaining_time);
			fflush(stdout);
			//calculates how long is remaining and then actually prints it properly -
			//fflush makes sure stdout isnt doing anything wacky so it looks right

			usleep(10000);
		} while(remaining_time > FADE_DURATION);

		ma_sound_set_fade_in_milliseconds(&sound, -1, 0, FADE_DURATION * 1000);


		//cleanup because we try to be good programmers here!
		ma_sound_uninit(&sound);
		ma_engine_uninit(&engine);

		//LSB of audio_flags is for a station ID, if we AND it with 1 and make sure its 0
		//then we dont play a segue after a station ID.
		//if there arent enough segues, just add some blank audio files. seriously.
		//not every song needs a segue after it.
		//plays a segue starting at song end minus FADE_DURATION
		//then moves on when there is less than FADE_DURATION left in the segue
		if(!(program_flags & 0x01)) {


		}
		//error handling is more lax in the segue because we simply dont care as much.
		//

		/*----------------------------------------------------------------
		 *
		 * 
		 *
		 * ok were done with playing a thing! now lets do it again i guess?
		 * or should we add a segue after everything? (as long as it wasnt
		 * a station id)
		 *
		 *
		 *-----------------------------------------------------------------
		 */

	}
	return 0;
}


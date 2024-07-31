#define MINIAUDIO_IMPLEMENTATION
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <libconfig.h>
#include "miniaudio.h"

#define DEFAULT_FADE_DURATION 2 //in seconds as an integer
#define DEFAULT_REPETITION_LIMIT 8 //how many songs before we don't care if a repeat is selected
				   //wait those two should totally be in the conf file instead
#define DEFAULT_CONFIG_FILE "otto10.conf" //set a default config file if none is specified in the arguments


//program flow:
//
//1:
//initializations and stuff
//open a database
//read from config file
//set variables and defaults
//
//
//begin loop in TIME_WATCH:
//2: TIME_WATCH
//check flags for what needs to be queued
//if no flags, check for requests(or make requests a flag?)
//when time remaining is less than FADE_DURATION go to SEGUE_START or FILE_SELECT if playsegue flag is down
//if nothing is playing set to ID_START
//
//3: ID_START
//pick a random ID, check it against a repetition prevention array
//set to TIME_WATCH
//
//4: SEGUE_START
//select random segue and start it playing
//set to FILE_SELECT
//
//5: FILE_SELECT
//check flags, if an id needs playing, select one and enter ID_START
//check if any programming is scheduled to play, if so do it 
//select and store as a string the audio file, go to AUDIO_START
//
//6: AUDIO_START
//start the file and set to TIME_WATCH go to 2
//
//
//
//
//start playing the file
//continue checking the time during playing
//when remaining time < FADE_DURATION, pick a segue and start it playing
//when remaining time in segue < FADE_DURATION, return to the file selection

enum state {
	TIME_WATCH,
	FILE_SELECT,
	AUDIO_START,
	SEGUE_START,
	ID_START
};


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

//flags for IDs and config stuff
uint8_t program_flags = 0x00;

//everything should be a state machine
int program_state = AUDIO_START;

time_t mark_1;
time_t mark_2;


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

	//add some if statements here to check if its defined in the conf file
	int repetition_limit = DEFAULT_REPETITION_LIMIT;
	int fade_duration = DEFAULT_FADE_DURATION;

	//avoid immediate repeats
	int last_songs_played[repetition_limit];
	int last_ids_played[repetition_limit];
	int last_segues_played[repetition_limit];
	//these all go here because the conf file might change them

	while(1){
		mark_1 = clock();

		switch(program_state) {
			case TIME_WATCH:

				/*----------------------------------------------------------------------------------
				 *
				 *
				 *
				 *this section of the code checks the time to determine what flags to put up
				 *should determine a fixed max number of slots for scheduled programming
				 *maybe 7? uint8_t with 1 bit for ID and 7 for programs - 
				 *adjust the mask according to
				 *what has actually been set.
				 *also checks if there are requests and pulls them into a variable - 
				 *maybe an int as a unique 
				 *id in the db?
				 *
				 *
				 *
				 * --------------------------------------------------------------------------------
				 */
				//get current time! from the cursor
				ma_sound_get_cursor_in_pcm_frames(&sound, &current_time_pcm_frames);
				current_time_seconds = (float)current_time_pcm_frames / (float)ma_engine_get_sample_rate(&engine);

				remaining_time = length_seconds - current_time_seconds;
				printf("\rRemaining time: %02i:%02i \r", (int)remaining_time/60, (int)remaining_time%60);
				fflush(stdout);

				if(remaining_time < 0.01) {
					ma_engine_uninit(&engine);
					ma_sound_uninit(&sound);

				}

				break;

			case ID_START:

				/*
				 * 
				 *
				 * this section plays a station ID when necessary
				 *
				 *
				 *
				 */

				break;
			case FILE_SELECT:
				/*---------------------------------------------------------------------------------
				 *
				 *
				 *
				 *this section of the code checks if any flags are up and selects 
				 *an audio file to play!
				 *also puts the relevant flag back down after selecting a file
				 *note: order of precedence for flags?
				 *also: this section of code selects what the next segue will be
				 *whether a segue will be played or not.
				 *
				 *
				 *---------------------------------------------------------------------------------
				 */

				/*---------------------------------------------------------------------------------
				 *
				 *
				 *
				 * this section of the code parses metadata, spits out a log file, sends to RDS
				 *
				 *
				 *
				 *---------------------------------------------------------------------------------
				 */

			case AUDIO_START:
				/*---------------------------------------------------------------------------------
				 *
				 *
				 *
				 *
				 *this section of the code plays whatever the program decides is
				 *the current audio file!
				 *
				 *
				 *
				 *
				 *---------------------------------------------------------------------------------
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
				ma_sound_set_fade_in_milliseconds(&sound, 0, 1, fade_duration * 1000);
				//start playing it!
				ma_sound_start(&sound);


				//check the total length of whats playing and then convert it to seconds as a float
				ma_sound_get_length_in_pcm_frames(&sound, &length_pcm_frames);
				length_seconds = (float)length_pcm_frames / (float)ma_engine_get_sample_rate(&engine);

				program_state = TIME_WATCH;
				break;

		}

		//repeat the loop only every 10ms
		do {
			mark_2 = clock();
		} while(mark_2 < (mark_1 + 0.010));

	}
}


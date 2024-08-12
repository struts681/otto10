#define MINIAUDIO_IMPLEMENTATION
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <libconfig.h>
#include "miniaudio.h"
#include "defs.h"

#define ever ;; //hehe

#define MAX_PATH_SIZE 4096

#define DEFAULT_FADE_DURATION 2 //in seconds as an integer
#define DEFAULT_REPETITION_LIMIT 8 //how many songs before we don't care if a repeat is selected
				   //wait those two should totally be in the conf file instead
#define DEFAULT_CONFIG_FILE "otto10.conf" //set a default config file if none is specified in the arguments

#define DEFAULT_MUSIC_CSV "/home/amelia/otto10/database/music.csv"
#define DEFAULT_ID_CSV "/home/amelia/otto10/database/ids.csv"
#define DEFAULT_SEGUE_CSV "/home/amelia/otto10/database/segues.csv"

#define DEFAULT_MUSIC_DIR "/home/amelia/Music/"
#define DEFAULT_ID_DIR "/home/amelia/otto10/ids/"
#define DEFAULT_SEGUE_DIR "/home/amelia/otto10/segues/"




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

//variables that store the location of things
char *music_dir;
char *id_dir;
char *segue_dir;

char *music_csv;
char *id_csv;
char *segue_csv;

char *audio_info[3];

char sound_path[MAX_PATH_SIZE];

int music_csv_size_in_lines;
int id_csv_size_in_lines;
int segue_csv_size_in_lines;

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
double length_seconds;
//float current_time_seconds;
float remaining_time;

//flags for IDs and config stuff
uint32_t program_flags = 0x00;

//everything should be a state machine
int program_state = TIME_WATCH;

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

	/*
	 *
	 * below: sets all the paths if not otherwise specified
	 *
	 */

	//set default config file as defined, if none is specified in arguments
	if(argc > 1) {
		otto_config_file = argv[1];
	}
	else otto_config_file = DEFAULT_CONFIG_FILE;

	//set default music library directory if not specified in arguments
	if(0) {
		//music_dir = ;
	}
	else music_dir = DEFAULT_MUSIC_DIR;

	//set id directory as above
	if(0) {
		//id_dir = ;
	}
	else id_dir = DEFAULT_ID_DIR;

	//set segue directory as above
	if(0) {
		//segue_dir = ;
	}
	else segue_dir = DEFAULT_SEGUE_DIR;

	if(0) {
		//music_csv = ;
	}
	else music_csv = DEFAULT_MUSIC_CSV;

	if(0) {
		//id_csv = ;
	}
	else id_csv = DEFAULT_ID_CSV;

	if(0) {
		//segue_csv = ;
	}
	else segue_csv = DEFAULT_SEGUE_CSV;

	/*
	 *
	 * below: count lines in all the csv files so we can randomize good
	 *
	 */

	music_csv_size_in_lines = count_file_lines(music_csv);
	if(music_csv_size_in_lines == -1) {

		recreate_csv_db(music_dir, music_csv);

		music_csv_size_in_lines = count_file_lines(music_csv);
		printf("%i lines found.\n", music_csv_size_in_lines);
	}


	id_csv_size_in_lines = count_file_lines(id_csv);
	if(id_csv_size_in_lines == -1) {
		FILE *fp = fopen(id_csv, "w");
		printf("no id db found. creating one at %s...\n", id_csv);
		if(fp == NULL) return -1;

		process_directory(id_dir, fp);

		fclose(fp);
		id_csv_size_in_lines = count_file_lines(id_csv);
	}

	segue_csv_size_in_lines = count_file_lines(segue_csv);
	if(segue_csv_size_in_lines == -1) {
		FILE *fp = fopen(segue_csv, "w");
		printf("no segue db found. creating one at %s...\n", segue_csv);
		if(fp == NULL) return -1;

		process_directory(segue_dir, fp);

		fclose(fp);

		segue_csv_size_in_lines = count_file_lines(segue_csv);

	}


	printf("otto10 starting with \n"
			"config file:    %s \n"
			"music database: %s \n"
			"id database:    %s \n"
			"segue database: %s \n"
			"music path:     %s \n"
			"id path:        %s \n"
			"segue path:     %s \n",
			otto_config_file,
			music_csv,
			id_csv,
			segue_csv,
			music_dir,
			id_dir,
			segue_dir);


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

	for(ever){
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

				if(ma_sound_is_playing(&sound)) {

					remaining_time = remaining_time_in_sound(&engine, &sound);
					printf("\r%2.2f remaining in the song", remaining_time);
				}

				else {
					program_flags = 0;
					program_state = FILE_SELECT;
					printf("nothing is playing. selecting the next file.\n");
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

				//int flags = parse_flags(program_flags);
				if(!program_flags) {
					printf("now selecting song.\n");
					int song_selection = random_number(music_csv_size_in_lines);
					printf("selected song #: %i of %i\n", song_selection, music_csv_size_in_lines);
					if(read_from_csv(music_csv, song_selection, audio_info)) {
						printf("error selecting a song.\n");
						return -1;
					}
					trim_quotes(audio_info[0]);
					trim_quotes(audio_info[2]);
					snprintf(sound_path, sizeof(sound_path), "%s%s/%s", music_dir, audio_info[2], audio_info[0]);
					printf("song: %s\n", sound_path);

					program_state = AUDIO_START;
				}

				break;

			case AUDIO_START:

				//play_song returns the length in seconds as a float
				//length_seconds = play_song(&engine, &sound, sound_path, fade_duration);
				//printf("duration: %2.2f\n", length_seconds);


				//init an engine
				ma_status = ma_engine_init(NULL, &engine);
				if(ma_status != MA_SUCCESS) {
					perror("engine init failed");
					return -1;
				}
				else printf("successfully initialized miniaudio engine\n");


				//init the sound
				printf("initializing miniaudio sound from %s\n", sound_path);
				ma_status = ma_sound_init_from_file(&engine, sound_path, 0, NULL, NULL, &sound);
				if(ma_status != MA_SUCCESS) {
					perror("sound init failed\n");
					return -1;
				}
				else printf("successfully initialized a sound from %s\n", sound_path);

				//fade in & start
				ma_sound_set_fade_in_milliseconds(&sound, 0, 1, fade_duration * 1000);
				ma_sound_start(&sound);

				//go back to time watch mode
				program_state = TIME_WATCH;

				break;

		}

		//end switch

		//repeat the loop only every 10ms
		do {
			mark_2 = clock();
		} while(mark_2 < (mark_1 + 0.010));

	}
}





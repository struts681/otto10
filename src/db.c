#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <time.h>

#define ever ;;

#define MAX_PATH_SIZE 4096

void write_to_csv(FILE *fp, const char *file) {
	char file_path[MAX_PATH_SIZE];
	char song_name[MAX_PATH_SIZE];
	char artist_name[MAX_PATH_SIZE];

	const char *base_file_name = strrchr(file, '/');
	if(base_file_name) base_file_name++; //finds the last occurence of /
	else base_file_name = file;	     //which creates a string of /filename.mp3 for example

	strncpy(file_path, base_file_name, MAX_PATH_SIZE); //copies the above into file_path 
	strncpy(song_name, file_path, MAX_PATH_SIZE); //gives us a copy of that file path as the song name
	strrchr(song_name, '.');
	char *dot = strrchr(song_name, '.');
	if(dot) *dot = '\0'; //replace the last occurence of '.' with a null character!

	char tmp[MAX_PATH_SIZE];
	strncpy(tmp, file, MAX_PATH_SIZE);
	char *slash = strrchr(tmp, '/'); //find the last /
	if(slash) { //make sure its there
		*slash = '\0'; //last slash is now null, cuts off the file
		slash = strrchr(tmp, '/'); //now find the last / again
		if(slash) slash++; //advance past it
		else slash = tmp;
		strncpy(artist_name, slash, MAX_PATH_SIZE); //now we start just after that slash, and end
							    //where the next one *was*
							    //this gives us the directory!
	}

	fprintf(fp, "\"%s\",\"%s\",\"%s\"", file_path, song_name, artist_name);
}

void process_directory(const char *dir, FILE *fp) {
	printf("opening directory: %s\n", dir);
	DIR *dp = opendir(dir); //open a directory
	if(dp == NULL) {
		perror("failed to open directory"); //null pointer checks
		return;
	}

	struct dirent *entry; //a directory entry
	struct stat statbuf; //information about files specified in the dirent
	char path[MAX_PATH_SIZE];

	while(entry = readdir(dp)) { //read each directory!
		snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name); //constructs the full path of everything
		if(stat(path, &statbuf) == -1) { //gets information about stuff
			perror("failed to get file status (whatever that means)");
			continue; //just keep going if it fails
		}

		if(S_ISDIR(statbuf.st_mode)) { //check if an entry is another directory
			if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
				continue;  //skip . and .. directories otherwise thatd be horrible
			process_directory(path, fp); //recurse!
		}
		else {
			char *ext = strrchr(entry->d_name, '.'); //look for the last . for the extension
			if(ext && !(strcasecmp(ext, ".wav") || //is it a wav?
						!strcasecmp(ext, ".flac")   || //a flac?
						!strcasecmp(ext, ".mp3"))) {   //an mp3?
				write_to_csv(fp, path);//if so, you may be entitled to financial compensation
			}
		}
	}

	closedir(dp); //finish up
}


int count_file_lines(const char *input_file) {
	printf("func begin\n");
	FILE *fp = fopen(input_file, "r");
	if(fp == NULL) {
		printf("null file pointer - does csv file exist?\n\n");
				return 1;
				}

	char buf[4096];
	int lines = 0;
	
	printf("1");

	for(ever) {
		size_t res = fread(buf, 1, 65536, fp);
		if(ferror(fp)) return -1;

		for(int i = 0; i < res; i++) {
			if(buf[i] == '\n') lines++;

		}
		if(feof(fp)) break;

	}
	fclose(fp);
	return lines;
}


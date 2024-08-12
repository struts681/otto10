void trim_quotes(char *str);

double play_song(ma_engine *_engine, ma_sound *_sound, char *_audio_file, int _fade);
int random_number(int max);

void write_to_csv(FILE *fp, const char *file);
void process_directory(const char *dir, FILE *fp);
int count_file_lines(const char *input_file);
int read_from_csv(const char *filename, int line_number, char **result);
void free_csv_result(char **result);


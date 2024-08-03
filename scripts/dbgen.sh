#!/bin/bash

output_file="$2"

rm -f "$output_file"
touch "$output_file"

process_directory() {
	local dir="$1"

	find "$dir" -type f \( -iname "*.wav" -o -iname "*.flac" -o -iname "*.mp3" \) -print0 | while IFS= read -r -d '' file; do
	write_csv "$file"
done

}


write_csv(){

	local file="$1"

	local FILEPATH=$(basename "$file")
	local SONGNAME=$(basename "${file%.*}")
	local ARTISTNAME=$(basename "$(dirname "$file")")

	echo "\"$FILEPATH\",\"$SONGNAME\",\"$ARTISTNAME\"" >> "$output_file"
}

SECONDS=0

process_directory "$1"

echo "completed database generation in $((SECONDS / 60)) minutes and $((SECONDS % 60)) seconds"

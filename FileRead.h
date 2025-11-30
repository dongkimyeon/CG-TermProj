#ifndef FILEREAD_H
#define FILEREAD_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

inline char* filetobuf(const char* file)
{
	if (!file) return NULL;

	FILE* fptr = fopen(file, "rb");
	if (!fptr) // Return NULL on failure
		return NULL;

	// Seek to the end of the file to determine its length
	if (fseek(fptr,0, SEEK_END) !=0) {
		fclose(fptr);
		return NULL;
	}

	long length = ftell(fptr);
	if (length <0) {
		fclose(fptr);
		return NULL;
	}

	// Safe cast to size_t
	size_t ulen = static_cast<size_t>(length);

	// Rewind to beginning
	if (fseek(fptr,0, SEEK_SET) !=0) {
		fclose(fptr);
		return NULL;
	}

	// Allocate buffer for file contents plus null terminator
	char* buf = static_cast<char*>(malloc(ulen +1));
	if (!buf) {
		fclose(fptr);
		return NULL;
	}

	// If file is empty, return an empty string
	if (ulen ==0) {
		buf[0] = '\0';
		fclose(fptr);
		return buf;
	}

	// Read the file. Use fread with size=1,count=ulen to avoid issues when length is large.
	size_t read = fread(buf,1, ulen, fptr);
	fclose(fptr);

	if (read != ulen) {
		// reading error
		free(buf);
		return NULL;
	}

	buf[ulen] = '\0'; // Null terminator
	return buf; // Return the buffer
}

#endif // FILEREAD_H

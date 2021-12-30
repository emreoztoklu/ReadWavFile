// all of your legacy C code here
#include <unistd.h>		//POSIX FONK
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define TRUE 1 
#define FALSE 0
#define SIZE_OF_RIFF 1
#define SIGNED 0
#define UNSIGNED 1

#define FIELD_SIZE_4 4
#define FIELD_SIZE_2 2


 FILE *ptrin;
 FILE *ptrout;


 unsigned char buffer4[FIELD_SIZE_4];
 unsigned char buffer2[FIELD_SIZE_2];

// WAVE file wav.header format
typedef struct  {
	unsigned char	riff[FIELD_SIZE_4];				// ChunkID:			RIFF string
	unsigned int	overall_size	;				// ChunkSize:		overall size of file in bytes
	unsigned char	wave[FIELD_SIZE_4];				// Format:			WAVE string
	unsigned char	fmt_chunk_marker[FIELD_SIZE_4];	// Subchunk1ID:		fmt string with trailing null char
	unsigned int	length_of_fmt;					// Subchunk1Size:	length of the format data
	unsigned int	format_type;					// AudioFormat:		format type.						"1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	unsigned int	channels;						// NumChannels:		no.of channels						"Mono = 1, Stereo = 2, etc."
	unsigned int	sample_rate;					// SampleRate:		sampling rate (blocks per second)	"8000, 44100, etc"
	unsigned int	byterate;						// ByteRate:		SampleRate * NumChannels * BitsPerSample/8
	unsigned int	block_align;					// BlockAlign:		NumChannels * BitsPerSample/8
	unsigned int	bits_per_sample;				// BitsPerSample:	bits per sample, 8- 8bits, 16- 16 bits etc
	unsigned char	data_chunk_header [FIELD_SIZE_4];//Subchunk2ID:		DATA string or FLLR string
	unsigned int	data_size;						// Subchunk2Size:	NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
}headercontent;


typedef struct 
{
	headercontent 	header;
	const char 		*name;
}wavfile;



int wavReader( const char *inputfilename, const char *outputfilename, int txsize/*, int saveAs*/);
char* seconds_to_time(float seconds);




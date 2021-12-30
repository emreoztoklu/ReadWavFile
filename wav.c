/**
 * Read and parse a wave file
 *
 **/
#include "wav.h"


#define LITTLE_TO_BIG_ENDIAN(buff)   (buff[0] |(buff[1]<<8) | (buff[2]<<16) | (buff[3]<<24));


int wavReader( const char *inputfilename, const char *outputfilename, int txsize/*, int saveAs*/)
{
	int rawsize=0;
	//int *tempBufPtr=storageBuf;
	wavfile *wav= (wavfile*)malloc(sizeof(wavfile));
	wav->name= inputfilename;

 	char cwd[1024];
 	getcwd(cwd,sizeof(cwd));

 	strcat(cwd,"\\");
	strcat(cwd, wav->name);

	unsigned int i;
	for(i=0; i<= sizeof(cwd);i++){
		if(cwd[i]== '\\'){
			cwd[i]='/';
		}
		
	}
	printf("%s\n", cwd);
 	printf("Opening  file..\n");
    
	// open file read binary
 	if ((ptrin = fopen(cwd, "rb"))== NULL){
		printf("Error opening file\n");
		exit(EXIT_FAILURE);
 	}

/*********************************************************************************************************************************/
/*
* ChunkID 
*
	0         4            Contains the letters "RIFF" in ASCII form  (0x52494646 big - endian form).

	0x52 49 46 46
	R    I   F  F
*/

	int read = 0;
	// read header parts
 	read = fread(wav->header.riff, sizeof(wav->header.riff), SIZE_OF_RIFF, ptrin);
 	printf("(1-4): %s \n", wav->header.riff); 

/*********************************************************************************************************************************/
/*
* ChunkSize 
* 
	4         4          36 + SubChunk2Size, or more precisely:

	4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
	This is the size of the rest of the chunk following this number.  
	This is the size of the entire file in bytes minus 8 bytes for the two fields not included in this count:
							   
	ChunkID and ChunkSize.   

	0X1E B0 02 00    (big endian)	  514.851.328  ??? 
	0X00 02 B0 1E    (little endian)  176.158 byte


	*(LTTLE ENDIAN)
*/
 	read = fread(buffer4, sizeof(buffer4), 1, ptrin);
 	printf("%u %u %u %u \n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);

    // convert little endian to big endian 4 byte int
	wav->header.overall_size = LITTLE_TO_BIG_ENDIAN(buffer4);
	printf("(5-8) Overall size: bytes:%u, Kb:%u \n", wav->header.overall_size, wav->header.overall_size / 1024);

/*********************************************************************************************************************************/
/*
* Format 
* 
	8         4     Contains the letters "WAVE"    (0x57 41 56 45 big-endian form).
	
	0x57 41 56 45
	  W  A  V  E

The "WAVE" format consists of two subchunks: "fmt " and "data":
The "fmt " subchunk describes the sound data's format:


*/
 	read = fread(wav->header.wave, sizeof(wav->header.wave), 1, ptrin);
 	printf("(9-12) Wave marker: %s\n", wav->header.wave);

/*********************************************************************************************************************************/
/*
*  "fmt" ChuckSýze
		Subchunk1ID  
			12        4       Contains the letters "fmt "		
			(0x666d7420 big-endian form).
*/

	read = fread(wav->header.fmt_chunk_marker, sizeof(wav->header.fmt_chunk_marker), 1, ptrin);
	printf("(13-16) Fmt marker: %s\n", wav->header.fmt_chunk_marker);

 	read = fread(buffer4, sizeof(buffer4), 1, ptrin);

 	printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);	
 	// convert little endian to big endian 4 byte integer

 	wav->header.length_of_fmt = LITTLE_TO_BIG_ENDIAN(buffer4);
/*********************************************************************************************************************************/
	
 	printf("(17-20) Length of Fmt header: %u \n", wav->header.length_of_fmt);
 	read = fread(buffer2, sizeof(buffer2), 1, ptrin); printf("%u %u \n", buffer2[0], buffer2[1]);

 	wav->header.format_type = buffer2[0] | (buffer2[1] << 8);

 	char format_name[10] = "";

 	if (wav->header.format_type == 1)
   		strcpy(format_name,"PCM"); 
 	else if (wav->header.format_type == 6)
  		strcpy(format_name, "A-law");
 	else if (wav->header.format_type == 7)
  		strcpy(format_name, "Mu-law");

 	printf("(21-22) Format type: %u %s \n", wav->header.format_type, format_name);
 	read = fread(buffer2, sizeof(buffer2), 1, ptrin);

 	printf("%u %u \n", buffer2[0], buffer2[1]);

 	wav->header.channels = buffer2[0] | (buffer2[1] << 8);
 	printf("(23-24) Channels: %u \n", wav->header.channels);

 	read = fread(buffer4, sizeof(buffer4), 1, ptrin);
 	printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
	
 	wav->header.sample_rate = buffer4[0] |
						(buffer4[1] << 8) |
						(buffer4[2] << 16) |
						(buffer4[3] << 24);

 	printf("(25-28) Sample rate: %u\n", wav->header.sample_rate);

 	read = fread(buffer4, sizeof(buffer4), 1, ptrin);
 	printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);

 	wav->header.byterate  = buffer4[0] |
						(buffer4[1] << 8) |
						(buffer4[2] << 16) |
						(buffer4[3] << 24);
 	printf("(29-32) Byte Rate: %u , Bit Rate:%u\n", wav->header.byterate, wav->header.byterate*8);

 	read = fread(buffer2, sizeof(buffer2), 1, ptrin);
 	printf("%u %u \n", buffer2[0], buffer2[1]);

	 wav->header.block_align = buffer2[0] |
					(buffer2[1] << 8);
 	printf("(33-34) Block Alignment: %u \n", wav->header.block_align);

 	read = fread(buffer2, sizeof(buffer2), 1, ptrin);
 	printf("%u %u \n", buffer2[0], buffer2[1]);

 	wav->header.bits_per_sample = buffer2[0] |
					(buffer2[1] << 8);
 	printf("(35-36) Bits per sample: %u \n", wav->header.bits_per_sample);

	read = fread(wav->header.data_chunk_header, sizeof(wav->header.data_chunk_header), 1, ptrin);
 	printf("(37-40) Data Marker: %s \n", wav->header.data_chunk_header);

 	read = fread(buffer4, sizeof(buffer4), 1, ptrin);
 	printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);

 	wav->header.data_size = buffer4[0] |
				(buffer4[1] << 8) |
				(buffer4[2] << 16) | 
				(buffer4[3] << 24 );
 	printf("(41-44) Size of data chunk: %u \n\n", wav->header.data_size);


 	// calculate no.of samples
 	long num_samples = (8 * wav->header.data_size) / (wav->header.channels * wav->header.bits_per_sample);
 	printf("Number of samples:%lu \n", num_samples);

 	long size_of_each_sample = (wav->header.channels * wav->header.bits_per_sample) / 8; // kinda byte
 	printf("Size of each sample:%ld bytes\n", size_of_each_sample);

 	// calculate duration of file
 	float duration_in_seconds = (float) wav->header.overall_size / wav->header.byterate;
 	printf("Approx.Duration in seconds=%f\n", duration_in_seconds);
 	printf("Approx.Duration in h:m:s=%s\n", seconds_to_time(duration_in_seconds));
 	getcwd(cwd,sizeof(cwd));
 	strcat(cwd,"\\");
	strcat(cwd, outputfilename);
	for(i=0; i<= sizeof(cwd);i++)
	{
		if(cwd[i]== '\\')
		{
			cwd[i]='/';
		}
		
	}
	printf("%s\n", cwd);
 	 // read each sample from data chunk if PCM
 	if (wav->header.format_type == 1)//PCM
	 { 
		long i =0;
		char data_buffer[size_of_each_sample];
		int  size_is_correct = TRUE;
		// make sure that the bytes-per-sample is completely divisible by num.of channels
		long bytes_in_each_channel = (size_of_each_sample / wav->header.channels);
		if ((bytes_in_each_channel  * wav->header.channels) != size_of_each_sample)
		{
			printf("Error: %ld x %ud <> %ldn", bytes_in_each_channel, wav->header.channels, size_of_each_sample);
			size_is_correct = FALSE;
		}
		if (size_is_correct)
		{ 
			// the valid amplitude range for values based on the bits per sample
			long low_limit = 0l;
			long high_limit = 0l;
			switch (wav->header.bits_per_sample)
			{
					case 8:
					low_limit = -128;
					high_limit = 127;
					break;
					case 16:
					low_limit = -32768;
					high_limit = 32767;
					break;
					case 32:
					low_limit = -2147483648;
					high_limit = 2147483647;
					break;
				}
								
			printf("\n\n.Valid range for data values : %ld to %ld \n", low_limit, high_limit);
			ptrout=fopen(cwd,"wb");
			for (i =1; i <= num_samples; i++)
			{
				read = fread(data_buffer, sizeof(data_buffer), 1, ptrin);
				if (read == 1)
				{
					// dump the data read
					unsigned int  xchannels = 0;
					int data_in_channel = 0;
					int offset = 0; // move the offset for every iteration in the loop below
					for (xchannels = 0; xchannels < wav->header.channels; xchannels ++ )
					{

						// convert data from little endian to big endian based on bytes in each channel sample
						if (bytes_in_each_channel == 4)
						{
							data_in_channel = (data_buffer[offset] & 0x00ff) | 
												((data_buffer[offset + 1] & 0x00ff) <<8) | 
												((data_buffer[offset + 2] & 0x00ff) <<16) | 
												(data_buffer[offset + 3]<<24);
						}
						else if (bytes_in_each_channel == 2)
						{
							data_in_channel = (data_buffer[offset] & 0x00ff) |
												(data_buffer[offset + 1] << 8);
						}
						else if (bytes_in_each_channel == 1)
						{
							data_in_channel = data_buffer[offset] & 0x00ff;
							data_in_channel -= 128; //in wave, 8-bit are unsigned, so shifting to signed
						}
						offset += bytes_in_each_channel;
						if(txsize>0)
						{
//							unsigned short int tempData=0;
//							if((saveAs== UNSIGNED)&& (data_in_channel<0))
//							{
//								tempData= data_in_channel;						
//								fprintf(ptrout,"%9hu ",tempData);
//							}
//							else
							fprintf(ptrout,"%9hu, ",data_in_channel);
							//printf("%d\n",data_in_channel);
							txsize--;
						}
						rawsize+=1;
						if ((rawsize % 10) == 0)
						{
							//printf("\n");
							fprintf(ptrout,"\n");
						}					
					}

				}

			} 
		} 
 } 

 printf("Closing file..\n");
 fclose(ptrin);
 fclose(ptrout);
 free(wav);
}






/**
 * Convert seconds into hh:mm:ss format
 * Params:
 *	seconds - seconds value
 * Returns: hms - formatted string
 **/
 char* seconds_to_time(float raw_seconds){
  	char *hms;
  	int hours, hours_residue, minutes, seconds, milliseconds;
  	hms = (char*) malloc(100);

  	sprintf(hms, "%f", raw_seconds);

  	hours = (int) raw_seconds/3600;
  	hours_residue = (int) raw_seconds % 3600;
  	minutes = hours_residue/60;
  	seconds = hours_residue % 60;
  	milliseconds = 0;

  	// get the decimal part of raw_seconds to get milliseconds
  	char *pos;
  	pos = strchr(hms, '.');
  	int ipos = (int) (pos - hms);
  	char decimalpart[15];
  	memset(decimalpart, ' ', sizeof(decimalpart));
  	strncpy(decimalpart, &hms[ipos+1], 3);
  	milliseconds = atoi(decimalpart);	

  
  	sprintf(hms, "%d:%d:%d.%d", hours, minutes, seconds, milliseconds);
  	return hms;
}

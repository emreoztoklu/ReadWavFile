#include <stdio.h>
#include <string.h>
#include "main.h"
#include "wav.h"
#include "typedef.h"

#define SIZE_OF_SPEED 	(600)
#define IGNITON_BLOCK	(65535)
#define PLAYBACK_SIZE 	(8192)
#define SCAN_SIZE 	  	(1000)  // Sesteki ana frekans 76hz civar� bu da 580 sample eder 580*2 den b�y�k bir tarama aral�g� koydum
#define SPEED_SHIFT	 	(620)   //sesin  �rnek verirsek 10.1 km/h den 10.2 km/h ye ge�ti�indeki kayma miktar�
#define UPWARD 			(1)
#define DOWNWARD		(0)
#define SMALL_SCAN		100
uint32_t arr_indexofClosestTranstion[SIZE_OF_SPEED+1][2];
int32_t temp_SumMin=0;
uint32_t recordedA=0;
uint32_t recordedB=0;

/* Closest transition fonksiyonu her h�z�n i�in
ayr�lan 8192 size l�k arrayda ilk ba�tan ve son ba�tan 1000
eleman� tarar, her eleman�n kendinden �nceki ve kendinden sonraki elemanlar�n� kontrol eder.
kendinden �nceki negatif kendinden sonraki pozitifse bu bunlar� toplar ve bir sonu� elde eder
bu sonu�lardan en k���k olan� bize  negatif side'dan pozitif side'a en k�sa ge�i� noktas�n� verir
en k�sa ge�isi almam�z�n sebebi clickleri bast�rmakt�r. */
/*void closestTranstion(const char *inputfilename, uint32_t convert_data_size)
{
	FILE *stream;
	int16_t temp_Arr[convert_data_size];
	memset(temp_Arr,0,convert_data_size);
	stream= fopen(inputfilename,"rb");
	for(uint32_t i=0; i<convert_data_size;i++)
	{
		fscanf(stream,"%9d ",&temp_Arr[i]);
		//printf("%d",temp_Arr[i]);
	}
	fclose(stream);
	for(int speed=0; speed<=SIZE_OF_SPEED; speed++)
	{
		temp_SumMin= temp_Arr[speed*SPEED_SHIFT+IGNITON_BLOCK-1]+ temp_Arr[speed*SPEED_SHIFT+IGNITON_BLOCK+1];//ilk h�z array�n�n ilk eleman�n� minimum farkl� s�f�r ge�i� 																																// kabul ettik
		arr_indexofClosestTranstion[speed][0] = speed*SPEED_SHIFT+IGNITON_BLOCK;//Yukarda kabul etti�imiz eleman�n indisini tuttuk.
		for(uint32_t curr_indexA=(speed*SPEED_SHIFT+IGNITON_BLOCK); curr_indexA< (speed*SPEED_SHIFT+IGNITON_BLOCK+SCAN_SIZE); curr_indexA++)
		{
			if((temp_Arr[curr_indexA-1]<0)&&(temp_Arr[curr_indexA+1]>0))
			{
				if(temp_SumMin>((temp_Arr[curr_indexA-1]<0)+(temp_Arr[curr_indexA+1]>0)))
				{
					temp_SumMin=(temp_Arr[curr_indexA-1]+ temp_Arr[curr_indexA+1]);
					arr_indexofClosestTranstion[speed][0]=curr_indexA;
					recordedA=curr_indexA;
				}
			}	
		}
		printf("arr_indexofClosestTranstion[%d][0]=%d\n",speed,recordedA);
		temp_SumMin=temp_Arr[((speed)*SPEED_SHIFT+PLAYBACK_SIZE-1 + IGNITON_BLOCK)-1]+temp_Arr[((speed)*SPEED_SHIFT+PLAYBACK_SIZE-1 + IGNITON_BLOCK)+1];
		arr_indexofClosestTranstion[speed][1] = (speed)*SPEED_SHIFT+PLAYBACK_SIZE-1 + IGNITON_BLOCK;
		for(uint32_t curr_indexB =((speed)*SPEED_SHIFT+PLAYBACK_SIZE-1 + IGNITON_BLOCK); curr_indexB> ((speed)*SPEED_SHIFT+PLAYBACK_SIZE-1 + IGNITON_BLOCK-(SCAN_SIZE+1)); curr_indexB--)
		{			
			if((temp_Arr[curr_indexB-1]<0)&&(temp_Arr[curr_indexB+1]>0))
			{
				if(temp_SumMin>((temp_Arr[curr_indexB-1]<0)+(temp_Arr[curr_indexB+1]>0)))
				{
					temp_SumMin=(temp_Arr[curr_indexB-1]+ temp_Arr[curr_indexB+1]);
					arr_indexofClosestTranstion[speed][1]=curr_indexB;					
					recordedB=curr_indexB;
				}
			}	
		}
		printf("arr_indexofClosestTranstion[%d][1]=%d\n",speed,recordedB);
		//printf ("%dth speed_size=%d\n", speed,recordedB-recordedA);
	}
	
}
/* s�f�r� bulma
fonksiyonu her h�z i�in ayr�lan 8192 size l�k array�n ba�tan ve sondan 1000 eleman�n� tarar
ve s�f�r ge�i�leri bulur, her buldugu s�f�r ge�isin kendinden �nceki ilk 100 eleman�n� toplar ve ortalamas�n�
al�r eger sonuc s�f�r dan kucukse buldugu s�f�r negatif taraftan artarak gelen s�f�rd�r.
buldugunda bayrag� kald�r�r ve taramaya devam edilmez.*/

/*uint32_t arr_indexofzeros[SIZE_OF_SPEED+1][2];
void zeroTransitions(const char *inputfilename, uint32_t converted_data_size)
{
	FILE *stream;
	int16_t temp_Arr[converted_data_size];
	memset(temp_Arr,0,converted_data_size);
	stream= fopen(inputfilename,"rb");
	for(uint32_t i=0; i<converted_data_size;i++)
	{
		fscanf(stream,"%9d ",&temp_Arr[i]);
		//printf("%d\n",temp_Arr[i]);
	}
	fclose(stream);
	for(int speed=0; speed<=SIZE_OF_SPEED; speed++)
	{
		arr_indexofzeros[speed][0] = speed*SPEED_SHIFT+IGNITON_BLOCK;//ilk eleman�n olma ihtimaline kar��		
		uint8_t is_first_zero_found=0;
		for(uint32_t curr_indexA=(speed*SPEED_SHIFT+IGNITON_BLOCK);curr_indexA<((speed*SPEED_SHIFT)+IGNITON_BLOCK+SCAN_SIZE); curr_indexA++)
		{
			int32_t temp_Sum=0;		
			if((temp_Arr[curr_indexA]==0)&&(is_first_zero_found==0))// sampling rate'den dolay� �rnekleme s�f�ra denk gelmezse ve bulamazsak nap�caz ?
			{
				
				for(int j=SMALL_SCAN;j>0;j--)
				{
					temp_Sum+=temp_Arr[curr_indexA-j];
				}
				if((temp_Sum/SMALL_SCAN)<0)//demek oluyor ki negatif side'dan artarak gelen s�f�r.
				{
					arr_indexofzeros[speed][0]= curr_indexA; //adresi kaydettik
					recordedA=curr_indexA;
					is_first_zero_found=1;
				}	
				
			}		
		}
		printf("arr_indexofzeros[%d][0]=%d\n",speed,recordedA);
		arr_indexofzeros[speed][1] = (speed)*SPEED_SHIFT+PLAYBACK_SIZE-1 + IGNITON_BLOCK;//son eleman�n olma ihtimaline kar��
		is_first_zero_found=0;
		for(uint32_t curr_indexB =((speed)*SPEED_SHIFT+PLAYBACK_SIZE-1 + IGNITON_BLOCK);curr_indexB>((speed)*SPEED_SHIFT+PLAYBACK_SIZE+IGNITON_BLOCK-1001); curr_indexB--)
		{
			int32_t temp_Sum=0;
			if((temp_Arr[curr_indexB]==0)&&(is_first_zero_found==0)) // sampling rate'den dolay� �rnekleme s�f�ra denk gelmezse ve bulamazsak nap�caz ?
			{
				
				for(int k=SMALL_SCAN;k>0;k--)
				{
					temp_Sum+=temp_Arr[curr_indexB-k];
				}
				if((temp_Sum/SMALL_SCAN)<0)
				{
					//demek oluyor ki negatif side'dan artarak gelen s�f�r.
					arr_indexofzeros[speed][1]= curr_indexB; //adresi kaydettik
					recordedB=curr_indexB;
					is_first_zero_found=1;
				}					
			}
		}
		printf("arr_indexofzeros[%d][1]=%d\n",speed,recordedB);
	}
	
}*/
void binaryConverter( const char *inputfilename, const char *outputfilename, int txsize)
{
	FILE *Fin;
	FILE *Fout;
 	Fin= fopen(inputfilename, "rb");
 	Fout=fopen(outputfilename,"wb");
 	unsigned short int data[txsize];
	long int  i=0;
	do
	{
		fscanf(Fin,"%9hu,",&data[i]);
		printf("%hu\n",data[i]);
		fwrite(&data[i], sizeof(unsigned short int),1, Fout);
		i++;
	}while(!feof(Fin));
 fclose(Fin);  
 fclose(Fout);	
}
#define LAST_SPEED 		(60*10)
#define IGNITION_AREA	65535
unsigned short int* dizi[LAST_SPEED+1][2];
unsigned short int 	highest_value_of_soundpackage=0;
unsigned short int* wavfileBeginPtr;
unsigned short int* wavFileEndOfIgnitionPtr;
unsigned int y=0;
void find_highestindex_of_soundpackages(const char *inputfilename, const char *outputfilename, int txsize)
{
	FILE *Fin;
	FILE *Fout;
	int rawsize=0;
 	Fin= fopen(inputfilename, "rb");
 	Fout=fopen(outputfilename,"wb");
 	unsigned short int data[txsize];
	long int  i=0;
	do
	{
		fscanf(Fin,"%9hu,",&data[i]);
		i++;
	}while(!feof(Fin));
	wavfileBeginPtr= &data[0];
	fprintf(Fout,"{ ");
	for(unsigned short int speed=0;speed<=LAST_SPEED; speed++)
	{
		highest_value_of_soundpackage= 0;
		dizi[speed][0] = wavFileEndOfIgnitionPtr+(speed*620);
		
		for(y=(speed*620+IGNITION_AREA);y<((speed*620)+1000+IGNITION_AREA); y++)
		{
			if((highest_value_of_soundpackage < *(wavfileBeginPtr+y+1))&&(*(wavfileBeginPtr+y+1)<32768))
			{
				highest_value_of_soundpackage = *(wavfileBeginPtr+y+1);
				dizi[speed][0] = wavfileBeginPtr+y+1;
			}
		 }
		fprintf(Fout,"%x, ",(0x080186A0+sizeof(unsigned short int)*(y+1)));
		rawsize+=1;
		if ((rawsize % 6)== 0)
		{
			fprintf(Fout,"\n");
		}
	}
	fprintf(Fout," }");
//	fprintf(Fout,"{ ");
//	for(unsigned short int speed=0;speed<=LAST_SPEED; speed++)
//	{
//	   highest_value_of_soundpackage= 0;
//	   dizi[speed][1] = wavFileEndOfIgnitionPtr+ (speed)*620+8192-1;
//	  for(unsigned int z=((speed)*620+8192-1 + IGNITION_AREA);z>((speed)*620+8192-1001+IGNITION_AREA); z--)
//	  {
//		  if((highest_value_of_soundpackage < *(wavfileBeginPtr+z-1))&&(*(wavfileBeginPtr+z-1)<32768))
//		  {
//			  highest_value_of_soundpackage = *(wavfileBeginPtr+z-1);
//			  dizi[speed][1] = wavfileBeginPtr+z-1;
//			  fprintf(Fout,"0x080186A0+%u, ",(z-1));
//		  }
//	  }
//	}
//	fprintf(Fout," }");
 	fclose(Fin);  
 	fclose(Fout);		
}
int main(void)
{
	uint32_t size=450000;//size of data to be extracted from source wav data. // in terms of uint16_t//450k adet ve 900kB l�k data data
	wavReader("src/sample.wav", "src/sample.txt", size);
	binaryConverter("src/sample.txt","src/sample.bin",size);
	//find_highestindex_of_soundpackages("sample.txt","sample_arr.txt",size);
	//zeroTransitions("sample.txt",size);
	//closestTranstion("sample.txt",size);
	return 0;
}



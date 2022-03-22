#include <stdio.h>

FILE *Fin;
FILE *Fout;




int main()
{
	Fin= fopen("audioSample.txt","rb");
	Fout=fopen("audioSample.bin","wb");
	unsigned short int data[450000];
	long int  i=0;
//	for(unsigned int i=0; i<450000;i++)
//	{
//		fscanf(Fin,"%hu,",&data[i]);
//		printf("%hu\n",data[i]);
//		fwrite(&data[i], sizeof(unsigned short int),1, Fout);		
//	}
	do{
		fscanf(Fin,"%9hu,",&data[i]);
		printf("%hu\n",data[i]);
		fwrite(&data[i], sizeof(unsigned short int),1, Fout);
		i++;
	}while(!feof(Fin));
 fclose(Fin);  
 fclose(Fout);


}


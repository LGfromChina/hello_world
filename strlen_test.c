/*#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
 unsigned int a1=0;
 int 	      a2=0;
 char testval[]="abfcdjdjeudh";
 char testval2[]="dffgegegddgdgegdgdgeg";
 a1=strlen(testval)-13;
 a2=strlen(testval)-10;
 printf("%d\n%d\n",a1,a2);
 return 0;
}*/
#include <cstdlib> 
#include <iostream> 
 
using namespace std; 
 
 
 
#define N 8 
#define M 1<<(N-1) 
 
#define ERR     (0) 
#define OK      (1) 
 
 
 
#define LOG_ERR(s)\ 
{\ 
    printf("line[%d], %s\n", __LINE__, s);\ 
    system("pause");\ 
} 
 
typedef unsigned int UL; 
typedef unsigned short US; 
typedef unsigned char UC; 
 
 
 
char inputFileName[] = "test.jpg"; 
char outputFileName[] = "output.txt"; 
 
char binaryBuff[N + 1]; 
 
 
void toBinaryString(UL num)     
{        
    int i;     
 
    memset(binaryBuff, 0, N + 1); 
     
    for (i=0; i<N ;i++)     
    { 
        if ((num&M)==0) 
        { 
            strcat(binaryBuff, "0"); 
        } 
        else 
        { 
            strcat(binaryBuff, "1"); 
        } 
         
        num<<=1;     
    }        
} 
 
 
 
 
int main() 
{ 
    FILE *inputFile = NULL; 
    FILE *outputFile = NULL; 
 
    char c = 0; 
 
 
    inputFile = fopen(inputFileName, "rb"); 
    if (NULL == inputFile) 
    { 
        LOG_ERR("call fopen(inputFileName, \"rb\") failed!"); 
        return 0; 
    } 
 
    outputFile = fopen(outputFileName, "wb"); 
    if (NULL == inputFile) 
    { 
        LOG_ERR("call fopen(outputFileName, \"wb\") failed!"); 
        return 0; 
    } 
 
    while(c = fgetc(inputFile)) 
    { 
        if (feof(inputFile)) 
        { 
            break; 
        } 
 
        toBinaryString(c); 
 
        fprintf(outputFile, "%s\n", binaryBuff); 
    } 
     
    fflush(outputFile); 
 
    fclose(inputFile); 
    fclose(outputFile); 
     
    //system("pause"); 
    return 0; 
} 

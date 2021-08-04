#include<stdio.h>
#include<stdlib.h>
void Bubble_Sort(int arr[],int len)
	{
		int i,j,tmp;
		i  = 0;
		j  = 0;
		tmp= 0;
		for(i=0;i<len-1;i++)
			{
				for(j=i+1;j<len;j++)
					{
					
					 if(arr[i]>arr[j])
						{		 						  	 							   tmp    = arr[i];
						   arr[i] = arr[j];
						   arr[j] = tmp;								
						}
					}
							
			
			}
	}


 



  

int main()
{	
	int arr_scanf[10]={0};
	for (int i=0;i<10;i++)
	{
		scanf("%d",&arr_scanf[i]);	
	}
	Bubble_Sort(arr_scanf,10);
	for(int i= 0 ;i<10;i++)
	{
	printf("%d\n",arr_scanf[i]);	
	}	

	return 0;
}

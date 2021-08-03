#include<stdio.h>
#include<stdlib.h>
void  convert_to_bin(int int_num)
{
	
	
			int i  = 0;  // 取余
			//int k  = 1;
			int arr_val[100]={0}; //存放二进制的数组
			int count_bit=0; //二进制位数
			int flag_add=1;//进位标志，初始值为1， 即负数补码最后加1
			int count_add=0; //将位数补充到4的倍数
			int result_bit=0; //二进制相加的位数结果
	if(int_num>0)
   	 {	//将参数 取余取整
			while(int_num)
			{
					   i = int_num%2;
			  arr_val[count_bit] = i;
				    int_num/=2;
			  	     count_bit++;	
			}

		count_add=count_bit % 4;

			if(count_add==0)

			{
			count_add= count_bit;
				for(int j= count_add-1;j>=0;--j)
		 		
				{

				  printf("%d",arr_val[j]);
				}
			}

		 	else if (count_add!=0)
			{
			count_add= count_bit+4-(count_bit%4);
			for(int j= count_add-1;j>=count_bit;--j)
				{
				arr_val[j]=0;  //bu 0
				printf("%d",arr_val[j]);
				}
			for(int j= count_bit-1;j>=0;--j)
				{
				 printf("%d",arr_val[j]);
				
				}
			}
 	
     
   	  }

	//负数取余取整补码
 	else if(int_num<0)
  	 { 
				int_num=-int_num;
				while(int_num)
				{
				i=int_num%2;
				arr_val[count_bit]=i;
			  	int_num/=2;
				count_bit++;	
				}
			

   
				count_add=count_bit % 4;

		if(count_add==0)
		{
			count_add= count_bit;
			for(int j= count_add-1;j>=0;--j)

	 		//取反
			{
				if (arr_val[j]==0)
				{
				arr_val[j]=1;
		
				}
				else if(arr_val[j]==1)
				{
				arr_val[j]=0;
				}

			  printf("%d",arr_val[j]); //watch state
			}

		}
		//位数不足四的倍数 补位
		else if (count_add!=0)
		{
			count_add= count_bit+4-(count_bit%4);
			for(int j= count_add-1;j>=count_bit;--j)

				{
				arr_val[j]=1;  //bu 0
				}
	
			for(int j= count_bit-1;j>=0;--j)
				
				{
					if (arr_val[j]==0)
					{
					arr_val[j]=1;
					}
			
					else if(arr_val[j]==1)
					{
					arr_val[j]=0;
					}
				
				}

 	
		
	
		}

		
	 			//加1
				for(int j= 0;j<=count_add-1;++j)
				{

		 		 	 result_bit = arr_val[j]+flag_add;
			
		 			switch(result_bit)
					{
					case 0:  // 未产生进位
					   flag_add=0;
			     	 	   arr_val[j]=0;
			
					   break;
					case 1:  // 未产生进位
					   flag_add=0;
			     	 	   arr_val[j]=1;
			
					   break;
					case 2:   //产生进位
					   flag_add=1;
			     	 	   arr_val[j]=0;
					   break;
					default:printf("err:no value match");
					}
		 			 
					printf("%d",arr_val[count_add-1-j]);

				}


  	 }
}

int main()
{
		int int_num = 0;
		int int_bin = 0;
		scanf("%d",&int_num);
		convert_to_bin(int_num);	
		return 0;

	
}


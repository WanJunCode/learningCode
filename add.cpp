#include <stdio.h>
#include <cstdlib>
int add(int num1, int num2){
	while(num2!=0){
		int sum=num1^num2;
		printf("sum = [%d]\n",sum);
		num2=(num1&num2)<<1;
		printf("num2 = [%d]\n",num2);
		num1=sum;
	}
	return num1;
}

int main(int argc,char**argv){
	int a=atoi(argv[1]);
	int b=atoi(argv[2]);
	int r=add(a,b);
	printf("r = [%d]\n",r);
	return 1;
}

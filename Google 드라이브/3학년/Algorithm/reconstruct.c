#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int **temp;

int LCS(const char* x, const char* y, int m, int n);
void print_lcs(const char *x, const char *y, int i, int j, char* lcs_buf);

int main(int argc, const char *argv[]){
	int i=strlen(argv[1]),j=strlen(argv[2]);
	int lcs_length, k, l;
	char* lcs_buf;
	
	temp = (int**)malloc(sizeof(int*)*(i+1));
	for(k=0;k<=i;k++){
		*(temp+k) = (int*)malloc(sizeof(int)*(j+1));
		for(l=0;l<=k;l++){
			temp[k][l] = 0xffffffff;
		}
	}
	if(i>j)
		lcs_buf = (char*)malloc(sizeof(char)*i);
	else
		lcs_buf = (char*)malloc(sizeof(char)*j);


	lcs_length = LCS(argv[1],argv[2],i,j);
	printf("A: %s\nB: %s\n\n",argv[1],argv[2]);
	printf("LCS Lenght is %d\n\n",lcs_length);

	for(l=0;l<=j;l++){
		for(k=0;k<=i;k++){
			printf("%d	",temp[k][l]);
		}
		printf("\n");
	}

	print_lcs(argv[1],argv[2],i,j,lcs_buf);
	printf("\nLSC is %s\n",lcs_buf);
	free(lcs_buf);

	for(k=0;k<i;k++){
		free(*(temp+k));
	}
	free(temp);


}

int LCS(const char* x, const char* y, int m, int n){
	int i,j;
	for (i=0;i<=m;i++){
	    for (j=0;j<=n;j++){
		if(i==0||j==0){
			temp[i][j] = 0;
		}
		else if(x[i-1] == y[j-1]){
			temp[i][j] = 1 + temp[i-1][j-1];
		}
		else{
			if(temp[i-1][j]>temp[i][j-1])
				temp[i][j] = temp[i-1][j];
			else
				temp[i][j] = temp[i][j-1];
		}
	    }
	}
	return temp[m][n];
}


void print_lcs(const char *x, const char *y, int i, int j, char* lcs_buf){
	if(i==0 || j==0){
		return;
	}
	else if(x[i-1]==y[j-1]){
		lcs_buf[temp[i][j]-1]=x[i-1];
		print_lcs(x,y,i-1,j-1,lcs_buf);
	}
	else if(temp[i-1][j]>=temp[i][j-1]){
		print_lcs(x,y,i-1,j,lcs_buf);
	}
	else
		print_lcs(x,y,i,j-1,lcs_buf);
}


#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>


int main(int argc, char *argv[]) {
	int input, output;
	int lsk, pagesize, temp;
	void *source, *target;
	size_t filesize;

	struct timespec begin, end;
	clock_gettime(0, &begin);

	if(argc != 3){
	 	printf("Arguments are too little or too much\n");
		exit(1);
	}

	if((input = open(argv[1], O_RDONLY,0666)) == -1){
		printf("%s open error\n",argv[1]);
		exit(1);
	}

	if((output = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, 0666)) == -1){
		printf("%s open error\n",argv[1]);
		exit(1);
	}


	filesize = lseek(input, 0, SEEK_END);
	if(filesize<=0){
		printf("%s hasn't any contents\n",argv[1]);
		exit(1);
	}

	pagesize =getpagesize();

	printf("pagesize %d\n",pagesize);
	lsk = lseek(output, filesize - 1, SEEK_SET);
	write(output, "\0", 1);

	source = mmap(0, filesize, PROT_READ, MAP_SHARED, input, 0);
	target = mmap(0, filesize, PROT_WRITE, MAP_SHARED, output, 0);

	memcpy(target, source, filesize);
	msync(target,pagesize,MS_ASYNC);
	munmap(source, pagesize);
	munmap(target, pagesize);




	close(input);
	close(output);
	
	clock_gettime(0, &end);

	printf("total time : %.8f\n",(float)(end.tv_sec - begin.tv_sec) + (float)(end.tv_nsec - begin.tv_nsec)/1000000000);
	return 0;
}


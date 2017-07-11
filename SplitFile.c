/*
Name: Rohit Pawar
Class: CS 590
Assignment : Program 1
Description: Program takes filename as an input and checks if the file size is greater than 10000 bytes program will split the file into multiple files of size 10000 bytes.
*/

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#define BUFFSIZE 10000
#define EXTCHAR "."
void main(int argc, char *argv[])
{
	int fileDes;
	int fileSize, originalFileSize;
	char buffer[BUFFSIZE];
	char *fileName = NULL;
	if(argc != 2)
	{// file name parameter missing
		printf("\nPlease provide FileName as first argument\n");
		return;
	}
	else
	{// argc == 2
		char originalFileName[strlen(argv[1])+1];
		strncpy(originalFileName,argv[1],strlen(argv[1]));
		fileName = strstr(argv[1],EXTCHAR);
		if((fileDes = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0)
		{
			printf("\nfcntl error for fd %d\n", atoi(argv[1]));
			return;
		}
		if((fileDes = open(argv[1], O_RDONLY))<0)
		{// File open error
			printf("\nFile \"%s\" does not exist or could not be opened. \n",argv[1]);
			return;
		}
		*fileName = '\0'; // Remove file extension from argv[1]
		originalFileSize = fileSize = lseek(fileDes ,0, SEEK_END); // Get the file size using lseek and file descriptor
		printf("\nProcessing File \"%s\" of size %d bytes.\n",originalFileName,fileSize); 
		lseek(fileDes,0,SEEK_SET);// Set FD back to starting position
		int splitFileCount = 1;
		int positionFromSet = 0;
		int readBytes = 0;
		char splitFileName[100];
		while(fileSize > 10000) //FileSize > 10000 bytes 
		{//Loop to split files in 10K chunks
			sprintf(splitFileName,"%s.%d",argv[1],splitFileCount);//Create a new split file name
			int splitFileDes = open(splitFileName,O_WRONLY | O_CREAT | O_TRUNC); // Create file if not exists or truncate and open if exists
			if(splitFileDes < 0)
			{//Open/Create Error
				printf("\nError in open/create file %s\n",splitFileName);
			}
			int readBytes = read(fileDes,buffer, BUFFSIZE);// Read 10K bytes from file descriptor
			if(readBytes < 10000)
			{// Read Error
				printf("\nError reading 10K bytes from file\n");
				return;
			}
			int bytesWritten = write(splitFileDes,buffer,BUFFSIZE); //Write 10K bytes to split file descriptor.
			if(bytesWritten != BUFFSIZE)
			{// Write failed
				printf("\nCouldn't write 10K bytes to file \"%s\"\n",splitFileName);
				return;
			}
			positionFromSet = readBytes*splitFileCount;
			fileSize = fileSize - readBytes;
			lseek(fileDes,positionFromSet,SEEK_SET); //Seek the file descriptor to start from last read position
			splitFileCount++;
			close(splitFileDes);
			
		} 
		if(fileSize > 0)
		{// Last split file creation
			sprintf(splitFileName,"%s.%d",argv[1],splitFileCount);//Create a new split file name
			int splitFileDes = open(splitFileName,O_WRONLY | O_CREAT | O_TRUNC);
			readBytes = read(fileDes,buffer,fileSize); // Read reamaining bytes.
			if(readBytes < 0)
			{//Read Error
				printf("\nError reading %d bytes from file\n",fileSize);
				return;
			}
			int bytesWritten = write(splitFileDes,buffer,fileSize); //Write remaining bytes.
			if(bytesWritten != fileSize)
			{// Write failed
				printf("\nCouldn't write %d bytes to file \"%s\"\n",readBytes,splitFileName);
				return;
			}
			close(splitFileDes);
		}
		
		printf("\nSuccessfully split file \"%s\"(%d bytes) to %d files ( <= 10000 bytes each)\n\n",originalFileName, originalFileSize,splitFileCount);
	}
}

#include <cstring>
#include <cstdio>   
#include <cstdlib> 
#include <cctype>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

void printUsage() {
    printf("\nUsage:\n");
    printf("  Flag 0: ./a.out <input_file> 0 <block_size>\n");
    printf("  Flag 1: ./a.out <input_file> 1\n");
    printf("  Flag 2: ./a.out <input_file> 2 <start_index> <end_index>\n\n");
}

long long validateInputFileAndGetFileSize(const char *filename) {
    // Try to open the file in read-only mode
    int fd=open(filename, O_RDONLY);
    if(fd==-1){
        if(errno==ENOENT){
            fprintf(stderr,"Error: Input file '%s' does not exist.\n",filename);
        }else if(errno==EACCES){
            fprintf(stderr, "Error: No read permission for '%s'.\n",filename);
        }else{
            perror("Error opening file");
        }
        return -1;
    }
    struct stat st;
    stat(filename, &st);
    if(S_ISDIR(st.st_mode)) 
    {
        fprintf(stderr, "Error: '%s' is a directory, not a file.\n", filename);
        return -1;
    }

    // Get file size
    long long size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
        printf("Error getting file size");
        close(fd);
        return -1;
    }
    // Empty file check
    if (size == 0) {
        fprintf(stderr, "Error: Input file is empty.\n");
        close(fd);
        return -1;
    }
    close(fd);
    return size; // Returns file size if valid
}

int makeDirectory()
{
    if (mkdir("Assignment1", 0700) == -1) {
        if (errno != EEXIST) { // Ignore if already exists
            perror("Error: mkdir");
            return -1;
        }
    }
    return 0;
}

int  makeOutputFile(int flag, const char* input_file_name, int fdin)
{
    char output_path[512];
    snprintf(output_path, sizeof(output_path), "Assignment1/%d_%s",flag, input_file_name);
    int fdOutputFile = open(output_path, O_CREAT | O_WRONLY | O_TRUNC, 0600);
    if (fdOutputFile == -1) { printf("Error in opening output file"); close(fdin); return -1; }
    return fdOutputFile;
}

//displays excution progress in percentage on terminal
void showProgress(long long written, long long total) {
    if (total == 0) return; 
    int percent = (int)((written * 100) / total);
    printf("\rProgress: %d%% completed...", percent);
    fflush(stdout);
}

//Range Reversal 
int reverseFileonIndexBased(const char *input_file_name,int flag,int start_index,int end_index)
{
    int n,fdin,fdout;
    long long bufferSize=4096;
    char buf[4096]; char reversedbuf[4096];
    fdin=open(input_file_name, O_RDONLY);
    if(fdin == -1){ perror("Error in opening input file"); return -1; }

    fdout= makeOutputFile(flag, input_file_name , fdin);
    // fdout=open("target", O_CREAT|O_WRONLY | O_TRUNC, 0642);
    if (fdout == -1) { perror("Error in opening output file"); close(fdin); return -1; }

    long long totalInputFileSize=lseek(fdin, 0, SEEK_END);
    long long lengthOfTextFile=start_index;
    long long noofblocks=lengthOfTextFile/bufferSize;
    long long remainder=lengthOfTextFile % bufferSize;
    long long written = 0;
    //[0-START_INDEX-1]
    if(remainder>0){
        lseek(fdin,noofblocks*bufferSize,SEEK_SET);
        read(fdin,buf,remainder);
        for(int j=0;j<remainder;j++)
        {
            reversedbuf[j]=buf[remainder-j-1];
        } 
        write(fdout,reversedbuf,remainder);
        written+=remainder;
        showProgress(written, totalInputFileSize);
    }

    for(long long i = noofblocks-1; i >= 0; i--) {
        lseek(fdin, i * bufferSize, SEEK_SET);
        if((n=read(fdin,buf,bufferSize)) >0)
        {
            for(int j=0;j<n;j++)
            {
                reversedbuf[j]=buf[n-j-1];
            } 
            write(fdout,reversedbuf,n);
            written+=n;
            showProgress(written, totalInputFileSize);
        }
    }

    //[START_INDEX END_INDEX]-> No reversal
    lengthOfTextFile=end_index-start_index+1;
    noofblocks=lengthOfTextFile/bufferSize;
    remainder =lengthOfTextFile % bufferSize;
    for(long long i = 0; i<noofblocks; i++) {
        lseek(fdin, i*bufferSize+start_index, SEEK_SET);
        if((n=read(fdin,buf,bufferSize)) >0)
        {
            write(fdout,buf,n);
            written+=n;
            showProgress(written, totalInputFileSize);
        }
    }
     if(remainder>0){
        lseek(fdin,start_index+noofblocks*bufferSize,SEEK_SET);
        read(fdin,buf,remainder);
        write(fdout,buf,remainder);
        written+=remainder;
        showProgress(written, totalInputFileSize);
    }

    //[END_INDEX+1, EOF] 
    lengthOfTextFile=totalInputFileSize-end_index-1;
    noofblocks=lengthOfTextFile/bufferSize;
    remainder = lengthOfTextFile % bufferSize;
    if(remainder>0){
        lseek(fdin,end_index+1+noofblocks*bufferSize,SEEK_SET);
        read(fdin,buf,remainder);
        for(int j=0;j<remainder;j++)
        {
            reversedbuf[j]=buf[remainder-j-1];
        } 
        write(fdout,reversedbuf,remainder);
        written+=remainder;
        showProgress(written, totalInputFileSize);
    }
    for(long long i = noofblocks-1; i >= 0; i--) {
        lseek(fdin, end_index+1+i*bufferSize, SEEK_SET);
        if((n=read(fdin,buf,bufferSize)) >0)
        {
            for(int j=0;j<n;j++)
            {
                reversedbuf[j]=buf[n-j-1];
            } 
            write(fdout,reversedbuf,n);
            written+=n;
            showProgress(written, totalInputFileSize);
        }
    }

    close(fdin);
    close(fdout);
    return 1;
}

//function to reverse input-file block by block
int reverseFileBlockWise(const char *input_file_name,int flag, int block_size)
{
    int n,fdin,fdout;
    // char buf[block_size]; char reversedbuf[block_size];
    char *buf = (char *) malloc(block_size);
    char *reversedbuf = (char *) malloc(block_size);
    fdin=open(input_file_name, O_RDONLY);
    if (fdin == -1) { printf("Error in opening input file");
    free(buf);
    free(reversedbuf); 
    return -1; 
    }

    fdout= makeOutputFile(flag, input_file_name , fdin);
    // fdout=open("target", O_CREAT|O_WRONLY | O_TRUNC, 0642);
    if (fdout == -1) { printf("Error in opening output file"); close(fdin);free(buf);
    free(reversedbuf); return -1; }

    long long lengthOfTextFile=lseek(fdin, 0, SEEK_END);
    long long noOfBlocks=lengthOfTextFile/block_size;
    long long remainder = lengthOfTextFile % block_size;
    long long written = 0; 

    for(long long i = 0; i < noOfBlocks; i++) {
        lseek(fdin, i * block_size, SEEK_SET);
        if((n=read(fdin,buf,block_size)) >0)
        {
            for(int j=0;j<n;j++)
            {
                reversedbuf[j]=buf[n-j-1];
            } 
            written+=block_size;
            write(fdout,reversedbuf,n);
            showProgress(written, lengthOfTextFile);
        }
    }
    if(remainder>0){
        lseek(fdin,noOfBlocks*block_size,SEEK_SET);
        read(fdin,buf,remainder);
        for(int j=0;j<remainder;j++)
            {
                reversedbuf[j]=buf[remainder-j-1];
            } 
            written+=remainder;
        write(fdout,reversedbuf,remainder);
        showProgress(written, lengthOfTextFile);
    }
    close(fdin);
    close(fdout);
    free(buf);  
    free(reversedbuf);
    return 0;
}

//function for complete file reversal
int reverseCompleteFile(const char *inputFileName,int flag)
{
    int n,fdin,fdout;
    long long bufferSize=4096;
    char buf[4096]; char reversedbuf[4096];
    fdin=open(inputFileName, O_RDONLY);
    if (fdin == -1) { perror("Error in opening input file"); return -1; }

    fdout= makeOutputFile(flag, inputFileName , fdin);
    // fdout=open("target", O_CREAT|O_WRONLY | O_TRUNC, 0642);
    if (fdout == -1) { perror("Error in opening output file"); return -1; }

    long long totalLengthOfInputFile=lseek(fdin, 0, SEEK_END);
    long long noofblocks=totalLengthOfInputFile / bufferSize;
    long long remainder =totalLengthOfInputFile % bufferSize;
    long long written = 0;

    //remainder shows partially filled buffer. 
    if(remainder>0){
        lseek(fdin,noofblocks*bufferSize,SEEK_SET);
        if((n = read(fdin, buf, remainder)) > 0) {
            for(int j=0; j<n; j++) 
                reversedbuf[j] = buf[n-j-1];
            write(fdout, reversedbuf, n);
            written += remainder;
            showProgress(written, totalLengthOfInputFile);
        }
    }

    for(long long i = noofblocks-1; i >= 0; i--) {
        lseek(fdin, i * bufferSize, SEEK_SET);
        n=read(fdin,buf,bufferSize);
        if(n>0)
        {
            for(int j=0;j<n;j++)
            {
                reversedbuf[j]=buf[n-j-1];
            } 
            write(fdout,reversedbuf,n);
            written+=n;
            showProgress(written,totalLengthOfInputFile);
        }
    }
    close(fdin);
    close(fdout);
    return 0;
}

bool isDigits(const char *s)
{
     for (int i = 0; s[i]; ++i) {
        if (!isdigit(s[i])) return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    long long blockSize;
    long long startIndex;
    long long endIndex;
    
    if(argc<3){
        fprintf(stderr, "Error: Too few arguments.\n"); 
        printUsage();
        return -1;
    }
    //check Flag is number 
    if (!isDigits(argv[2])) {
        fprintf(stderr, "Error: Flag must be a number (0, 1, or 2).\n");
        // printUsage();
        return -1;
    }
    char* inputFileName =argv[1];
    int flag=atoi(argv[2]);
    if(flag!=0 && flag!=1 && flag!=2){ 
        fprintf(stderr, "Error: Invalid flag. Flag must be a number (0, 1, or 2).\n");
        return -1; 
    }
    //tio check input file exist and is with read permission granted.
    long long fileSize=validateInputFileAndGetFileSize(inputFileName);
    if(fileSize==-1){
        return -1;
    }

    if(argc==3){
        //flag==1; 2 arguments i.e filename, flag
        if(flag!=1){ 
            fprintf(stderr,"Error:Wrong arguments are passed\n"); return -1;
        } 
    }
    else if(argc==4){
        //flag==0
        if(flag!=0){printf("Error: Wrong argument are passed\n"); return -1;}
        if (!isDigits(argv[3])) {
            printf("Error: Block Size must be a positive number\n"); 
            return -1;
        }
        blockSize=atoi(argv[3]);  
        if(blockSize<=0){
            printf("Block size must be greater then zero");
            return -1;
        }  
    }
    else if(argc==5){
        //flag==2
        if(flag!=2){fprintf(stderr,"Error: Wrong arguments are passed\n"); return -1;}
        if (!isDigits(argv[3]) || !isDigits(argv[4])) {
            fprintf(stderr, "Error: Start and end indices must be positive numbers.\n");
            return -1;
        }
        startIndex=atoll(argv[3]);
        endIndex=atoll(argv[4]);
         if (startIndex < 0 || endIndex < 0) {
            fprintf(stderr, "Error: Indices cannot be negative.\n");
            return -1;
        }
        if (startIndex > endIndex) {
            fprintf(stderr, "Error: Start index cannot be greater than end index.\n");
            return -1;
        }
        if (endIndex >= fileSize) {
            fprintf(stderr, "Error: End index is out of file bounds.\n");
            return -1;
        }
    }
    else {
       printf("Error\n"); return -1;
    }


    //Calling reversal specific functions based on flag and arguments
    if(argc==3 && flag==1){
        makeDirectory();
        reverseCompleteFile(inputFileName,flag);
    }
    else if(argc==4 && flag==0){
        makeDirectory();
        reverseFileBlockWise(inputFileName,flag,blockSize);
    }
    else if(argc==5 && flag==2){
        makeDirectory();
        reverseFileonIndexBased(inputFileName,flag,startIndex,endIndex);
    }
    else{
       printf("Error\n");
       return -1;
    }

    printf("\nReversal completed successfully!\n");
    return 0;
}



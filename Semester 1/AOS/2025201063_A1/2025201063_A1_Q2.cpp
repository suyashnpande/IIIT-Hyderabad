#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<cstring>
#include<cstdio>   
#include<cstdlib> 
#include<cctype>
#include<cerrno>

bool isDigits(const char *s)
{
    for (int i = 0; s[i]; ++i) {
        if (!isdigit(s[i])) return false;
    }
    return true;
}

bool checkFilePath(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return false; // does not exist
    } 
    return true;
}

//Print Usage Function for user help 
void printUsage() {
    printf("\nUsage:\n");
    printf("./a.out <newfilepath> <oldfilepath> <directory> <flag> [<blockSize>|<start> <end>] \n");
    printf("  For flag 0: 5 arguments (flag 0, block size)\n");
    printf("  For flag 1: 4 arguments\n");
    printf("  For flag 2: 6 arguments (flag 2, start, end)\n\n");
}

//FLAG 1
bool FunctionToCheckFullFileReversal(const char* outputFile, const char *inputFile)
{
    long long n;
    long long bufferSize=4096;
    char bufnew[4096]; 
    char bufold[4096]; 
    int fdnew=open(inputFile, O_RDONLY); // for matching //fdReversed
    if (fdnew == -1) { perror("error in opening file"); return false; }

    int fdold=open(outputFile,O_RDONLY);//reverse //fdoriginal
    if (fdold == -1) { perror("error in opening file"); return false; }

    long long totalLengthOfInputFile=lseek(fdnew, 0, SEEK_END); //reversed
    long long totalLengthOfOutputFile=lseek(fdold, 0, SEEK_END); //original
    //if length dont match
    if(totalLengthOfInputFile != totalLengthOfOutputFile) {
        close(fdnew);
        close(fdold);
        return false;
    }

    long long noofblocks=totalLengthOfInputFile / bufferSize; 
    long long remainder =totalLengthOfInputFile % bufferSize;
    //remainder shows partially filled buffer.
    if(remainder>0){
        lseek(fdnew,noofblocks*bufferSize,SEEK_SET); //Reversed file , take pointer to last remaining part
        lseek(fdold,0,SEEK_SET);                     //Original File, take pointer to 0

        long long r1=read(fdnew,bufnew,remainder);
        long long r2=read(fdold,bufold,remainder);
        if(r1!=remainder || r2!=remainder){
            perror("Error in read");
            close(fdnew); close(fdold);
            // printf("remainder error");
            return false;
        }

        //match both buffers
        for(int i=0;i<remainder;i++)
        {
            if(bufnew[i]!=bufold[remainder-i-1]){   
                close(fdnew);
                close(fdold);
                return false;
            }
        }
    }
    long long offset=remainder;
    for(long long i=noofblocks-1;i>=0;i--)
    {
        lseek(fdnew, i * bufferSize, SEEK_SET);
        long long r1=read(fdnew,bufnew,bufferSize);
        lseek(fdold,offset,SEEK_SET);
        long long r2=read(fdold,bufold,bufferSize);
        if(r1!=bufferSize || r2!=bufferSize){
            perror("Error in read");
            close(fdnew); close(fdold);
            // printf("error is here");
            return false;
        }
        //match buffers:
        for(long long i=0;i<bufferSize;i++)
        {
            if(bufnew[i]!=bufold[bufferSize-i-1]){
                close(fdnew);
                close(fdold);
                return false;
            }
        }
        offset+=bufferSize;
    }
    close(fdnew);
    close(fdold);
    return true;
}

//FLAG 0
bool functionToCheckBlockReversals(const char* originalFile, const char *reversedFile, long long blockSize)
{
    long long n,fdOriginal,fdReversed;
    long long bufferSize=blockSize;
    // char bufOriginal[bufferSize]; char bufReversed[bufferSize];
    char *bufOriginal = (char *) malloc(bufferSize);
    char *bufReversed = (char *) malloc(bufferSize);

    if(!bufOriginal || !bufReversed){
        perror("malloc error"); free(bufOriginal); free(bufReversed); return false;
    }

    fdOriginal=open(originalFile, O_RDONLY); 
    if (fdOriginal == -1) { perror("error in opening file"); return false; }
    fdReversed=open(reversedFile,O_RDONLY);
    if (fdReversed == -1) { perror("error in opening file"); return false; }
    
    long long totalLengthOfInputFile=lseek(fdOriginal, 0, SEEK_END); 
    long long totalLengthOfOutputFile=lseek(fdReversed, 0, SEEK_END); 
    //if length dont match
    if(totalLengthOfInputFile != totalLengthOfOutputFile) return false;
   
    long long noofblocks=totalLengthOfInputFile / bufferSize; 
    long long remainder =totalLengthOfInputFile % bufferSize;
    //remainder shows partially filled buffer.
    for(long long i = 0; i < noofblocks; i++) {
        lseek(fdOriginal, i * bufferSize, SEEK_SET);
        lseek(fdReversed, i * bufferSize, SEEK_SET);
        read(fdOriginal,bufOriginal,bufferSize);
        read(fdReversed,bufReversed,bufferSize);
        
        for(int i=0;i<bufferSize;i++)
        {
            if(bufOriginal[i]!=bufReversed[bufferSize-i-1]){
                return false;
            }
        }
    }
    if(remainder>0){
        lseek(fdOriginal, noofblocks * bufferSize, SEEK_SET);
        lseek(fdReversed, noofblocks* bufferSize, SEEK_SET);
        read(fdOriginal,bufOriginal,remainder);
        read(fdReversed,bufReversed,remainder);
        
        for(int i=0;i<remainder;i++)
        {
            if(bufOriginal[i]!=bufReversed[remainder-i-1]){
                return false;
            }
        }
    }
    close(fdOriginal);
    close(fdReversed);   
    return true;
}

//FLAG 2
bool functionToCheckBlockExceptBetweenIndexReversals(const char* originalFile, const char *reversedFile, long long startIndex, long long endIndex)
{
    //0 to StartIndex-1 
    long long n,fdOriginal,fdReversed;
    long long bufferSize=4096;
    char bufOriginal[4096]; char bufReversed[4096];
    fdOriginal=open(originalFile, O_RDONLY); 
    if (fdOriginal == -1) { perror("error in opening file"); printf("Hello16"); return false; }

    fdReversed=open(reversedFile,O_RDONLY);
    if (fdReversed == -1) { perror("error in opening file"); printf("Hello15");return false; }

    long long totalLengthOfInputFile=lseek(fdOriginal, 0, SEEK_END); 
    long long totalLengthOfOutputFile=lseek(fdReversed, 0, SEEK_END); 
    //if length dont match
    if(totalLengthOfInputFile != totalLengthOfOutputFile) return false;

    long long noofblocks=startIndex / bufferSize; 
    long long remainder =startIndex % bufferSize;
    if(remainder>0){
        lseek(fdReversed,noofblocks*bufferSize,SEEK_SET);
        lseek(fdOriginal,0,SEEK_SET); 
                        
        long long r1=read(fdReversed,bufReversed,remainder);
        long long r2=read(fdOriginal,bufOriginal,remainder);
        if(r1<0 || r2<0){
            perror("Error in read");
            close(fdOriginal); close(fdReversed);
            return false;
        }

        for(int i=0;i<remainder;i++)
        {
            if(bufReversed[i]!=bufOriginal[remainder-i-1]){   
               return false;
            }
        } 
    }

    long long offset=remainder;
    for(long long i=noofblocks-1;i>=0;i--)
    {
        lseek(fdReversed, i * bufferSize, SEEK_SET);
        long long r1=read(fdReversed,bufReversed,bufferSize);
        lseek(fdOriginal,offset,SEEK_SET);
        long long r2=read(fdOriginal,bufOriginal,bufferSize);
        if(r1<0 || r2<0){
            close(fdOriginal); close(fdReversed);
            return false;
        }

        for(long long i=0;i<bufferSize;i++)
        {
            if(bufReversed[i]!=bufOriginal[bufferSize-i-1]){
                return false;
            }
        }
        offset+=bufferSize;
    }


    //StartIndex TO EndIndex
    long long lengthOfThisPart=endIndex-startIndex+1;
    noofblocks=lengthOfThisPart/bufferSize;
    remainder = lengthOfThisPart%bufferSize;
    offset=startIndex;
    for(long long i = noofblocks; i > 0; i--) {
        lseek(fdOriginal,offset,SEEK_SET);
        lseek(fdReversed,offset,SEEK_SET);

        long long r1=read(fdOriginal,bufOriginal,bufferSize);
        long long r2=read(fdReversed,bufReversed,bufferSize);
        
        if(r1<0 || r2<0){
            perror("Error in read");
            close(fdOriginal); close(fdReversed);
            return false;
        }

        for(long long i=0;i<bufferSize;i++)
        {
            if(bufOriginal[i]!=bufReversed[i]){
                return false;
            }
            offset++;
        }
    }
    if(remainder>0){
        lseek(fdOriginal,offset,SEEK_SET);
        lseek(fdReversed,offset,SEEK_SET);
        long long r1=read(fdOriginal,bufOriginal,remainder);
        long long r2=read(fdReversed,bufReversed,remainder);
        
        if(r1<0 || r2<0){
            // perror("Error in read");
            close(fdOriginal); close(fdReversed);
            return false;
        }

        for(long long i=0;i<remainder;i++)
        {
            if(bufOriginal[i]!=bufReversed[i]){
                return false;
            }
            offset++;
        }
    }

    //EndIndex+1 to EOF                                  
    lengthOfThisPart=totalLengthOfInputFile-endIndex-1;  
    noofblocks=lengthOfThisPart / bufferSize;            
    remainder =lengthOfThisPart % bufferSize;
    //remainder shows partially filled buffer.
    offset=endIndex+1+noofblocks*bufferSize;
    if(remainder>0){
        lseek(fdReversed,offset,SEEK_SET);
        lseek(fdOriginal,(endIndex+1),SEEK_SET);                    

        long long r1=read(fdReversed,bufReversed,remainder);
        long long r2=read(fdOriginal,bufOriginal,remainder);
        
        if(r1<0 || r2<0){
            // perror("Error in read");
            close(fdOriginal); close(fdReversed);
            return false;
        }

        for(int i=0;i<remainder;i++)
        {
            if(bufReversed[i]!=bufOriginal[remainder-i-1]){   
                return false;
            }
        }                                                   
    }
    long long offset2=endIndex+1+remainder;
    offset-=bufferSize;
    for(long long i=noofblocks-1;i>=0;i--)
    {
        lseek(fdReversed,offset2, SEEK_SET);
        long long r1=read(fdReversed,bufReversed,bufferSize);

        lseek(fdOriginal,offset,SEEK_SET);
        long long r2=read(fdOriginal,bufOriginal,bufferSize);

        if(r1<0 || r2<0){
            perror("Error in read");
            close(fdOriginal); close(fdReversed);
            return false;
        }

        for(long long i=0;i<bufferSize;i++)
        {
            if(bufReversed[i]!=bufOriginal[bufferSize-i-1]){
                return false;
            }
        }
        offset-=bufferSize;
        offset2+=bufferSize;

    }
    return true;

}

//WriteMsg function
void writeMsg(const char *msg, const char *filename, const char *ans) {
    write(STDOUT_FILENO,msg, strlen(msg));
    write(STDOUT_FILENO,filename, strlen(filename));
    write(STDOUT_FILENO,ans, strlen(ans));
}

int permissionDetailfunction(const char* file_path){
    //user - r,w,e 
    //group - r,w,e 
    //others - r,w,e 
    struct stat dir;
    if(stat(file_path, &dir) == -1) {
        // writeMsg("Directory is created: No\n");
        return -1;
    }
    //User:rwe
    if(dir.st_mode & S_IRUSR){ 
        writeMsg("User has read permission on ", file_path, ": Yes\n");
    }
    else{
        writeMsg("User has read permission on ", file_path, ": No\n");
    }

    if(dir.st_mode & S_IWUSR){
        writeMsg("User has write permission on ", file_path, ": Yes\n");
    }
    else{
        writeMsg("User has write permission on ", file_path, ": No\n");
    }
    if(dir.st_mode & S_IXUSR){
        writeMsg("User has execute permission on ", file_path, ": Yes\n");
    }
    else{
        writeMsg("User has execute permission on ", file_path, ": No\n");
    }

    //Group:rwe 
    if(dir.st_mode & S_IRGRP){ 
        writeMsg("Group has read permission on ", file_path, ": Yes\n");
    }
    else{
        writeMsg("Group has read permission on ", file_path, ": No\n");
    }
    if(dir.st_mode & S_IWGRP){
        writeMsg("Group has write permission on ", file_path, ": Yes\n");
    }
    else{
        writeMsg("Group has write permission on ", file_path, ": No\n");
    }
    if(dir.st_mode & S_IXGRP){
        writeMsg("Group has execute permission on ", file_path, ": Yes\n");
    }
    else{
        writeMsg("Group has execute permission on ", file_path, ": No\n");
    }

    //Other:rwe
     if(dir.st_mode & S_IROTH){ 
        writeMsg("Other has read permission on ", file_path, ": Yes\n");;
    }
    else{
        writeMsg("Other has read permission on ", file_path, ": No\n");
    }
    if(dir.st_mode & S_IWOTH){
        writeMsg("Other has write permission on ", file_path, ": Yes\n");
    }
    else{
        writeMsg("Other has write permission on ", file_path, ": No\n");
    }
    if(dir.st_mode & S_IXOTH){
        writeMsg("Other has execute permission on ", file_path, ": Yes\n");
    }
    else{
        writeMsg("Other has execute permission on ", file_path, ": No\n\n");
    }

    return 0;
}

int main(int argc, char *argv[])
{
// ./a.out <newfilepath> <oldfilepath> <directory> <flag> [<blockSize>|<start> <end>]
// For flag 0: 5 arguments (flag 0, block size)
// For flag 1: 4 arguments
// For flag 2: 6 arguments (flag 2, start, end)

    if(argc<5 || argc>7){
        fprintf(stderr,"Number of arguments are more or lesser than required/n");
        printUsage();
        return -1;
    }

    const char *new_file_path=argv[1];
    const char *old_file_path=argv[2];
    const char *directory=argv[3];

    if(!checkFilePath(new_file_path)){
        printf("Error: Enter correct <newfilepath>, file not found\n");
        return -1;
    }
    if(!checkFilePath(old_file_path)){
        printf("Error: Enter correct <outputfilepath>, file not found\n");
        return -1;
    }
    if(!isDigits(argv[4])){
        printf("Flag value should be 0 or 1 or 2\n");
        printUsage();
        return -1;
    }
    int flag=atoi(argv[4]);
    if(flag!=0 && flag!=1 && flag!=2){
        fprintf(stderr,"Flag value should be 0 or 1 or 2\n");
        return -1;
    }
    if((flag==0 && argc!=6) || (flag==1 && argc!=5) || (flag==2 && argc!=7))
    {
        fprintf(stderr, "Improper number of arguments for input flag\n");
        printUsage();
        return -1;
    }

    //line 1: Directory is created
    struct stat directoryInfo;
    if (stat(directory, &directoryInfo) == -1) {
        write(STDOUT_FILENO, "Directory is created: No\n", strlen("Directory is created: No\n"));
        // return 0;
    }
    else{
        if (S_ISDIR(directoryInfo.st_mode)) {
            write(STDOUT_FILENO, "Directory is created: Yes\n", strlen("Directory is created: Yes\n"));
        } 
        else {
            write(STDOUT_FILENO, "Directory is created: No\n", strlen("Directory is created: No\n"));
        }
    }

    //line2 Both Files are Same
    if(flag==0 && argc==6){
        //block size must be numbers only
        if (!isDigits(argv[5])) {
            fprintf(stderr, "Error: Block size must be positive numbers.\n");
            return -1;
        }
        long long blockSize=atoll(argv[5]);
        //block size!=0
        if(blockSize<=0){
            fprintf(stderr,"Block size must be greater then zero");
            return -1;
        }
        //we will again reverse the outputfile generated and match each character with inputFile.
        if( functionToCheckBlockReversals(old_file_path, new_file_path, blockSize)==true ){
             write(STDOUT_FILENO, "Whether file contents are correctly processed: Yes\n", strlen("Whether file contents are correctly processed: Yes\n"));
        }
        else{
            write(STDOUT_FILENO, "Whether file contents are correctly processed: No\n", strlen("Whether file contents are correctly processed: No\n"));
        }
    }

    else if(flag==1 && argc==5){
        if( FunctionToCheckFullFileReversal(old_file_path, new_file_path)==true ){
            write(STDOUT_FILENO, "Whether file contents are correctly processed: Yes\n", strlen("Whether file contents are correctly processed: Yes\n"));
        }
        else{
            write(STDOUT_FILENO, "Whether file contents are correctly processed: No\n", strlen("Whether file contents are correctly processed: No\n"));
        }
    }
    if(flag==2 && argc==7)
    {
        //check start and end indexe are digits
        if (!isDigits(argv[5]) || !isDigits(argv[6])) {
            fprintf(stderr, "Start and end indices must be positive numbers.\n");
            return -1;
        }
        long long startIndex=atoll(argv[5]);
        long long endIndex=atoll(argv[6]);
        if (startIndex < 0 || endIndex < 0) {
            fprintf(stderr, "Indices cannot be negative.\n");
            return -1;
        }
        if (startIndex > endIndex) {
            fprintf(stderr, "Start index cannot be greater than end index.\n");
            return -1;
        }

        if(functionToCheckBlockExceptBetweenIndexReversals(old_file_path, new_file_path,startIndex, endIndex)==true ){
            write(STDOUT_FILENO, "Whether file contents are correctly processed: Yes\n", strlen("Whether file contents are correctly processed: Yes\n"));
        }
        else{
            write(STDOUT_FILENO, "Whether file contents are correctly processed: No\n", strlen("Whether file contents are correctly processed: No\n"));
        }
    }

    // line 3 both file SIZE are same
    struct stat inputfileinfo;
    struct stat outputfileinfo;
    if(stat(old_file_path, &inputfileinfo)==-1){
        write(STDOUT_FILENO,"Error to detect input file\n", strlen("Error to detect input file\n"));
    }
    if(stat(new_file_path, &outputfileinfo)==-1){
        write(STDOUT_FILENO,"Error to detect output file\n", strlen("Error to detect output file\n"));
    }

    if(inputfileinfo.st_size==outputfileinfo.st_size){
        write(STDOUT_FILENO,"Both Files Sizes are Same: Yes\n", strlen("Both Files Sizes are Same: Yes\n"));
    }
    else{
         write(STDOUT_FILENO,"Both Files Sizes are Same: No\n", strlen("Both Files Sizes are Same: No\n"));
    }

    // 27 lines 
    permissionDetailfunction(new_file_path);
    permissionDetailfunction(old_file_path);
    permissionDetailfunction(directory);
    return 0;
}
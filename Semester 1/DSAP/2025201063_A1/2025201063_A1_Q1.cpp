#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

//we run this function every time after seam removal
void calculateEnergyMatrix(int height, int width, int **energyMatrix, int ***rgbMatrix)
{
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            int jm = (j == 0 ? width-1: j-1);
            int jp = (j == width-1 ? 0 : j+1);
            int im = (i == 0 ? height-1 : i-1);
            int ip = (i == height-1 ? 0 : i+1);

            int r_x = abs(rgbMatrix[i][jp][0] - rgbMatrix[i][jm][0]);
            int g_x = abs(rgbMatrix[i][jp][1] - rgbMatrix[i][jm][1]);
            int b_x = abs(rgbMatrix[i][jp][2] - rgbMatrix[i][jm][2]);

            int r_y = abs(rgbMatrix[ip][j][0] - rgbMatrix[im][j][0]);
            int g_y = abs(rgbMatrix[ip][j][1] - rgbMatrix[im][j][1]);
            int b_y = abs(rgbMatrix[ip][j][2] - rgbMatrix[im][j][2]);

            energyMatrix[i][j] = sqrt((r_x*r_x + g_x*g_x + b_x*b_x) + (r_y*r_y + g_y*g_y + b_y*b_y));
        }
    }
    return;
}
//Function to calculate DP Matrix,and parent[] alongside from energyMatrix
void calculateDpMatrix(int height, int width, int **energyMatrix, int **dp,int **parent)
{
    for(int j=0;j<width;j++){
        dp[0][j]=energyMatrix[0][j];
    }
    for(int i=1;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            int u=dp[i-1][j];
            int ld=0;
            if(j-1>=0) ld+=dp[i-1][j-1];
            else ld=1e8;
            int rd=0;
            if(j+1<width) rd+=dp[i-1][j+1];
            else rd=1e8;
            // dp[i][j]=min(u,min(ld,rd));
            if (ld <= u && ld <= rd) {
                dp[i][j] = energyMatrix[i][j] + ld;
                parent[i][j] = j-1;
            }
            else if (u <= rd) {
                dp[i][j] = energyMatrix[i][j] + u;
                parent[i][j] = j;
            }
            else {
                dp[i][j] = energyMatrix[i][j] + rd;
                parent[i][j] = j+1;
            }
        }
    }    
}
//For height 
void calculateDpMatrixH(int height, int width, int **energyMatrix, int **dp,int **parentH)
{
    for(int r=0;r<height;r++){
        dp[r][0]=energyMatrix[r][0];
    }
    for(int c=1;c<width;c++)
    {
        for(int r=0;r<height;r++)
        {
            int up= (r>0)? dp[r-1][c-1]: 1e8;
            int mid=dp[r][c-1];
            int down=(r<height-1)? dp[r+1][c-1]:1e8;
            
            if(up<=mid && up<=down){
                dp[r][c]=energyMatrix[r][c]+up;
                parentH[r][c]=r-1;
            }
            else if(mid<=down)
            {
                dp[r][c]=energyMatrix[r][c]+mid;
                parentH[r][c]=r;
            }
            else{
                dp[r][c]=energyMatrix[r][c]+down;
                parentH[r][c]=r+1;
            }
        }
    }    
}

//DetectSeam()
void DetectSeam(int height, int width,int **dp, int **parent, int *seam){
    int smallValue=1e8;
    int colIndex=-1;
    // int seam[height];
    for(int j=0;j<width;j++)
    {
        if(dp[height-1][j]<smallValue){
            smallValue=dp[height-1][j];
            colIndex=j;
        }
    }
    seam[height-1]=colIndex;
    for(int i=height-2;i>=0;i--)
    {
        colIndex=parent[i][colIndex]; 
        seam[i]=colIndex;
    }
}
//for height
void DetectSeamH(int height, int width,int **dp, int **parentH, int *seamH){
    int smallValue=1e8;
    int rowIndex=-1;
    for(int r=0;r<height;r++)
    {
        if(dp[r][width-1]<smallValue){
            smallValue=dp[r][width-1];
            rowIndex=r;
        }
    }
    seamH[width-1]=rowIndex;
    for(int c=width-2;c>=0;c--)
    {
        rowIndex=parentH[rowIndex][c+1];//confuse 
        seamH[c]=rowIndex;
    }
}

//RemoveSeam()
void removeSeam(int height, int &width, int ***rgbMatrix,int **energyMatrix, int *seam)
{
    for (int i = 0; i < height; i++) {
        int colToRemove = seam[i];
        // shift pixels left
        for (int j = colToRemove; j < width - 1; j++) {
            rgbMatrix[i][j][0] = rgbMatrix[i][j+1][0]; // B
            rgbMatrix[i][j][1] = rgbMatrix[i][j+1][1]; // G
            rgbMatrix[i][j][2] = rgbMatrix[i][j+1][2]; // R
        }
    }
    // reduce image width
    width = width - 1;
}
//for height
void removeSeamH(int &height, int width, int ***rgbMatrix,int **energyMatrix, int *seamH)
{
    for (int j = 0; j < width; j++) {
        int rowToRemove = seamH[j];
        // shift pixels upwards
        for (int i= rowToRemove; i < height - 1; i++) {
            rgbMatrix[i][j][0] = rgbMatrix[i+1][j][0]; // B
            rgbMatrix[i][j][1] = rgbMatrix[i+1][j][1]; // G
            rgbMatrix[i][j][2] = rgbMatrix[i+1][j][2]; // R
        }
    }
    // reduce image width
    height = height - 1;
}

Mat showImage(int height, int width, int ***rgbMatrix)
{
    Mat img(height, width, CV_8UC3);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            img.at<Vec3b>(i, j)[0] = (uchar)rgbMatrix[i][j][0]; // B
            img.at<Vec3b>(i, j)[1] = (uchar)rgbMatrix[i][j][1]; // G
            img.at<Vec3b>(i, j)[2] = (uchar)rgbMatrix[i][j][2]; // R
        }
    }
    imshow("Resized Image", img);
    waitKey(30);
    return img;
}

void colorSeam(int height, int &width, int ***rgbMatrix,int **energyMatrix, int *seam)
{
    for (int i = 0; i < height; i++) {
        int colToRemove = seam[i];
        rgbMatrix[i][colToRemove][0]=0;
        rgbMatrix[i][colToRemove][1]=0;
        rgbMatrix[i][colToRemove][2]=0;
    }
}
//for height
void colorSeamH(int height, int &width, int ***rgbMatrix,int **energyMatrix, int *seamH)
{
    for (int j = 0; j < width; j++) {
        int rowToRemove = seamH[j];
        rgbMatrix[rowToRemove][j][0]=255;
        rgbMatrix[rowToRemove][j][1]=0;
        rgbMatrix[rowToRemove][j][2]=0;
    }
}

int main()
{
    string Imagepath;
    cout<<"Enter path of image\n";
    cin>>Imagepath;

    //read image in rgb format save it in mat object type
    Mat image = imread(Imagepath, IMREAD_COLOR);
    //if image read is empty
    if(image.empty())
    {
        cout<<"Error: "<<Imagepath<<" not found"<<endl;
        // cin.get();
        return -1;
    }

    //extract height, width of image
    int height=image.size().height; // image.rows
    int width=image.size().width;  // image.cols
    int newHeight;
    int newWidth;
    int origHeight = height;
    int origWidth = width;
    //take input from user
    cout<<"Given Image has Height:"<<height<<" and Width:"<<width<<endl;
    cout<<"Enter the new height and width (This value should be strictly lesser than original height and width)"<<endl;
    cin>>newHeight>>newWidth;
    if(newHeight==0 || newWidth==0){
        cout<<"New height and new width value should be strictly greater than 0"<<endl;
        return -1;
    }

    if(newHeight>=height || newWidth>=width){
        cout<<"New height and new width value should be strictly lesser than original image height and width"<<endl;
        return -1;
    }
    
    // int rgbMatrix[height][width][3];
    int*** rgbMatrix = new int**[height];
    for (int i = 0; i < height; i++) {
        rgbMatrix[i] = new int*[width];
        for (int j = 0; j < width; j++) {
            rgbMatrix[i][j] = new int[3]; 
        }
    }

    //extract rgb value in 3d matrix
    for(int row=0;row<height;row++)
    {
        for(int col=0;col<width;col++)
        {
            Vec3b imagepixel = image.at<Vec3b>(row, col);
            // cout<<imagepixel<<endl;
            rgbMatrix[row][col][0]=imagepixel.val[0];
            rgbMatrix[row][col][1]=imagepixel.val[1];
            rgbMatrix[row][col][2]=imagepixel.val[2];
        }
    }

    // 2D arrays
    int** energyMatrix = new int*[height];
    int** dp = new int*[height];
    int** parent = new int*[height];
    int** parentH = new int*[height];
    for (int i = 0; i < height; i++) {
        energyMatrix[i] = new int[width];
        dp[i] = new int[width];
        parent[i] = new int[width];
         parentH[i] = new int[width];
    }
   
    int* seam = new int[height];
    int* seamH = new int[width];

    while(width!=newWidth)
    {
        calculateEnergyMatrix(height, width,energyMatrix, rgbMatrix);
        calculateDpMatrix(height,width,energyMatrix, dp, parent);
        DetectSeam(height,width,dp,parent,seam);
        colorSeam(height,width,rgbMatrix,energyMatrix,seam);
        showImage(height,width,rgbMatrix);
        removeSeam(height,width,rgbMatrix,energyMatrix,seam);
        // if(height>newHeight)
        // {
        //     calculateEnergyMatrix(height, width,energyMatrix, rgbMatrix);
        //     calculateDpMatrixH(height,width,energyMatrix, dp, parentH);
        //     DetectSeamH(height,width,dp,parentH,seamH);
        //     colorSeamH(height,width,rgbMatrix,energyMatrix,seamH);
        //     showImage(height,width,rgbMatrix);
        //     removeSeamH(height,width,rgbMatrix,energyMatrix,seamH);
        // }
    }
    while(height!=newHeight)
    {
            calculateEnergyMatrix(height, width,energyMatrix, rgbMatrix);
            calculateDpMatrixH(height,width,energyMatrix, dp, parentH);
            DetectSeamH(height,width,dp,parentH,seamH);
            colorSeamH(height,width,rgbMatrix,energyMatrix,seamH);
            showImage(height,width,rgbMatrix);
            removeSeamH(height,width,rgbMatrix,energyMatrix,seamH);
    }
    Mat resizedImage=showImage(height,width,rgbMatrix);
    waitKey(0);
    bool saved = imwrite("resizedImage.jpg", resizedImage);
    if(saved) {
        cout<<"Resized image saved as resizedImage.jpg"<<endl;
    } else {
        cout<<"Error: Could not save the image!"<<endl;
    }
    // delete rgbMatrix
    for (int i=0;i<origHeight;i++) {
        for (int j=0;j<origWidth;j++) {
            delete[] rgbMatrix[i][j];
        }
        delete[] rgbMatrix[i];
    }
    delete[] rgbMatrix;

    // delete 2D arrays
    for (int i=0;i<origHeight;i++) {
        delete[] energyMatrix[i];
        delete[] dp[i];
        delete[] parent[i];
        delete[] parentH[i];
    }
    delete[] energyMatrix;
    delete[] dp;
    delete[] parent;
    delete[] parentH;
    delete[] seam;
    delete[] seamH;

    return 0;
}
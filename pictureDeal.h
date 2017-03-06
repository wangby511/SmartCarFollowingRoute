#ifndef PICTUREDEAL_H_INCLUDED
#define PICTUREDEAL_H_INCLUDED
#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <opencv2\opencv.hpp>
#include <cmath>
//#include "coordinate.h"
using namespace std;
using namespace cv;

template<class T> class Image {
private:IplImage* imgp;
public:
Image(IplImage* img=0) {imgp=img;}
~Image(){imgp=0;}
inline T* operator[](const int rowIndx) {
return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));
}
};
typedef struct
{
    unsigned char b,g,r;
} RgbPixel;

typedef struct
{
    float b,g,r;
} RgbPixelFloat;

typedef struct
{
    unsigned char h,s,v;
} HsvPixel;

typedef struct
{
    float h,s,v;
} HsvPixelFloat;

typedef Image<RgbPixel> RgbImage;
typedef Image<unsigned char> BwImage;
typedef Image<RgbPixelFloat>  RgbFloatImage;
typedef Image<HsvPixelFloat>  HsvFloatImage;
typedef Image<HsvPixel>  HsvImage;

int flag[4]={0};
CvPoint2D32f originpoints[4]; //保存四个点的原始坐标
const CvPoint2D32f newpoints[4]={cvPoint2D32f(0, 0),cvPoint2D32f(400, 0),cvPoint2D32f(0,400),cvPoint2D32f(400,400)};; //保存这四个点的新坐标
IplImage *g_pGrayImage = NULL;
IplImage *g_pBinaryImage = NULL;
IplImage *g_pBinaryImage1 = NULL;
IplImage *g_pBinaryImage2 = NULL;
CvCapture* cam;
double greenPointh,greenPoints,greenPointv;
IplImage *InitialImg=NULL;
IplImage* TransImg =cvCreateImage(cvSize(400,400),IPL_DEPTH_8U,3);
IplImage* TransImg2 =cvCreateImage(cvSize(400,400),IPL_DEPTH_8U,3);//创建一个400*400的24位彩色图像，保存变换结果//为透视变换后图像申请空间
CvMat* transmat=cvCreateMat(3,3,CV_32FC1); //创建一个3*3的单通道32位浮点矩阵保存变换数据
vector<coordinate> point;

void mouse(int mouseevent,int x,int y,int flags,void *param)
{
    if(mouseevent==CV_EVENT_LBUTTONDOWN)
    {
        int sx=x,sy=y;
        if(flag[0]==0){originpoints[0] = cvPoint2D32f(sx, sy);flag[0]=1;cout<<"1: "<<sx<<";"<<sy<<endl;}
        else if(flag[1]==0){originpoints[1] = cvPoint2D32f(sx, sy);flag[1]=1;cout<<"2: "<<sx<<";"<<sy<<endl;}
        else if(flag[2]==0){originpoints[2] = cvPoint2D32f(sx, sy);flag[2]=1;cout<<"3: "<<sx<<";"<<sy<<endl;}
        else if(flag[3]==0){originpoints[3] = cvPoint2D32f(sx, sy);flag[3]=1;cout<<"4: "<<sx<<";"<<sy<<endl;}
        else {}
    }
}

void on_trackbar(int pos)
{
    cvThreshold(g_pGrayImage, g_pBinaryImage, pos, 255, CV_THRESH_BINARY);
    cvShowImage("二值图", g_pBinaryImage);
}

void cvThin (IplImage* src, IplImage* dst, int iterations = 1)
{
    cout<<"cvThin ing..."<<endl;
    cvCopyImage(src, dst);
    BwImage dstdat(dst);
    IplImage* t_image = cvCloneImage(src);
    BwImage t_dat(t_image);
    for (int n = 0; n < iterations; n++)
        for (int s = 0; s <= 1; s++)
        {
            cvCopyImage(dst, t_image);
            for (int i = 0; i < src->height; i++)
                for (int j = 0; j < src->width; j++)
                    if (t_dat[i][j])
                    {
                        int a = 0, b = 0;
                        int d[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1},{1, 0}, {1, -1}, {0, -1}, {-1, -1}};
                        int p[8];
                        p[0] = (i == 0) ? 0 : t_dat[i-1][j];
                        for (int k = 1; k <= 8; k++)
                        {
                            if (i+d[k%8][0] < 0 || i+d[k%8][0] >= src->height ||j+d[k%8][1] < 0 || j+d[k%8][1] >= src->width)p[k%8] = 0;
                            else p[k%8] = t_dat[ i+d[k%8][0] ][ j+d[k%8][1] ];
                            if (p[k%8]) {b++;if (!p[k-1]) a++;}
                        }
                        if (b >= 2 && b <= 6 && a == 1)
                            if (!s && !(p[2] && p[4] && (p[0] || p[6])))dstdat[i][j] = 0;
                        else if (s && !(p[0] && p[6] && (p[2] || p[4])))
                            dstdat[i][j] = 0;
                        }
        }
    cvReleaseImage(&t_image);
}

CvPoint2D32f findRedCenter(const IplImage* srcimg)//寻找红色中心 H分量范围
{
    IplImage *dst_image = cvCreateImage(cvGetSize(srcimg),IPL_DEPTH_32F,srcimg->nChannels);
	IplImage *src_image_32 = cvCreateImage(cvGetSize(srcimg),IPL_DEPTH_32F,srcimg->nChannels);

    cvConvertScale(srcimg,src_image_32);//将原图转换为32f类型
    cvCvtColor(src_image_32,dst_image,CV_BGR2HSV);//得到HSV图，保存在dst_image中  参数范围H(0,360) S(0,1) V(0,255)
    HsvFloatImage hsvimg(dst_image);

    int sumx=0,sumy=0,nred=1,i,j;
    for (i=0;i<dst_image->height;++i)
    for (j=0;j<dst_image->width;++j)
    {
        if ( ( (hsvimg[i][j].h>=0 && hsvimg[i][j].h<=12)||(hsvimg[i][j].h>=350 && hsvimg[i][j].h<=360) ) && hsvimg[i][j].s>=0.4   && hsvimg[i][j].v>=100 )
        {sumx+=j;sumy+=i;++nred;}
    }
    cvReleaseImage(&dst_image);cvReleaseImage(&src_image_32);//释放内存
    return cvPoint2D32f(sumx/nred,sumy/nred);
}

coordinate get_car_head(const IplImage* srcimg)//找出小车头部的位置
{
    coordinate carHead;
    CvPoint2D32f carHeadCvPoint;
    carHeadCvPoint=findRedCenter(srcimg);
    carHead.x=carHeadCvPoint.x;
    carHead.y=carHeadCvPoint.y;
    return carHead;
}

CvPoint2D32f findGreenCenter(const IplImage* srcimg)//寻找绿色中心 H分量范围
{
    IplImage *dst_image = cvCreateImage(cvGetSize(srcimg),IPL_DEPTH_32F,srcimg->nChannels);
	IplImage *src_image_32 = cvCreateImage(cvGetSize(srcimg),IPL_DEPTH_32F,srcimg->nChannels);

    cvConvertScale(srcimg,src_image_32);//将原图转换为32f类型
    cvCvtColor(src_image_32,dst_image,CV_BGR2HSV);//得到HSV图，保存在dst_image中  参数范围H(0,360) S(0,1) V(0,255)
    HsvFloatImage hsvimg(dst_image);

    int sumx=0,sumy=0,ngreen=1,i,j;
    for (i=0;i<dst_image->height;++i)
    for (j=0;j<dst_image->width;++j)
    {
        if ( hsvimg[i][j].h>=95 && hsvimg[i][j].h<=120 && hsvimg[i][j].s>=0.5   && hsvimg[i][j].v>=100 )
        if ( hsvimg[i][j].h>=95 && hsvimg[i][j].h<=120 && hsvimg[i][j].s>=0.5   && hsvimg[i][j].v>=100 )
            //if ( hsvimg[i][j].h>=110 && hsvimg[i][j].h<=130 && hsvimg[i][j].s>=0.3   && hsvimg[i][j].v>=180 )
        {sumx+=j;sumy+=i;++ngreen;}
    }
    cvReleaseImage(&dst_image);cvReleaseImage(&src_image_32);//释放内存
    return cvPoint2D32f(sumx/ngreen,sumy/ngreen);
}

coordinate get_car_tail(const IplImage* srcimg)//找出小车尾部的位置
{
    coordinate carTail;
    CvPoint2D32f carHeadCvPoint;
    carHeadCvPoint=findGreenCenter(srcimg);
    carTail.x=carHeadCvPoint.x;
    carTail.y=carHeadCvPoint.y;
    return carTail;
}

//void cvGoodFeaturesToTrack2(IplImage* srcImage)
//{
//    int cornerCount=50; //
//	CvPoint2D32f corners[50]; //
//	double qualityLevel = 0.05; //
//	double minDistance = 10; //
//	IplImage *grayImage = 0, *corners1 = 0, *corners2 = 0; //
//	int i;
//	CvScalar color = CV_RGB(255,0,0); //
//	grayImage=cvCreateImage(cvGetSize(srcImage),IPL_DEPTH_8U,1);//将原图灰度化
//	cvCvtColor(srcImage,grayImage,CV_BGR2GRAY);//创建2个与原图大小相同的临时图像
//	corners1= cvCreateImage(cvGetSize(srcImage),IPL_DEPTH_32F, 1);
//	corners2= cvCreateImage(cvGetSize(srcImage),IPL_DEPTH_32F, 1);
//	cvGoodFeaturesToTrack(grayImage,corners1,corners2,corners,&cornerCount,qualityLevel,minDistance,0);//角点检测
//	for(i=0;i<cornerCount;i++)
//    {
//        cout<<i<<":"<<corners[i].x<<"  "<<corners[i].y<<endl;
//        point.push_back(coordinate(corners[i].x,corners[i].y));
//    }
//    if(cornerCount>0)
//    {
//        for(i=0;i<cornerCount;++i)
//        {
//            cvCircle(srcImage,cvPoint(corners[i].x,corners[i].y),6,CV_RGB(255,0,0),2,CV_AA,0);
//        }
//    }
//    cvNamedWindow("srcImage");
//    cvShowImage("srcImage", srcImage);
//}
int cornerCount=30;
int store[50][2]={0};
void cvGoodFeaturesToTrack2(IplImage* srcImage)
{
	CvPoint2D32f corners[50]; //
	double qualityLevel = 0.05; //
	double minDistance = 30; //
	IplImage *grayImage = 0, *corners1 = 0, *corners2 = 0; //
	int i;
	CvScalar color = CV_RGB(255,255,255); //
	CvScalar color2 = CV_RGB(0,0,0);
	grayImage = srcImage;
	corners1=srcImage;corners2=srcImage;
	cvGoodFeaturesToTrack(grayImage,corners1,corners2,corners,&cornerCount,qualityLevel,minDistance,0);//角点检测
	for(i=0;i<cornerCount;i++)
    {
        cout<<i<<":"<<corners[i].x<<"  "<<corners[i].y<<endl;
        store[i][0]=corners[i].x;
        store[i][1]=corners[i].y;
        if (corners[i].x==0 && corners[i].y==0) break;
    }
    cornerCount=i;
}

CvPoint2D32f corners[50];
int start[2]={400,400};
void getpoint()
{
//    start[0]=400;
//    start[1]=400;
    coordinate tem;
    int k,j,a,b,i;
    bool p[50]={0};
    int now=20000;
    int starti=0;
    int dismin = 200000;
    int dis;
    int next;
    for(i=0;i<cornerCount;++i)
    {
        dis = (store[i][0]-start[0])*(store[i][0]-start[0])+(store[i][1]-start[1])*(store[i][1]-start[1]);
        if (dis<dismin)
        {
            dismin= dis;
            next=i;
        }
    }
    tem.x=(int)store[next][0];
    tem.y= (int)store[next][1];
    a=next;


    ///while (true){if (cvWaitKey(10)>=0)break;}
    ///cvNamedWindow("srcImage");
    ///cvShowImage("srcImage", TransImg);
    ///cvCircle(TransImg,cvPoint(start[0],start[1]),6,CV_RGB(0,0,255),2,CV_AA,0);
    ///cvCircle(TransImg,cvPoint(tem.x,tem.y),6,CV_RGB(255,255,0),2,CV_AA,0);

    p[next]=1;
    starti=next;
    point.push_back(tem);
    cout<<"start[0,1]="<<start[0]<<";"<<start[1]<<endl;
    cout<<"tem.x,tem.y="<<tem.x<<";"<<tem.y<<endl;
    cout<<"point[0].x,y="<<point[0].x<<";"<<point[0].y<<endl;
    for (i=0;i<cornerCount-1;++i)
    {
        int dismin = 200000;
        int dis;
        int next;
        for (b=0;b<cornerCount;++b)
        {
            if (p[b]==1) continue;
            dis = (store[b][0]-store[starti][0])*(store[b][0]-store[starti][0])+(store[b][1]-store[starti][1])*(store[b][1]-store[starti][1]);
            if (dis<dismin)
            {
                dismin= dis;
                next=b;
            }
        }
        tem.x = (int)store[next][0];
        tem.y = (int)store[next][1];
        cvCircle(TransImg,cvPoint(tem.x,tem.y),6,CV_RGB(255,255,0),2,CV_AA,0);
        //while (true){if (cvWaitKey(10)>=0)break;}
//
//        cvNamedWindow("srcImage");
//        cvShowImage("srcImage", TransImg);



        point.push_back(tem);
        p[next]=1;
        starti=next;
    }

    cout<<"point:"<<a<<endl<<next<<endl;
    tem.x =(int) (point[0].x * 0.25+point[point.size()-1].x* 0.75);
    tem.y =(int)( point[0].y * 0.25+point[point.size()-1].y* 0.75);
    point.push_back(tem);

//    while (true){if (cvWaitKey(10)>=0)break;}
//    cvCircle(TransImg,cvPoint(tem.x,tem.y),6,CV_RGB(255,0,0),2,CV_AA,0);
//    cvNamedWindow("srcImage");
//    cvShowImage("srcImage", TransImg);

    tem.x =(int)( point[0].x * 0.50+point[point.size()-2].x* 0.50);
    tem.y = (int)(point[0].y * 0.50+point[point.size()-2].y* 0.50);
    point.push_back(tem);

    tem.x =(int)( point[0].x * 0.75+point[point.size()-3].x* 0.25);
    tem.y = (int)(point[0].y * 0.75+point[point.size()-3].y* 0.25);
    point.push_back(tem);


//
//    while (true){if (cvWaitKey(10)>=0)break;}
//    cvCircle(TransImg,cvPoint(tem.x,tem.y),6,CV_RGB(255,0,0),2,CV_AA,0);
//    cvNamedWindow("srcImage");
//    cvShowImage("srcImage", TransImg);
//
//    while(true){if(cvWaitKey(10)>=0)break;}
//    cvDestroyWindow("srcImage");
}

void pictureInitial(IplImage* InitialImg,IplImage* TransImg)
{
    CvCapture* cam =cvCreateCameraCapture(0);
    InitialImg =cvQueryFrame(cam);

    cvNamedWindow("win1");
    InitialImg=cvQueryFrame(cam);
    cvNamedWindow("win1");
    cvShowImage("win1", InitialImg);
    cvSetMouseCallback("win1", mouse,NULL);
    while(true){if(cvWaitKey(10)>=0&&flag[3]==1)break;}//停留等候


    cvGetPerspectiveTransform(originpoints, newpoints, transmat); ///根据四个点计算变换矩阵
    cvWarpPerspective(InitialImg, TransImg, transmat); ///根据变换矩阵计算图像的变换

    g_pGrayImage = cvCreateImage(cvGetSize(TransImg), IPL_DEPTH_8U, 1);//转化为灰度图
	cvCvtColor(TransImg, g_pGrayImage, CV_BGR2GRAY);    //创建二值图
	g_pBinaryImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1);
	g_pBinaryImage1 = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1);
	g_pBinaryImage2 = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1);

	cvDestroyWindow("win1");
	cvNamedWindow("二值图", CV_WINDOW_AUTOSIZE);
    int nThreshold = 0;// 滑动条
    cvCreateTrackbar("二值图阈值", "二值图", &nThreshold, 254, on_trackbar);
    on_trackbar(1);
    while(true){if(cvWaitKey(10)>=0)break;}
    cout<<"二值图阈值为："<<nThreshold<<endl;

    cvThreshold(g_pGrayImage, g_pBinaryImage, nThreshold, 255, CV_THRESH_BINARY);
    cvNot(g_pBinaryImage,g_pBinaryImage1);        //取反
    cvThin(g_pBinaryImage1, g_pBinaryImage2, 20);  //细化函数，调用20次
    cout<<"ok"<<endl;
    cvGoodFeaturesToTrack2(g_pBinaryImage2);///cvGoodFeaturesToTrack2为cvGoodFeaturesToTrack的封装函数
	//while(true){if(cvWaitKey(10)>=0)break;}///return;

//    cout<<"Hough变换"<<endl;
//    CvMemStorage* storage = cvCreateMemStorage(); //创建一片内存区域存储线段数据
//    CvSeq* lines = cvHoughLines2(g_pBinaryImage2, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 30, 40, 40); //Hough变换找直线
//    IplImage* Hough_img1 = cvCreateImage(cvGetSize(g_pBinaryImage2), 8, 1); //新建一幅同样大的图像，用于画出找到的直线段
//    cvSetZero(Hough_img1); //填充为黑色
//    for(int i = 0; i < lines->total; i++)
//    {
//        CvPoint* line = (CvPoint*) cvGetSeqElem(lines, i); //读取第i条线段的两个端点
//        cvLine(Hough_img1,line[0],line[1],cvScalar(255)); //用白色画出这条线段
//    }
//    cvNamedWindow("Hough后的图像");
//    cvShowImage("Hough后的图像", Hough_img1);
//
//    while(true){if(cvWaitKey(10)>=0)break;}
      cvDestroyWindow("二值图");

}



#endif // PICTUREDEAL_H_INCLUDED

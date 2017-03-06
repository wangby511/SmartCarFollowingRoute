#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <opencv2\opencv.hpp>
#include <cmath>
#include "coordinate.h"
#include "run.h"
#include "the_car.h"
#include "the_port.h"
#include "pictureDeal.h"
using namespace std;
using namespace cv;
coordinate delta1, delta2, car_head, car_tail;
extern int flag[4];
extern CvPoint2D32f originpoints[4]; //保存四个点的原始坐标
extern const CvPoint2D32f newpoints[4]; //保存这四个点的新坐标
///CvPoint2D32f redPoint;
///CvPoint2D32f greenPoint;
extern IplImage *g_pGrayImage;
extern IplImage *g_pBinaryImage;
extern IplImage *g_pBinaryImage1;
extern IplImage *g_pBinaryImage2;
extern CvCapture* cam;
extern IplImage *InitialImg;
extern IplImage* TransImg;
extern IplImage* TransImg2;
extern CvMat* transmat;
extern vector<coordinate> point;
extern CvPoint2D32f corners[50];

extern double greenPointh,greenPoints,greenPointv;
extern double redPointh,redPoints,redPointv;

void mouse1(int mouseevent,int x,int y,int flags,void *param)
{
    if(mouseevent==CV_EVENT_LBUTTONDOWN)
    {
        int j=x,i=y;

        IplImage *dst_image =    cvCreateImage(cvGetSize(TransImg),IPL_DEPTH_32F,TransImg->nChannels);
	    IplImage *src_image_32 = cvCreateImage(cvGetSize(TransImg),IPL_DEPTH_32F,TransImg->nChannels);

        cvConvertScale(TransImg,src_image_32);//将原图转换为32f类型
        cvCvtColor(src_image_32,dst_image,CV_BGR2HSV);//得到HSV图，保存在dst_image中  参数范围H(0,360) S(0,1) V(0,255)
        HsvFloatImage hsvimg(dst_image);

        greenPointh=hsvimg[i][j].h;greenPoints=hsvimg[i][j].s;greenPointv=hsvimg[i][j].v;
        cout<<"mouse1,"<<hsvimg[i][j].h<<";"<<hsvimg[i][j].s<<";"<<hsvimg[i][j].v<<endl;
        cvDestroyWindow("win-TransImg");
        cvReleaseImage(&dst_image);
        cvReleaseImage(&src_image_32);//释放内存
    }
}

void mouse2(int mouseevent,int x,int y,int flags,void *param)
{
    if(mouseevent==CV_EVENT_LBUTTONDOWN)
    {
        int j=x,i=y;

        IplImage *dst_image =    cvCreateImage(cvGetSize(TransImg),IPL_DEPTH_32F,TransImg->nChannels);
	    IplImage *src_image_32 = cvCreateImage(cvGetSize(TransImg),IPL_DEPTH_32F,TransImg->nChannels);

        cvConvertScale(TransImg,src_image_32);//将原图转换为32f类型
        cvCvtColor(src_image_32,dst_image,CV_BGR2HSV);//得到HSV图，保存在dst_image中  参数范围H(0,360) S(0,1) V(0,255)
        HsvFloatImage hsvimg(dst_image);

        cout<<hsvimg[i][j].h<<";"<<hsvimg[i][j].s<<";"<<hsvimg[i][j].v<<endl;
        cvReleaseImage(&dst_image);
        cvReleaseImage(&src_image_32);//释放内存
    }
}

int main()
{
	int count = 1;
	double sine,cosine;
	the_car car;
	//vector<coordinate> point;

//	coordinate p1(20,380), p2(20), p3(380,20), p4(380);
//	point.push_back(p1);
//	point.push_back(p2);
//	point.push_back(p3);
//	point.push_back(p4);
	adj_cam();
	pictureInitial(InitialImg, TransImg);///开始图像处理封装函数
	//cvReleaseImage(&InitialImg);cvReleaseImage(&TransImg);cvDestroyAllWindows;
	adj_cam();



    cam = cvCreateCameraCapture(0);
    if(!cam)return 0;
	InitialImg = cvQueryFrame(cam);
    //cvGetPerspectiveTransform(originpoints, newpoints, transmat);
    cvWarpPerspective(InitialImg, TransImg, transmat);
    car_head = get_car_head(TransImg);
    //cvNamedWindow("win-TransImg");
    //cvShowImage("win-TransImg", TransImg);
    //cout<<"cvSetMouseCallback('win-TransImg', mouse1,NULL);"<<endl;
    //cvSetMouseCallback("win-TransImg", mouse1,NULL);
    start[0]=car_head.x;
    start[1]=car_head.y;

    cout<<"get and start[0,1]="<<start[0]<<"  "<<start[1]<<endl;
    cvNamedWindow("TransImg-1");
    cvShowImage("TransImg-1", TransImg);


    getpoint();
    cvDestroyWindow("TransImg-1");

    CvScalar color22 = CV_RGB(255,0,0);
    CvScalar color23 = CV_RGB(0,255,0);
    CvScalar color24 = CV_RGB(0,0,255);


	while (1)
	{
        InitialImg = cvQueryFrame(cam);
        cvGetPerspectiveTransform(originpoints, newpoints, transmat); ///根据四个点计算变换矩阵
        cvWarpPerspective(InitialImg, TransImg, transmat); ///根据变换矩阵计算图像的变换
        //if(!TransImg){car.move_s();return 0;}

		car_head = get_car_head(TransImg);
		car_tail = get_car_tail(TransImg);

		sine = get_sine(car_head, car_tail, point[count]);
		cosine = get_cosine(car_head, car_tail, point[count]);
		///cout<<car_head.x<<"  "<<car_head.y<<endl;
		///cout<<car_tail.x<<"  "<<car_tail.y<<endl;

		///run(car, sine,cosine);

//		if (cosine < 0.15)car.move_b();
//		else if (sine > -0.25 && sine < 0.25)car.move_f();
//		else
//			if (sine >= 0.25)car.move_r();
//			else car.move_l();

		cout<<"count="<<count<<endl;
		switch(get_dct(car_head,car_tail,count))
		{
            case'w':
            {
                car.move_f();
                break;
            }
            case'a':
            {
                car.move_l();
                break;
            }
            case's':
            {
                car.move_b();
                break;
            }
            case'd':
            {
                car.move_r();
                break;
            }
		}
		if (get_destination(car_head,car_tail,count))
        {
            ++count;
            count=count%point.size();
            //if(count==0) {car.move_s();return 0;}///停车键
        }

		cvCircle(TransImg,cvPoint(car_head.x,car_head.y),4,color22,4,CV_AA,0);
		cvCircle(TransImg,cvPoint(car_tail.x,car_tail.y),4,color23,4,CV_AA,0);
		cvCircle(TransImg,cvPoint(point[count].x,point[count].y),6,color24,6,CV_AA,0);
		cvNamedWindow("Run-TransImg");
		cvShowImage("Run-TransImg",TransImg);
		//cout<<"car_tail"<<car_tail.x<<"  "<<car_tail.y<<endl;

		//cvSetMouseCallback("win-TransImg", mouse1);
		//cvReleaseImage(&TransImg2);
        if (cvWaitKey(1)>=0)return 0;
	}
	while (true){if (cvWaitKey(10)>=0)break;}
	cvReleaseCapture(&cam);
	return 0;
}

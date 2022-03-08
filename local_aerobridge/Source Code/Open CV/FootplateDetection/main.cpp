//author: Cheng Yupeng
//discribe: detect the foodplate in SQ B772, when distance is too closed， this code dosen't work


#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>
#include <opencv2/legacy/compat.hpp>

#include<iostream>
#include<vector>
#include <stdlib.h>
#include<dirent.h>
#include<string.h>
#include<math.h>

using namespace cv;
using namespace std;

static Mat src; Mat src_gray;
static float n,m,ratio;             //前两点和高度之比
static float a,b,ratio_shape;       //长宽之比
static float c,d,ratio_shape_h;
static float e,f,ratio_shape_best;
static int best=0;
static int muchpoly;
static Mat imageROI;

static vector<Point> model;
static int template_number=0;
static int matched=0;

int main(int argc,char** argv)
{


	 //import image by string
	 IplImage *desimg,*srcimg;
	 //string dirName = "D:/mobility lab/New folder/testing/DataCollect_2016-4-27-6-52-56 - SQ B772 - PBB Docking/";
	 string dirName = "C:/Users/bojan/Desktop/TestFeatures/SQB772/";
	 DIR *dir;
	 dir = opendir(dirName.c_str());
	 string imgName;
	 struct dirent *ent;

	 if (dir != NULL) {
	         while ((ent = readdir (dir)) != NULL) {
	              imgName= ent->d_name;
	              if(imgName.compare(".")!= 0 && imgName.compare("..")!= 0)
	                        {
	                          string aux;
	                          aux.append(dirName);
	                          aux.append(imgName);
	                          cout << aux << endl;
	                          Mat src= imread(aux);
	                          //imshow(aux,src);
	                          waitKey(0);


//int main()
//{
//	//input the image
//	Mat src = cv::imread("000162.png");
   // namedWindow("origin",CV_WINDOW_NORMAL);
	//imshow("origin",src);

	//define the interesting region
	imageROI=src(Rect(0,0,src.cols,(src.rows)));
	//namedWindow("ROI",CV_WINDOW_NORMAL);
	//imshow("ROI",imageROI);


    // get the edge of the input image
	Mat gray,bw;
	GaussianBlur(imageROI, src_gray, cv::Size(7,7), 2, 2);


	//adjust the contrast
	Mat new_src=src_gray.clone();
	//equalizeHist( gray, new_src );                           //  Histogram equalization
	//namedWindow("contrast",CV_WINDOW_NORMAL);
	//imshow("contrast",new_src);

	Mat new2_src=new_src.clone();

	 for(int y = 0; y < imageROI.rows; y++ )
    {
       for(int x = 0; x < imageROI.cols; x++ )
       {
              for(int c = 0; c < 3; c++ )
              {
		             /* int gray_level=0;
					  double distribute=0;*/
                     new_src.at<Vec3b>(y,x)[c]= saturate_cast<uchar>( 3.5*(src_gray.at<Vec3b>(y,x)[c] ));
		     //         gray_level=new_src.at<uchar>(y,x);
					  //int threshold=80;
					  // if(gray_level<threshold)
					  // {
					  //distribute=(1/(2.5*threshold))*exp(-(gray_level*gray_level)/(2*threshold*threshold));		 //using gussian distribute to distinguish black part
					  //new2_src.at<uchar>(y,x)= 2*255*distribute*gray_level;
					  // }
              }
       }
     }
	// namedWindow("contrast",CV_WINDOW_NORMAL);
	 //imshow("contrast",new_src);


	 cvtColor(new_src,gray,CV_BGR2GRAY);
	 //namedWindow("GRAY",CV_WINDOW_NORMAL);
	 //imshow("GRAY",gray);
	 equalizeHist( gray, new2_src );                           //  Histogram equalization
	 //namedWindow("contrast2",CV_WINDOW_NORMAL);
	 //imshow("contrast2",new2_src);




	//canny edge detction
	Canny(gray,bw,500,200,5,true);                 //第一个阈值确定绝对边缘，第二个使边缘平滑连续
	//namedWindow("canny",CV_WINDOW_NORMAL);
	//imshow("canny",bw);

	//close operation
	Mat src_close;
	Size kernalSize (15,5);                          //第一个参数大  横着的断线就容易被连起来
    Mat element = getStructuringElement (MORPH_RECT, kernalSize, Point(1,1)  );
	morphologyEx( bw, src_close, MORPH_CLOSE, element );
    //namedWindow("morphology",CV_WINDOW_NORMAL);
	//imshow("morphology",src_close);

	//find the contours
	vector<vector<Point> > contours;
	vector<cv::Vec4i> hierarchy;
    findContours(src_close.clone(), contours,hierarchy,CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

	//show the contours of the input image
	Mat savedGrayMat = Mat::zeros(gray.rows,gray.cols, CV_8UC1);
	drawContours(savedGrayMat, contours,-1, Scalar(255),2,8,hierarchy);
	//namedWindow("contours",CV_WINDOW_NORMAL);
	//imshow("contours", savedGrayMat);

	//approximate of the contours,and get the convex
	vector<vector<Point> > POLY;
	vector<vector<Point> > POLY_precise;
	vector<vector<Point> > POLY_rough;
	vector<vector<Point> > POLY_best;
	vector<Point> approx;
	vector<Point> convex_hull;

	//moment
	vector<Moments> mu(contours.size() );
	vector<Point2f> mc( contours.size() );

	Mat dst = imageROI.clone();

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.001, true);
		POLY.push_back(approx);
	    if (fabs(contourArea(approx)) <500 || fabs(contourArea(approx))>20000)                                      //将面积太小的轮廓直接跳过  注意continue用法
		continue;


		int k=contours[i].size();
				              int len;                                          //for this approx
							  len=contours[i].size();
							  int   *num=new   int[len];
							  int   *nums=new   int[len];

							   for(int   j=0;j<len;j++)
                               {
                                  num[j]=contours[i].at(j).y;
                                  nums[j]=contours[i].at(j).x;
                               }

							   sort(num,num+len);              //高度
							   sort(nums,nums+len);             //宽度

							   a=num[len-1]-num[0];
							   b=nums[len-1]-nums[0];
							   ratio_shape=b/a;

		// compute the central momment
            mu[i] = moments( contours[i], false );
            mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
			//circle( dst, mc[i], 4, Scalar(255,0,0), -1, 8, 0 );



       if(ratio_shape>5 && ratio_shape<20)// && new2_src.at<uchar>(mc[i])<50)  //&& new2_src.at<uchar>(mc[i])<50
		POLY_rough.push_back(approx);
	}


	for(int i = 0; i < POLY_rough.size(); i++)
	{
	//计算外接矩形和本身轮廓的面积
    Rect bound=boundingRect(POLY_rough[i]);
	Rect x, y, w, h = boundingRect(POLY_rough[i]);
	//rectangle(dst,Point(bound.br().x,bound.br().y), Point(bound.tl().x,bound.tl().y),(0, 0, 255),2);

	 RotatedRect rectPoint = minAreaRect(POLY_rough[i]);
     //定义一个存储以上四个点的坐标的变量
     Point2f fourPoint2f[4];
     //将rectPoint变量中存储的坐标值放到 fourPoint的数组中
     rectPoint.points(fourPoint2f);
	 //将坐标放到向量中
	 vector<Point> fourPoint;
	 for(int i = 0; i <4; i++)
	 {
		 fourPoint.push_back(fourPoint2f[i]);
	 }

	 //画出旋转的最小矩形
	/* for (int i = 0; i < 3; i++)
        {
            line(dst, fourPoint2f[i], fourPoint2f[i + 1], Scalar(0,0,255), 3);
        }
        line(dst, fourPoint2f[0], fourPoint2f[3], Scalar(0,0,255), 3);  */

	//计算包凸
	convexHull(POLY_rough[i], convex_hull, true);

    float area_rotated,area_rect,area_approx,difference;
	area_rect=((bound.br().x-bound.tl().x)*(bound.br().y-bound.tl().y));
	area_rotated=fabs(contourArea(fourPoint));
	area_approx=fabs(contourArea(POLY_rough[i]));
	difference=area_rotated-area_approx;
	cout<<"area"<<"\t";
	cout<<area_approx<<endl;
	cout<<area_rect<<endl;
	cout<<area_rotated<<endl;
	if(difference<2.5*area_approx && difference>0.5*area_approx)
		POLY_precise.push_back(POLY_rough[i]);
	}

	// find the biggest one
	int max=0,best=0;
	for(int i = 0; i < POLY_precise.size(); i++)
	{
		if(max<fabs(contourArea(POLY_precise[i])))
		{
			best=i;
			max=fabs(contourArea(POLY_precise[i]));
		}
	}

	//drawContours(dst, POLY_rough,-1, Scalar(255,0,0), 2);
	//drawContours(dst, POLY_precise,-1, Scalar(0,255,0), 2);
	drawContours(dst, POLY_precise,best, Scalar(0,0,255), 2);
	namedWindow("dst",CV_WINDOW_NORMAL);
	imshow("dst", dst);


	while(uchar(waitKey(1))!='Q');


                    }
	                  	                }
	                  	                   closedir (dir);
	                  	               } else {
	                  	                   cout<<"not present"<<endl;
	                  	                  }
	     return 0;
	 }

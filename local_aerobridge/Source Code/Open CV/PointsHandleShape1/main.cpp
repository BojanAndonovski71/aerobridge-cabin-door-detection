/*
straight arrow detect in airplane door
author: Cheng Yupeng (Neil)
version: 3st
describe: this code is to detect the doorhandle of air plane MAS B737
*/

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

#include<iostream>
#include<fstream>
#include<vector>
#include <stdlib.h>
#include<dirent.h>
#include<string.h>

#include<Windows.h>
#include<Winbase.h>
#include<TCHAR.h>

 using namespace cv;
 using namespace std;

 static Mat src; Mat ROI_gray;
 static Mat imageROI;
 static float n,m,ratio;             //前两点和高度之比
 static float a,b,ratio_shape;       //长宽之比
 static float c,d,ratio_shape_h;
 static float e,f,ratio_shape_best;
 static int best=0;
 static int muchpoly;

 static vector<Point> model;
 static int template_number=0;
 static int matched=0;



string GetFilename (const string& str)
{
  size_t found;
  string file;
  found=str.find_last_of("/\\");
  file = str.substr(found+1);
  return file;
}

string read_value(string section, string key, string config_path)
{
	char read_value[100];
	DWORD len = 100;

	char section_char[100],key_char[100],config_path_char[100];
	strcpy(section_char, section.c_str());
	strcpy(key_char, key.c_str());
	strcpy(config_path_char, config_path.c_str());
	GetPrivateProfileString(TEXT(section_char), TEXT(key_char),TEXT("Not Found"),read_value,len,TEXT(config_path_char));

	return read_value;
}

string GetFoldername (const string& str)
{
	size_t found;
	string folder;
	found=str.find_last_of("/\\");
	folder = str.substr(0,found);
	return folder;
}

template <typename T> string tostr(const T& t) {
   ostringstream os;
   os<<t;
   return os.str();
}
 int main(int argc,char** argv)
 {


	  IplImage *desimg,*srcimg;
		//string dirName = "C:/Users/bojan/Desktop/TestFeatures/MASB737/";
		//string dirName = "C:/Users/bojan/Desktop/TestFeatures/ANAB787/" "/";
	 string dirName = argv[1];//
	  //string dirName = "C:/Users/bojan/Desktop/TestFeatures/B787/000133.png";
	  //string dirName = "C:/Users/bojan/Desktop/TestFeatures/MASB737/";
	 // string dirName = "C:/Users/bojan/Desktop/TestFeatures/MASB737/000203.png";
		//string dirName = "C:/Users/bojan/Desktop/TestFeatures/A320/" ;
		//string dirName = "C:/Users/bojan/Desktop/TestFeatures/ScootB787PBB/";
		//string dirName = "C:/Users/bojan/Desktop/TestFeatures/ScootB787/";
		//string dirName = "C:/Users/bojan/Desktop/TestFeatures/SQB772/";
		 // string dirName = "C:/Users/bojan/Desktop/TestFeatures/SQB772Rain/";
		//string dirName = "C:/Users/bojan/Desktop/TestFeatures/TigerAirA320/000151.png" ;
	   // string dirName = "C:/Users/bojan/Desktop/TestFeatures/TigerAirA320Rain" "/";
		//string dirName = "C:/Users/bojan/Desktop/TestFeatures/SilkAirB737" "/";

		 	string imageName = GetFilename(dirName);
		 	string temp = GetFoldername(dirName);
		 	string flightName = GetFilename(temp);

			DIR *dir;
			 //dir = opendir(dirName.c_str());
			 string imgName;
			 struct dirent *ent;
			 if (true) {
			       // while ((ent = readdir (dir)) != NULL) {
			              //imgName= ent->d_name;
			              if(true)
			                        {
			                          string aux=dirName ;
			                         // aux.append(dirName);
			                          //aux.append(imgName);

 	                                 Mat src= imread(aux);
 	                                 //imshow(aux,src);
 	                                 waitKey(0);

/*
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
 	                               	                         		 	                           Mat image= imread(aux);
 	                               	                         		 	                          waitKey(0);
*/


 	   //build the template
 	   string template_handle1_str = read_value("DoorHandle_exe","template_handle1","C:/Config/Config.ini");
 	   // cout<<template_handle1_str<<endl;
 	   cv::Mat src_T = imread(template_handle1_str);

 	    //Mat src_T = imread("template-1.png");
 		Mat gray_T,bw_T;
 		Mat src_gray_T;
 		GaussianBlur(src_T, src_gray_T, Size(3, 3), 2, 2);
 		try
 				{
 					cvtColor(src_gray_T,gray_T,CV_BGR2GRAY);
 				}
 				catch( cv::Exception& e )
 				{
 				    const char* err_msg = e.what();
 				    std::cout << "exception caught: " << err_msg << std::endl;
 				    cout<<flightName<<";"<<imageName<<";"<<"0"<<endl;
 				    return 0;
 				}
 	 	//cvtColor(src_gray_T,gray_T,CV_BGR2GRAY);
 		Canny(gray_T,bw_T,1200,600,5,true);
 		vector<vector<Point> > template_handle;
 	    vector<Vec4i> hierarchy_T;
 		findContours(bw_T.clone(), template_handle,hierarchy_T,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
 		//cout<<template_handle.size()<<endl;
 		float max=0;
 		string pixel;
 		for (int i=0;i<template_handle.size();i++)
 		{
 			float area;
 			area= contourArea(template_handle[i]);
 			if( area > max)
 			{
 			    max=area;
 				template_number=i;
 		     }
 		}
 		approxPolyDP(Mat(template_handle[template_number]), model, arcLength(Mat(template_handle[template_number]), true)*0.0095, true);

 		Mat template_handle_p = Mat::zeros(src_T.rows,src_T.cols, CV_8UC1);
 		drawContours(template_handle_p, template_handle,template_number, Scalar(255),2,8,hierarchy_T);
 		//namedWindow("template_handle",CV_WINDOW_NORMAL);
 		//imshow("template_handle",template_handle_p);


 	    //input the image
 	 	//Mat src = imread("000186.png");
 		//namedWindow("origin",CV_WINDOW_NORMAL);
 		//imshow("origin",src);


 	    //define the interesting region
 		imageROI=src(Rect(0,0,src.cols,0.7*(src.rows)));
 /*		namedWindow("ROI",CV_WINDOW_NORMAL);
 		imshow("ROI",imageROI)*/;


 		// get the edge of the input image
 		Mat gray,bw;
 		GaussianBlur(imageROI, ROI_gray, cv::Size(3, 3), 2, 2);
 	 	cvtColor(ROI_gray,gray,CV_BGR2GRAY);
 		Canny(gray,bw,1200,600,5,true);
 		//namedWindow("canny",CV_WINDOW_NORMAL);
 		//imshow("canny",bw);

 		//close operation
 		Mat src_close;
 		Size kernalSize (9,9);
 	 	Mat element = getStructuringElement (MORPH_ELLIPSE, kernalSize, Point(1,1)  );
 	 	morphologyEx( bw, src_close, MORPH_CLOSE, element );
 	/*	namedWindow("morphology",CV_WINDOW_NORMAL);
 		imshow("morphology",src_close);*/

 		//find the contours
 	 	vector<vector<Point> > contours;
 	    vector<cv::Vec4i> hierarchy;

 		findContours(src_close.clone(), contours,hierarchy,CV_RETR_TREE,CHAIN_APPROX_SIMPLE);
 		//cout<<hierarchy.size()<<endl;

 		//show the contours of the input image
 		Mat savedGrayMat = Mat::zeros(src.rows,src.cols, CV_8UC1);
 		drawContours(savedGrayMat, contours,-1, Scalar(255), 2,8,hierarchy);
 	/*	namedWindow("contours",CV_WINDOW_NORMAL);
 		imshow("contours",savedGrayMat);*/


 		//approximate of the contours,and get the convex
 		vector<vector<Point> > POLY;
 		vector<vector<Point> > POLY_precise;
 		vector<vector<Point> > POLY_rough;
 		vector<vector<Point> > POLY_best;
 		vector<Point> approx;
 		vector<Point> convex_hull;

 		//general detection the handle
 		Mat dst = src.clone();
 		for (int i = 0; i < contours.size(); i++)
 		{

 			approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.0095, true);
 			POLY.push_back(approx);
 			convexHull(approx, convex_hull, true);
 	        if (fabs(contourArea(contours[i])) <3000 || fabs(contourArea(contours[i])) >10000 )                                      //将面积太小的轮廓直接跳过  注意continue用法
 			continue;

 			//计算外接矩形和本身轮廓的面积
 		    Rect bound=boundingRect(contours[i]);
 			float area_rect,area_approx,difference;
 		    area_rect=((bound.br().x-bound.tl().x)*(bound.br().y-bound.tl().y));
 			area_approx=fabs(contourArea(convex_hull));
 			difference=area_rect-area_approx;
 		    //cout<<"area"<<"\t";
 			//cout<<area_rect<<endl;
 			//cout<<area_approx<<endl;


             // general filter the handle from all the contours according to the area and the ratio.
 				 int k;
 				 int m=hierarchy[i][3];

 				 if(convex_hull.size() < approx.size()  && difference>(0.5*area_approx))
 				 {

 					          k=contours[i].size();
 					          //cout<<"child size"<<"\t"; cout<<k<<endl;

 				              int len;                                          //for this approx
 							  len=approx.size();
 							  int   *num=new   int[len];
 							  int   *nums=new   int[len];

 							   for(int   j=0;j<len;j++)
                                {
                                   num[j]=approx.at(j).y;
                                   nums[j]=approx.at(j).x;
                                }
 							   sort(num,num+len);              //高度
 							   sort(nums,nums+len);             //宽度

 							   a=num[len-1]-num[0];
 							   b=nums[len-1]-nums[0];
 							   ratio_shape=b/a;

 							   //cout<<"ratio"<<"\t";
 							   //cout<<ratio_shape<<endl;

 							   if(ratio_shape>2 && ratio_shape<5 )
 							   {
 								  //cout<<"catch a rectangle"<<endl;
 								  //cout<<"poly"<<"\t"; cout<<approx.size()<<endl;
 								  //cout<<approx<<endl;

 								  //cout<<ratio_shape<<endl;
 								  POLY_rough.push_back(contours[i]);
 							   }
 				 }
 		}
 	    //filter the handle
 		double comparing;
 		double min=20;
 		for(int j = 0; j < POLY_rough.size(); j++)
 		{
 			 comparing = matchShapes(model, POLY_rough[j],2, 0.0);
 			  if ( comparing != 0 && comparing<min)
 			  {
 				  min=comparing;
 				  matched=j;
 			  }
 		}


         if(POLY_rough.size()!=0)
         {
 		vector<Moments> mu(1);
 		vector<Point2f> mc(1);
 	    // compute the central momment
        mu[0] = moments( POLY_rough[matched], false );
        mc[0] = Point2f( mu[0].m10/mu[0].m00 , mu[0].m01/mu[0].m00 );
 	    circle( dst, mc[0], 8, Scalar(255,0,255), -1, 8, 0 );
 	    //cout<<mc;
 	    ofstream fout("C:/Users/bojan/Desktop/TestFeatures/Results/handle_center.txt", ios::app);
 	    fout<<dirName<<";";
 	    fout<<imgName<<";";
 	    fout<<"1"<<";";
 	    fout<<mc<<endl;

 	   ///cout<<flightName<<";"<<imageName<<";"<<"1"<<";"<<mc<<endl; ovaaaaaaaaaaaaaaaaa
 	   cout<<flightName<<";"<<imageName<<";"<<"1";
 	    //cout<<"1";
 	    //return 1;

 		int right_point, right=0,left_point, left=100000;
 	    for(int j=0;j<POLY_rough[matched].size();j++)
 		{

 		   if (right<POLY_rough[matched].at(j).x)
 		  {
 			   right=POLY_rough[matched].at(j).x;
 			   right_point=j;
 		   }
 			if (left>POLY_rough[matched].at(j).x)
 		  {

 			   left=POLY_rough[matched].at(j).x;
 		       left_point=j;
 		   }
 		}
     	//circle( dst, POLY_rough[matched].at(right_point), 4, Scalar(0,0,255), -1, 8, 0 );
 		//circle( dst, POLY_rough[matched].at(left_point), 4, Scalar(0,0,255), -1, 8, 0 );

 		float width=0, high=0;
 		width= POLY_rough[matched].at(right_point).x - POLY_rough[matched].at(left_point).x;
 		high=0.2*width;
 		cout<<";"<<mc;

 		//circle( dst, Point(POLY_rough[matched].at(left_point).x,mc[0].y-0.5*high), 4, Scalar(0,0,255), -1, 8, 0 );//2
 		//circle( dst, Point(POLY_rough[matched].at(right_point).x,mc[0].y-0.5*high), 4, Scalar(0,0,255), -1, 8, 0 );//3
 		//circle( dst, Point(mc[0].x,mc[0].y-high), 8, Scalar(0,0,255), -1, 8, 0 );//1
 		//circle( dst, Point(POLY_rough[matched].at(left_point).x,mc[0].y+0.5*high), 4, Scalar(0,0,255), -1, 8, 0 ); //4
 		//circle( dst, Point(POLY_rough[matched].at(right_point).x,mc[0].y+0.5*high), 4, Scalar(0,0,255), -1, 8, 0 );//5
 		//circle( dst, Point(mc[0].x,mc[0].y+high), 4, Scalar(0,0,255), -1, 8, 0 );

 		line(dst,Point(POLY_rough[matched].at(left_point).x,mc[0].y-0.5*high),Point(mc[0].x,mc[0].y-high), Scalar(0,255,0));
 		line(dst,Point(mc[0].x,mc[0].y-high), Point(POLY_rough[matched].at(right_point).x,mc[0].y-0.5*high),Scalar(0,255,0));
 		line(dst,Point(POLY_rough[matched].at(right_point).x,mc[0].y-0.5*high),Point(POLY_rough[matched].at(right_point).x,mc[0].y+0.5*high),Scalar(0,255,0));
 		line(dst,Point(POLY_rough[matched].at(right_point).x,mc[0].y+0.5*high),Point(mc[0].x,mc[0].y+high),Scalar(0,255,0));
 		line(dst,Point(mc[0].x,mc[0].y+high),Point(POLY_rough[matched].at(left_point).x,mc[0].y+0.5*high),Scalar(0,255,0));
        line(dst,Point(POLY_rough[matched].at(left_point).x,mc[0].y+0.5*high),Point(POLY_rough[matched].at(left_point).x,mc[0].y-0.5*high),Scalar(0,255,0));
       // float w=pow((POLY_rough[matched].at(right_point).x - POLY_rough[matched].at(left_point).x )*(POLY_rough[matched].at(right_point).x - POLY_rough[matched].at(left_point).x)
        //+ (POLY_rough[matched].at(right_point).y - POLY_rough[matched].at(left_point).y )*(POLY_rough[matched].at(right_point).y - POLY_rough[matched].at(left_point).y),0.5);
        if (!pixel.empty()){
               	   	   pixel.append("-");
               	   	      	  	  	  }
               	   	    	 pixel.append(tostr(width));
               	   	         pixel.append(";");
               	   	         pixel.append(tostr(high));



        /*
 		ofstream mycout("D:/mobility lab/任务7/POINT VERSION 2//hello.txt",ios::app);

 		 mycout<<"file"<<"\t";
         mycout<<imgName<<"\t";
 	     mycout<<"point1"<<"\t";
         mycout<<Point(POLY_rough[matched].at(left_point).x,mc[0].y-0.5*high)<<"\t";
 		 mycout<<"point2"<<"\t";
         mycout<<Point(mc[0].x,mc[0].y-high)<<"\t";
 		 mycout<<"point3"<<"\t";
         mycout<<Point(POLY_rough[matched].at(right_point).x,mc[0].y-0.5*high)<<"\t";
 		 mycout<<"point4"<<"\t";
         mycout<<Point(POLY_rough[matched].at(right_point).x,mc[0].y+0.5*high)<<"\t";
 		 mycout<<"point5"<<"\t";
         mycout<<Point(mc[0].x,mc[0].y+high)<<"\t";
 		 mycout<<"point6"<<"\t";
         mycout<<Point(POLY_rough[matched].at(left_point).x,mc[0].y+0.5*high)<<"\t";
 		 mycout<<endl;
 		 */
 		}
         cout<< ":"<<pixel;
         cout<<endl;

 		if(POLY_rough.size()==0)
 	{
 			     //ofstream fout("C:/Users/bojan/Desktop/TestFeatures/Results/handle_center.txt", ios::app);
 			 	 //fout<<dirName<<";";
 			 	 ///fout<<imgName<<";";
 			 	 //fout<<"0"<<endl;

 			 cout<<flightName<<";"<<imageName<<";"<<"0"<<endl;
 	    //return 0;
 	}

 	//Mat result(src.size(),CV_8U,cv::Scalar(255));
 	//drawContours(dst, POLY_rough,-1, Scalar(0,0,255), 2);
 	//drawContours(dst, POLY_rough,matched, Scalar(0,255,0), 2);
 	//drawContours(dst, POLY_best,-1, Scalar(0,255,0), 2);
 	//imwrite( "D:/mobility lab/任务2 箭头/day 20  straight arrow detection/result/185.png", dst);
 	namedWindow("dst",CV_WINDOW_NORMAL);
 	imshow("dst", dst);
 	//while(uchar(waitKey(1))!='Q');

                     }
 	                  	              // }
 	                  	                 //  closedir (dir);
 	                  	               } else {
 	                  	                  // cout<<"not present"<<endl;
 	                  	                  }
 	     return 0;
 	 }

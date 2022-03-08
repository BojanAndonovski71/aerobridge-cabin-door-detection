/*
straight arrow detect in airplane door
author: Cheng Yupeng (Neil)
version: 3rd
describe: detect the big banded arrow, and mark the five points which build up the arrow
*/

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

#include<fstream>
#include<iostream>
#include<vector>
#include <stdlib.h>
#include<dirent.h>
#include<string.h>
#include<math.h>
#include<Windows.h>
#include<Winbase.h>
#include<TCHAR.h>

using namespace cv;
using namespace std;

static Mat src; Mat src_gray;
static float n,m,ratio;             //前两点和高度之比
static float a,b,ratio_shape;       //长宽之比
static float c,d,ratio_shape_h;
static float e,f,ratio_shape_best;
static int best=0;
static int muchpoly;

static vector<Point> model;
static int template_number=0;
static int matched=0;

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

string GetFilename (const string& str)
{
  size_t found;
  string file;
  found=str.find_last_of("/\\");
  file = str.substr(found+1);
  return file;
}

string GetFoldername (const string& str)
{
	size_t found;
	string folder;
	found=str.find_last_of("/\\");
	folder = str.substr(0,found);
	return folder;
}

static float Angle(Point cen, Point first, Point second)
{
    float dx1=0, dx2=0, dy1=0, dy2=0;
    float angle=0;

    dx1 = first.x - cen.x;
	//dx1=fabs(dx1);
    dy1 = first.y - cen.y;
   // dy1=fabs(dy1);
    dx2 = second.x - cen.x;
   // dx2=fabs(dx2);
    dy2 = second.y - cen.y;
   // dy2=fabs(dy2);
    float c = (float)sqrt(dx1 * dx1 + dy1 * dy1) * (float)sqrt(dx2 * dx2 + dy2 * dy2);

    if (c == 0) return -1;

    angle = (float)acos((dx1 * dx2 + dy1 * dy2) / c);


    return angle;
}


int main(int argc,char *argv[])
{
	cout<<"************************Detect Curved Arrow*************************"<<endl;
	 IplImage *desimg,*srcimg;
	 string config_path = argv[1];//"C:\\Config\\Config.ini";
	 string path = argv[2];
	 string dirName = GetFoldername(path);//read_value("Aerobridge","dummy",config_path);
	 string flightName = GetFilename(dirName);
	 cout<<flightName<<endl;

	 DIR *dir;
	 dir = opendir(dirName.c_str());
	 string imgName;
	 struct dirent *ent;

	 if (dir != NULL)
	 {
	         while ((ent = readdir (dir)) != NULL)
	         {
	              imgName= GetFilename(path);//ent->d_name;
	              if(imgName.compare(".")!= 0 && imgName.compare("..")!= 0)
	                        {
	                          string aux;
	                          aux.append(dirName);
	                          aux.append("\\");
	                          aux.append(imgName);
	                          //aux.append(orig_path);
	                          cout << aux << endl;
	                          Mat src= imread(aux);


	                         //imshow(aux,src);
	                          waitKey(0);

		//build the template

	    string template_arrow_str = read_value("CurvedArrow","template_arrow",config_path);
	    cout<<template_arrow_str<<endl;
	    //char rect_file_char[100];
	    //strcpy(rect_file_char, rect_file_str.c_str());

	 	Mat src_T = imread(template_arrow_str);
		Mat gray_T,bw_T;
		Mat src_gray_T;
		GaussianBlur(src_T, src_gray_T, Size(3, 3), 2, 2);
	 	cvtColor(src_gray_T,gray_T,CV_BGR2GRAY);
		Canny(gray_T,bw_T,1200,600,5,true);
		vector<vector<Point> > template_handle;
	    vector<Vec4i> hierarchy_T;
		findContours(bw_T.clone(), template_handle,hierarchy_T,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		//cout<<template_handle.size()<<endl;
		float max=0;
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
		/*namedWindow("template_handle",CV_WINDOW_NORMAL);
		imshow("template_handle",template_handle_p);*/



	    //input the image
	 	//Mat src = cv::imread("000204.png");
		//namedWindow("origin",CV_WINDOW_NORMAL);
		//imshow("origin",src);

		// get the edge of the input image
		Mat gray,bw;
		GaussianBlur(src, src_gray, cv::Size(9, 9), 2, 2);
	 	cvtColor(src_gray,gray,CV_BGR2GRAY);
		Canny(gray,bw,1200,600,5,true);
	/*	namedWindow("canny",CV_WINDOW_NORMAL);
		imshow("canny",bw);*/

		//close operation
		Mat src_close;
		Size kernalSize (7,7);
	 	Mat element = getStructuringElement (MORPH_ELLIPSE, kernalSize, Point(1,1)  );
	 	morphologyEx( bw, src_close, MORPH_CLOSE, element );
		/*namedWindow("morphology",CV_WINDOW_NORMAL);
		imshow("morphology",src_close);*/

		//find the contours
		vector<vector<Point> > contours;
	    vector<cv::Vec4i> hierarchy;

		findContours(src_close.clone(), contours,hierarchy,CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		//cout<<hierarchy.size()<<endl;

		//show the contours of the input image
		Mat savedGrayMat = Mat::zeros(src.rows,src.cols, CV_8UC1);
		//drawContours(savedGrayMat, contours,-1, Scalar(255),1,8,hierarchy);
		//imshow("contours", savedGrayMat);

		//approximate of the contours,and get the convex
		vector<vector<Point> > POLY;
		vector<vector<Point> > POLY_precise;
		vector<vector<Point> > POLY_rough;
		vector<vector<Point> > POLY_best;
		vector<Point> approx;
		vector<Point> convex_hull;

		//output image
		Mat dst = src.clone();
		for (int i = 0; i < contours.size(); i++)
		{

			approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.0095, true);
			POLY.push_back(approx);
			convexHull(approx, convex_hull, true);
	        if (fabs(contourArea(approx)) <2000 || fabs(contourArea(approx))>10000)                                      //将面积太小的轮廓直接跳过  注意continue用法
			continue;

			//calculate the up bounding rectangle
			Rect bound=boundingRect(contours[i]);
			float area_rect,area_approx,difference;
		    area_rect=((bound.br().x-bound.tl().x)*(bound.br().y-bound.tl().y));
			area_approx=fabs(contourArea(contours[i]));
			difference=area_rect-area_approx;
		    //cout<<"area"<<"\t";
			//cout<<area_rect<<endl;
			//cout<<area_approx<<endl;

			// if the
			// if(POLY_precise.size()==0)
			 {
				 int k;
				 int m=hierarchy[i][3];

				 if(convex_hull.size() < approx.size() && difference<3*area_approx && difference>0.2*area_approx)
				 {

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

							   //cout<<"child ratio"<<"\t";
							   //cout<<ratio_shape_h<<endl;
							   //cout<<endl;

							   //POLY_rough.push_back(contours[m]);

							   if(ratio_shape>2 && ratio_shape<8)
							   {
								  //cout<<"catch rough one"<<endl;
								  //cout<<"poly"<<"\t"; cout<<approx.size()<<endl;
								  //cout<<approx<<endl;

								  //cout<<ratio_shape<<endl;
								  POLY_rough.push_back(approx);
							   }


				 }
			 }
		}

		if(POLY_rough.size()>0)
		{
          for(int i=0;i<POLY_rough.size();i++)
							   {



							   //find the most right point of every candidate contour
							   int right_point, right=0, left_point, left=100000;
							   int line_1_r=1, line_1_l=1;
							   int line_2_r=(-1), line_2_l=-1;
							   double right_angle=0;
							   double left_angle=0;
							   for(int j=0;j<POLY_rough[i].size();j++)
							   {
								   if (right<POLY_rough[i].at(j).x)
								   {
									   right=POLY_rough[i].at(j).x;
									   right_point=j;
								   }
								   if (left>POLY_rough[i].at(j).x)
								   {
									   left=POLY_rough[i].at(j).x;
									   left_point=j;
								   }
							   }
							   if(right_point==POLY_rough[i].size()) line_1_r=-POLY_rough[i].size()+1;
							   if(right_point==0) line_2_r=POLY_rough[i].size()-1;

							   if(left_point==POLY_rough[i].size()) line_1_l=-POLY_rough[i].size()+1;
							   if(left_point==0) line_2_l=POLY_rough[i].size()-1;

							   /*circle( dst, POLY_rough[i].at(right_point), 4, Scalar(0,0,255), -1, 8, 0 );
							   circle( dst, POLY_rough[i].at(left_point), 4, Scalar(0,0,255), -1, 8, 0 );*/

							   /*line(dst,POLY_rough[i].at(right_point),POLY_rough[i].at(right_point+line_1_r),Scalar(0,0,255),2);
							   line(dst,POLY_rough[i].at(right_point),POLY_rough[i].at(right_point+line_2_r),Scalar(0,0,255),2);*/

							   //line(dst,POLY_rough[i].at(left_point),POLY_rough[i].at(left_point+line_1_l),Scalar(0,0,255),5);
							   //line(dst,POLY_rough[i].at(left_point),POLY_rough[i].at(left_point+line_2_l),Scalar(0,0,255),5);


							//   left_angle=Angle(POLY_rough[i].at(left_point),POLY_rough[i].at(left_point+line_1_l),POLY_rough[i].at(right_point+line_2_l));

							   right_angle=Angle(POLY_rough[i].at(right_point),POLY_rough[i].at(right_point+line_1_r),POLY_rough[i].at(right_point+line_2_r));
							   right_angle=right_angle*180/3.14;
							   if (right_angle<70 && right_angle>30)
								   POLY_precise.push_back(POLY_rough[i]);

							  }

		}


		double comparing;
		double min=20;
		 for(int j=0;j<POLY_precise.size();j++)
		 {
			       //                 int max=0;
								  //  int lenb;                                          //FOR chirld contours
							   //     lenb=POLY_precise[i].size();
							   //     int   *numb=new   int[lenb];
							   //      int   *numsb=new   int[lenb];

							   //     for(int   j=0;j<lenb;j++)
          //                     {
          //                        numb[j]=POLY_precise[i].at(j).y;
          //                        numsb[j]=POLY_precise[i].at(j).x;
          //                     }
							   //sort(numb,numb+lenb);              //高度
							   //sort(numsb,numsb+lenb);             //宽度
							   //e=numb[lenb-1]-numb[0];
							   //f=numsb[lenb-1]-numsb[0];
							   //ratio_shape_best=f/e;
			       //        if(ratio_shape_best>max )
							   //{
								  // max=ratio_shape_best;
								  // best=i;
							   //}

			   comparing = matchShapes(model, POLY_rough[j],3, 0.0);
			  if ( comparing != 0 && comparing<min)
			  {
				  min=comparing;
				  best=j;
			  }
		//	 POLY_best.push_back(POLY_rough[best]);
		 }



		 //find the five point in the banded arrow
		 //find the five points
	if(POLY_precise.size()!=0)
	{
		vector<Moments> mu(1);
		vector<Point2f> mc(1);
		// compute the central momment
        mu[0] = moments( POLY_precise[best], false );
        mc[0] = Point2f( mu[0].m10/mu[0].m00 , mu[0].m01/mu[0].m00 );
	    circle( dst, mc[0], 8, Scalar(0,0,255), -1, 8, 0 );


	   int right_point, right=0,left_point, left=100000;
	   for(int j=0;j<POLY_precise[best].size();j++)
		{

		   if (right<POLY_precise[best].at(j).x)
		  {
			   right=POLY_precise[best].at(j).x;
			   right_point=j;
		   }
			if (left>POLY_precise[best].at(j).x)
		  {
			   left=POLY_precise[best].at(j).x;
		       left_point=j;
		   }
		}
    	circle( dst, POLY_precise[best].at(right_point), 8, Scalar(0,0,255), -1, 8, 0 );
		circle( dst, POLY_precise[best].at(left_point),8, Scalar(0,0,255), -1, 8, 0 );

		float distance_1,distance_2,length;
		int line_1_r=1,line_2_r=(-1);
		int middle_1, middle_2;
		int trangle_1,trangle_2;
		if(right_point==POLY_precise[best].size())  line_1_r=-POLY_precise[best].size()+1;
		middle_1= right_point+line_1_r;
		if(right_point==0) line_2_r=POLY_precise[best].size()-1;
		middle_2 = right_point+line_2_r;

		distance_1=sqrt(pow((POLY_precise[best].at(right_point).x-POLY_precise[best].at(middle_1).x),2)+pow((POLY_precise[best].at(right_point).y-POLY_precise[best].at(middle_1).y),2));
		distance_2=sqrt(pow((POLY_precise[best].at(right_point).x-POLY_precise[best].at(middle_2).x),2)+pow((POLY_precise[best].at(right_point).y-POLY_precise[best].at(middle_2).y),2));
		length=arcLength(POLY_precise[best],true);

		/*line_1_r=1;
		line_2_r=(-1);
		if(distance_1>0.05*length) trangle_1=middle_1;
		else{
			if(middle_1==POLY_precise[best].size())  line_1_r=-POLY_precise[best].size()+1;
		    trangle_1= middle_1+line_1_r;
		}
		if(distance_2>0.05*length) trangle_2=middle_2;
		else{
		    if(middle_2==0) line_2_r=POLY_precise[best].size()-1;
		    trangle_2 = right_point+line_2_r;
		}*/
		// make sure the distance between two point is bigger than threshold
		while(distance_1<0.05*length)
		{
			line_1_r=1;
			if(middle_1==POLY_precise[best].size())  line_1_r=-POLY_precise[best].size()+1;
		    trangle_1= middle_1+line_1_r;
			distance_1=sqrt(pow((POLY_precise[best].at(right_point).x-POLY_precise[best].at(trangle_1).x),2)+pow((POLY_precise[best].at(right_point).y-POLY_precise[best].at(trangle_1).y),2));
			middle_1=trangle_1;
		}
		trangle_1=middle_1;
		while(distance_2<0.05*length)
		{
			line_2_r=-1;
			if(middle_2==0) line_2_r=POLY_precise[best].size()-1;
		    trangle_2 = middle_2+line_2_r;
			distance_2=sqrt(pow((POLY_precise[best].at(right_point).x-POLY_precise[best].at(trangle_2).x),2)+pow((POLY_precise[best].at(right_point).y-POLY_precise[best].at(trangle_2).y),2));
			middle_2=trangle_2;
		}
		trangle_2=middle_2;

		circle( dst, POLY_precise[best].at(trangle_1), 8, Scalar(0,0,255), -1, 8, 0 );
		circle( dst, POLY_precise[best].at(trangle_2), 8, Scalar(0,0,255), -1, 8, 0 );

		string mycout_str = read_value("CurvedArrow","log_file",config_path);
		char mycout_char[100];
		strcpy(mycout_char, mycout_str.c_str());
		ofstream mycout(TEXT(mycout_char),ios::app);

		/*mycout<<"file"<<"\t";
        mycout<<imgName<<"\t";
	    mycout<<"point1"<<"\t";
        mycout<<POLY_precise[best].at(trangle_1)<<"\t";
		mycout<<"point2"<<"\t";
        mycout<<POLY_precise[best].at(right_point)<<"\t";
		mycout<<"point3"<<"\t";
        mycout<<POLY_precise[best].at(trangle_2)<<"\t";
		mycout<<"point4"<<"\t";
        mycout<<mc[0]<<"\t";
		mycout<<"point5"<<"\t";
        mycout<<POLY_precise[best].at(left_point)<<"\t";
		mycout<<endl;*/

		mycout<<flightName<<";"<<imgName<<";"<<"1";
		mycout<<";"<<POLY_precise[best].at(trangle_1);
		mycout<<";"<<POLY_precise[best].at(right_point);
		mycout<<";"<<POLY_precise[best].at(trangle_2);
		mycout<<";"<<mc[0];
		mycout<<";"<<POLY_precise[best].at(left_point)<<endl;
		cout<<"1"<<endl;
		return 1;
	}

	if(POLY_precise.size()==0)
	{
		string mycout_str = read_value("CurvedArrow","log_file",config_path);
		char mycout_char[100];
		strcpy(mycout_char, mycout_str.c_str());
		ofstream mycout(TEXT(mycout_char),ios::app);

		cout<<"0"<<endl;
		mycout<<flightName<<";"<<imgName<<";"<<"0"<<endl;
		return 0;

	}


		//Mat result(src.size(),CV_8U,cv::Scalar(255));

	//drawContours(dst, POLY_precise,-1, Scalar(0,0,255), 3);
	//drawContours(dst, POLY_rough,-1, Scalar(255,0,0), 2);
	drawContours(dst, POLY_precise,best, Scalar(0,255,0), 2);
	//imwrite( "D:/mobility lab/任务2 箭头/day 20  straight arrow detection/result/185.png", dst);
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






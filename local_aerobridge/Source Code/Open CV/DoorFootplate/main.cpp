#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

#include<iostream>
#include<vector>
#include <stdlib.h>
#include<dirent.h>
#include<string.h>

using namespace cv;
using namespace std;


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


 static Mat src; Mat ROI_gray;
 static Mat imageROI;
 static float n,m,ratio;
 static float a,b,ratio_shape;
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
	 	 //cout << argv[0]<<endl;


	    // string dirName = "C:/Users/bojan/Desktop/TestFeatures/MASB737/";
	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/ANAB787/" "/";
	 	string dirName = argv[1];
	 	//string dirName="C:/Users/bojan/Desktop/TestFeatures/ANAB787/000192.png";////"C:/Users/bojan/Desktop/TestFeatures/B787/000137.png";////"C:/Users/bojan/Desktop/TestFeatures/B787/000137.png";
	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/A320/000175.png" ;
	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/ScootB787PBB/";
	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/ScootB787/";
	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/SQB772/";
	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/SQB772Rain/";
	   //string dirName = "C:/Users/bojan/Desktop/TestFeatures/TigerAirA320" "/";
	   //string dirName = "C:/Users/bojan/Desktop/TestFeatures/TigerAirA320Rain" "/";
	   //string dirName = "C:/Users/bojan/Desktop/TestFeatures/SilkAirB737" "/";

	 string imageName = GetFilename(dirName);
	 string temp = GetFoldername(dirName);
	 string flightName = GetFilename(temp);



	 DIR *dir;
		 dir = opendir(dirName.c_str());
		 string imgName;
		 struct dirent *ent;

		 if (true) {
		        // while ((ent = readdir (dir)) != NULL) {
		              //imgName= ent->d_name;
		              if(true)
		                        {
		                          string aux =dirName;
		                          //aux.append(dirName);
		                          //aux.append(imgName);
		                          //cout << aux << endl;
		                          Mat image= imread(aux);
		                          //imshow(aux,image);
		                          waitKey(0);

	                          //build the template
	                          string template_footplate_str = read_value("DoorFootplate","template_footplate","C:/Config/Config.ini");

	                          	  //cout<<template_footplate_str<<endl;
							        cv::Mat src_T = imread(template_footplate_str);
	                          		Mat gray_T,bw_T;
	                          		Mat src_gray_T;
	                          		GaussianBlur(src_T, src_gray_T, Size(3,3), 2, 2);
	                          	 	cvtColor(src_gray_T,gray_T,CV_BGR2GRAY);
	                          		Canny(gray_T,bw_T,1200,600,5,true);
	                          		vector<vector<Point> > template_handle;
	                          	    vector<Vec4i> hierarchy_T;
	                          	    string pixel;
	                          		findContours(bw_T.clone(), template_handle,hierarchy_T,CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
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
	                          		approxPolyDP(Mat(template_handle[template_number]), model, arcLength(Mat(template_handle[template_number]), true)*0.000711, true);

	                          		Mat template_handle_p = Mat::zeros(src_T.rows,src_T.cols, CV_8UC1);
	                          		//drawContours(template_handle_p, template_handle,template_number, Scalar(255),2,8,hierarchy_T);
	                          		//namedWindow("template_handle",CV_WINDOW_NORMAL);
	                          		//imshow("template_handle",template_handle_p);

	                          				//input the image
	                          			 	//Mat src = imread("000210.png");
	                          				//namedWindow("origin",CV_WINDOW_NORMAL);
	                          				//imshow("origin",src);

	                          				//define the interesting region
	                          				imageROI=image(Rect(0,0,image.cols,1*(image.rows)));
	                          				//namedWindow("ROI",CV_WINDOW_NORMAL);
	                          				//imshow("ROI",imageROI);

	                          				// get the edge of the input image
	                          				Mat gray,bw;
	                          				GaussianBlur(imageROI, ROI_gray, cv::Size(3,3), 2,2);
	                          				//medianBlur(imageROI, ROI_gray, 1);
	                          			    //blur( imageROI, ROI_gray, Size(1,1) );
	                          			 	cvtColor(ROI_gray,gray,CV_BGR2GRAY);
	                          				Canny(gray,bw,1200,600,5,true);
	                          				//namedWindow("canny",CV_WINDOW_NORMAL);
	                          				//imshow("canny",bw);

	                          				//close operation
	                          				Mat src_close;
	                          				Size kernalSize (3,3);
	                          			 	//Mat element = getStructuringElement (MORPH_ELLIPSE, kernalSize, Point(2,2));
	                          			 	Mat element = getStructuringElement (MORPH_RECT, kernalSize, Point(2,2));
	                          			 	//morphKernel = getStructuringElement(MORPH_RECT, Size(3,3));
	                          			 	//morphKernel = getStructuringElement(MORPH_RECT, Size(5, 1));
	                          			 	morphologyEx( bw, src_close, MORPH_CLOSE, element );
	                          				//namedWindow("morphology",CV_WINDOW_NORMAL);
	                          				//imshow("morphology",src_close);

	                          				//find the contours
	                          				vector<vector<Point> > contours;
	                          			    vector<cv::Vec4i> hierarchy;

	                          				findContours(src_close.clone(), contours,hierarchy,CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
	                          				//cout<<hierarchy.size()<<endl;

	                          				//show the contours of the input image
	                          				Mat savedGrayMat = Mat::zeros(image.rows,image.cols, CV_8UC1);
	                          				drawContours(savedGrayMat, contours,-1, Scalar(255), 1,8,hierarchy);
	                          				//namedWindow("contours",CV_WINDOW_NORMAL);
	                          				//imshow("contours",savedGrayMat);


	                          				//approximate of the contours,and get the convex
	                          				vector<vector<Point> > POLY;
	                          				vector<vector<Point> > POLY_precise;
	                          				vector<vector<Point> > POLY_rough;
	                          				vector<vector<Point> > POLY_best;
	                          				vector<Point> approx;
	                          				vector<Point> convex_hull;
	                          				bool found=false;

	                          				//general detection the handle
	                          				Mat dst = image.clone();
	                          				for (int i = 0; i < contours.size(); i++)
	                          				{

	                          					 	 	  vector<Moments> mu(1);
	                          							  vector<Point2f> mc(1);
	                          							  int gray_level;
	                          						      // compute the central momment
	                          					          mu[0] = moments( contours[i], false );
	                          					          mc[0] = Point2f( mu[0].m10/mu[0].m00 , mu[0].m01/mu[0].m00 );

	                          					approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.00071, true);
	                          					POLY.push_back(approx);
	                          					convexHull(approx, convex_hull, true);
	                          			        if (fabs(contourArea(contours[i])) <9000 || fabs(contourArea(contours[i])) >90000 )                                      //将面积太小的轮廓直接跳过  注意continue用法
	                          					continue;
	                          		            // general filter the handle from all the contours according to the area and the ratio.
	                          						 int k;
	                          						 int m=hierarchy[i][3];

	                          						 if( convex_hull.size() < approx.size() && m != -1 )
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

	                          									   if(ratio_shape>2.209 && ratio_shape<120.3 && b>350  && b<700 && a>40  && a<80)
	                          									   {
	                          										  //cout<<"catch a rectangle"<<endl;
	                          										  //cout<<"poly"<<"\t"; cout<<approx.size()<<endl;
	                          										  //cout<<approx<<endl;

	                          										  //cout<<ratio_shape<<endl;
	                          										  POLY_rough.push_back(convex_hull);
	                          									   }
	                          						 }
	                          				}
	                          			    //filter the handle
	                          				double comparing;
	                          				double min=615;
	                          				for(int j = 0; j < POLY_rough.size(); j++)
	                          				{
	                          					                if(j == 0){
	                          					                	cout<<flightName<<";"<<imageName<<";"<<"1";
	                          					                		}




	                          					comparing = matchShapes(model, POLY_rough[j],2, 0.0);
	                          					  if ( comparing != 0 && comparing<min)
	                          					  {

	                          						 //novo
	                          							        	RotatedRect final_rect = minAreaRect(POLY_rough[j]);
	                          							        	Point2f final_point[4];
	                          							        	final_rect.points(final_point);
	                          							        	vector<Point> finalpoint;
	                          							        	vector<Point> drawpoint;
	                          							        	// end novo
	                          							        	Vector<int> h;
	                          				Vector<int> w;
	                          				for(int l = 0; l <4; l++)
	                          				 {
	                          				   h.push_back(final_point[l].y);
	                          					     }
	                          				for(int l = 0; l <4; l++)
	                          				 {
	                          				   w.push_back(final_point[l].x);
	                          				 }
	                          				sort(h.begin(),h.end());
	                          				sort(w.begin(),w.end());
	                          				for(int k= 0; k<4 ; k++)
	                          				{
	                          				if(h[0] == h[1])
	                          				h[1] = h[1]+1;
	                          				if(h[2] == h[3])
	                          				h[3] == h[3]+1;
	                          				for(int m = 0; m <4; m++)
	                          				{
	                          				int number =final_point[m].y;
	                          				if ( number == h[k])
	                          				{
	                          				finalpoint.push_back(final_point[m]);
	                          				}
	                          					}
	                          			}
	                          			if (finalpoint[0].x < finalpoint[1].x)
	                          			{
	                          			drawpoint.push_back(finalpoint[0]);
	                          			drawpoint.push_back(finalpoint[1]);
	                          			}
	                          			else
	                          			{
	                          			drawpoint.push_back(finalpoint[1]);
	                          			drawpoint.push_back(finalpoint[0]);
	                          			}
	                          			if (finalpoint[2].x < finalpoint[3].x)
	                          			{
	                          			drawpoint.push_back(finalpoint[3]);
	                          			drawpoint.push_back(finalpoint[2]);
	                          			}
	                          			else
	                          			{
	                          		  drawpoint.push_back(finalpoint[2]);
	                          			drawpoint.push_back(finalpoint[3]);
	                          			}

	                          			circle( dst, drawpoint[0], 10, Scalar(255,0,255), -1, 8, 0 );
	                          			circle( dst, drawpoint[1], 10, Scalar(255,0,255), -1, 8, 0 );
	                          			circle( dst, drawpoint[2], 10, Scalar(255,0,255), -1, 8, 0 );
	                          			circle( dst, drawpoint[3], 10, Scalar(255,0,255), -1, 8, 0 );

	                          			 float width=pow((drawpoint[1].x - drawpoint[0].x )*(drawpoint[1].x - drawpoint[0].x) + (drawpoint[1].y - drawpoint[0].y )*(drawpoint[1].y - drawpoint[0].y),0.5);
	                          			 float height=pow((drawpoint[2].x - drawpoint[1].x )*(drawpoint[2].x - drawpoint[1].x) + (drawpoint[2].y - drawpoint[1].y )*(drawpoint[2].y - drawpoint[1].y),0.5);
	                          		     //cout<<width<<";"<<height<<"-";
	                          						  min=comparing;
	                          						  matched=j;
	                          						  	  	  Moments mu;
	                          								  Point2f mc;
	                          								  mu = moments( POLY_rough[j], false );
	                          								  mc = Point2f( mu.m10/mu.m00 , mu.m01/mu.m00 );
	                          								  //circle( dst, mc, 12, Scalar(255,0,0), -1, 8, 0 );
	                          								  //ofstream mycout("C:/Users/bojan/Desktop/TestFeatures/Results/moments_footplate1.txt",ios::app);
	                          								  //mycout<<mc;
	                          								  //cout<<mc<<endl;;
	                          								  //cout<<"1"<<endl;;
	                          								 found=true;
	                          								cout<<";"<<mc;
	                          								//cout<<flightName<<";"<<imageName<<";"<<"1";
	                          								//cout<<";"<<mc;
	                          								 //cout<<"1"<<";"<<mc;
	                          								if (!pixel.empty()){
	                          									                			  pixel.append("-");
	                          									                			     }
	                          									                			  pixel.append(tostr(width));
	                          									                			  pixel.append(";");
	                          									                			  pixel.append(tostr(height));


	                          					  }

	                          					}
	                          				cout<< ":"<<pixel;
	                          			    cout<<endl;
	                       	             if(found==false){
	                       	            	 cout<<flightName<<";"<<imageName<<";"<<"0"<<endl;
	                       	             }
	                          			drawContours(dst, POLY_rough,matched, Scalar(0,255,0), 2);
	                          			//drawContours(dst, POLY_best,-1, Scalar(0,255,0), 2);
	                          			//imwrite( "D:/mobility lab/任务2 箭头/day 20  straight arrow detection/result/185.png", dst);
	                          			namedWindow("dst",CV_WINDOW_NORMAL);
	                          			imshow("dst", dst);


	                        }
	                        		 		                  	                   //}
	                        		 		                  	                   //closedir (dir);
	                        		 		                  	               } else {
	                        		 		                  	                   cout<<"not present"<<endl;
	                        		 		                  	                  }
	                        		 		     return 0;
	                        	}

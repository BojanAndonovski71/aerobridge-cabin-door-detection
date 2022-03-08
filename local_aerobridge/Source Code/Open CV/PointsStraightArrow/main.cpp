/*
straight arrow detect in airplane door
author: Cheng Yupeng (Neil)
version: 3rd
describe: detect the straight arrow and mark the four point in the arrow
*/

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>
#include <opencv2/legacy/compat.hpp>

#include<fstream>
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
static int muchpoly;

#include<Windows.h>
#include<Winbase.h>
#include<TCHAR.h>

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
	 //import image by string
	 IplImage *desimg,*srcimg;
	 	 	//string dirName = "D:/mobility lab/New folder/testing/DataCollect_2016-5-10-23-20-12 SilkAir B737 Docking/";
	 	 	//string dirName = "D:/mobility lab/New folder/testing/DataCollect_2016-4-27-6-52-56 - SQ B772 - PBB Docking/";
	 	    //string dirName = "C:/Users/bojan/Desktop/TestFeatures/MASB737/";
	 	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/ANAB787/" "/";
	 	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/B787/" "/";
	        string dirName = argv[1];
	        //string dirName = "C:/Users/bojan/Desktop/TestFeatures/A320/000185.png";
	        //string dirName = "C:/Users/bojan/Desktop/TestFeatures/MASB737/";
	 	 	//string imageName = GetFilename(dirName);
	 	 	//string temp = GetFoldername(dirName);
	 	 	//string flightName = GetFilename(temp);

	        string imageName = GetFilename(dirName);
	        string temp = GetFoldername(dirName);
	        string flightName = GetFilename(temp);

	        DIR *dir;
	 	 	//dir = opendir(dirName.c_str());
	 	 	string imgName;
	 	 	struct dirent *ent;

	 	 	if (true) {
	 	 		         //while ((ent = readdir (dir)) != NULL) {
	 	 		              //imgName= ent->d_name;
	 	 		              if(true)
	 	 		                        {
	 	 		                          string aux=dirName;
	 	 		                          //aux.append(dirName);
	 	 		                          //aux.append(imgName);
	 	 		                          //cout << aux << endl;
	                          Mat src= imread(aux);
	                          //imshow(aux,src);
	                          waitKey(0);

	      /*  DIR *dir;
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
	        	                           	                               	                         		 	                          //cout << aux << endl;
	        	                           	                               	                         		 	                           Mat src= imread(aux);
	        	                           	                               	                         		 	                          waitKey(0);
	        	                           	                               	                         		 	                          */
	    //close operation
		Mat src_close;
		Size kernalSize (3,3);
	 	Mat element = getStructuringElement (MORPH_CROSS, kernalSize, Point(1,1)  );
	 	morphologyEx( src, src_close, MORPH_CLOSE, element );
		//get the edge of the input image
		Mat gray,bw;
		GaussianBlur(src_close, src_gray, cv::Size(9, 9), 2, 2);
	 	cvtColor(src_gray,gray,CV_BGR2GRAY);
		Canny(gray,bw,1200,600,5,true);
		//find the contours
		vector<vector<Point> > contours;
		findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		//show the contours of the input image
		Mat savedGrayMat = Mat::zeros(src.rows,src.cols, CV_8UC1);
		drawContours(savedGrayMat, contours,-1, Scalar(255), CV_FILLED);

		//approximate of the contours,and get the convex
		vector<vector<Point> > POLY;
		vector<vector<Point> > POLY_precise;
		vector<vector<Point> > POLY_rough;
		vector<Point> approx;
		vector<Point> convex_hull;
		//output image
		string pixel;
		Mat dst = src.clone();
		for (int i = 0; i < contours.size(); i++)
		{

			approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.0095, true);
			POLY.push_back(approx);
			convexHull(approx, convex_hull, true);
	        if (fabs(contourArea(contours[i])) <600 )
			continue;
			 if(POLY_precise.size()==0)
			 {
				 if(approx.size()>4)
				 {
				              int len;
							  len=approx.size();
							  int   *num=new   int[len];
							  int   *nums=new   int[len];
							   for(int   j=0;j<len;j++)
                               {
                                  num[j]=approx.at(j).y;
                                  nums[j]=approx.at(j).x;
                               }
							   sort(num,num+len);
							   sort(nums,nums+len);
							   a=num[len-1]-num[0];
							   b=nums[len-1]-nums[0];
							   ratio_shape=a/b;
							   if(ratio_shape>3)
							   {
								  //cout<<"catch rough one"<<endl;
								  //cout<<ratio_shape<<endl;
								  POLY_rough.push_back(approx);
							   }
				 }
			 }
			 }

			 // find the 4 point of the arrow
			 if(POLY_rough.size()!=0 && POLY_precise.size()==0)
			 {

				 //for(int i = 0; i < POLY_rough.size(); i++)
				  for(int i = 0; i < 1; i++)
				 {
				   int right_point, right=0, left_point, left=100000, top_point, top=10000;
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
								   if(top>POLY_rough[i].at(j).y)
								   {
									   top=POLY_rough[i].at(j).y;
									   top_point=j;
								   }
							   }
				 circle( dst, POLY_rough[i].at(right_point), 6, Scalar(0,0,255), -1, 8, 0 );//3
				 circle( dst, POLY_rough[i].at(left_point), 6, Scalar(0,0,255), -1, 8, 0 );//1
				 circle( dst, POLY_rough[i].at(top_point), 6, Scalar(0,0,255), -1, 8, 0 );//2
                 Rect bound=boundingRect(POLY_rough[i]);
	             //rectangle(dst,Point(bound.br().x,bound.br().y), Point(bound.tl().x,bound.tl().y),(0, 0, 255),2);
				 circle( dst, Point(0.5*(bound.br().x+bound.tl().x),bound.br().y), 10, Scalar(0,0,255), -1, 8, 0 );//4
				 vector<Moments> mu(1);
				 vector<Point2f> mc(1);
				 // compute the central momment
						mu[0] = moments( POLY_rough[i], false );
						mc[0] = Point2f( mu[0].m10/mu[0].m00 , mu[0].m01/mu[0].m00 );
						circle( dst, mc[0], 8, Scalar(255,0,255), -1, 8, 0 );
						//cout<<mc[0]<<endl;
						//cout<<"1"<<endl;
						//ofstream centerpoints("C:/Users/bojan/Desktop/TestFeatures/Results/straight-arrow-center1.txt",ios::app);
						//centerpoints<<mc[0];
						//return 1;
						// this one for corner points

								//ofstream fout("C:/Users/bojan/Desktop/TestFeatures/Results/Points/StraightArrow/straightarrow1.txt", ios::app);
						 	    //fout<<dirName<<";";
						 	    //fout<<imgName<<";";
						 	    //fout<<"1"<<";";
						 	    //fout<<POLY_rough[i].at(left_point)<<";";;
						 	    //fout<<POLY_rough[i].at(top_point)<<";";
						 	    //fout<<POLY_rough[i].at(right_point)<<";";;
						 	    //fout<<Point(0.5*(bound.br().x+bound.tl().x),bound.br().y)<<endl;
						 	    //fout<<mc<<endl;
						        //cout<<dirName<<";"<<"1"<<";"<<mc<<endl;
						        //cout<<POLY_rough[i].at(left_point)<<";"<<POLY_rough[i].at(top_point)<<";"<<POLY_rough[i].at(right_point)<<";"<<Point(0.5*(bound.br().x+bound.tl().x),bound.br().y)<<endl;
						        cout<<flightName<<";"<<imageName<<";"<<"1";
								cout<<";"<<mc;
				                //koordinati
				                //cout<<POLY_rough[i].at(left_point)<<";"<<POLY_rough[i].at(top_point)<<";"
				                //<<POLY_rough[i].at(top_point)<<";"<<Point(0.5*(bound.br().x+bound.tl().x),bound.br().y)<<endl;
				         float width=pow((POLY_rough[i].at(right_point).x - POLY_rough[i].at(left_point).x )*(POLY_rough[i].at(right_point).x - POLY_rough[i].at(left_point).x) +
				         (POLY_rough[i].at(right_point).y -POLY_rough[i].at(left_point).y )*(POLY_rough[i].at(right_point).y - POLY_rough[i].at(left_point).y),0.5);

				         float height=pow((POLY_rough[i].at(top_point).x - Point(0.5*(bound.br()+bound.tl())).x )*(POLY_rough[i].at(top_point).x - Point(0.5*(bound.br()+bound.tl())).x) +
						 (POLY_rough[i].at(top_point).y -Point(bound.br()).y )*(POLY_rough[i].at(top_point).y -Point(bound.br()).y),0.5);

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
			     if(POLY_rough.size()==0 && POLY_precise.size()==0)
			 {
			    	 //ofstream fout("C:/Users/bojan/Desktop/TestFeatures/Results/Points/StraightArrow/straightarrow1.txt", ios::app);
			    	 //fout<<dirName<<";";
			    	 //fout<<imgName<<";";
			    	 //fout<<"0"<<endl;

			     //ofstream mycout("C:/Users/bojan/Desktop/Points/straightarrow.txt",ios::app);
				 //mycout<<"file"<<"\t";
                 //mycout<<imgName<<"\t";
				 //mycout<<"nothing found"<<"\t";
				 // mycout<<endl;
				 cout<<flightName<<";"<<imageName<<";"<<"0"<<endl;
			     //cout<<dirName<<";"<<"0"<<endl;
				 //return 0;
			 }

	drawContours(dst, POLY_precise,-1, Scalar(0,255,0), 2);
	drawContours(dst, POLY_rough,-1, Scalar(0,255,0), 2);
	//imwrite( "D:/mobility lab/任务2 箭头/day 20  straight arrow detection/straight arrow result/185.png", dst);
	namedWindow("dst",CV_WINDOW_NORMAL);
	imshow("dst", dst);
              }

	                  	             // }
	                  	                   //closedir (dir);
	                  	                } else {
	                  	                 // cout<<"not present"<<endl;
	                  	                  }


	     return 0;
	 }

/*
 * main.cpp
 *
 *  Created on: Feb 26, 2017
 *      Author: vinuk
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

using namespace std;
using namespace cv;

Mat src; Mat src_gray;
int thresh = 5;
int max_thresh = 60000;
RNG rng(12345);

const char* source_window = "Source image";
const char* corners_window = "Corners detected";


string read_value(string section, string key, string config_path)
{
	char read_value[100];
	DWORD len = 100;

	char section_char[100],key_char[100],config_path_char[100];
	strcpy(section_char, section.c_str());
	strcpy(key_char, key.c_str());
	strcpy(config_path_char, config_path.c_str());
	GetPrivateProfileString(TEXT(section_char), TEXT(key_char),TEXT("Not Found"),read_value,len,TEXT(config_path_char));

	//cout<< read_value;
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

int main(int argc,char *argv[])
{

           IplImage *desimg,*srcimg;
           cout<<"************************Detect Rectangular Window************************"<<endl;
           string config_path = argv[1];//"C:\\Config\\Config.ini";
           string path = argv[2];
           string dirName = GetFoldername(path);//read_value("Aerobridge","dummy",config_path);
           string flightName = GetFilename(dirName);
           cout<<flightName<<endl;




            DIR *dir;
            dir = opendir(dirName.c_str());
            string imgName;
            struct dirent *ent;

            if (dir != NULL) {
                     while ((ent = readdir (dir)) != NULL) {
                          imgName= GetFilename(path);//ent->d_name;
                          if(imgName.compare(".")!= 0 && imgName.compare("..")!= 0)
                                    {
                                      string aux;
                                      aux.append(dirName);
                                      aux.append("\\");
                                      aux.append(imgName);
                                      //aux.append(orig_path);
                                      cout << aux << endl;
                                      Mat image= imread(aux);

                                      waitKey(0);
      Mat gray,bw,dil,erd, dst_final;

	  Mat new_src=image.clone();

	 for(int y = 0; y < image.rows; y++ )
    {
       for(int x = 0; x < image.cols; x++ )
       {
              for(int c = 0; c < 3; c++ )
              {
                     new_src.at<Vec3b>(y,x)[c]= saturate_cast<uchar>( 1.5*(image.at<Vec3b>(y,x)[c] ));

              }
       }
     }

      cv::GaussianBlur(new_src, src_gray, cv::Size(3,3),1,1,BORDER_DEFAULT); //original
      medianBlur(new_src, src_gray, 11);
      blur( new_src, src_gray, Size(3,3) );
      cvtColor(src_gray,gray,CV_BGR2GRAY);
	  //namedWindow("gray",CV_WINDOW_NORMAL);
	  //imshow("gray",gray);


      Canny(gray,bw,600,1200,5,true);
	  //namedWindow("canny",CV_WINDOW_NORMAL);
      //imshow("canny",bw);


                    Mat grad,bw1;
                    Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(20,10));
                    morphologyEx(bw, grad, MORPH_GRADIENT, morphKernel);
					//namedWindow("m1",CV_WINDOW_NORMAL);
					//imshow("m1",grad);

                    threshold(grad, bw1, 255.0, 255.0, THRESH_BINARY | THRESH_OTSU);
					//namedWindow("threshold",CV_WINDOW_NORMAL);
					//imshow("threshold",bw1);

      vector<vector<Point> > contours;
	  vector<vector<Point> > rough;
	  vector<vector<Point> >rough_color;
	  vector<vector<Point> >precise;
      vector<Vec4i> hierarchy;
	  Mat dst = image.clone();
      findContours(bw1.clone(), contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

      vector<Point> approx;

      for( int i = 0; i< contours.size(); i++ )
      {

           approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.01, true);
           if (fabs(contourArea(approx)) <2000 || fabs(contourArea(approx))>50000)                                      //将面积太小的轮廓直接跳过  注意continue用法
			   continue;

                  Rect r = boundingRect(contours[i]);
				  if( (float) (r.height/r.width) > 1.5 && (float) (r.height/r.width) <3)
                  {
                   rough.push_back(approx);
                   }
      }
	  for(int i = 0; i < rough.size(); i++)
	  {
		  Rect bound=boundingRect(rough[i]);
	      Rect x, y, w, h = boundingRect(rough[i]);
	      rectangle(dst,Point(bound.br().x,bound.br().y), Point(bound.tl().x,bound.tl().y),Scalar(255, 0, 0),2);

	      RotatedRect rectPoint = minAreaRect(rough[i]);
          Point2f fourPoint2f_rough[4];
          rectPoint.points(fourPoint2f_rough);
	      vector<Point> fourPoint_rough;
	      for(int i = 0; i <4; i++)
	      	  	  {
		     fourPoint_rough.push_back(fourPoint2f_rough[i]);
	      	  	  }
	  }

	  if(rough.size() !=0 )
	  {
	  for( int i = 0; i< rough.size(); i++ )
	  {
		  vector<Moments> mu(1);
		  vector<Point2f> mc(1);
		  int gray_level;
	      // compute the central momment
          mu[0] = moments( rough[i], false );
          mc[0] = Point2f( mu[0].m10/mu[0].m00 , mu[0].m01/mu[0].m00 );
		  //circle( dst, mc[0], 4, Scalar(255,0,0), -1, 8, 0 );

		  //cout<<"pixel coordinates"<<"\t";
		  //cout<<mc[0]<<endl;

		  gray_level=gray.at<uchar>(mc[0]);
		  if(gray_level<200 && gray_level>20)
		  {rough_color.push_back(rough[i]);}
	  }

	  for(int i = 0; i < rough_color.size(); i++)
	  {
		  Rect bound=boundingRect(rough_color[i]);
	      Rect x, y, w, h = boundingRect(rough_color[i]);
	      //rectangle(dst,Point(bound.br().x,bound.br().y), Point(bound.tl().x,bound.tl().y),(0, 0, 255),2);

	      RotatedRect rectPoint = minAreaRect(rough_color[i]);
          Point2f fourPoint2f_color[4];
          vector<Point> fourPoint_color;
	      for(int i = 0; i <4; i++)
	      	  	  {
		  fourPoint_color.push_back(fourPoint2f_color[i]);
	      	  	  }

	     for (int i = 0; i < 3; i++)
	     	 	 {
            line(dst, fourPoint2f_color[i], fourPoint2f_color[i + 1], Scalar(0,255,0), 3);
	     	 	 }
        line(dst, fourPoint2f_color[0], fourPoint2f_color[3], Scalar(0,255,0), 3);
	  	  	  }

	  	  }

	  int rightest=0;
	  int right_number;
	  if(rough_color.size() !=0 )
	  {
	  for( int i = 0; i< rough_color.size(); i++ )
	  {
		  Moments mu;
		  Point2f mc;
		  mu = moments( rough_color[i], false );
		  mc = Point2f( mu.m10/mu.m00 , mu.m01/mu.m00 );
		  if(mc.x>rightest)
		  {
			  rightest=mc.x;
			  right_number=i;
		  }
	  }
	  precise.push_back(rough_color[right_number]);
	  }


	  // final decision
	  if(precise.size() !=0 )
	  {
	  Moments muf;
	  Point2f mcf;
	  muf = moments( precise[0], false );
	  mcf = Point2f( muf.m10/muf.m00 , muf.m01/muf.m00 );
	  circle( dst, mcf, 4, Scalar(255,0,0), -1, 8, 0 );
	  cout<<"pixel coordinates"<<"\t";
	  cout<<mcf<<endl;

	  string dist_rect_str = read_value("Rectangular Window","center_log_file",config_path);
	  char dist_rect_char[100];
	  strcpy(dist_rect_char, dist_rect_str.c_str());
	  ofstream mycout(TEXT(dist_rect_char),ios::app);

	  mycout<<flightName<<";"<<imgName<<";"<<"1"<<";"<<mcf<<endl;
	  return 1;
	  	    //mycout<<"point1"<<"\t";
	  	    //mycout<<mcf<<"\t";
	  	    //mycout<<endl;



	  if(mcf.x>0.33*image.cols)
	  {

       Rect bound=boundingRect(precise[0]);
	   Rect x, y, w, h = boundingRect(precise[0]);
	   RotatedRect rectPoint = minAreaRect(precise[0]);
       Point2f fourPoint2f_precise[4];
       rectPoint.points(fourPoint2f_precise);
	   vector<Point> fourPoint_precise;
	   for(int i = 0; i <4; i++)
	   {
		 fourPoint_precise.push_back(fourPoint2f_precise[i]);
	   }
	    for (int i = 0; i < 3; i++)
        {
            line(dst, fourPoint_precise[i], fourPoint_precise[i + 1], Scalar(0,0,255), 3);
        }
        line(dst, fourPoint_precise[0], fourPoint_precise[3], Scalar(0,0,255), 3);

        //Tuka coordinati na ceitirita tocki od prozorot
        string point_rect_str = read_value("Rectangular Window","coord_log_file",config_path);
        char point_rect_char[100];
        strcpy(point_rect_char, point_rect_str.c_str());

        ofstream mycout(TEXT(point_rect_char),ios::app);
        mycout<<flightName<<";"<<imgName<<";"<<"1"<<";";
        mycout<<";"<<fourPoint_precise[0];
        mycout<<";"<<fourPoint_precise[1];
        mycout<<";"<<fourPoint_precise[2];
        mycout<<";"<<fourPoint_precise[3];
       	mycout<<endl;
       	return 1;



	  }
	  }


                    Mat S(0, 2, CV_32SC1);
                    cvtColor( image, src_gray, CV_BGR2GRAY );
                    Mat dst1, dst_norm,dst_norm_scaled;
                    dst1 = Mat::zeros( src.size(), CV_32FC1 );
                    int blockSize = 2;
                    int apertureSize = 3;
                    double k = 0.04;

                   resize(image, image, Size(640,480), 0, 0, INTER_CUBIC);
                   char* source_window4 = "Detected window on the input image";
                   namedWindow( source_window4, CV_WINDOW_NORMAL );
                   imshow( source_window4, dst );

			       //while(uchar(waitKey(1))!='Q');




      }
      }
      closedir (dir);
      } else {
      cout<<"not present"<<endl;
      }
      return 0;

    }





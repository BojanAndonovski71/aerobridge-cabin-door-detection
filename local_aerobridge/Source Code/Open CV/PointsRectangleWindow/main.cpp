//author: Cheng Yupeng
//describtion: detect the door window, have been tested in sqb 772, scoot b787;
//date: 2017.1.9
//version: 2nd;

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
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

Mat src; Mat src_gray;
int thresh = 5;
int max_thresh = 60000;
RNG rng(12345);

const char* source_window = "Source image";
const char* corners_window = "Corners detected";

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
		   //string dirName = "D:/mobility lab/New folder/testing/DataCollect_2016-4-27-6-52-56 - SQ B772 - PBB Docking/";
		    //string dirName = "C:/Users/bojan/Desktop/TestFeatures/MASB737/";
		 	 //string dirName = "C:/Users/bojan/Desktop/TestFeatures/ANAB787/" "/";
	string dirName = argv[1];
	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/B787/000145.png";
		 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/A320/" ;
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
                                      Mat image= imread(aux);



      Mat gray,bw,dil,erd, dst_final;
	  Mat new_src=image.clone();
	  for(int y = 0; y < image.rows; y++ )
	  	  {
       for(int x = 0; x < image.cols; x++ )
       	   {
             new_src.at<Vec3b>(y,x)[0]= saturate_cast<uchar>( 4*(image.at<Vec3b>(y,x)[0] ));
       	   }
     }


      //GaussianBlur(new_src, src_gray, cv::Size(3,3),1,1,BORDER_DEFAULT); //original
      medianBlur(new_src, src_gray, 15);
      //blur( new_src, src_gray, Size(3,3) );
      cvtColor(src_gray,gray,CV_BGR2GRAY);


	  Mat gray_improved=gray.clone();
	  for(int y = 0; y < gray.rows; y++ )
     {
       for(int x = 0; x < gray.cols; x++ )
       {
		      float k=(float)gray.at<uchar>(y,x)/255;
			  int kk=255*pow(k,0.3);
			  if(kk>255) kk=255;
              gray_improved.at<uchar>(y,x)=kk;
			  //new_src.at<Vec3b>(y,x)[c]= saturate_cast<uchar>( 50*log(1+(image.at<Vec3b>(y,x)[c] )));
       }
      }

      Canny(gray_improved,bw,200,600,5,true);
      Mat grad,bw1;
      Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(15,15));
      morphologyEx(bw, grad, MORPH_GRADIENT, morphKernel);
	  //namedWindow("m1",CV_WINDOW_NORMAL);
	  //imshow("m1",grad);
      vector<vector<Point> > contours;
	  vector<vector<Point> > rough;
	  vector<vector<Point> >rough_color;
	  vector<vector<Point> >precise;
      vector<Vec4i> hierarchy;
	  Mat dst = image.clone();
      findContours(grad.clone(), contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
      vector<Point> approx;

      for( int i = 0; i< contours.size(); i++ )
      {

           approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.01, true);
           if (fabs(contourArea(approx)) <4000 || fabs(contourArea(approx))>50000)                                      //将面积太小的轮廓直接跳过  注意continue用法
		   //if (fabs(contourArea(approx)) <2000)
			   continue;

		   float area_contour=contourArea(approx);
           Rect r = boundingRect(contours[i]);
		   float area_rectangle=r.height*r.width;
		   float ratio_cr=(float)(area_contour/area_rectangle);
           if( (float)r.height/r.width > 1.5 && (float)r.height/r.width <4 && ratio_cr>0.5)
           rough.push_back(approx);
      }
	  Mat dst_1=image.clone();
	  drawContours(dst_1, rough,-1, Scalar(255,0,0), 2);
	  Mat interMat = Mat::zeros(image.rows,image.cols, CV_8UC1);

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
		  //circle( dst, mc[0], 4, Scalar(0,0,255), -1, 8, 0 );
		  gray_level=gray.at<uchar>(mc[0]);
		  if(gray_level<200 && gray_level>20)
		  {rough_color.push_back(rough[i]);}
	  }
	  //drawContours(dst, rough_color,-1, Scalar(0,255,0), 2);
	  for(int i = 0; i < rough_color.size(); i++)
	  {
		  Rect bound=boundingRect(rough_color[i]);
	      Rect x, y, w, h = boundingRect(rough_color[i]);
	      //rectangle(dst,Point(bound.br().x,bound.br().y), Point(bound.tl().x,bound.tl().y),(0, 0, 255),2);

	      RotatedRect rectPoint = minAreaRect(rough_color[i]);
          Point2f fourPoint2f_color[4];
         //将rectPoint变量中存储的坐标值放到 fourPoint的数组中
         rectPoint.points(fourPoint2f_color);
	     vector<Point> fourPoint_color;
	     for(int i = 0; i <4; i++)
	     {
		     fourPoint_color.push_back(fourPoint2f_color[i]);
	     }

	     for (int i = 0; i < 3; i++)
        {
            line(interMat, fourPoint2f_color[i], fourPoint2f_color[i + 1], 255, 3);
        }
        line(interMat, fourPoint2f_color[0], fourPoint2f_color[3],255, 3);
	  }
	  }

	  vector<vector<Point> > contour_ag;
	  findContours(interMat,contour_ag,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	  string pixel;
	  if(contour_ag.size()!=0)
        {
			 //ofstream mycout("D:/mobility lab/任务8/point V2//Scoot B787.txt",ios::app);
			// mycout<<"file"<<"\t";
             //mycout<<imgName<<"\t";
            cout<<flightName<<";"<<imageName<<";"<<"1";
			for(int i=0;i<contour_ag.size();i++)
			{
				// if(i == 0){

					 //cout<<flightName<<";"<<imageName<<";"<<"1";
				       //        }


				 Moments mu;
				 Point2f mc;
				 mu = moments( contour_ag[i], false );
				 mc = Point2f( mu.m10/mu.m00 , mu.m01/mu.m00 );
				 circle( dst, mc, 8, Scalar(255,0,0), -1, 8, 0 );

				RotatedRect final_rect = minAreaRect(contour_ag[i]);
                Point2f final_point[4];
                final_rect.points(final_point);
	            vector<Point> finalpoint;
				vector<Point> drawpoint;
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
               for (int j = 0; j < 3; j++)
               {
                   line(dst, drawpoint[j],drawpoint[j + 1], Scalar(0,0,255), 3);
                }
                line(dst, drawpoint[0], drawpoint[3], Scalar(0,0,255), 3);
			    //mycout<<"window"<<"\t" ;
		        //mycout<<i<<"\t";
	            //mycout<<"point1"<<"\t";
                //mycout<<drawpoint[0]<<"\t";
		        //mycout<<"point2"<<"\t";
                //mycout<<drawpoint[1]<<"\t";
		        ///mycout<<"point3"<<"\t";
                //mycout<<drawpoint[2]<<"\t";
		        // mycout<<"point4"<<"\t";
                //mycout<<drawpoint[3]<<"\t";

        	   circle( dst, drawpoint[0], 10, Scalar(255,0,255), -1, 8, 0 );
        	   circle( dst, drawpoint[1], 10, Scalar(0,0,255), -1, 8, 0 );
        	   circle( dst, drawpoint[2], 10, Scalar(255,0,0), -1, 8, 0 );
        	   circle( dst, drawpoint[3], 10, Scalar(255,0,255), -1, 8, 0 );
        	   cout<<";"<<mc;
        	   float width=pow((drawpoint[1].x - drawpoint[0].x )*(drawpoint[1].x - drawpoint[0].x) + (drawpoint[1].y - drawpoint[0].y )*(drawpoint[1].y - drawpoint[0].y),0.5);
        	   float height=pow((drawpoint[2].x - drawpoint[1].x )*(drawpoint[2].x - drawpoint[1].x) + (drawpoint[2].y - drawpoint[1].y )*(drawpoint[2].y - drawpoint[1].y),0.5);

        	   if (!pixel.empty()){
        	   	   pixel.append("-");
        	   	      	  	  	  }
        	   	    	 pixel.append(tostr(width));
        	   	         pixel.append(";");
        	   	         pixel.append(tostr(height));


			}

			//cout<<flightName<<";"<<imageName<<";"<<"1"<<";"<<mc<<endl;
		}
	  cout<< ":"<<pixel;
	  cout<<endl;
	 //cout<<endl;
		if(contour_ag.size()==0)
	    {
		//ofstream mycout("D:/mobility lab/任务8/point V2//Scoot B787.txt",ios::app);
	    //mycout<<"file"<<"\t";
        //mycout<<imgName<<"\t";
	    //mycout<<"nothing found"<<"\t";
	    //mycout<<endl;
	    cout<<flightName<<";"<<imageName<<";"<<"0"<<endl;
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

      }
     //}
     // closedir (dir);
      } else {
      //cout<<"not present"<<endl;
      }
      return 0;

    }




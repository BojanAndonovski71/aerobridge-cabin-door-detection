/*
 * generate_data1.cpp
 *
 *  Created on: Sep 23, 2016
 *      Author: bojan
 */


// generate_data.cpp

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
	string dirName = argv[1];
	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/B787/";
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
	                         		 	                          //cout << aux << endl;
	                         		 	                           Mat image= imread(aux);
	                         		 	                          waitKey(0);



	                  cv::Mat noise(image.size(),image.type());
	                  float m = (50,50,50);
	                  float s = (50,50,50);
	                  cv::randn(noise, m, s); //mean and variance
	                  image += noise;

*/
	                  //cv::imshow( "rgb.jpg", image);


	         Mat rgb,src_gray;
	        // downsample and use it for processing
	        // pyrDown(image, rgb);
	        Mat small2,small1;
	        //cv::GaussianBlur(image, src_gray, cv::Size(3, 3), 2,2,BORDER_DEFAULT);
	        GaussianBlur(image, src_gray, Size(3,3), 2, 2,BORDER_DEFAULT);
	        //cv::GaussianBlur(rgb, src_gray, cv::Size(3, 3),2,2,BORDER_DEFAULT); //novo
	        // cv::GaussianBlur(rgb, src_gray, cv::Size(3, 3),2,2); //novo
	        // medianBlur(image, src_gray,1); //ova bese najdobro
	        // medianBlur(rgb, src_gray, 15);
	        //blur( image, src_gray, Size(1,1) );
	        //cvtColor(rgb, small, CV_BGR2GRAY); //originalno
	        cvtColor(src_gray, small1, CV_BGR2GRAY);//novo
	        Canny(small1,small2,600,1300,5,true);//novo
	        //Canny(gray_T,bw_T,1200,600,5,true);

	        // morphological gradient
	        Mat grad;
	        //Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(14,4));
	        Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(8, 3));
	        morphologyEx(small1, grad, MORPH_GRADIENT, morphKernel);
	        // binarize
	        Mat bw;
	        threshold(grad, bw, 255.0, 255.0, THRESH_BINARY | THRESH_OTSU);
	        //connect horizontally oriented regions
	        Mat connected;
	        //morphKernel = getStructuringElement(MORPH_RECT, Size(5, 2));
			//morphKernel = getStructuringElement(MORPH_CROSS, Size(13, 2));
	        morphKernel = getStructuringElement(MORPH_RECT, Size(5, 1));
	        morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);
	        //find contours
	        Mat mask = Mat::zeros(bw.size(), CV_8UC1);
	        //std::vector<std::vector<cv::Point> > contours;
	        vector<vector<Point> > contours;
	        vector<Vec4i> hierarchy;
	        //findContours(connected, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE,Point(0,0));
	        findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	        // findContours(connected, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	        //findContours(connected, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	 		//findContours(bw.clone(), contours,hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	        // filter contours
	        // vector<Moments> mu(1);
	        //vector<Point2f> mc(1);
	        bool found=false;
	        string pixel;
	        for(int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	        {
                if(idx == 0){
                	cout<<flightName<<";"<<imageName<<";"<<"1";
                }
	        	Rect rect = boundingRect(contours[idx]);

	            //novo
	        	RotatedRect final_rect = minAreaRect(contours[idx]);
	        	Point2f final_point[4];
	        	final_rect.points(final_point);
	        	vector<Point> finalpoint;
	        	vector<Point> drawpoint;
	        	// end novo

	        	Mat maskROI(mask, rect);
	            maskROI = Scalar(0, 0, 0);
	            // fill the contour
	            drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
	            // ratio of non-zero pixels in the filled region
	            double r = (double)countNonZero(maskROI)/(rect.width*rect.height);

	            vector<Moments> mu(1);
	            vector<Point2f> mc(1);
	            mu[0] = moments( contours[idx], false );
	            mc[0] = Point2f( mu[0].m10/mu[0].m00 , mu[0].m01/mu[0].m00 );
	            //cout<<dirName<<";"<<imgName<<";";
	            if (r > .45 /* assume at least 45% of the area is filled if it contains text */
	                &&
	               (rect.height >12 && rect.width >12 && rect.height < 100 && rect.width < 100 && rect.width/rect.height>1.03&& rect.width/rect.height<4.1) /* constraints on region size */
	                //(rect.height >9 && rect.width >9 && rect.height < 140 && rect.width < 140 && rect.width/rect.height>1.03&& rect.width/rect.height<4.9) /* constraints on region size */

					/* these two conditions alone are not very robust. better to use something
	                like the number of significant peaks in a horizontal projection as a third condition */
	                )
	            {
	                		  rectangle(src_gray, rect, Scalar(0, 255, 0), 2);
	                		  //novo
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



	                		  Moments mu;
	                		  Point2f mc;
	                		  mu = moments( contours[idx], false );
	                		  mc = Point2f( mu.m10/mu.m00 , mu.m01/mu.m00 );
	                		  circle( src_gray, mc, 4, Scalar(255,0,0), -1, 8, 0 );
	                		  circle( src_gray, drawpoint[0], 10, Scalar(255,0,255), -1, 8, 0 );
	                		  circle( src_gray, drawpoint[1], 10, Scalar(255,0,255), -1, 8, 0 );
	                		  circle( src_gray, drawpoint[2], 10, Scalar(255,0,255), -1, 8, 0 );
	                		  circle( src_gray, drawpoint[3], 10, Scalar(255,0,255), -1, 8, 0 );
	                		  //ofstream mycout("C:/Users/bojan/Desktop/TestFeatures/Results/moments_text3.txt",ios::app);
	                		  //mycout<<fourPoint_precise[0];
	                		  //mycout<<fourPoint_precise[1];
	                		  //mycout<<mc;
	                		  //mycout<<endl;
	                		  found=true;

	                		  cout<<";"<<mc; //ova bese dobro
	                		  //cout<<"1"<<";"<<mc<<endl;
	                		  //cout<<"2"<<drawpoint[0]<<";"<<drawpoint[1]<<";"<<drawpoint[2]<<";"<<drawpoint[3];
	                		  //float width=pow((drawpoint[1].x - drawpoint[0].x )*(drawpoint[1].x - drawpoint[0].x) + (drawpoint[1].y - drawpoint[0].y )*(drawpoint[1].y - drawpoint[0].y),0.5);
	                		  //float height=pow((drawpoint[2].x - drawpoint[1].x )*(drawpoint[2].x - drawpoint[1].x) + (drawpoint[2].y - drawpoint[1].y )*(drawpoint[2].y - drawpoint[1].y),0.5);
	                		  //cout<<"3"<<";"<<width<<";"<<height;
	                		  if(idx = hierarchy[idx][0])
	                		  	      {
	                			  float width=pow((drawpoint[1].x - drawpoint[0].x )*(drawpoint[1].x - drawpoint[0].x) + (drawpoint[1].y - drawpoint[0].y )*(drawpoint[1].y - drawpoint[0].y),0.5);
	                			  float height=pow((drawpoint[2].x - drawpoint[1].x )*(drawpoint[2].x - drawpoint[1].x) + (drawpoint[2].y - drawpoint[1].y )*(drawpoint[2].y - drawpoint[1].y),0.5);
	                			  //cout<<width<<";"<<height<<"-";
	                			  if (!pixel.empty()){
	                			  pixel.append("-");
	                			     }
	                			  pixel.append(tostr(width));
	                			  pixel.append(";");
	                			  pixel.append(tostr(height));

	                		  	      }

	            			}

	         }
	        cout<< ":"<<pixel;
	             cout<<endl;


	             if(found==false){
	             cout<<flightName<<";"<<imageName<<";"<<"0"<<endl;
	        	 //return 0;
	        }

	       cv::imshow( "rgb.jpg", src_gray);
	        //cv::waitKey(0);
	                        }
	                    //}
	                  	                // closedir (dir);
	                  	               } else {
	                  	                   //cout<<"not present"<<endl;
	                  	                  }
	    return 0;
	 }



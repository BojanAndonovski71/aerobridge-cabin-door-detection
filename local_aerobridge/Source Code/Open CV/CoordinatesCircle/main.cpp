
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include<sstream>
#include <vector>
#include<dirent.h>
#include<fstream>
#include <cmath>
using namespace cv;
using namespace std;

int thresh = 1;
int max_thresh = 300;
Mat src;


int main(int argc,char** argv)
{

	IplImage *desimg,*srcimg;
			     string dirName = "C:/Users/bojan/Desktop/TestFeatures/MASB737/";
				 //string dirName = "C:/Users/bojan/Desktop/TestFeatures/A320/" "/";
				 //string dirName = "C:/Users/bojan/Desktop/TestFeatures/TigerAirA320" "/";
				 //string dirName = "C:/Users/bojan/Desktop/TestFeatures/TigerAirA320Rain" "/";
				 // string dirName = "C:/Users/bojan/Desktop/TestFeatures/SilkAirB737" "/";

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
	  Mat src_gray, gray;
	  cvtColor( image,src_gray, CV_BGR2GRAY );
	  medianBlur(src_gray, src_gray,15); //ova bese najdobro
	  Mat canny_output;
	  Canny(src_gray,canny_output,800,500,5,true);
      std::vector<cv::Vec3f> circles;
       //vector<Vec3f> circles;
     // Point_<int> Point2i;
      vector<Point> center;
      vector<Point> approx;
      HoughCircles( canny_output, circles, CV_HOUGH_GRADIENT, 1, canny_output.rows/4, 6, 66, 10,800 );
    	  /// Draw the circles detected
    	  for( size_t i = 0; i < circles.size(); i++ )
    	  {
    	      Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    	      int radius = cvRound(circles[i][2]);
    	      circle( image, center, radius, Scalar(0,255,0),10 );
    	      //ofstream fout("C:/Users/bojan/Desktop/TestFeatures/Results/circ_window_center.txt");
    	      //cv::Point(center.x, center.y);
    	      //cv::Point(circles[i][0], circles[i][1]);
    	      //cv::Point(circles[i][0]+radius, circles[i][1]+radius);
    	      	  	  	  	//fout<<Point(circles[i][0], circles[i][1])<<"\t";;
    	      	  	  	    //fout<<endl;
    	      	  	  	  	//fout<<"\t";   //Jump to next column
    	      	  	  	    //fout<<radius;      //Write value of y in next column
    	      	  	  	  	//fout<<endl;   //Jump to first column of next row
    	      	  	  	    //circle( image, Point(center.x, center.y), 6, Scalar(0,0,255), -1, 8, 0 ); ova bese od coordinati
    	      	  	  	    //line(image, Point(center.x, center.y), Point(center.x+radius/1.5, center.y+radius/1.5), Scalar( 110, 220, 0 ),  4, 2  ); ove bese od koordinati
    	      	  	        //line( image, Point( 15, 20 ), Point( 70, 50), Scalar( 110, 220, 0 ),  2, 8 );
    	      	  	  	    //line(image, Point pt1, Point pt2, const Scalar& color, int thickness=1, int lineType=LINE_8, int shift=0 )
    	      	  	        circle( image, center, 14, Scalar(255,0,255), -1, 8, 0 );
    	      	  	        approx.push_back(Point(circles[i][0], circles[i][1]));
    	      	  	        cout<<approx;
    	      	  	        ofstream fout("C:/Users/bojan/Desktop/TestFeatures/Results/circ_window_center.txt", ios::app);
    	      	  	        fout<<approx;
    	      	  	        fout<<endl;
    	      	  	        // approx.push_back(Point(circles[i][2]));
    	      	            //cout<<"catch rough one"<<endl;
    	      	            // cout<<approx<<endl;
    	      	            cout<<"1";
    	      	            return 1;
    	      	            //fout<<Point(circles[i][0], circles[i][1],radius);
    	      	            // fout<<endl;   //Jump to first column of next row
    	      	            //fout.close();


    	 resize(image, image, Size(640,480), 0, 0, INTER_CUBIC);
         char* source_window4 = "Detected window on the input image";
         namedWindow( source_window4, CV_WINDOW_AUTOSIZE );
         imshow( source_window4, image );
				              }


				         }
				         }

				 }

				         else {
				        	 	 	 cout<<"not present"<<endl;
				 }
				// fout.close();
	 return 0;
}


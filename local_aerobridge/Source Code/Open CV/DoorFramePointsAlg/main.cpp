
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cvaux.h>
#include <iostream>
#include <stdio.h>
#include <opencv\cxcore.h>
#include <conio.h>
#include<math.h>
#include<windows.h>
#include<Windows.h>
#include<Winbase.h>
#include<TCHAR.h>

#include<iostream>
#include<fstream>
#include<vector>
#include <stdlib.h>
#include<dirent.h>
#include<string.h>

using namespace std;
using namespace cv;
char* array();

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
	 	             	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/B787/" "/";
	 	             	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/A320/" ;
	 	             	    //string dirName = "C:/Users/bojan/Desktop/TestFeatures/ScootB787PBB/";
	 	             	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/ScootB787/";
	 	             	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/SQB772/";
	 	             	 	string dirName = argv[1];//
	 	             	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/B787/000131.png";
	                        //"C:/Users/bojan/Desktop/TestFeatures/SQB772Rain/";
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
	              if(true)//imgName.compare(".")!= 0 && imgName.compare("..")!= 0)
	                        {
	                          string aux = dirName;
	                          //aux.append(dirName);
	                          //aux.append(imgName);
	                          //cout << aux << endl;
	                          Mat src= imread(aux);
	                         // imshow(aux,src);
	                          waitKey(0);


	                          	                           	                             /*   DIR *dir;
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
	                          	                           	                               	                         		 	                          waitKey(0);
*/


	std::string s;
	//s<<dirName<<;
	std::string getText();
//	Mat src=imread("000212.png",1);
//	namedWindow("src",CV_WINDOW_NORMAL);
//	imshow("src",src);
	Mat gray,blur,canny,bird;
	Point2f inputQuad[4];
	Point2f outputQuad[4];
	Mat lambda( 2, 4, CV_32FC1 );

	lambda = Mat::zeros( src.rows, src.cols, src.type());
	//  image cordinate
	inputQuad[0] = Point2f( 0,0 );
    inputQuad[1] = Point2f( src.cols,0);
    inputQuad[2] = Point2f( src.cols,src.rows);
    inputQuad[3] = Point2f( 0,src.rows );
	//  real coordinate
    outputQuad[0] = Point2f( 250,0 );
    outputQuad[1] = Point2f( src.cols-250,0);
    outputQuad[2] = Point2f( src.cols,src.rows);
    outputQuad[3] = Point2f( 0,src.rows);

	lambda = getPerspectiveTransform( inputQuad, outputQuad );
	warpPerspective(src,bird,lambda,bird.size() );

	/*namedWindow("bird eye",CV_WINDOW_NORMAL);
	imshow("bird eye",bird);*/

	// strength contrast
	string pixel;
	Mat new_src=bird.clone();
	for(int y = 0; y < bird.rows; y++ )
    {
       for(int x = 0; x < bird.cols; x++ )
       {
              for(int c = 0; c < 3; c++ )
              {
                      new_src.at<Vec3b>(y,x)[c]= saturate_cast<uchar>( 3*(bird.at<Vec3b>(y,x)[c] ));

              }
       }
     }



    GaussianBlur(new_src, blur,Size(7,7), 2, 2);
	cvtColor(blur, gray, CV_BGR2GRAY);
    Canny(gray, canny, 400, 200, 5);                                     // 120, 300,
	/*namedWindow("canny",CV_WINDOW_NORMAL);
	imshow("canny",canny);*/



	//detect all the line
	vector<Vec4i> lines;
	vector<Vec4i> vertical_lines;
	vector<Vec4i> horizontal_lines;
	vector<Point> Try;
	vector<vector<Point> > classes;
    HoughLinesP(canny,lines,1,0.01, 60, 20, 30 );

	Mat dst=src.clone();
	Mat hline=bird.clone();


	/*for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        line( dst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 1, CV_AA);
    }  */

	//find the vertical line
	float angle;
	float a;
	for( size_t i = 0; i < lines.size(); i++ )
	{
		Vec4i l = lines[i];
		float t=0;
		if((l[0]-l[2])==0)
		{
			angle=-90;
		}
		else
		{
		t=(double)(l[1]-l[3])/(l[0]-l[2]);
		a =atan(t);
		angle = a* 180 * 0.3184;
		}
		if(abs(angle)>80)
		{
			vertical_lines.push_back(l);
		}
	}
    //draw vertical line in the hline
	for( size_t i = 0; i < vertical_lines.size(); i++ )
    {
        Vec4i m = vertical_lines[i];
        line( hline, Point(m[0], m[1]), Point(m[2], m[3]), Scalar(0,255,0), 1, CV_AA);
    }
	/*namedWindow("hline",CV_WINDOW_NORMAL);
	imshow("hline",dst);*/

    //draw the vertical line in a black image
	Mat black(bird.size(),CV_8U,Scalar(0));
	for( size_t i = 0; i < vertical_lines.size(); i++ )
    {
        Vec4i m = vertical_lines[i];
        line( black, Point(m[0], m[1]), Point(m[2], m[3]),255, 1, CV_AA);
    }
	/*namedWindow("black",CV_WINDOW_NORMAL);
	imshow("black",black);*/
	// first step dialation to build the contour
	Mat black_close;
	Size kernalSize (20,60);
	Mat element = getStructuringElement (MORPH_ELLIPSE, kernalSize, Point(1,1)  );
	morphologyEx( black, black_close, MORPH_CLOSE, element );
    /*namedWindow("morphology",CV_WINDOW_NORMAL);
	imshow("morphology",black_close);*/

	//find the contour
	vector<vector<Point> > contours;
	vector<cv::Vec4i> hierarchy;
	findContours(black_close.clone(), contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_NONE);
	//show the contours of the input image
	Mat interMat = Mat::zeros(src.rows,src.cols, CV_8UC1);
	drawContours(interMat, contours,-1, Scalar(255),2,8,hierarchy);
	/*namedWindow("contours",CV_WINDOW_NORMAL);
	imshow("contours",interMat);*/

	//filter the contour
	vector<vector<Point> > POLY;
    vector<vector<Point> > POLY_precise;
	vector<vector<Point> > POLY_rough;
	vector<vector<Point> > POLY_best;
	vector<Point> approx;
	vector<Point> convex_hull;

	for (int i = 0; i < contours.size(); i++)
	{
		if (fabs(contourArea(contours[i]))>40000)
		continue;

		convexHull(contours[i], convex_hull, true);

		//calculate the area ratio
		float area_contour, area_convex, area_ratio;
		area_contour=contourArea(contours[i]);
		area_convex=contourArea(convex_hull);
		area_ratio = (float)area_contour/area_convex;

		//calculate the up bounding rectangle to get the height
		Rect bound=boundingRect(contours[i]);
		float height;
		height=bound.br().y-bound.tl().y;

		if(height>0.25*src.rows)
			POLY.push_back(contours[i]);
	}


	Mat interMat_2 = Mat::zeros(src.rows,src.cols, CV_8UC1);
	drawContours(interMat_2, POLY,-1, Scalar(255),2,8);
	/*namedWindow("contours_filter",CV_WINDOW_NORMAL);
	imshow("contours_filter",interMat_2);*/

	Mat inter_close;
	Size kernalSize_2 (40,20);
	Mat element_1 = getStructuringElement (MORPH_ELLIPSE, kernalSize_2, Point(1,1)  );
	morphologyEx( interMat_2, inter_close, MORPH_CLOSE, element_1 );
    /*namedWindow("morphology_2",CV_WINDOW_NORMAL);
	imshow("morphology_2",inter_close);*/


	//inverse bird view
	Mat bird_inverse;
	Point2f inputQuad_inverse[4];
	Point2f outputQuad_inverse[4];
	Mat lambda_inverse( 2, 4, CV_32FC1 );

	lambda_inverse = Mat::zeros( inter_close.rows, inter_close.cols, inter_close.type());
	//  image cordinate
	inputQuad_inverse[0] = Point2f( 250,0 );
    inputQuad_inverse[1] = Point2f( inter_close.cols-250,0);
    inputQuad_inverse[2] = Point2f( inter_close.cols,inter_close.rows);
    inputQuad_inverse[3] = Point2f( 0,inter_close.rows );
	//  real coordinate
    outputQuad_inverse[0] = Point2f( 0,0 );
    outputQuad_inverse[1] = Point2f( inter_close.cols,0);
    outputQuad_inverse[2] = Point2f( inter_close.cols,inter_close.rows);
    outputQuad_inverse[3] = Point2f( 0,inter_close.rows);

	lambda_inverse = getPerspectiveTransform( inputQuad_inverse, outputQuad_inverse );
	warpPerspective(inter_close,bird_inverse,lambda_inverse,bird_inverse.size() );




	vector<vector<Point> > contour_ag;
	findContours(bird_inverse.clone(), contour_ag,CV_RETR_TREE,CV_CHAIN_APPROX_NONE);

	Mat interMat_3 = Mat::zeros(src.rows,src.cols, CV_8UC1);
	drawContours(interMat_3, contour_ag,-1, Scalar(255),2,8);
	/*namedWindow("contours_3",CV_WINDOW_NORMAL);
	imshow("contours_3",interMat_3);*/

	for (int i = 0; i < contour_ag.size(); i++)
	{
		convexHull(contour_ag[i], convex_hull, true);

		//calculate the area ratio
		float area_contour_ag, area_convex_ag, area_ratio_ag;
		area_contour_ag=contourArea(contour_ag[i]);
		area_convex_ag=contourArea(convex_hull);
		area_ratio_ag = (float)area_contour_ag/area_convex_ag;

		//find the toppest point
		int top_point,top=10000;
		for(int j=0;j<contour_ag[i].size();j++)
		{
			if (contour_ag[i].at(j).y<top)
			{
				top=contour_ag[i].at(j).y;
				top_point=j;
			}
		}
		//find the rightest point
		int right_point,right=0;
		for(int j=0;j<contour_ag[i].size();j++)
		{
			if (contour_ag[i].at(j).x>right)
			{
				right=contour_ag[i].at(j).x;
				right_point=j;
			}
		}
		int right_distance=0;
		int max=bird_inverse.cols;
		right_distance = max-right;

		//find the leftest point
		int leftest_point,left=10000;
		for(int j=0;j<contour_ag[i].size();j++)
		{
			if (contour_ag[i].at(j).x<left)
			{
				left=contour_ag[i].at(j).x;
				leftest_point=j;
			}
		}
		int left_distance=left;

		//find shape ratio
		int leng;
		int ag;
		int bg;
		float ratio_shape;
        leng=contour_ag[i].size();
		int   *numg=new   int[leng];
		int   *numgs=new   int[leng];
		for(int   j=0;j<leng;j++)
		{
			numg[j]=contour_ag[i].at(j).y;
            numgs[j]=contour_ag[i].at(j).x;
		}
		sort(numg,numg+leng);              //高度
        sort(numgs,numgs+leng);             //宽度
        ag=numg[leng-1]-numg[0];
        bg=numgs[leng-1]-numgs[0];
        ratio_shape=(float)ag/bg;
		//if(ratio_shape>3 && area_ratio_ag>0.5 && top<100)
	   // if(ratio_shape>4 && top<200 && right_distance>0.1*max && left_distance>0.1*max)
		if(ratio_shape>2 && top<200)
		{
			POLY_rough.push_back(contour_ag[i]);
		}
	}



	// final decition by area
	if(POLY_rough.size()>1)
	{
	Vector<float> area;
	for (int i = 0; i < POLY_rough.size(); i++)
	{
		area.push_back(contourArea(POLY_rough[i]));
	}
	sort(area.begin(), area.end());
	float first = area[area.size()-2];
    float second = area[area.size()-1];

	for (int i = 0; i < POLY_rough.size(); i++)
	{
		float aera_2;
		aera_2=contourArea(POLY_rough[i]);
		if(aera_2==first  || aera_2==second)
		{
			POLY_precise.push_back(POLY_rough[i]);
		}
	}
	}
	if(POLY_rough.size() == 1)
	{
		POLY_precise.push_back(POLY_rough[0]);
	}



	if(POLY_precise.size()!=0)
        {
		char* array();
		ofstream fout("C:/Users/bojan/Desktop/TestFeatures/Results/Points/DoorFrame/real_time_frame.txt", ios::app);
		std::string s;
		//s<<dirName<<";";
		//std::string getText(fout);
		fout<<dirName<<";";
		fout<<imgName<<";";
		cout<<flightName<<";"<<imageName<<";";
			 if(POLY_precise.size() == 2)
			 {
			   Point point_1,point_2,point_3,point_4,point_5,point_6,point_22,point_66;
			for(int i=0;i<POLY_precise.size();i++)
			{
				RotatedRect final_rect = minAreaRect(POLY_precise[i]);
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
				for(int f=0; f<4; f++)
				{
					//circle(dst,drawpoint[f],4,Scalar(0,0,255));
				}
               for (int j = 0; j < 3; j++)
               {
                   line(dst, drawpoint[j],drawpoint[j + 1], Scalar(255,255,0), 6);
                }
                line(dst, drawpoint[0], drawpoint[3], Scalar(255,255,0), 6);
			   //mycout<<"frame"<<"\t" ;
		       //mycout<<i<<"\t";
			   if(i == 0)
			   {
			    point_1= Point ((drawpoint[0].x + drawpoint[1].x)/2, (drawpoint[0].y + drawpoint[1].y)/2);
				point_3 = Point ((drawpoint[2].x + drawpoint[3].x)/2, (drawpoint[2].y + drawpoint[3].y)/2);
				point_2 = Point ((point_1.x + point_3.x)/2,(point_1.y + point_3.y)/2 );
				//circle(dst,point_1,10,Scalar(255,0,255)); //1
				//circle(dst,point_2,8,Scalar(0,255,255));// center
				circle(dst,point_3,10,Scalar(0,100,255));//2


				point_22= Point (drawpoint[2].x , drawpoint[2].y);
				circle(dst,point_22,10,Scalar(255,150,255)); //11

				float height1=pow((point_1.x -point_3.x )*(point_1.x - point_3.x) +(point_1.y - point_3.y )*(point_1.y - point_3.y),0.5);
				float width1=2*pow((point_22.x -point_3.x )*(point_1.x - point_3.x) +(point_1.y - point_3.y )*(point_1.y - point_3.y),0.5);


				fout<<"1"<<";";
				fout<<point_1<<";";
				fout<<point_2<<";";
				fout<<point_3<<";";
				cout<<"1"<<";"<<point_1<<";"<<point_2<<";"<<point_3;
				if (!pixel.empty()){
					                			  pixel.append("-");
					                			     }
					                			  pixel.append(tostr(width1));
					                			  pixel.append(";");
					                			  pixel.append(tostr(height1));

			   }
			   if(i == 1)
			   {
				point_4= Point ((drawpoint[0].x + drawpoint[1].x)/2, (drawpoint[0].y + drawpoint[1].y)/2);
				point_6 = Point ((drawpoint[2].x + drawpoint[3].x)/2, (drawpoint[2].y + drawpoint[3].y)/2);
				point_5 = Point ((point_4.x + point_6.x)/2,(point_4.y + point_6.y)/2 );
				//circle(dst,point_4,10,Scalar(50,60,255)); //4
				//circle(dst,point_5,4,Scalar(0,0,255)); //center
				circle(dst,point_6,4,Scalar(0,0,255)); //6
				//fout<<"2"<<";";
				fout<<point_4<<";";
				fout<<point_5<<";";
				fout<<point_6<<endl;

				point_66= Point (drawpoint[2].x , drawpoint[2].y);
				circle(dst,point_66,10,Scalar(255,150,255)); //11

				float height2=pow((point_4.x -point_6.x )*(point_4.x - point_6.x) +(point_4.y - point_6.y )*(point_4.y - point_6.y),0.5);
				float width2=2*pow((point_66.x -point_6.x )*(point_66.x - point_6.x) +(point_66.y - point_6.y )*(point_66.y - point_6.y),0.5);

				cout<<";"<<point_4<<";"<<point_5<<";"<<point_6;
				if (!pixel.empty()){
									                			  pixel.append("-");
									                			     }
									                			  pixel.append(tostr(height2));
									                			  pixel.append(";");
									                			  pixel.append(tostr(width2));

			   }
			}
			 }
			 if(POLY_precise.size() == 1)
			 {
				Point point_1,point_2,point_3,point_21;
				RotatedRect final_rect = minAreaRect(POLY_precise[0]);
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
				for(int f=0; f<4; f++)
				{
				  //circle(dst,drawpoint[f],4,Scalar(0,0,255));
				}
               for (int j = 0; j < 3; j++)
               {
                   line(dst, drawpoint[j],drawpoint[j + 1], Scalar(255,255,0), 6);
                }
                line(dst, drawpoint[0], drawpoint[3], Scalar(255,255,0), 6);

			    //mycout<<"frame"<<"\t" ;
		        //mycout<<0<<"\t";
			    point_1 = Point ((drawpoint[0].x + drawpoint[1].x)/2, (drawpoint[0].y + drawpoint[1].y)/2);
				point_3 = Point ((drawpoint[2].x + drawpoint[3].x)/2, (drawpoint[2].y + drawpoint[3].y)/2);
				point_2 = Point ((point_1.x + point_3.x)/2,(point_1.y + point_3.y)/2 );
				circle(dst,point_1,4,Scalar(0,0,255));
				circle(dst,point_2,4,Scalar(0,0,255));
				circle(dst,point_3,4,Scalar(0,0,255));
				fout<<"2"<<";";
				fout<<point_1<<";";
				fout<<point_2<<";";
				fout<<point_3<<endl;

				point_21= Point (drawpoint[2].x , drawpoint[2].y);
				circle(dst,point_21,10,Scalar(255,150,255)); //11

				float height21=pow((point_1.x -point_3.x )*(point_1.x - point_3.x) +(point_1.y - point_3.y )*(point_1.y - point_3.y),0.5);
				float width21=2*pow((point_21.x -point_3.x )*(point_21.x - point_3.x) +(point_21.y - point_3.y )*(point_21.y - point_3.y),0.5);

				cout<<"2"<<";"<<point_1<<";"<<point_2<<";"<<point_3;
				if (!pixel.empty()){
													                			  pixel.append("-");
													                			     }
													                			  pixel.append(tostr(height21));
													                			  pixel.append(";");
													                			  pixel.append(tostr(width21));
			 }

		}
	 cout<< ":"<<pixel;
	 cout<<endl;
		if(POLY_precise.size()==0)
	    {
			ofstream fout("C:/Users/bojan/Desktop/TestFeatures/Results/Points/DoorFrame/real_time_frame.txt", ios::app);
			fout<<dirName<<";";
			fout<<imgName<<";";
			fout<<"0"<<endl;
		    string real_time_file;
		    real_time_file.append(dirName);
		    real_time_file.append(imgName);
		    cout<<flightName<<";"<<imageName<<";"<<"0"<<endl;

	    }



	Mat interMat_4 = Mat::zeros(src.rows,src.cols, CV_8UC1);


	drawContours(interMat_4, POLY_rough,-1, Scalar(255),2,8);
	//drawContours(dst, POLY_precise,-1, Scalar(0,0,255),2,8);
	/*namedWindow("filter++",CV_WINDOW_NORMAL);
	imshow("filter++",interMat_4);*/
	namedWindow("dst",CV_WINDOW_NORMAL);
	imshow("dst",dst);

	//while(uchar(waitKey(1))!='Q');
}

										  //}
	                  	                  // closedir (dir);
	                  	               }   else {
	                  	                   cout<<"not present"<<endl;
	                  	                  }
	     return 0;
	 }

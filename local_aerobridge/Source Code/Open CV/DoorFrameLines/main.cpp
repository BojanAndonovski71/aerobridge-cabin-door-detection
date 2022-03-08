



#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cvaux.h>
#include <iostream>
#include <stdio.h>
#include <opencv\cxcore.h>
#include <conio.h>
#include<math.h>
#include<windows.h>


#include<iostream>
#include<fstream>
#include<vector>
#include <stdlib.h>
#include<dirent.h>
#include<string.h>

using namespace std;
using namespace cv;

int main(int argc,char** argv)
{
	 IplImage *desimg,*srcimg;
	 //string dirName = "C:/Users/bojan/Desktop/TestFeatures/SQB772/";
	 //string dirName = "C:/Users/bojan/Desktop/TestFeatures/MASB737/";
	             	 	 //string dirName = "C:/Users/bojan/Desktop/TestFeatures/ANAB787/" "/";
	             	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/B787/" "/";
	             	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/A320/" ;
	             	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/ScootB787PBB/";
	             	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/ScootB787/";
	             	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/SQB772/";
	             	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/SQB772Rain/";
	             	 	//string dirName = "C:/Users/bojan/Desktop/TestFeatures/TigerAirA320" "/";
	             	   //string dirName = "C:/Users/bojan/Desktop/TestFeatures/TigerAirA320Rain" "/";
	             	 	string dirName = "C:/Users/bojan/Desktop/TestFeatures/SilkAirB737" "/";
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
	                         // imshow(aux,src);
	                          waitKey(0);

//int main(void)
//{
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

	//namedWindow("bird eye",CV_WINDOW_NORMAL);
	//imshow("bird eye",bird);

	// strength contrast
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
	//namedWindow("canny",CV_WINDOW_NORMAL);
	//imshow("canny",canny);



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
	//namedWindow("hline",CV_WINDOW_NORMAL);
	//imshow("hline",dst);

    //draw the vertical line in a black image
	Mat black(bird.size(),CV_8U,Scalar(0));
	for( size_t i = 0; i < vertical_lines.size(); i++ )
    {
        Vec4i m = vertical_lines[i];
        line( black, Point(m[0], m[1]), Point(m[2], m[3]),255, 1, CV_AA);
    }
	//namedWindow("black",CV_WINDOW_NORMAL);
	//imshow("black",black);
	// first step dialation to build the contour
	Mat black_close;
	Size kernalSize (20,60);
	Mat element = getStructuringElement (MORPH_ELLIPSE, kernalSize, Point(1,1)  );
	morphologyEx( black, black_close, MORPH_CLOSE, element );
    //namedWindow("morphology",CV_WINDOW_NORMAL);
	//imshow("morphology",black_close);

	//find the contour
	vector<vector<Point> > contours;
	vector<cv::Vec4i> hierarchy;
	findContours(black_close.clone(), contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_NONE);
	//show the contours of the input image
	Mat interMat = Mat::zeros(src.rows,src.cols, CV_8UC1);
	drawContours(interMat, contours,-1, Scalar(255),2,8,hierarchy);
	//namedWindow("contours",CV_WINDOW_NORMAL);
	//imshow("contours",interMat);

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
	//namedWindow("contours_filter",CV_WINDOW_NORMAL);
	//imshow("contours_filter",interMat_2);

	Mat inter_close;
	Size kernalSize_2 (40,20);
	Mat element_1 = getStructuringElement (MORPH_ELLIPSE, kernalSize_2, Point(1,1)  );
	morphologyEx( interMat_2, inter_close, MORPH_CLOSE, element_1 );
    //namedWindow("morphology_2",CV_WINDOW_NORMAL);
	//imshow("morphology_2",inter_close);


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
	//namedWindow("contours_3",CV_WINDOW_NORMAL);
	//imshow("contours_3",interMat_3);

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

	if(POLY_precise.size()!=0)
        {
		     ofstream mycout("C:/Users/bojan/Desktop/TestFeatures/Results/corner-door-frame1.txt",ios::app);
			 mycout<<"file"<<"\t";
             mycout<<imgName<<"\t";
			for(int i=0;i<POLY_precise.size();i++)
			{
				RotatedRect final_rect = minAreaRect(POLY_precise[i]);
                Point2f final_point[4];
                final_rect.points(final_point);
	            vector<Point> finalpoint;
	            for(int l = 0; l <4; l++)
	            {
		           finalpoint.push_back(final_point[l]);
	            }
	            for (int j = 0; j < 3; j++)
               {
                   line(dst, finalpoint[j],finalpoint[j + 1], Scalar(0,0,255), 3);
                }
                line(dst, finalpoint[0], finalpoint[3], Scalar(0,0,255), 3);

                Moments muf;
                Point2f mcf;
                muf = moments( POLY_precise[i], false );
                mcf = Point2f( muf.m10/muf.m00 , muf.m01/muf.m00 );
                circle( dst, mcf, 4, Scalar(255,0,0), -1, 8, 0 );
			   mycout<<"frame"<<"\t" ;
		       mycout<<i<<"\t";
	           mycout<<"point1"<<"\t";
               mycout<<finalpoint[1]<<"\t";
		       mycout<<"point2"<<"\t";
               mycout<<finalpoint[2]<<"\t";
		       mycout<<"point3"<<"\t";
               mycout<<finalpoint[3]<<"\t";
		       mycout<<"point4"<<"\t";
               mycout<<finalpoint[0]<<"\t";
               cout<<mcf<<endl;


			}
            mycout<<endl;

            ofstream youcout("C:/Users/bojan/Desktop/TestFeatures/Results/center-door-frame1.txt",ios::app);
            youcout<<"file"<<"\t";
                         youcout<<imgName<<"\t";
            			for(int i=0;i<POLY_precise.size();i++)
            			{
            				Moments muf;
                            Point2f mcf;
                            muf = moments( POLY_precise[i], false );
                            mcf = Point2f( muf.m10/muf.m00 , muf.m01/muf.m00 );
                            circle( dst, mcf, 4, Scalar(255,0,0), -1, 8, 0 );
                           youcout<<mcf<<endl;
                           cout<<1<<endl;
                           return 1;
            			}
            youcout<<endl;


		}

		if(contour_ag.size()==0)
	    {
		ofstream mycout("C:/Users/bojan/Desktop/TestFeatures/Results/corner-door-frame1.txt",ios::app);
	    mycout<<"file"<<"\t";
        mycout<<imgName<<"\t";
	    mycout<<"nothing found"<<"\t";
	    mycout<<endl;
	    cout<<0<<endl;
	    return 0;
	    ofstream youcout("C:/Users/bojan/Desktop/TestFeatures/Results/center-door-frame1.txt",ios::app);
	    youcout<<"file"<<"\t";
	    youcout<<imgName<<"\t";
	    youcout<<"nothing found"<<"\t";
	    youcout<<endl;
	    }



	//Mat interMat_4 = Mat::zeros(src.rows,src.cols, CV_8UC1);


	//drawContours(interMat_4, POLY_rough,-1, Scalar(255),2,8);
	//drawContours(dst, POLY_precise,-1, Scalar(0,0,255),2,8);
	//namedWindow("filter++",CV_WINDOW_NORMAL);
	//imshow("filter++",interMat_4);
	namedWindow("dst",CV_WINDOW_NORMAL);
	imshow("dst",dst);

	//while(uchar(waitKey(1))!='Q');
}

										  }
	                  	                   closedir (dir);
	                  	               } else {
	                  	            	 cout<<"not present"<<endl;
	                  	                  }
	     return 0;
	 }

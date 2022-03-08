
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cvaux.h>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <opencv\cxcore.h>
#include <conio.h>
#include<math.h>
#include<windows.h>
#include <fstream>
#include<vector>
#include <stdlib.h>
#include<dirent.h>
#include<string.h>
#include <algorithm> // for std::copy
#include<Windows.h>
#include<Winbase.h>
#include<TCHAR.h>


using namespace std;
using namespace cv;


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


int main()
{
	cout<<"*****Distance BtwFeatures**********"<<endl;
	string config_path = "C:\\Config.ini";

	string arrow_file_str = read_value("DistanceBtwFeatures","arrow_file",config_path);
	char arrow_file_char[100];
	strcpy(arrow_file_char, arrow_file_str.c_str());

	ifstream arrow_file(TEXT(arrow_file_char),ios::app);
	std::vector<double> fpvec1;
	fpvec1.resize(2);
	char c1;
	  for ( int i = 0 ; i < 1; i ++)
	       {
		  arrow_file.get(c1);
		  arrow_file >> fpvec1[i];
		  arrow_file.get(c1);
		  arrow_file >> fpvec1[i+1];
		  arrow_file.get(c1);
	          std::cout << fpvec1[i] <<" and "<< fpvec1[i+1] << std::endl;
		       double x_arrow=fpvec1[i];
		       double y_arrow=fpvec1[i+1];

		       string rect_file_str = read_value("DistanceBtwFeatures","rect_file",config_path);
		       	char rect_file_char[100];
		       	strcpy(rect_file_char, rect_file_str.c_str());

   	ifstream rect_file(TEXT(rect_file_char),ios::app);

	  //ifstream rect_file("C:/Users/bojan/Desktop/TestFeatures/Results/distance_pixels_rectwindow1.txt",ios::app);
	  	std::vector<double> fpvec2;
	  	fpvec2.resize(2);
	  	char c2;
	  	  for ( int i = 0 ; i < 1; i ++)
	  	       {
	  		rect_file.get(c2);
	  		rect_file >> fpvec2[i];
	  		rect_file.get(c2);
	  		rect_file >> fpvec2[i+1];
	  		rect_file.get(c2);
	  	          std::cout << fpvec2[i] <<" and "<< fpvec2[i+1] << std::endl;
	  				double x_rect=fpvec2[i];
	  			    double y_rect=fpvec2[i+1];


    double distancex = pow ((x_arrow - x_rect),2);
    double distancey = pow ((y_arrow - y_rect),2);
    double calcdistance = pow((distancex - distancey),0.5);

    std::cout << "Distance" << std::endl;
    std::cout << calcdistance << std::endl;

    if ( 90 < calcdistance < 120 ) {
    			std::cout << "Distance ok" << std::endl;
    			cout<<"1"<<endl;
    			 return 1;
    			    }
    			      else {
    			      std::cout << "Not" << std::endl;
    			      return 0;
    			         }

	  	       }
	       }
                return 0;

    }

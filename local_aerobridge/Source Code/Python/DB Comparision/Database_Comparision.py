# Program for formatting raw data in Python 2.7
# Author: Vinu Karthek R
# Version : 1.0.0
# Release Date : 17 January 2017


from os import path
import os
from os import listdir
from os.path import isfile, join, splitext
import copy
import numpy
import math
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

from ConfigParser import ConfigParser
from pip._vendor.distlib.compat import raw_input
from logging.config import fileConfig
from numpy import dtype, append, average, nan, NaN
from pandas.io.sas.sas_constants import column_name_length_length
from pandas.io.html import _remove_whitespace
from pandas.core.frame import DataFrame
from numpy.distutils.conv_template import header
from Tkconstants import TOP
from matplotlib.path import Path

import pandas as pd
import sqlite3
from cProfile import label




debug = False
plot_enable = True
plot_save = False


db_path = r"C:/sqlite/aerobridge.db"
img_folder = r"D:\Images"

file_formatted_excel=""
flightName,imageName = "", ""

columns_list=['Flight', 'Image','Feature', 'Condition','Result']
db_table_list = ['TEXT','RECTANGULARWINDOW','CURVEDARROW','STRAIGHTARROW','DOORFRAMELINES','DOORFOOTPLATE','DOORHANDLE']
feature_dict = {}
dataframe_list = []

st_arrow_thres = 15


#Print function for simplicity
def toprint(printcontent):
    if(debug):
        print'{}'. format(printcontent)


def read_db(Table): # Read database file & process it for comparision
    conn = sqlite3.connect(db_path)
    query = "select * from "+Table
    df = pd.read_sql_query(query, conn)
    conn.close()
    df = process_db_df(df)
    return df

def process_db_df(df): #Split Points(P) into X & Y
    column_name_list = df.columns.tolist()
    toprint(column_name_list)
    col_to_del = ['ID']
    for column_name in column_name_list:
        if 'P' in column_name:
            col_num = column_name.lstrip('P')
            x_col_name = 'X'+col_num
            y_col_name = 'Y'+col_num
            df[x_col_name]=df[column_name].str.split(',').str.get(0)
            df[y_col_name]=df[column_name].str.split(',').str.get(1)    
            col_to_del.append(column_name) #Delete points (P) column after split
    
    df = df_delete_col(df, col_to_del)
    return df

def df_delete_col(df, col_name_list=[]): #Delete Column Specified in col_name
    column_name_list = df.columns.tolist()
    for col_name in col_name_list:
        column_name_list.remove(col_name)
    df = df[column_name_list]
    return df

def read_csv(file_rawdata):
    #Reading the no of rows to be skipped in the beginning
    rows_ignore_int = 0    
    #Read the input csv with only desired columns, skipping initial unwanted rows
    df = pd.read_csv(file_rawdata, skiprows = rows_ignore_int, index_col=False, dtype = 'str',error_bad_lines=False)
    toprint(df.columns.tolist())
    print("**********************Read Successful*************************")
    return df

def read_points_from_db(flightName,imageName,featName):
    #global feature_list
    x_coordinates,y_coordinates,co_ordinates = [] , [], {}
    #Read all available points from each feature

    df = dataframe_list[feature_dict[featName]]
    #Search datalog for Image in flight & get row index
    df = df[(df['ImageName']==imageName) & (df['Flight']==flightName)]
     
    if len(df.index.values >0 ):
        index = df.index.values[0]
        X,Y = [], []
                 
        for column in df.columns.tolist():
            if('X' in column):
                temp_x = float(df[column][index])
                if not (math.isnan(temp_x)):
                    X.append(temp_x)
                     
            if('Y' in column):
                temp_y = float(df[column][index])
                if not (math.isnan(temp_y)):
                    Y.append(temp_y)
        x_coordinates.append(X)
        y_coordinates.append(Y) 
    co_ordinates['X']=x_coordinates[0]
    co_ordinates['Y']=y_coordinates[0]
    return co_ordinates  

def read_points_from_compare_df(df, flightName,imageName,featName):
    #global feature_list
    x_coordinates,y_coordinates,co_ordinates = [] , [], {}
    
    df = df[(df['Image']==imageName) & (df['Flight']==flightName)]
    
    if len(df.index.values >0 ):
        index = df.index.values[0]
        X,Y = [], []
                
        for column in df.columns.tolist():
            if('X' in column):
                temp_x = float(df[column][index])
                if not (math.isnan(temp_x)):
                    X.append(temp_x)
                    
            if('Y' in column):
                temp_y = float(df[column][index])
                if not (math.isnan(temp_y)):
                    Y.append(temp_y)
        x_coordinates.append(X)
        y_coordinates.append(Y) 
    co_ordinates['X']=x_coordinates[0]#[feature_dict[featName]]
    co_ordinates['Y']=y_coordinates[0]#[feature_dict[featName]]
    return co_ordinates  

def calc_rel_distance(p1,p2): #p1[x1,y1] p2[x2,y2]
    
    dist = math.hypot(float(p1[0]) - float(p2[0]), float(p1[1]) - float(p2[1]))
    dist = (round(dist,2))
    return dist   

def update_result_df(df, parameter,condition,data):
    output_list = [flightName,imageName,parameter,condition,data]
    df = df.append((pd.DataFrame([output_list],columns= columns_list)),ignore_index=True)
    return df

def compare_st_arrow(db_co_ordinates, test_coordinates):
    plot_points( db_co_ordinates, test_coordinates  ,meanpoint = [],Title = '' )
    print "TE: "+str(test_coordinates)
    print "DB: "+str(db_co_ordinates)
    st_arrow_count = []
    
    for i in range(len(test_coordinates["X"])):
        p1 = [test_coordinates["X"][i] , test_coordinates["Y"][i] ]
        p2 = [db_co_ordinates["X"][i] , db_co_ordinates["Y"][i] ]
        st_arrow_count.append(calc_rel_distance(p1,p2) <=st_arrow_thres)
        
    return st_arrow_count.count(True) == len(st_arrow_count)
            

def plot_points( db_co_ordinates, test_coordinates  ,meanpoint = [],Title = '' ):
    
    if (plot_enable):
        
        image_path =path.join(img_folder,flightName+"\\"+imageName)
          
        if Title == "":
            Title = image_path
        im = plt.imread(image_path)
        plt.imshow(im)
        
        #Plot point on Image
        x , y =  db_co_ordinates["X"] ,  db_co_ordinates["Y"]
        plt.scatter(x,y, c= "b", marker= "*", label="DB Points", s=40 )
        
        #Plot point on Image
        x , y =  test_coordinates["X"] ,  test_coordinates["Y"]
        plt.scatter(x,y, c= "w", marker= "o", label="Detection Points", s=10 )
        '''plt.annotate(item,
        xy=(x, y), xytext=(-20, 20),
        textcoords='offset points', ha='right', va='bottom',
        bbox=dict(boxstyle='round,pad=0.25', fc='white', alpha=0.35),
        arrowprops=dict(arrowstyle = '->', connectionstyle='arc3,rad=0'))'''
        
        if not meanpoint == []:
            plt.scatter([meanpoint[0]],[meanpoint[1]], marker = "*", c= 'r', s=30)
        plt.title(Title)    
        plt.legend(loc='lower left', numpoints=1, ncol=3, fontsize=8)
        mng = plt.get_current_fig_manager()
        mng.resize(*mng.window.maxsize())
        
        if plot_save:
            my_path  = os.path.dirname(os.path.realpath(__file__)) #Gives present working dir path
            out_img_name = Title.split(':')
            directory = os.path.join(my_path, flightName,imageName)
            if not os.path.exists(directory):
                os.makedirs(directory)
            #plt.figure(figsize=(23.5, 13.0))
            plt.savefig(os.path.join(directory,out_img_name[0]))
        plt.show() 
        

#main_function 
def main(): 
    
    global flightName,imageName
    
    print "Hello World !!!"
    
    file_path = r"C:\Config\Processed Data\Co-Ordinates\Co-Ordinates.csv"#raw_input("Enter the path for the exe file: ")
    #Frame Output Filepath
    file_path_seperated = path.split(file_path)
    file_directory = file_path_seperated[0]
    file_formatted_csv=path.join(file_directory,'Compare_Results'+'.csv')
    
    #Create Result Dataframe
    result_df = DataFrame()
    #Read CSV File for DB Comparision
    compare_df = read_csv(file_path)
    #Initializing Variables & Reading DB Tables
    for db_table in db_table_list:
        feature_dict[db_table] = db_table_list.index(db_table)
        df = read_db(db_table)
        dataframe_list.append(df) 
        print "**************************************"+db_table+"**************************************"
        print df
    toprint(dataframe_list)
    print feature_dict
    
    for row, index in compare_df.iterrows():
        imageName = compare_df['Image'][row]
        flightName = compare_df['Flight'][row]
        featName = compare_df['Feature'][row]       
        print flightName + " : "+ imageName
        test_coordinates = read_points_from_compare_df(compare_df, flightName,imageName,featName)        
        db_co_ordinates = read_points_from_db(flightName,imageName,featName) #Reads co-ordinates of all features
        
        
        
        if featName == "STRAIGHTARROW":
            if compare_df['PassFail'][row] == '1':
                if_pass = compare_st_arrow(db_co_ordinates, test_coordinates)
                result_df = update_result_df(result_df, featName, "", ("Pass" if if_pass else "Fail"))
                print ("Pass" if if_pass else "Fail")
            else:
                result_df = update_result_df(result_df, featName, "", "Not Detected")
                print 'Not Detected'
        
        
        
    #Store Output to CSV File  
    result_df.to_csv(file_formatted_csv, index = False, header= True)    
    
if __name__ == "__main__":
    main()

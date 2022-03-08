
import pandas as pd
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
from codecs import ignore_errors
from pandas.io.gbq import _parse_data
import subprocess

import sqlite3

def run_command(command):
    p = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    #pout = p.communicate(input="3")[0]
    for line in p.stdout.readlines():
        print(line)
        if (";0" in line) or (";1;" in line) or (";2;" in line):
            retval = line
            break
    
    #retval = p.wait()
    return retval

def inc():
    global var
    var = 2

def read_csv(file_rawdata):
    #Reading the no of rows to be skipped in the beginning
    rows_ignore_int = 0    
    #Read the input csv with only desired columns, skipping initial unwanted rows
    df = pd.read_csv(file_rawdata, delimiter  = "*",skiprows = rows_ignore_int, dtype = 'str',error_bad_lines=False)
    
     
    #Remove Whitespaces From Columns
    col_header = df.columns.tolist()
    #df = rename_column(df, col_header, col_header_stripped)
         
    #df = rename_column(df, ip_format, op_format)
    print("**********************Read Successful*************************")
    return df
    
def parse_data(data):
    data_list = []
    col_list = []
    count = 0
    
    data_list = data.split(":")
    
    if len(data_list) > 1:
        temp_list = []
        pixel_list = data_list[1].split("-")
        for item in pixel_list:
            temp_list.append(item.split(";"))
        pixel_list = temp_list    
        
            
    data_str = data_list[0]
    data_str = data_str.replace(";",",")
    data_str = data_str.replace("[","")
    data_str =  data_str.replace("]","")
    data_list = data_str.split(",")
    
    for i in range(len(data_list)):
        if i == 0:
            col_list.append("Flight")
        elif i == 1:
            col_list.append("Image")
        elif i == 2:
            col_list.append("Pass/Fail")
        else:
            if i % 2 == 0:
                col_list.append("Y"+str((i-3 - count )))
                count = count +1
            else:
                col_list.append("X"+str((i-2 - count )))
            
    df = pd.DataFrame([data_list],columns= col_list)     
    
    return df , pixel_list

def calc_angle(test_coordinates, coordinates):
    X1= test_coordinates[0]
    X2= coordinates[0]
    Y1= test_coordinates[1]
    Y2= coordinates[1]

    radians = math.atan2(abs(Y2-Y1), abs(X2-X1))
    degrees = math.degrees(radians)
    return degrees
        

def read_db(flight,feature): # Read database file & process it for comparision
    
    conn = sqlite3.connect( r"C:/sqlite/aerobridge.db")
    #query = "select * from "+table
    query = "SELECT Height, Width FROM PHYSICALDIMENSIONS WHERE Flight = \""+flight+"\" and Feature = \""+feature+"\";"
    #print query
    df = pd.read_sql_query(query, conn)
    conn.close()
    return df

#main_function 
def main():
    '''file_data = "C:\Config\Log Files\Text(Center).txt"
    
    
    df = read_csv(file_data)
    #df.dropna(subset=["data"], inplace=True)
    
    
    col_name = df.columns.tolist()
    print df[col_name[0]]
    
    df['data'] = df['data'].str.replace(";",",")
    df['data'] = df['data'].str.replace("[","")
    df['data'] = df['data'].str.replace("]","")
    
    df['data']=df['data'].str.split(',')
    print  df
    
    print df.columns.tolist()
    global var 
    var = 1
    print var
    inc()
    print var'''
    
    s = "flight;image;pass;[x,y];[x2,y2];[1,2];[1111,222]:height;width-height1;width1-width"
    
    result_tuple =  parse_data(s)
    print result_tuple[0]
    print result_tuple[1]
    
    
    print calc_angle([100,100], [0,0])
    #print run_command(['C:\Users\Vinu Karthek\workspace\Test\Debug\Test.exe'])
    
    read_db("A320","Text")

if __name__ == "__main__":
    main()
    




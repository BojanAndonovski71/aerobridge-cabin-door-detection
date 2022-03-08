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
import subprocess
import sqlite3

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
#from Database_Comparision import db_path



#Run Mode : Debug | Normal
debug=True
plot_enable = True #Display plots
plot_save = False #plot_enable must be "True" for this to work
#Globals
file_config=""
file_directory=""
file_formatted_csv=""
file_formatted_excel=""
flightName,imageName = "", ""


vote_thres = 50.0/100

relarive_radius = 600
dfl_hor_thr , dfl_ver_thr = 560 , 660

columns_list=['Flight', 'Image','Parameter', 'Condition','Result']
global feature_list# ['CurvedArrow','DoorFramelines','DoorFootplate','RectangularWindow']
feature_dict = {}
marker_dict = {'curvedarrow':'>', 'doorframelines':'*', 'doorfootplate':'o', 'rectangularwindow':'+' , 'text':'<'}
dataframe_list = []


#Print function for simplicity
def toprint(printcontent):
    if(debug):
        print'{}'. format(printcontent)

#Function to read the "Key" values from ini file
def configfile_read_key(config_filename,sectionname,option):
    config = ConfigParser()
    config.read(config_filename)
    readvalue = config.get(sectionname, option)
    return readvalue

#Function to read the all "Section" Names from ini file

def configfile_read_sections(config_filename):
    config = ConfigParser()
    config.read(config_filename)
    sections = config.sections()
    return sections

def configfile_read_all_key(config_filename,sectionname):
    key_dict = {}
    key_list = []
    config = ConfigParser()
    config.read(config_filename)
    key_dict = config.items(sectionname)
    
    for key, value in key_dict:
        key_list.append(key)
    return key_list

def configfile_sections_exists(config_filename,sectionname):
    config = ConfigParser()
    config.read(config_filename)
    isPresent = config.has_section(sectionname)
    return isPresent

def configfile_key_exists(config_filename,sectionname, option):
    config = ConfigParser()
    config.read(config_filename)
    isPresent = config.has_option(sectionname, option)
    return isPresent

#Whitespace remover in string with commas

def space_remove(unformatted_string):
    formatted_string = []
    for element_read in unformatted_string:
        element_read_stripped = element_read.strip()
        formatted_string.append(element_read_stripped)
    return formatted_string

def strip_whitespaces(file_config, df):
    
    op_format_read = configfile_read_key(file_config,'Formatstrings','Output_String')
    op_format_list = op_format_read.split(',')
    op_format = space_remove(op_format_list)
    for items in op_format:
        df[items]=df[items].str.strip()
    return df

def is_non_zero_file(fpath):  
    return os.path.isfile(fpath) and os.path.getsize(fpath) > 0

def run_command(command):
    p = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    #pout = p.communicate(input="3")[0]
    for line in p.stdout.readlines():
        toprint(line)
        if (";0" in line) or (";1;" in line) or (";2;" in line):
            #print line
            retval = line
            break
    #retval = p.wait()
    return retval

def run_exe(image_file_path):
    global feature_list , feature_dict, dataframe_list , file_config

    feature_list = configfile_read_all_key(file_config, "EXE")

    height_width_list = []
    for exe in feature_list:
        print exe+"**************"
        feature_dict[exe] = feature_list.index(exe)
        print image_file_path


        if exe == "doorhandle":
            doorhandle_exe_list =  configfile_read_all_key(file_config, "DoorHandle")
            for doorhandle_exe in doorhandle_exe_list:
                result_str = run_command([configfile_read_key(file_config,'DoorHandle',doorhandle_exe),image_file_path.replace("\\","/")])
                if (";1;" in result_str):
                    break      
                
        elif exe=="someother feature":
            print"Type code here"    
         
        else:
            
            result_str = run_command([configfile_read_key(file_config,'EXE',exe),image_file_path.replace("\\","/")])#, ini_path,image])        print result_str
            #result_str = configfile_read_key(file_config,'EXE',exe)
        df , height_width = parse_data(result_str)
        print df
        dataframe_list.append(df) 

        height_width_list.append(height_width)
    return height_width_list
           
    

'''def parse_data(data_str):
    data_list = []
    col_list = []
    count = 0
    
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
            col_list.append("PassFail")
        else:
            if i % 2 == 0:
                col_list.append("Y"+str((i-3 - count )))
                count = count +1
            else:
                col_list.append("X"+str((i-2 - count )))
            
    df = pd.DataFrame([data_list],columns= col_list)     
    return df'''

def parse_data(data):
    data_list = []
    col_list = []
    count = 0
    
    data_list = data.split(":")

    pixel_list = []
    if len(data_list) > 1:
        temp_list = []
        pixel_list = data_list[1].split("-")
        for item in pixel_list:
            temp_list.append(item.split(";"))
        pixel_list = temp_list
        print pixel_list   
        
            
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
            col_list.append("PassFail")
        else:
            if i % 2 == 0:
                col_list.append("Y"+str((i-3 - count )))
                count = count +1
            else:
                col_list.append("X"+str((i-2 - count )))
            
    df = pd.DataFrame([data_list],columns= col_list)
    
    return df , pixel_list

def read_csv(file_rawdata):
    #Reading the no of rows to be skipped in the beginning
    rows_ignore_int = 0    
    #Read the input csv with only desired columns, skipping initial unwanted rows
    df = pd.read_csv(file_rawdata, skiprows = rows_ignore_int, index_col=False, dtype = 'str',error_bad_lines=False)
    toprint(df.columns.tolist())
     
    #Remove Whitespaces From Columns
    col_header = df.columns.tolist()
    col_header_stripped = space_remove(col_header)
    #df = rename_column(df, col_header, col_header_stripped)
         
    #df = rename_column(df, ip_format, op_format)
    print("**********************Read Successful*************************")
    return df

def read_data(file_config,sectionname,key):
    file_data = configfile_read_key(file_config,sectionname,key)
    toprint(file_data)
    df = read_csv( file_data)
    return df

def rename_column(df,ip_format,opt_format):    
    #Create Dictionary
    toprint("rename_column")
    toprint(ip_format)
    toprint(opt_format)
    column_name_new = {}
    for i in range(0,ip_format.__len__()):
        column_name_new[ip_format[i]] = opt_format[i]
    
    #Rename Column Headers
    df = df.rename(columns=column_name_new)
    toprint(df.columns.tolist())
    return df
  
def insert_column(file_config, df):
    insert_column_read = configfile_read_key(file_config,'Insert_Column','Column_Name')
    insert_column_list = insert_column_read.split(',')
    insert_column = space_remove(insert_column_list)
    toprint(insert_column)
    
    insert_value_read = configfile_read_key(file_config,'Insert_Column','Column_Value')
    insert_value_list = insert_value_read.split(',')
    insert_value = space_remove(insert_value_list)
    toprint(insert_value)
    for i in range(0, len(insert_column)):
        df.insert(i, insert_column[i], insert_value[i], allow_duplicates=False)
        toprint(df.columns.tolist())
    print("***********************Column Insert Complete*************************")
    return df

def reorder_column(file_config, df):
    column_order_read = configfile_read_key(file_config,'Column_Order','Column_Order')
    column_order_list = column_order_read.split(',')
    column_order = space_remove(column_order_list)
    toprint(column_order)
    df=df[column_order]
    return df

def get_files_from_folder(path):
    file_list = []
    if (isfile(path)): # Single File
        file_list.append(path)
        
    else: # Multiple Files
        for f in listdir(path):
            file_path = join(path, f)
            file_name,extension = splitext(f)
            if ( isfile(file_path) and extension == ".png"):
                file_list.append(file_path)
    #toprint(file_list)
    return file_list

def read_points_from_datalog(image_path, flightName,imageName):
    
    #global feature_list
    x_coordinates,y_coordinates,co_ordinates = [] , [], {}
    #Read all available points from each feature
    for feature in feature_list:
        df = dataframe_list[feature_dict[feature]]
        #Search datalog for Image in flight & get row index
        df = df[(df['Image']==imageName) & (df['Flight']==flightName)]
        index = df.index.values[0]
        X,Y = [], []
        #toprint(df.columns.tolist())
        toprint(feature)
        
        if ((df['PassFail'][index]=='1') or (df['PassFail'][index]=='2')):
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
    co_ordinates['X']=x_coordinates
    co_ordinates['Y']=y_coordinates
    return co_ordinates        

def calculate_doorframe_parameters(subfeature_dict):
    global dfl_hor_thr , dfl_ver_thr
    dfl_points_dict = {}
    
    for subfeature in subfeature_dict:
        if ("doorframelines" in subfeature):
            dfl_points_dict[subfeature] = subfeature_dict[subfeature]
    
    if len(dfl_points_dict) == 2:
        dfl_hor_thr =  abs(dfl_points_dict['doorframelines1'][0] - dfl_points_dict['doorframelines2'][0])
    
    if len(dfl_points_dict) == 6:
        dfl_ver_thr =  abs(dfl_points_dict['doorframelines1'][1] - dfl_points_dict['doorframelines3'][1])
        dfl_hor_thr = abs(dfl_points_dict['doorframelines2'][0] - dfl_points_dict['doorframelines5'][0])
        subfeature_dict['doorframelines1'] = subfeature_dict['doorframelines2']
        subfeature_dict['doorframelines2'] = subfeature_dict['doorframelines5']
        del subfeature_dict['doorframelines3'] , subfeature_dict['doorframelines4'] ,subfeature_dict['doorframelines5'], subfeature_dict['doorframelines6']
        
    if len(dfl_points_dict) == 3:
        dfl_ver_thr = abs(dfl_points_dict['doorframelines1'][1] - dfl_points_dict['doorframelines3'][1])
        subfeature_dict['doorframelines1'] = subfeature_dict['doorframelines2']
        del subfeature_dict['doorframelines2'] , subfeature_dict['doorframelines3']
        
    print dfl_points_dict
    return subfeature_dict

def plot_points(image_path, subfeature_dict, subfeature_valid_list  ,meanpoint = [],Title = '' ):
    
    if (plot_enable or 'Raw Image:' in Title):
           
        if Title == "":
            Title = image_path
        
        im = plt.imread(image_path)
        plt.imshow(im)
        
        #Plot point on Image
        for item in subfeature_dict:
            #print subfeature_dict[item]
            x , y =  subfeature_dict[item][0] ,  subfeature_dict[item][1]
            if not (x == []):
                if (subfeature_valid_list[item]):
                    color = 'g'
                else:
                    color = 'r'
                plt.scatter(x,y, c= color, s=40 )
                plt.annotate(
            item,
            xy=(x, y), xytext=(-20, 20),
            textcoords='offset points', ha='right', va='bottom',
            bbox=dict(boxstyle='round,pad=0.25', fc='white', alpha=0.35),
            arrowprops=dict(arrowstyle = '->', connectionstyle='arc3,rad=0'))
        if not meanpoint == []:
            plt.scatter([meanpoint[0]],[meanpoint[1]], marker = "*", c= 'w', s=30)
        plt.title(Title)    
        plt.legend(loc='lower left', numpoints=1, ncol=3, fontsize=8)
        mng = plt.get_current_fig_manager()
        mng.resize(*mng.window.maxsize())
        #mng.window.state('zoomed')
        if plot_save:
            my_path  = os.path.dirname(os.path.realpath(__file__)) #Gives present working dir path
            out_img_name = Title.split(':')
            directory = os.path.join(my_path, flightName,imageName)
            if not os.path.exists(directory):
                os.makedirs(directory)
            #plt.figure(figsize=(23.5, 13.0))
            plt.savefig(os.path.join(directory,out_img_name[0]))
        plt.show() 
        
            

def str2bool(string):
    return string.lower() in ("asc", "ascending", "inc", "increasing", "true", "1", "t")

def average(array):
    #list_str = [x for x in list_str if str(x) != 'nan']
    #list_int = map(float, list_str)
    average = reduce(lambda x, y: x + y, array) / len(array)
    return average

def calc_meanpoint(co_ordinates):
    meanpoint=[]
    x_coordinates,y_coordinates = co_ordinates['X'] , co_ordinates['Y']
    x_list, y_list = [], []
    for item in range(len(x_coordinates)):
        if not (x_coordinates[item] == []):
            for subitem in x_coordinates[item]:
                x_list.append(subitem)
            for subitem in y_coordinates[item]:
                y_list.append(subitem)          
    
    meanpoint.append(average(x_list))
    meanpoint.append(average(y_list))
    return meanpoint

def calc_distance(x_list,y_list, meanpoint):
    distance = []
    #midpoint = calc_midpoint(x_list,y_list)
    for i in range(len(x_list)) :
        if str(i) != 'nan':
            dist = math.hypot(float(x_list[i]) - meanpoint[0], float(y_list[i]) - meanpoint[1])
            distance.append(round(dist,2))
        else:
            distance.append(nan)   
    return distance

def calc_rel_distance(p1,p2): #p1[x1,y1] p2[x2,y2]
    
    dist = math.hypot(float(p1[0]) - float(p2[0]), float(p1[1]) - float(p2[1]))
    dist = (round(dist,2))
    return dist   

def check_if_between(dfl1 , dfl2, test_coordinates):
    
    #This function takes in co-ordinates of door framelines , feature and name of feature, then checks if the feature
    #point is between door framelines
    
    if dfl1[0] >= dfl2[0]:
        upper_range , lower_range = dfl1[0] , dfl2[0]
    else:
        upper_range , lower_range = dfl2[0] , dfl1[0]
    #print str(upper_range) + str(lower_range)
       
    if ((test_coordinates[0] <= upper_range) and (test_coordinates[0] >= lower_range) )  :
        dist1 = abs(dfl1[1]-test_coordinates[1])#calc_rel_distance(dfl1, test_coordinates)
        dist2 = abs(dfl2[1]-test_coordinates[1])#calc_rel_distance(dfl2, test_coordinates)
        if (dist1<=(dfl_ver_thr/2) or dist2<=(dfl_ver_thr/2) ):
            return True
        else:
            return False
    else:
        return False
      
    
def check_if_above(test_coordinates, feature_coordinates):
    #Compare if test coordinate is above test_coordinates
    
    x_test_coordinates, y_test_coordinates= test_coordinates[0], test_coordinates[1]
    x_feature_coordinates,y_feature_coordinates = feature_coordinates[0], feature_coordinates[1]
    
    vertical_difference = y_test_coordinates - y_feature_coordinates 
    hor_difference = abs(x_test_coordinates - x_feature_coordinates)
    
    if ((hor_difference <= (dfl_hor_thr/2.0)) and (vertical_difference >=0)):
        return True
    else:
        return False
    
def check_if_side(test_coordinates, feature_coordinates, side ):
    
    
    x_test_coordinates,y_test_coordinates= test_coordinates[0],test_coordinates[1]
    x_feature_coordinates,y_feature_coordinates = feature_coordinates[0],feature_coordinates[1]
    
    hor_difference = x_test_coordinates - x_feature_coordinates
    dist = abs(hor_difference)
    ver_difference = abs(y_test_coordinates - y_feature_coordinates)
    
    if (side == 'r'):
        if ((dist <= dfl_hor_thr) and (hor_difference <=0) and (ver_difference <=(dfl_ver_thr/2.0))):
            return True
        else:
            return False
    else:
        if ((dist <= dfl_hor_thr) and (hor_difference >=0) and (ver_difference <=(dfl_ver_thr/2.0))):
            return True
        else:
            return False
    
def check_if_below(test_coordinates, feature_coordinates):
    #Compare if test coordinate is below test_coordinates
    
    x_test_coordinates, y_test_coordinates= test_coordinates[0], test_coordinates[1]
    x_feature_coordinates,y_feature_coordinates = feature_coordinates[0], feature_coordinates[1]
    
    vertical_difference = y_test_coordinates - y_feature_coordinates 
    hor_difference = abs(x_test_coordinates - x_feature_coordinates)
    
    
    if ((hor_difference < relarive_radius) and (vertical_difference <0)):
        return True
    else:
        return False

def update_result_df(df, parameter,condition,data):
    output_list = [flightName,imageName,parameter,condition,data]
    df = df.append((pd.DataFrame([output_list],columns= columns_list)),ignore_index=True)
    return df


def relative_distance(result_df):
    
    global no_of_tests      
    print '*****************************Relative Distance *****************************'
    for test_item in subfeature_dict:
        test_coordinates = subfeature_dict[test_item]
        length = len(subfeature_dict_rel_dist_cluster[test_item])
        for item in subfeature_dict:
            coordinates = subfeature_dict[item]
            dist = calc_rel_distance(test_coordinates, coordinates)
            #print test_item+':' + str(test_coordinates)+'---->'+item +':' + str(coordinates) + str(dist)
            if_valid = dist < relarive_radius
            subfeature_dict_rel_dist_cluster[test_item].append(if_valid)
            subfeature_valid_list[item] = if_valid
            
        subfeature_dict_rel_dist[test_item] = subfeature_valid_list[test_item]
        #plot_points(image_file_path, subfeature_dict, subfeature_valid_list, meanpoint=test_coordinates ,Title=(test_item+':'+image_file_path))
        length = len(subfeature_dict_rel_dist_cluster[test_item])
        vote_count = subfeature_dict_rel_dist_cluster[test_item].count(False)       
        subfeature_dict_rel_dist[test_item] = vote_count < ((length)/2.0)
                
        print test_item+':'+str(vote_count)+' Votes of '+str(length)
        result_df = update_result_df(result_df, test_item,'Vote',vote_count)
        result_df = update_result_df(result_df, test_item,'Relative Distance',str(subfeature_dict_rel_dist[test_item]))
        
    #print subfeature_dict_rel_dist
    plot_points(image_file_path, subfeature_dict, subfeature_dict_rel_dist ,Title=('1_Relative Distance:'+image_file_path))
    no_of_tests= 1.0
    return result_df

def doorframelines(result_df):
    global no_of_tests
    
    print '*****************************Between DoorFrameLines *****************************'  
    if ('doorframelines1' in subfeature_dict and 'doorframelines2' in subfeature_dict ):  
        dfl1 , dfl2 = subfeature_dict['doorframelines1'] , subfeature_dict['doorframelines2']     
        for test_item in subfeature_dict:
            test_coordinates = subfeature_dict[test_item]
            
            if_between = check_if_between(dfl1 , dfl2, test_coordinates)
            subfeature_dict_btw_dfl[test_item] = if_between
            subfeature_valid_list[test_item] = if_between
            result_df = update_result_df(result_df, test_item,'Between DFL',str(subfeature_valid_list[test_item]))
        
        print subfeature_dict_btw_dfl
        plot_points(image_file_path, subfeature_dict, subfeature_valid_list, meanpoint=[[dfl1[0],dfl2[0]],[dfl1[1],dfl2[1]]] ,Title=('2_Between DoorFrameLines:'+image_file_path))
        no_of_tests = no_of_tests+1.0
        
    print '*****************************Right DoorFrameLines *****************************'  
    if ('doorframelines1' in subfeature_dict and 'doorframelines2' in subfeature_dict ):  
        dfl1 , dfl2 = subfeature_dict['doorframelines1'] , subfeature_dict['doorframelines2']
        if dfl1[0] > dfl2[0]: 
            dfl =  dfl1 
        else:
            dfl = dfl2   
        for test_item in subfeature_dict:
            test_coordinates = subfeature_dict[test_item]
            if_left = check_if_side(dfl, test_coordinates, 'l')
            if_right = check_if_side(dfl, test_coordinates, 'r')
            subfeature_dict_l_dfl[test_item] = if_left or if_right
            subfeature_valid_list[test_item] = if_left or if_right
            result_df = update_result_df(result_df, test_item,'Right DFL',str(subfeature_valid_list[test_item]))
        
        print subfeature_dict_l_dfl
        plot_points(image_file_path, subfeature_dict, subfeature_valid_list, meanpoint = dfl,Title=('3_Right DoorFrameLines:'+image_file_path))
        no_of_tests = no_of_tests+1.0
        
    print '*****************************Left DoorFrameLines *****************************'  
    if ('doorframelines1' in subfeature_dict and 'doorframelines2' in subfeature_dict ):  
        dfl1 , dfl2 = subfeature_dict['doorframelines1'] , subfeature_dict['doorframelines2']
        if dfl1[0] < dfl2[0]: 
            dfl =  dfl1 
        else:
            dfl = dfl2   
        for test_item in subfeature_dict:
            test_coordinates = subfeature_dict[test_item]
            if_left = check_if_side(dfl, test_coordinates, 'l')
            if_right = check_if_side(dfl, test_coordinates, 'r')
            subfeature_dict_r_dfl[test_item] = if_right or if_left
            subfeature_valid_list[test_item] = if_right or if_left
            result_df = update_result_df(result_df, test_item,'Left DFL',str(subfeature_valid_list[test_item]))
        
        print subfeature_dict_r_dfl
        plot_points(image_file_path, subfeature_dict, subfeature_valid_list, meanpoint = dfl,Title=('4_Left DoorFrameLines:'+image_file_path))
        no_of_tests = no_of_tests+1.0
        
    print '*****************************One  DoorFrameLines *****************************'     
    if not ('doorframelines1' in subfeature_dict and 'doorframelines2' in subfeature_dict ):
        run_one_dfl = False
        if ('doorframelines1' in subfeature_dict):
            dfl = subfeature_dict['doorframelines1']
            run_one_dfl = True
        elif ('doorframelines2' in subfeature_dict):
            dfl = subfeature_dict['doorframelines2']
            run_one_dfl = True
        if (run_one_dfl):
            for test_item in subfeature_dict:
                test_coordinates = subfeature_dict[test_item]
                if_left = check_if_side(dfl, test_coordinates, 'l')
                if_right = check_if_side(dfl, test_coordinates, 'r')
                subfeature_dict_r_dfl[test_item] = if_right or if_left
                subfeature_valid_list[test_item] = if_right or if_left
                result_df = update_result_df(result_df, test_item,'Single DFL',str(subfeature_valid_list[test_item]))
                
            print subfeature_dict_r_dfl
            plot_points(image_file_path, subfeature_dict, subfeature_valid_list, meanpoint = dfl,Title=('2_Single DoorFrameLines:'+image_file_path))
            no_of_tests = no_of_tests+1.0
        
    return result_df

def above_footplate(result_df):
    
    global no_of_tests  
    print '*****************************Above Footplate *****************************'  
    if ('doorfootplate1' in subfeature_dict ): 
        dfp = subfeature_dict['doorfootplate1']      
        for test_item in subfeature_dict:
            test_coordinates = subfeature_dict[test_item]
            
            if_above = check_if_above(dfp, test_coordinates)
            subfeature_dict_abv_dfp[test_item] = if_above
            subfeature_valid_list[test_item] = if_above
            result_df = update_result_df(result_df, test_item,'Above Footplate',str(subfeature_valid_list[test_item]))
        
        #print subfeature_dict_abv_dfp
        plot_points(image_file_path, subfeature_dict, subfeature_valid_list, meanpoint=dfp ,Title=('5_Above Footplate:'+image_file_path))
        no_of_tests = no_of_tests+1.0
        
    return result_df

def above_doorhandle(result_df):
    print '*****************************Arrow & Window Above Handle *****************************'  
    if ('doorhandle1' in subfeature_dict ):
        dh = subfeature_dict['doorhandle1']
        subfeature_dict_abv_dh['doorhandle1'] = True
        if ('curvedarrow1' in subfeature_dict  ):       
            ca = subfeature_dict['curvedarrow1']
            if_above = check_if_above(dh, ca)
            subfeature_dict_abv_dh['curvedarrow1'] = if_above
            result_df = update_result_df(result_df, "Curved Arrow",'Above Handle',if_above)
                            
            print 'ca above dh:' + str(if_above)
        
        if ('straightarrow1' in subfeature_dict  ):       
            sa = subfeature_dict['straightarrow1']
            if_above = check_if_above(dh, sa)
            subfeature_dict_abv_dh['straightarrow1'] = if_above
            result_df = update_result_df(result_df, "Straight Arrow",'Above Handle',if_above)
                            
            print 'sa above dh:' + str(if_above)
            
        if ('rectangularwindow1' in subfeature_dict):       
            rw = subfeature_dict['rectangularwindow1']
            if_above = check_if_above(dh, rw)
            subfeature_dict_abv_dh['rectangularwindow1'] = if_above
            result_df = update_result_df(result_df, "Rect Window",'Above Handle',if_above)
            print 'rect_window above dh:' + str(if_above)
        
        if ('circularwindow1' in subfeature_dict ):       
            cw = subfeature_dict['circularwindow1']
            if_above = check_if_above(dh, cw)
            subfeature_dict_abv_dh['circularwindow1'] = if_above
            result_df = update_result_df(result_df, "Circular Window",'Above Handle',if_above)
            print 'cw above dh:' + str(if_above)
        
        #print     subfeature_dict_abv_dh
        plot_points(image_file_path, subfeature_dict, subfeature_dict_abv_dh ,Title=('6_Above DoorHandle:'+image_file_path))
    return result_df        

def final_ranking(result_df):
    
    print '*****************************Final Ranking *****************************'  
    for item in subfeature_dict:
        temp =[]
        
        if not subfeature_dict_rel_dist == {}:
            temp.append(subfeature_dict_rel_dist[item])
        
        if not subfeature_dict_btw_dfl == {}:
            temp.append(subfeature_dict_btw_dfl[item])
        
        if not subfeature_dict_r_dfl == {}:
            temp.append(subfeature_dict_r_dfl[item])
            
        if not subfeature_dict_l_dfl == {}:
            temp.append(subfeature_dict_l_dfl[item])
        
        if not subfeature_dict_abv_dfp == {}:
            temp.append(subfeature_dict_abv_dfp[item])
            
        if item in  subfeature_dict_abv_dh:
            temp.append(subfeature_dict_abv_dh[item])
        
        vote_count = temp.count(False)
        if_pass = temp.count(True)>(vote_thres*no_of_tests)
        subfeature_valid_list[item] = if_pass
        subfeature_dict_final_ranking[item] = if_pass
        
        print item+':'+str(vote_count)+' Votes of '+str(int(no_of_tests)) 
        result_df = update_result_df(result_df, item,'Vote',vote_count)
        result_df = update_result_df(result_df, item,'Final Ranking',str(subfeature_valid_list[item]))
    
    #print subfeature_dict_final_ranking
    plot_points(image_file_path, subfeature_dict, subfeature_valid_list ,Title=('7_Final Ranking:'+image_file_path))
    
    return result_df

def final_grouping(result_df):
    
    print '*****************************Final Grouping *****************************'
    #Verify Final Distance
    final_co_ordinates = {}
    final_x_co_ordinates ,final_y_co_ordinates= [] , []
    for item in subfeature_dict:
        if subfeature_dict_final_ranking[item]:
            final_x_co_ordinates.append([subfeature_dict[item][0]]) 
            final_y_co_ordinates.append([subfeature_dict[item][1]])
    final_co_ordinates['X']=final_x_co_ordinates
    final_co_ordinates['Y']=final_y_co_ordinates        
    
    if not final_x_co_ordinates == []:
        f_meanpoint = calc_meanpoint(final_co_ordinates)
        for item in subfeature_dict:
            if subfeature_dict_final_ranking[item]:
                distance = calc_distance([subfeature_dict[item][0]], [subfeature_dict[item][1]], f_meanpoint)
                #print distance
                subfeature_valid_list[item] = average(distance)<=relarive_radius
                subfeature_dict_final_grouping[item] = average(distance)<=relarive_radius
            else:
                subfeature_valid_list[item] = False
                subfeature_dict_final_grouping[item] = False
            result_df = update_result_df(result_df, item,'Group Remaining',str(subfeature_valid_list[item]))    
       
        
        print subfeature_valid_list
        plot_points(image_file_path, subfeature_dict, subfeature_valid_list ,Title=('8_Final Grouping:'+image_file_path) , meanpoint = f_meanpoint)
    
    return result_df

def read_db_phy_dim(flight,feature): # Read database file & process it for comparision
    
    db_path = configfile_read_key(file_config,'Aerobridge','DBPath')
    conn = sqlite3.connect( db_path)
    query = "SELECT Height, Width FROM PHYSICALDIMENSIONS WHERE Flight = \""+flight+"\" and Feature = \""+feature+"\";"
    df = pd.read_sql_query(query, conn)
    conn.close()
    return df

def get_phsical_dim(width,height,feature):
    actual_H =0
    actual_W =0

    if not "doorframelines" in feature:
        feature = ''.join([i for i in feature if not i.isdigit()]) #Remove numbers from string
    print feature +"******************"
    
    df =  read_db_phy_dim(flightName,feature)
    if not df.empty:
        actual_H = df["Height"][0]
        actual_W = df["Width"][0]

    '''if "rectangularwindow" in feature:
        actual_H =0.4
        actual_W =0.2

    elif "text" in feature:
        actual_H =0.3
        actual_W =0.45

    elif "curvedarrow" in feature:
        actual_H =0.4
        actual_W =0.35

    elif "doorhandle" in feature:
        actual_H =0.3
        actual_W =0.55
        
    elif "doorframelines1" in feature:
        actual_H =2.9
        actual_W=10.3

    elif "doorframelines2" in feature:
        actual_H =2.9
        actual_W =10.4

    elif "doorfootplate" in feature:
        actual_H =0.2
        actual_W =0.9

    elif "straightarrow" in feature:
        actual_H =0.2
        actual_W =0.9'''
      
    F =3300 #pixels
    Dist = 0.3 #meter measurement code here
    pix_H =(Dist*float(height))/F
    pix_W =(Dist*float(width))/F
    
    #print "Pixel Dimension :"+str(float(width))+":"+str(float(height))
    print "Physical Dimension:\n Height - "+str(pix_H)+" Actual Height - "+str(actual_H)+"\n Width - "+str(pix_W)+" Actual Width - "+str(actual_W)
    #comparision actual and pix
    if (  (pix_H <= actual_H) and (pix_W <= actual_W)):
        print "True"
        return True
    else:
        print "False"
        return False

def compare_physical_dimension(result_df):
    
    print '*****************************Physical Dimension *****************************'  
    for item in subfeature_dict:
        if subfeature_dict_final_grouping[item] and not subfeature_dict_final_grouping[item]==[]:
            h = subfeature_height_width_dict[item][0]
            w = subfeature_height_width_dict[item][1]
            
            if_pass = get_phsical_dim(h,w,item)
            subfeature_valid_list[item] = if_pass
            subfeature_dict_physical_dimension[item] = if_pass
        result_df = update_result_df(result_df, item,'Physical Dimension',str(subfeature_valid_list[item]))

    #print subfeature_dict_final_ranking
    plot_points(image_file_path, subfeature_dict, subfeature_valid_list ,Title=('Physical Dimension:'+image_file_path))
    return result_df

def calc_angle(test_coordinates, coordinates):
    X1= test_coordinates[0]
    X2= coordinates[0]
    Y1= test_coordinates[1]
    Y2= coordinates[1]

    radians = math.atan2(abs(Y2-Y1), abs(X2-X1))
    degrees = math.degrees(radians)
    return degrees

def read_db_phy_rel_dist(flight,feature1,feature2): # Read database file & process it for comparision
    
    db_path = configfile_read_key(file_config,'Aerobridge','DBPath')
    conn = sqlite3.connect( db_path)
    query = "SELECT Distance, Angle FROM PHYSICALRELDIST WHERE Flight = \""+flight+"\" and Feature1 = \""+feature1+ "\"and Feature2 = \""+feature2+"\";"
    #print query
    df = pd.read_sql_query(query, conn)
    conn.close()
    return df

def physical_relative_distance(result_df):
    print '*****************************Physical Relative Distance *****************************'

    for test_item in subfeature_dict:
        subfeature_dict_rel_dist_cluster[test_item] = []
        test_coordinates = subfeature_dict[test_item]
        print "******************************************************"+ test_item+"******************************************************"
        for item in subfeature_dict:
            if_valid = False
            if subfeature_dict_final_grouping[test_item]:
                if subfeature_dict_final_grouping[item] :
                    coordinates = subfeature_dict[item] 
                    dist = calc_rel_distance(test_coordinates, coordinates)
                    F =1000 #pixels
                    Dist = 0.6 #meter measurement code here
                    #do calculation
                    phy_dist =(Dist*float(dist))/F
                    angle = calc_angle(test_coordinates, coordinates)                 
                    actual_dist =0.0
                    actual_angle = 0
                    feature1 = test_item
                    feature2 = item
                    
                    if not "doorframelines" in test_item:
                        feature1 = ''.join([i for i in test_item if not i.isdigit()]) #Remove numbers from string
                    if not "doorframelines" in item:
                        feature2 = ''.join([i for i in item if not i.isdigit()]) #Remove numbers from string
                    print "#########################################################################"
                    df = read_db_phy_rel_dist(flightName,feature1,feature2)
                    
                    if df.empty:
                        df = read_db_phy_rel_dist(flightName,feature2,feature1)
                                        
                    if not df.empty:
                        actual_dist = df["Distance"][0]
                        actual_angle = df["Angle"][0]
                            
                        if_valid = phy_dist <= actual_dist and angle <= actual_angle
                        
                        print item+" :\n Distance - "+str(phy_dist)+": Actual Distance - "+str(actual_dist)+":\n Angle - "+str(angle)+": Actual Angle - "+str(actual_angle)+":\n Result: "+str(if_valid)
                        subfeature_dict_rel_dist_cluster[test_item].append(if_valid)
            else:
                subfeature_dict_rel_dist_cluster[test_item].append(if_valid)

        print subfeature_dict_rel_dist_cluster[test_item]    
        subfeature_dict_phy_rel_dist[test_item] = subfeature_valid_list[test_item]
        #plot_points(image_file_path, subfeature_dict, subfeature_valid_list, meanpoint=test_coordinates ,Title=(test_item+':'+image_file_path))
        length = len(subfeature_dict_rel_dist_cluster[test_item])
        vote_count = subfeature_dict_rel_dist_cluster[test_item].count(False)       
        subfeature_dict_phy_rel_dist[test_item] = vote_count <= ((length)/2.0)
                
        print test_item+':'+str(vote_count)+' Votes of '+str(length)
        result_df = update_result_df(result_df, test_item,'Vote',vote_count)
        result_df = update_result_df(result_df, test_item,'Physical Relative Distance',str(subfeature_dict_rel_dist[test_item]))
        
    plot_points(image_file_path, subfeature_dict, subfeature_dict_phy_rel_dist ,Title=('Physical Relative Distance:'+image_file_path))
    return result_df

    
#main_function 
def main():
    
    global feature_list, subfeature_dict, subfeature_valid_list, flightName, imageName ,image_file_path, no_of_tests , file_config
    
    global subfeature_dict_rel_dist_cluster , subfeature_dict_rel_dist , subfeature_height_width_dict
    
    global subfeature_dict_btw_dfl, subfeature_dict_r_dfl, subfeature_dict_l_dfl
    
    global subfeature_dict_abv_dfp,subfeature_dict_abv_dh ,subfeature_dict_final_ranking, subfeature_dict_final_grouping

    global subfeature_dict_physical_dimension, subfeature_dict_phy_rel_dist
    
    print "hello world!"
       
    file_config = r"C:\Config\Config.ini"#raw_input("Enter the path for the exe file: ")
    
    #Frame Output Filepath
    file_path_seperated = path.split(file_config)
    file_directory = file_path_seperated[0]
    file_formatted_csv=path.join(file_directory,'Output\Results'+'.csv')
    
    #Get File(s) from Folder
    image_folder_path = configfile_read_key(file_config,'Aerobridge','ImagePath')
    image_file_path_list = get_files_from_folder(image_folder_path)
    print(image_file_path_list)
   
    ''' #Initializing Variables
    feature_list = configfile_read_all_key(file_config, "Algorithm")
    for feature in feature_list:
        feature_dict[feature] = feature_list.index(feature)
        df = read_data(file_config, 'Algorithm',feature)
        dataframe_list.append(df)    
    toprint(feature_list)
    toprint(feature_dict)
    #toprint(dataframe_list)'''
    
    #Create Result Dataframe
    result_df = DataFrame()
    
    for image_file_path in image_file_path_list:
        subfeature_height_width_list = run_exe(image_file_path)
        print(subfeature_height_width_list)
        toprint(feature_list)
        toprint(feature_dict)
        print dataframe_list
        
        image_file_dir ,image_file_name = path.split(image_file_path)
        imageName = image_file_name
        
        file_path_seperated = path.split(image_file_dir)
        flightName = file_path_seperated[1]
        print "*********************"+flightName+ ':' + imageName+"****************************"
        
        #subfeature_list = []
        subfeature_valid_list = {}
        subfeature_dict = {}
        subfeature_height_width_dict = {}
        subfeature_dict_rel_dist_cluster = {}
        subfeature_dict_rel_dist, subfeature_dict_btw_dfl, subfeature_dict_r_dfl, subfeature_dict_l_dfl = {} ,{},{},{}
        subfeature_dict_abv_dfp,subfeature_dict_abv_dh ,subfeature_dict_final_ranking= {}, {},{}
        subfeature_dict_final_grouping = {}
        subfeature_dict_physical_dimension = {}
        subfeature_dict_phy_rel_dist = {}
        
        #Read all available feature Co-ordinates
        co_ordinates = {}
        co_ordinates = read_points_from_datalog(image_file_path, flightName,imageName)
        x_coordinates,y_coordinates = co_ordinates['X'] , co_ordinates['Y'] 
        toprint(co_ordinates)
        
        for i in range(len(x_coordinates)):
            temp = []
            if len(x_coordinates[i]) == 0:
                temp.append(feature_list[i] + '1')
            else:
                for j in range(len(x_coordinates[i])):
                    subfeature = feature_list[i] + str(j+1)
                    temp.append(subfeature)
                    #Init Result Dicts
                    subfeature_dict_rel_dist_cluster[subfeature] , subfeature_dict_btw_dfl[subfeature],subfeature_dict_abv_dfp[subfeature] = [],[],[]
                    subfeature_dict_l_dfl[subfeature],subfeature_dict_r_dfl[subfeature],subfeature_dict_abv_dh[subfeature] = [] , [], []
                    subfeature_dict_final_ranking[subfeature] = []
                    subfeature_dict_final_grouping[subfeature] = []
                    subfeature_dict_physical_dimension[subfeature] = []
                    subfeature_dict_phy_rel_dist[subfeature] = []
                    subfeature_valid_list[subfeature] = False
                    subfeature_dict[subfeature]=[x_coordinates[i][j] ,y_coordinates[i][j] ]

                    if not subfeature_height_width_list[i] == [] and j<=(len(subfeature_height_width_list[i])-1):
                        subfeature_height_width_dict[subfeature] = subfeature_height_width_list[i][j]
            #subfeature_list.append(temp)
        #print subfeature_list
        print subfeature_dict # Co-Ordinates of each sub feature
        print subfeature_height_width_dict
        meanpoint = calc_meanpoint(co_ordinates)
        plot_points(image_file_path, subfeature_dict, subfeature_valid_list, meanpoint , ('Raw Image:'+image_file_path))
        
        #Doorframelines
        subfeature_dict = calculate_doorframe_parameters(subfeature_dict)
        print "Hor Thres: "+str(dfl_hor_thr)
        print "Ver Thres: "+str(dfl_ver_thr)
        
        #Calculate Mean Point & Plot points
        meanpoint = calc_meanpoint(co_ordinates)
        plot_points(image_file_path, subfeature_dict, subfeature_valid_list, meanpoint , ('Raw Image:'+image_file_path))
        no_of_tests = 0
        #Log Meanpoint
        #result_df = update_result_df(result_df, 'meanpoint','',meanpoint)
        
        #Relative Distance Check
        result_df = relative_distance(result_df)
        print subfeature_dict_rel_dist
        
        #Door Frame Lines Check
        result_df = doorframelines(result_df)            
               
        #Door Foot Plate Check
        result_df = above_footplate(result_df)
        print subfeature_dict_abv_dfp
            
        #Door Handle Check
        result_df = above_doorhandle(result_df)
        print  subfeature_dict_abv_dh
        
        #Final Ranking
        result_df = final_ranking(result_df)
        print subfeature_dict_final_ranking
        
        #Final Grouping
        result_df = final_grouping(result_df)
        print subfeature_dict_final_grouping

        #Compare Physical Dimension
        result_df = compare_physical_dimension(result_df)
        print subfeature_dict_physical_dimension
        
        #Compare Physical Relative Dimension
        result_df = physical_relative_distance(result_df)
        print subfeature_dict_phy_rel_dist
        
        #Store Output to CSV File  
        result_df.to_csv(file_formatted_csv, index = False, header= True)
           
    print "Successfully generated the processed file"
    
if __name__ == "__main__":
    main()

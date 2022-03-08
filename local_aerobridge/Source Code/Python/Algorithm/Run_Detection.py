from fileinput import close
debug = True
import os
from os import listdir
from os import path
from os.path import isfile, join, splitext
from ConfigParser import ConfigParser
import subprocess


#Print function for simplicity
def toprint(printcontent):
    if(debug):
        print '{}'.format(printcontent)
     
def configfile_sections_exists(config_filename,sectionname):
    config = ConfigParser()
    config.read(config_filename)
    isPresent = config.has_section(sectionname)
    return isPresent
       
#Function to read the "Key" values from ini file
def configfile_read_key(config_filename,sectionname,option):
    config = ConfigParser()
    config.read(config_filename)
    readvalue = config.get(sectionname, option)
    return readvalue

#Function to write the "Key" values from ini file
def configfile_write_key(config_filename,sectionname,option , value):
    config = ConfigParser()
    #config.remove_section(sectionname)
    config.add_section(sectionname)
    config.set(sectionname, option, value)
    
    with open(config_filename,'wb') as cf:
        toprint("open")
        config.write(cf)
        cf.close()
        
def run_command(command):
    p = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    #pout = p.communicate(input="3")[0]
   
    for line in p.stdout.readlines():
        toprint(line)
    retval = p.wait()
    return retval

def subprocess_cmd(command):
    process = subprocess.Popen(command,stdout=subprocess.PIPE)
    proc_stdout = process.communicate()[0].strip()
    print proc_stdout



def main():

    print "hello world!!"
           
    ini_path = r"C:\Config\Config.ini"#raw_input("Enter the path for the exe file: ")
    file_directory = configfile_read_key(ini_path,'Aerobridge','EXEPath')#r'C:\Config\EXE'
    file_image = configfile_read_key(ini_path,'Aerobridge','ImagePath')
    
    rawdata_list = []
    if (isfile(file_image)): # Single File
        rawdata_list.append(file_image)
        file_path_seperated = path.split(file_image)
        
    else: # Multiple Files
        for f in listdir(file_image):
            file_path = join(file_image, f)
            file_name,extension = splitext(f)
            if ( isfile(file_path) and extension == ".png"):
                rawdata_list.append(file_path)
    toprint(rawdata_list)
    
    print"Script Executing Please Wait........."
    for image in rawdata_list:
        toprint(image)
        
        DoorFramelines = path.join(file_directory,'DoorFrameLines\Debug\DoorFrameLines'+'.exe')
        RectangularWindow = path.join(file_directory,'NajgolemaKontura\Debug\NajgolemaKontura'+'.exe')
        CurvedArrow = path.join(file_directory,'CurvedArrow\Debug\CurvedArrow'+'.exe')
        DoorFootplate = path.join(file_directory,'DoorFootplate\Debug\DoorFootplate'+'.exe')
        CircularWindow = path.join(file_directory,'CircularWindow\Debug\CircularWindow'+'.exe')
        StraightArrow = path.join(file_directory,'StraightArrow\Debug\StraightArrow'+'.exe')
        Handle1 = path.join(file_directory,'Handle1\Debug\Handle1'+'.exe')
        Handle2 = path.join(file_directory,'Handle2\Debug\Handle2'+'.exe')
        exe = path.join(file_directory,'Test\Debug\Test'+'.exe')
        
        print exe

        
        #run_command([DoorFramelines, ini_path,image])
        #run_command([CurvedArrow, ini_path,image])   
        #run_command([RectangularWindow, ini_path,image]) 
        #run_command([DoorFootplate, ini_path,image])   
        #run_command([Text, ini_path,image])  
        #run_command([CircularWindow, ini_path,image])   
        #run_command([StraightArrow, ini_path,image])   
        
        run_command([exe, "curvedarrow"])
    
        
    print "Script Successfully Executed!!!!!!"
    
if __name__ == "__main__":
    main()
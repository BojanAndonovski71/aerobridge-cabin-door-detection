import math


fin=open("D:/NTU/Dissertation/Aerobridge/Excel/CircularWindow.csv","r")
print("Started.....")
data =fin.readlines()
fin.close()
fout=open("D:/NTU/Dissertation/Aerobridge/Excel/CircularWindowProcessed.csv","w")
for line in data:
    row = line.split("\t")

    temp= row[0].strip()
    length = len(temp)-1
    temp=temp[2:length]
    temp = temp.split(",")
    CenterX=temp[0]
    temp=temp[1].split(";")
    CenterY = temp[0]
    radius= temp [1]

    flight = row[1].split("\"\n")[0]
    elements = [CenterX, CenterY, radius, flight]
    print(elements)
    fout.write(','.join(elements)+'\n')
fout.close()
    

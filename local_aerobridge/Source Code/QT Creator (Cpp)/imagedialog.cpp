#include "imagedialog.h"
#include "ui_imagedialog.h"
#include<mainwindow.h>


ImageDialog::ImageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageDialog)
{
    ui->setupUi(this);

 }

ImageDialog::~ImageDialog()
{
    delete ui;
}

void ImageDialog::init(QString path)
{
    MainWindow w;
    this->setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);

    w.global_store_imagefolderpath(imagePath);

    path = path +"/";
    imagePath = path;
    qDebug()<<"Image Folder Path : "<<imagePath<<":   "<<path ;

    //Get All Files In Folder
    QDir myDir(imagePath);
    allFiles = myDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
    currentIndex =0;
    QString image_path = path+allFiles[currentIndex];
    storeFileinfo(image_path);

    image.load(image_path);
    scene = new QGraphicsScene(this);
    scene->addPixmap(image);
    scene->setSceneRect(image.rect());
    ui->graphicsView->setScene(scene);

    ui->feature->currentIndexChanged(0);
    ui->feature->setEnabled(true);

}

void ImageDialog::mousePressEvent(QMouseEvent *ev) //Mouse Event
{
    //QString x = QString::number(ev->x());
    //QString y = QString::number(ev->y());
    //qDebug() << x << "," << y;
    QPointF p = ui->graphicsView->mapFrom(ui->graphicsView,ev->pos());
    int X = p.x();
    int Y = p.y();
    painter(X,Y);
}

void ImageDialog::painter(int X, int Y) //Records & Paints points on Image
{
    if(points>0)
    {
    QPainter painter(this);
    QPen pen(color, 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    double rad = 1;
    scene->addEllipse(X, Y, rad*4, rad*2,pen, QBrush(Qt::SolidPattern));
    switch(ui->feature->currentIndex()){
      case 0: //Eliptical Window
        EW.X_array.insert(count,X);
        EW.Y_array.insert(count,Y);
        qDebug()<< EW.X_array[count] << EW.Y_array[count] << count <<points << X << Y<<"***LEN***"<<EW.X_array.length();
        EW.Feature=ui->feature->currentText();
        break;

      case 1: // Rectangular Window
        RW.X_array.insert(count,X);
        RW.Y_array.insert(count,Y);
        qDebug()<< RW.X_array[count] << RW.Y_array[count] << count <<points << X << Y;
        RW.Feature=ui->feature->currentText();
        break;

      case 2: //Text
        Text.X_array.insert(count,X);
        Text.Y_array.insert(count,Y);
        qDebug()<< Text.X_array[count] << Text.Y_array[count] << count <<points << X << Y<<"***LEN***"<<Text.X_array.length();
        Text.Feature=ui->feature->currentText();
        break;


      case 3: //Straigh Arrow
        SA.X_array.insert(count,X);
        SA.Y_array.insert(count,Y);
        qDebug()<< SA.X_array[count] << SA.Y_array[count] << count <<points << X << Y;
        SA.Feature=ui->feature->currentText();
        break;

      case 4: //Curved Arrow
        CA.X_array.insert(count,X);
        CA.Y_array.insert(count,Y);
        qDebug()<< CA.X_array[count] << CA.Y_array[count] << count <<points << X << Y;
        CA.Feature=ui->feature->currentText();
        break;

      case 5: //Door Framelines
        Framelines.X_array.insert(count,X);
        Framelines.Y_array.insert(count,Y);
        qDebug()<< Framelines.X_array[count] << Framelines.Y_array[count] << count <<points << X << Y;
        Framelines.Feature=ui->feature->currentText();
        break;

      case 6: //Door Handle
        DH.X_array.insert(count,X);
        DH.Y_array.insert(count,Y);
        qDebug()<< DH.X_array[count] << DH.Y_array[count] << count <<points << X << Y;
        DH.Feature=ui->feature->currentText();
        break;

      case 7: //Door Footplate
        Footplate.X_array.insert(count,X);
        Footplate.Y_array.insert(count,Y);
        qDebug()<< Footplate.X_array[count] << Footplate.Y_array[count] << count <<points << X << Y;
        Footplate.Feature=ui->feature->currentText();
        break;

      default:
        break;
    }
    points=points-1;
    count=count+1;
    }
    else
    {
        ui->feature->setEnabled(true);
    }
}

void ImageDialog::on_prevImage_clicked() //Move To Previous Image
{
    if(currentIndex>0)
    {
      currentIndex=currentIndex-1;

    }
    QString prevImagePath =imagePath+allFiles[currentIndex];
    storeFileinfo(prevImagePath);

    image.load(prevImagePath);
    scene->addPixmap(image);
    scene->setSceneRect(image.rect());
    ui->graphicsView->setScene(scene);
    //Trigger Event
    ui->feature->currentIndexChanged(ui->feature->currentIndex());
}

void ImageDialog::on_nextImage_clicked() //Move To Next Image
{
    if(true)
    {
        qDebug()<<EW.X_array.length()<<Text.X_array.length();
        if(EW.X_array.length()==4)//Elliptical Window
        {
            ElipticalWindow(EW);
            clear_array(&EW);
         }

        if(RW.X_array.length()==4) //Rectangular Window
        {
            four_points(RW);
            clear_array(&RW);
        }

        if(Text.X_array.length()%4>=1 || Text.X_array.length()%4<=8) //==4 || Text.X_array.length()==8 || Text.X_array.length()==12 || Text.X_array.length()==16 || Text.X_array.length()==20) //Text
        {
            text_points(Text);
            clear_array(&Text);
        }

        if(Footplate.X_array.length()==4) //Door Footplate
        {
            four_points(Footplate);
            clear_array(&Footplate);
        }

        if(SA.X_array.length()==4) //Straigh Arrow
        {
            four_points(SA);
            clear_array(&SA);
        }

        if(CA.X_array.length()==5) //Curved Arrow
        {
            many_points(CA);
            clear_array(&CA);
        }

        if(DH.X_array.length()==6) //Door Handle
        {
            many_points(DH);
            clear_array(&DH);
        }

        if(Framelines.X_array.length()==8) //Door Framelines
        {
            many_points(Framelines);
            clear_array(&Framelines);
        }
        qDebug()<<EW.X_array.length()<<Text.X_array.length();




/*
     switch(ui->feature->currentIndex()){

      case 0: //Elliptical Window
            ElipticalWindow(EW);
            break;

        case 1: //Rectangular Window
            four_points(RW);
            break;

        case 2: //Text
            four_points(Text);
            break;

        case 3: //Straigh Arrow
          four_points(SA);
          break;

        case 4: //Curved Arrow
          many_points(CA);
          break;

        case 5: //Door Framelines
          many_points(Framelines);
          break;

        case 6: //Door Handle
          many_points(DH);
          break;

        case 7: //Door Footplate
            four_points(Footplate);
            break;

        default:
            qDebug()<<"Default";
            break;
      }*/
    }
    if(currentIndex+1<allFiles.length())
    {
        currentIndex=currentIndex+1;
    }
    QString nextImagePath =imagePath+allFiles[currentIndex];
    qDebug()<<"NextImgPath: "<<nextImagePath;
    storeFileinfo(nextImagePath);

    image.load(nextImagePath);
    scene->addPixmap(image);
    scene->setSceneRect(image.rect());
    ui->graphicsView->setScene(scene);
    //Trigger Event
    ui->feature->currentIndexChanged(ui->feature->currentIndex());
}

void ImageDialog::on_resetImage_clicked() //Reset Point Selection
{
    QString resetImagePath =imagePath+allFiles[currentIndex];
    storeFileinfo(resetImagePath);
    image.load(resetImagePath);
    scene->addPixmap(image);
    scene->setSceneRect(image.rect());
    ui->graphicsView->setScene(scene);

    //Trigger Event
    ui->feature->currentIndexChanged(ui->feature->currentIndex());
    clear_array(&EW);
    clear_array(&RW);
    clear_array(&Text);
    clear_array(&SA);
    clear_array(&CA);
    clear_array(&DH);
    clear_array(&Footplate);
    clear_array(&Framelines);
 }

void ImageDialog::on_feature_currentIndexChanged(int index) //Sets the no of Points  & Brush Color
{
    count = 0;
    //ui->feature->setEnabled(false);
    switch(index){
      case 0: //Eliptical Window
        color = Qt::green;
        points = 4;
        break;
      case 1: // Rectangular Window
        color = Qt::blue;
        points = 4;
        break;
      case 2: //Text
        color = Qt::red;
        points = 32;
        break;
      case 3: //Straigh Arrow
        color = Qt::darkCyan;
        points = 4;
        break;
      case 4: //Curved Arrow
        color = Qt::cyan;
        points = 5;
        break;
      case 5: //Door Framelines
        color = Qt::green;
        points = 8;
        break;
      case 6: //Door Handle
        color = Qt::darkMagenta;
        points = 6;
        break;
      case 7: //Door Footplate
        struct Footplate;
        color = Qt::yellow;
        points = 4;
        break;

      default:
        break;
    }
}

void ImageDialog::storeFileinfo(QString imagePath)
{
    QStringList splits = imagePath.split("/");
    imageName = splits.at(splits.size()-1);
    imageFolder= splits.at(splits.size()-2);

    this->setWindowTitle(imagePath);
}

/*void ImageDialog::CircularWindow(struct featureData s1)
{
    QPoint p1,p2,p3;


    p1.setX(s1.X_array[0]);
    p2.setX(s1.X_array[1]);
    p3.setX(s1.X_array[2]);

    p1.setY(s1.Y_array[0]);
    p2.setY(s1.Y_array[1]);
    p3.setY(s1.Y_array[2]);

    //qreal offset = qpow(2,1);
    //qDebug()<<offset();

    float ma = (p2.y() - p1.y()) / (p2.x() - p1.x());
    float mb = (p3.y() - p2.y()) / (p3.x() - p2.x());
    double centerx = (ma * mb * (p1.y() - p3.y()) + mb * (p1.x() + p2.x()) - ma * (p2.x() + p3.x())) / (2 * (mb - ma));
    double centery = (-1 / ma) * (centerx - (p1.x() + p2.x()) / 2) + (p1.y() + p2.y()) / 2;
    */


    /*float yDelta_a = p2.y() - p1.y();
    float xDelta_a = p2.x() - p1.x();
    float yDelta_b = p3.y() - p2.y();
    float xDelta_b = p3.x() - p2.x();


    float aSlope = yDelta_a/xDelta_a;
    float bSlope = yDelta_b/xDelta_b;
    float centerx = (aSlope*bSlope*(p1.y() - p3.y()) + bSlope*(p1.x() + p2.x())- aSlope*(p2.x()+p3.x()) )/(2* (bSlope-aSlope) );
    float centery = -1*(centerx - (p1.x()+p2.x())/2)/aSlope +  (p1.y()+p2.y())/2;

    qDebug()<< centerx << centery;


    double offset = pow(p2.x(),2) + pow(p2.y(),2);
    double bc =   ( pow(p1.x(),2) + pow(p1.y(),2) - offset )/2.0;
    double cd =   (offset - pow(p3.x(), 2) - pow(p3.y(), 2))/2.0;
    double det =  (p1.x() - p2.x()) * (p2.y() - p3.y()) - (p2.x() - p3.x())* (p1.y() - p2.y());

    //if (Math.abs(det) < TOL) { throw new IllegalArgumentException("Yeah, lazy."); }

    double idet = 1/det;

    double centerx =  (bc * (p2.y() - p3.y()) - cd * (p1.y() - p2.y())) * idet;
    double centery =  (cd * (p1.x() - p2.x()) - bc * (p2.x() - p3.x())) * idet;
    double tempX = (p2.x() - centerx);
    double tempY = (p2.y()-centery);
    double radius = sqrt( pow(tempX, 2) + pow(tempY, 2));

    qDebug()<< centerx <<centery <<radius;

    MainWindow conn;
    QString dbpath =conn.getDBPath();

    conn.connOpen(dbpath);
    QSqlQuery query;
    query.prepare("INSERT INTO CircularWindow(ID, Center_X, Center_Y, Radius, ImageName, Flight) values(:ID,:Center_X,:Center_Y,:Radius,:ImageName,:Flight)");
    query.bindValue(":ID",(currentIndex+1));
    query.bindValue(":Center_X",centerx);
    query.bindValue(":Center_Y",centery);
    query.bindValue(":Radius",radius);
    query.bindValue(":ImageName",imageName);
    query.bindValue(":Flight",imageFolder);
    query.exec();
    conn.connClose();
}*/

void ImageDialog::ElipticalWindow(struct featureData s1)
{
    QPoint p1,p2,p3,p4;


    p1.setX(s1.X_array[0]);
    p2.setX(s1.X_array[1]);
    p3.setX(s1.X_array[2]);
    p4.setX(s1.X_array[3]);

    p1.setY(s1.Y_array[0]);
    p2.setY(s1.Y_array[1]);
    p3.setY(s1.Y_array[2]);
    p4.setY(s1.Y_array[3]);


    double tempA=(pow(p3.x(),2)-pow(p4.x(),2))*(p2.x()-p1.x());
    double tempB=(pow(p3.y(),2)-pow(p4.y(),2))*(p2.x()-p1.x());
    double tempC=(pow(p3.y(),2)-pow(p1.y(),2))*(p3.x()-p4.x());
    double tempD=(pow(p2.x(),2)-pow(p1.x(),2))*(p3.x()-p4.x());
    double tempE=(pow(p4.x(),2)-pow(p1.x(),2))*(p2.x()-p3.x());
    double tempF=(pow(p4.y(),2)-pow(p1.y(),2))*(p2.x()-p3.x());
    double tempG=(pow(p3.x(),2)-pow(p2.x(),2))*(p4.x()-p1.x());
    double tempH=(pow(p3.y(),2)-pow(p2.y(),2))*(p4.x()-p1.x());
    double tempN=(p1.y()-p2.y())*(p3.x()-p4.x())-(p4.y()-p3.y())*(p2.x()-p1.x());
    double tempO=(p3.y()-p2.y())*(p4.x()-p1.x())-(p1.y()-p4.y())*(p2.x()-p3.x());

    double tempX=(tempN*tempF+tempN*tempH+tempO*tempC-tempO*tempB)/(tempO*tempA-tempO*tempD-tempN*tempE-tempN*tempG);

    double tempAY=(pow(p4.x(),2)-pow(p3.x(),2))*(p2.x()-p1.x());
    double tempBY=(pow(p4.y(),2)-pow(p3.y(),2))*(p2.x()-p1.x());
    double tempCY=(pow(p2.x(),2)-pow(p1.x(),2))*(p4.x()-p3.x());
    double tempDY=(pow(p2.y(),2)-pow(p1.y(),2))*(p4.x()-p3.x());

    double detY1=tempX*tempAY+tempBY-tempX*tempCY-tempDY;
    double detY2=2*((p1.y()-p2.y())*(p4.x()-p3.x())-(p3.y()-p4.y())*(p2.x()-p1.x()));
    double centery=detY1/detY2;

    double detX1=detY2*tempX*(pow(p2.x(),2)-pow(p1.x(),2))+detY2*(pow(p2.y(),2)-pow(p1.y(),2))+2*detY1*(p1.y()-p2.y());
    double detX2=2*tempX*detY2*(p2.x()-p1.x());
    double centerx=detX1/detX2;

    double a1=(p1.x()-centerx)*(p1.x()-centerx);
    double a2=(p1.y()-centery)*(p1.y()-centery);
    double b1=(p2.x()-centerx)*(p2.x()-centerx);
    double b2=(p2.y()-centery)*(p2.y()-centery);

    double detSemiaxeA1=a1*a1*b2-a1*a2*b1;
    double detSemiaxeA2=b2*a1-b1*a2-a2*a1+a2*b1;
    double a=detSemiaxeA1/detSemiaxeA2;

    double detSemiaxeB1=b2*a1-b1*a2;
    double detSemiaxeB2=a1-b1;
    double b=detSemiaxeB1/detSemiaxeB2;

    double p=sqrt(abs(a));
    double q=sqrt(abs(b));



    qDebug()<< "*****************"<<centerx <<centery <<p<<q;




    MainWindow conn;
    QString dbpath =conn.getDBPath();

    conn.connOpen(dbpath);
    QSqlQuery query;
    query.prepare("REPLACE INTO ElipticalWindow(ID, Center_X, Center_Y, p, q,Points,UID,ImageName, Flight) values(:ID,:Center_X,:Center_Y,:Semiaxe_A,:Semiaxe_B,:Points,:UID,:ImageName,:Flight)");
    query.bindValue(":ID",(currentIndex+1));
    query.bindValue(":Center_X",centerx);
    query.bindValue(":Center_Y",centery);
    query.bindValue(":Semiaxe_A",p);
    query.bindValue(":Semiaxe_B",q);

    QString Points;
    Points = QString::number(p1.x())+","+QString::number(p1.y())+";"+QString::number(p2.x())+","+QString::number(p2.y())+";"+QString::number(p3.x())+","+QString::number(p3.y())+";"+QString::number(p4.x())+","+QString::number(p4.y());

    query.bindValue(":Points",Points);

    query.bindValue(":UID",(imageFolder+"/"+imageName));
    query.bindValue(":ImageName",imageName);
    query.bindValue(":Flight",imageFolder);

    query.exec();
    conn.connClose();
}


void ImageDialog::four_points(struct featureData s)
{
    qDebug()<<s.Feature<<"is the feature";

    QPoint p1,p2,p3,p4;
    p1.setX(s.X_array[0]);
    p2.setX(s.X_array[1]);
    p3.setX(s.X_array[2]);
    p4.setX(s.X_array[3]);
    p1.setY(s.Y_array[0]);
    p2.setY(s.Y_array[1]);
    p3.setY(s.Y_array[2]);
    p4.setY(s.Y_array[3]);

    QString tableName;
    if(s.Feature=="Rectangular Window")
        tableName = "RectangularWindow";
    else if(s.Feature=="Straight Arrow")
        tableName= "StraightArrow";
    else
        tableName="DoorFootplate";

    MainWindow conn;
    QString dbpath =conn.getDBPath();
    conn.connOpen(dbpath);
    QSqlQuery query;
    QString SqlCommand ="REPLACE INTO "+tableName+"(ID, P1, P2, P3, P4, UID, ImageName, Flight) values(:ID,:p1,:p2,:p3,:p4,:UID,:ImageName,:Flight)";
    query.prepare(SqlCommand);
    query.bindValue(":ID",(currentIndex+1));
    query.bindValue(":p1",(QString::number(p1.x())+","+QString::number(p1.y())));
    query.bindValue(":p2",(QString::number(p2.x())+","+QString::number(p2.y())));
    query.bindValue(":p3",(QString::number(p3.x())+","+QString::number(p3.y())));
    query.bindValue(":p4",(QString::number(p4.x())+","+QString::number(p4.y())));
    query.bindValue(":UID",(imageFolder+"/"+imageName));
    query.bindValue(":ImageName",imageName);
    query.bindValue(":Flight",imageFolder);
    query.exec();
    conn.connClose();
}

void ImageDialog::text_points(struct featureData s)
{
    QString tableName= "Text";
    int no_of_text_features =( s.X_array.length())/4;
    qDebug()<<s.Feature<<"text feature"<< no_of_text_features;
    MainWindow conn;
    QString dbpath =conn.getDBPath();
    conn.connOpen(dbpath);
    for(int i=0;i<no_of_text_features;i++)
    {
        QSqlQuery query;
        QString SqlCommand ="REPLACE INTO "+tableName+"(ID, P1, P2, P3, P4, UID, ImageName, Flight) values(:ID,:p1,:p2,:p3,:p4,:UID,:ImageName,:Flight)";
        query.prepare(SqlCommand);
        query.bindValue(":ID",(currentIndex+1));
        for(int j=0;j<4;j++)
        {
            int current_point_index = (4*i)+j;
            QPoint p;
            p.setX(s.X_array.at(current_point_index));
            p.setY(s.Y_array.at(current_point_index));
            query.bindValue((":p"+QString::number(j+1)),(QString::number(p.x())+","+QString::number(p.y())));
        }
        query.bindValue(":UID",(imageFolder+"/"+imageName+"/"+QString::number(i+1)));
        query.bindValue(":ImageName",imageName);
        query.bindValue(":Flight",imageFolder);
        query.exec();
    }
    conn.connClose();
}


void ImageDialog::many_points(struct featureData s)
{
    qDebug()<<s.Feature<<"is the feature";
    MainWindow conn;
    QString dbpath =conn.getDBPath();
    conn.connOpen(dbpath);
    QSqlQuery query;
    QString SqlCommand;

    QString tableName;
    if(s.Feature=="Curved Arrow")
    {
        tableName = "CurvedArrow";
        SqlCommand="REPLACE INTO "+tableName+"(ID, P1, P2, P3, P4, P5, UID, ImageName, Flight) values(:ID,:p1,:p2,:p3,:p4,:p5,:UID,:ImageName,:Flight)";

    }
    else if(s.Feature=="Door Handle")
    {
        tableName="DoorHandle";
        SqlCommand="REPLACE INTO "+tableName+"(ID, P1, P2, P3, P4, P5, P6, UID, ImageName, Flight) values(:ID,:p1,:p2,:p3,:p4,:p5,:p6,:UID,:ImageName,:Flight)";

    }
    else
    {
        tableName="DoorFramelines";
        SqlCommand="REPLACE INTO "+tableName+"(ID, P1, P2, P3, P4, P5, P6, P7, P8, UID, ImageName, Flight) values(:ID,:p1,:p2,:p3,:p4,:p5,:p6,:p7,:p8,:UID,:ImageName,:Flight)";
    }
    qDebug()<<SqlCommand;

    query.prepare(SqlCommand);
    query.bindValue(":ID",(currentIndex+1));
    for(int i=0;i<s.X_array.length();i++)
    {
    query.bindValue((":p"+QString::number(i+1)),(QString::number(s.X_array[i])+","+QString::number(s.Y_array[i])));
    qDebug()<<":p"+QString::number(i+1)<< QString::number(s.X_array[i])+","+QString::number(s.Y_array[i]);
    }
    query.bindValue(":UID",(imageFolder+"/"+imageName));
    query.bindValue(":ImageName",imageName);
    query.bindValue(":Flight",imageFolder);
    query.exec();
    conn.connClose();
}

void ImageDialog::clear_array(featureData *s)
{
    s->X_array.clear();
    s->Y_array.clear();

    //iNum = s.Y_array.count();
    //for (int i=0; i<iNum; i++)
      //s.Y_array.removeAt(0);
}

void ImageDialog::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key()==Qt::Key_P)
    {
        on_prevImage_clicked();
    }
    if(ev->key()==Qt::Key_N)
    {
        on_nextImage_clicked();
    }
}




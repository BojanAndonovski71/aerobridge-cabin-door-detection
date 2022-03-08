#include "showpoints.h"
#include "ui_showpoints.h"
#include<mainwindow.h>

ShowPoints::ShowPoints(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowPoints)
{
    ui->setupUi(this);
}

ShowPoints::~ShowPoints()
{
    delete ui;
}

void ShowPoints::init(QString path, QString name)
{
    this->setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);

    path = path +"/";
    imagePath = path;
    qDebug()<<"Image Folder Path : "<<imagePath<<":   "<<path ;

    QDir myDir(imagePath);
    allFiles = myDir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
    if(name.isEmpty())
    {
        currentIndex =0;
    }
    else
        currentIndex = allFiles.indexOf(name);
    qDebug()<<allFiles.at(currentIndex);

    QString image_file_path = path+allFiles.at(currentIndex);
    storeFileinfo(image_file_path);

    image.load(image_file_path);
    scene = new QGraphicsScene(this);
    scene->addPixmap(image);
    scene->setSceneRect(image.rect());
    ui->graphicsView->setScene(scene);
    read_and_plot_points();
}

void ShowPoints::storeFileinfo(QString imagePath)
{
    QStringList splits = imagePath.split("/");
    imageName = splits.at(splits.size()-1);
    imageFolder= splits.at(splits.size()-2);

    this->setWindowTitle(imagePath);
 }


void ShowPoints::on_refreshImage_clicked()
{
    QString prevImagePath =imagePath+allFiles[currentIndex];
    storeFileinfo(prevImagePath);
    image.load(prevImagePath);
    scene->addPixmap(image);
    scene->setSceneRect(image.rect());
    ui->graphicsView->setScene(scene);
    read_and_plot_points();

}


void ShowPoints::on_prevImage_clicked() //Move To Previous Image
{
    if(currentIndex>0)
      currentIndex=currentIndex-1;

    QString prevImagePath =imagePath+allFiles[currentIndex];
    storeFileinfo(prevImagePath);
    image.load(prevImagePath);
    scene->addPixmap(image);
    scene->setSceneRect(image.rect());
    ui->graphicsView->setScene(scene);

    read_and_plot_points();
}

void ShowPoints::on_nextImage_clicked() //Move To Next Image
{
    if(currentIndex+1<allFiles.length())
    {
        qDebug()<< (currentIndex<allFiles.length());
        currentIndex=currentIndex+1;
    }

    QString nextImagePath =imagePath+allFiles[currentIndex];
    storeFileinfo(nextImagePath);
    image.load(nextImagePath);
    scene->addPixmap(image);
    scene->setSceneRect(image.rect());
    ui->graphicsView->setScene(scene);

    read_and_plot_points();


}

void ShowPoints::read_and_plot_points()
{
    draw_ellipse();
    qDebug()<<"********************************Ellipse Complete***********************";

    MainWindow conn;
    QString dbpath =conn.getDBPath();
    QStringList Queries,Features; //"SELECT Center,P2,P3,P4 FROM ElipticalWindow WHERE ImageName = (:ImageName) AND Flight = (:Flight)"
    Queries <<"SELECT P1,P2,P3,P4 FROM RectangularWindow WHERE ImageName = (:ImageName) AND Flight = (:Flight)"
                <<"SELECT P1,P2,P3,P4 FROM Text WHERE ImageName = (:ImageName) AND Flight = (:Flight)"
                <<"SELECT P1,P2,P3,P4 FROM StraightArrow WHERE ImageName = (:ImageName) AND Flight = (:Flight)"
               <<"SELECT P1,P2,P3,P4,P5 FROM CurvedArrow WHERE ImageName = (:ImageName) AND Flight = (:Flight)"
              <<"SELECT P1,P2,P3,P4,P5,P6,P7,P8 FROM DoorFramelines WHERE ImageName = (:ImageName) AND Flight = (:Flight)"
              <<"SELECT P1,P2,P3,P4,P5,P6 FROM DoorHandle WHERE ImageName = (:ImageName) AND Flight = (:Flight)"
             <<"SELECT P1,P2,P3,P4 FROM DoorFootplate WHERE ImageName = (:ImageName) AND Flight = (:Flight)";
    Features << "RectangularWindow" << "Text" << "StraightArrow" << "CurvedArrow" << "DoorFramelines" << "DoorHandle" << "DoorFootplate";

    for (int j=0; j<Features.length(); j++)
    {
        conn.connOpen(dbpath); //Connect to Database

        QSqlQuery query;
        query.prepare(Queries[j]);
        query.bindValue(":ImageName",imageName);
        query.bindValue(":Flight",imageFolder);

        if(query.exec())
        {
          QSqlQueryModel *sqlQueryModel = new QSqlQueryModel;
          sqlQueryModel ->setQuery (query);
          qDebug()<<Features[j]<<" : "<<sqlQueryModel->record(0).count();
          qDebug()<<"ROW:"<<sqlQueryModel->rowCount();
          qDebug() << sqlQueryModel->record(2).value(0).toString()<<sqlQueryModel->record(0).value(1).toString()<<sqlQueryModel->record(0).value(2).toString()<<sqlQueryModel->record(0).value(3).toString();
          QString temp;
          QStringList tempList;
          QList<int> X_array,Y_array;

          if(sqlQueryModel->rowCount()>0)
          {
              for (int row=0;row<sqlQueryModel->rowCount();row++)
              {
                  for (int i=0; i<sqlQueryModel->record(row).count(); i++)
                  {
                    bool isNull = sqlQueryModel->record(row).value(i).isNull();
                    if(!isNull)
                    {
                        temp = sqlQueryModel->record(row).value(i).toString();
                        tempList = temp.split(",");
                        X_array.insert(i,(tempList[0].toInt()));
                        Y_array.insert(i,(tempList[1].toInt()));
                        //qDebug()<<"Iteration:i"<<i;
                    }
                    //qDebug()<<"Iteration:j"<<row;
                   }
              }
          }
          //qDebug()<<"Done";
          draw_points(X_array, Y_array, Features[j]);
        }
        conn.connClose();
   }
}

void ShowPoints::draw_ellipse()
{
    MainWindow conn;
    QString dbpath =conn.getDBPath();
    conn.connOpen(dbpath); //Connect to Database
    QSqlQuery query;
    query.prepare("SELECT Center_X,Center_Y,p,q,Points FROM ElipticalWindow WHERE ImageName = (:ImageName) AND Flight = (:Flight)");
    query.bindValue(":ImageName",imageName);
    query.bindValue(":Flight",imageFolder);
    if(query.exec())
    {
        QSqlQueryModel *sqlQueryModel = new QSqlQueryModel;
        sqlQueryModel ->setQuery (query);
        qDebug() <<"*******####******"<< sqlQueryModel->record(0).value(0).toString()<<sqlQueryModel->record(0).value(1).toString()<<sqlQueryModel->record(0).value(2).toString()<<sqlQueryModel->record(0).value(3).toString()<<sqlQueryModel->record(0).value(4).toString();


        QList<int> X,Y;
        for (int i=0; i<1; i++)
        {
            bool isNull = sqlQueryModel->record(0).value(4).isNull();
            if(!isNull)
            {
                QPoint center;
                QStringList points_str, points_list;
                QString tempPoints;
                color = Qt::green;
                QPen pen(color, line_thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
                int p,q;
                center.setX(sqlQueryModel->record(0).value(0).toDouble());
                center.setY(sqlQueryModel->record(0).value(1).toDouble());
                p=sqlQueryModel->record(0).value(3).toInt();
                q=sqlQueryModel->record(0).value(3).toInt();
                QString Points = sqlQueryModel->record(0).value(4).toString();
                qDebug()<<"***********Ellipse****************"<<center.x()<<center.y()<<p<<q<<Points;
                points_list = Points.split(";");
                for(int j=0;j<points_list.length();j++)
                {
                    color  = Qt::white;
                    QPen points(color, 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
                    tempPoints=points_list.at(j);
                    points_str=tempPoints.split(",");
                    qDebug()<<points_str;
                    X.insert(j,points_str[0].toInt());
                    Y.insert(j,points_str[1].toInt());

                    scene->addEllipse(X[j], Y[j], 4, 2,points, QBrush(Qt::SolidPattern));

                }

                QLineF line1(X[0],Y[0],X[2],Y[2]);
                scene->addLine(line1, pen);
                QLineF line2(X[1],Y[1],X[2],Y[2]);
                scene->addLine(line2, pen);
                QLineF line3(X[1],Y[1],X[3],Y[3]);
                scene->addLine(line3, pen);
                QLineF line4(X[3],Y[3],X[0],Y[0]);
                scene->addLine(line4, pen);

                pen.setColor(Qt::blue);

                QLineF major((center.x()+p),center.y(),(center.x()-p),center.y());
                scene->addLine(major, pen);
                QLineF minor(center.x(),(center.y()+q),center.x(),(center.y()-q));
                scene->addLine(minor, pen);





                //scene->addEllipse(center, qreal::p,qreal::q,pen);//addEllipse(center.x(),center.y(),p,q, pen);
                pen.setColor(Qt::white);
                scene->addEllipse(center.x(), center.y(), 4, 2,pen, QBrush(Qt::SolidPattern));
             }
        }
    }
    conn.connClose();
}

void ShowPoints::draw_points(QList<int> X_array, QList<int> Y_array, QString Feature)
{
    QPainter painter(this);

    double rad = 1;

    if(Feature == "RectangularWindow")
    {
        color = Qt::blue;
        QPen pen(color, line_thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        connect_points(pen,X_array,Y_array);
    }
    else if(Feature == "Text")
    {
        color = Qt::red;
        QPen pen(color, line_thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        qDebug()<<"******************************"<<X_array.length();
        connect_points(pen,X_array,Y_array);
    }
    else if(Feature == "CurvedArrow")
    {

        color = Qt::cyan;
        QPen pen(color, line_thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        qDebug()<<":CA";
        draw_arrow(pen,X_array,Y_array);
    }
    else if(Feature == "StraightArrow")
    {
        color = Qt::darkCyan;
        QPen pen(color, line_thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        qDebug()<<":SA";
        draw_arrow(pen,X_array,Y_array);
    }
    else if(Feature == "DoorHandle")
    {
        color = Qt::darkMagenta;
        QPen pen(color, line_thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        qDebug()<<":DH";
        draw_handle(pen,X_array,Y_array);

    }
    else if(Feature == "DoorFramelines")
    {
        color = Qt::yellow;
        QPen pen(color, line_thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        qDebug()<<":DFL";
        draw_line(pen,X_array,Y_array);
    }
    else if(Feature == "DoorFootplate")
    {
        color = Qt::yellow;
        QPen pen(color, line_thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        connect_points(pen,X_array,Y_array);
    }


    for (int i=0; i<(X_array.size());i++)
    {

        QPen pen(color, 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        scene->addEllipse(X_array[i], Y_array[i], rad*4, rad*2,pen, QBrush(Qt::SolidPattern));
    }

}

void ShowPoints::draw_rect(QPen pen, QList<int> X, QList<int> Y)
{
    if(X.length()>0)
    {
    QLineF line(X[0],Y[0],X[1],Y[1]);
    scene->addLine(line, pen);
     QLineF line1(X[1],Y[1],X[2],Y[2]);
    scene->addLine(line1, pen);
     QLineF line2(X[3],Y[3],X[2],Y[2]);
    scene->addLine(line2, pen);
     QLineF line3(X[3],Y[3],X[0],Y[0]);
    scene->addLine(line3, pen);
    }
}

void ShowPoints::draw_line(QPen pen, QList<int> X, QList<int> Y) // Door Framelines
{
    qDebug()<<"*******************";
    if(X.length()>0)
    {
    QLineF line(X[0],Y[0],X[1],Y[1]);
    scene->addLine(line, pen);
     QLineF line1(X[2],Y[2],X[3],Y[3]);
    scene->addLine(line1, pen);
     QLineF line2(X[4],Y[4],X[5],Y[5]);
    scene->addLine(line2, pen);
     QLineF line3(X[6],Y[6],X[7],Y[7]);
    scene->addLine(line3, pen);
    }
}

void ShowPoints::draw_st_arrow(QPen pen, QList<int> X, QList<int> Y)
{
    if(X.length()>0)
    {
        qDebug()<<":SA";
        QLineF line(X[0],Y[0],X[1],Y[1]);
        scene->addLine(line, pen);
        QLineF line1(X[1],Y[1],X[2],Y[2]);
        scene->addLine(line1, pen);
        QLineF line2(X[1],Y[1],X[3],Y[3]);
        scene->addLine(line2, pen);
        QLineF line4(X[0],Y[0],X[2],Y[2]);
        scene->addLine(line4, pen);
    }
}

void ShowPoints::draw_curved_arrow(QPen pen, QList<int> X, QList<int> Y)
{
    if(X.length()>0)
    {
        qDebug()<<":CA";
        QLineF line(X[0],Y[0],X[1],Y[1]);
        scene->addLine(line, pen);
        QLineF line1(X[1],Y[1],X[2],Y[2]);
        scene->addLine(line1, pen);
        QLineF line2(X[1],Y[1],X[3],Y[3]);
        scene->addLine(line2, pen);
        QLineF line3(X[3],Y[3],X[4],Y[4]);
        scene->addLine(line3, pen);
        QLineF line4(X[0],Y[0],X[2],Y[2]);
        scene->addLine(line4, pen);
    }
}

void ShowPoints::draw_handle(QPen pen, QList<int> X, QList<int> Y)
{
    if(X.length()>0)
    {
        qDebug()<<":DH";
        QLineF line(X[0],Y[0],X[1],Y[1]);
        scene->addLine(line, pen);
        QLineF line1(X[1],Y[1],X[2],Y[2]);
        scene->addLine(line1, pen);
        QLineF line2(X[2],Y[2],X[3],Y[3]);
        scene->addLine(line2, pen);
        QLineF line3(X[3],Y[3],X[4],Y[4]);
        scene->addLine(line3, pen);
        QLineF line4(X[4],Y[4],X[5],Y[5]);
        scene->addLine(line4, pen);
        QLineF line5(X[5],Y[5],X[0],Y[0]);
        scene->addLine(line5, pen);
    }
}

void ShowPoints:: connect_points(QPen pen, QList<int> X, QList<int> Y)
{
    QLine line;
    int len = X.length();
    int no_of_rect = len/4;
    for(int j=0;j<no_of_rect;j++)
    {
        for(int i = 0; i <4; i++)
        {
            QPoint p1, p2;
            int cur_index = (4*j)+i;
            if(i==3)
            {
                p1.setX(X[cur_index]);
                p1.setY(Y[cur_index]);
                p2.setX(X[4*j]);
                p2.setY(Y[4*j]);
            }
            else
            {
                p1.setX(X[cur_index]);
                p1.setY(Y[cur_index]);
                p2.setX(X[cur_index+1]);
                p2.setY(Y[cur_index+1]);
            }
            line.setPoints(p1, p2);
            scene->addLine(line,pen);
        }
    }
}

void ShowPoints:: draw_arrow(QPen pen, QList<int> X, QList<int> Y)
{
    QLine line;
    int len = X.length();
    for(int i = 0; i <len; i++)
    {
        QPoint p1, p2;
        int x_mid, y_mid;
        if(i==2)
        {
            p1.setX(X[i]);
            p1.setY(Y[i]);
            p2.setX(X[0]);
            p2.setY(Y[0]);
            x_mid= (X[0]+X[i])/2;
            y_mid= (Y[0]+Y[i])/2;
        }
        else if(i==3)
        {
            p1.setX(x_mid);
            p1.setY(y_mid);
            p2.setX(X[i]);
            p2.setY(Y[i]);
        }
        else if(i==len-1)
        {
            p1.setX(X[i-1]);
            p1.setY(Y[i-1]);
            p2.setX(X[i]);
            p2.setY(Y[i]);
        }
        else
        {
            p1.setX(X[i]);
            p1.setY(Y[i]);
            p2.setX(X[i+1]);
            p2.setY(Y[i+1]);
        }
        line.setPoints(p1, p2);
        scene->addLine(line,pen);
    }
}

void ShowPoints::keyPressEvent(QKeyEvent *ev)
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


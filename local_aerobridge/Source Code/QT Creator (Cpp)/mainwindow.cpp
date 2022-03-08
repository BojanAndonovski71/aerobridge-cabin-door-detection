#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    ui->btn_read_table->setVisible(false);
     ui->btn_compare->setVisible(false);
    ui->db_tables->setEnabled(false);
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    int x=0;
    if(ev->key()==Qt::Key_1){
        x=0;
        qDebug() << "Eliptical Window" ;
     }
    if(ev->key()==Qt::Key_2){
        x=1;
        qDebug() << "Rectangular Window" ;
    }
    if(ev->key()==Qt::Key_3){
        x=2;
        qDebug() << "Text" ;
    }
    if(ev->key()==Qt::Key_4){
        x=3;
        qDebug() << "Straight Arrow" ;
    }
    if(ev->key()==Qt::Key_5){
        x=4;
        qDebug() << "Curved Arrow" ;
    }
    if(ev->key()==Qt::Key_6){
        x=5;
        qDebug() << "Door Frame Lines" ;
    }
    if(ev->key()==Qt::Key_7){
        x=6;
        qDebug() << "Door Handle" ;
    }
    if(ev->key()==Qt::Key_8){
        x=7;
        qDebug() << "Door Footplate" ;
    }
    qDebug()<<x;
}

void MainWindow::on_btn_connect_clicked() //Connect To Database
{
    MainWindow conn;
    QString dbpath;
    dbpath=ui->lineEdit_database_path->text(); //get DB Path
    if(!conn.connOpen(dbpath))
    {
        ui->label->setText("Failed.....");
    }
    else
    {
        ui->label->setText("Connected To Database.....");
        ui->db_tables->clear();
        ui->db_tables->addItems(conn.mydb.tables()); //Update Combobox
        conn.close();

        ui->btn_read_table->setVisible(true);
        ui->btn_compare->setVisible(true);
        ui->db_tables->setEnabled(true);
    }
 }


void MainWindow::on_btn_read_table_clicked() //Read Selected Table Data
{

    MainWindow conn;
    QString dbpath;
    dbpath=ui->lineEdit_database_path->text();
    conn.connOpen(dbpath);

    QSqlQueryModel *modal=new QSqlQueryModel();
    QSqlQuery* qry=new QSqlQuery(conn.mydb);
    QString tableName=ui->db_tables->currentText();
    //qDebug()<<tableName;

    QString Temp = "SELECT *FROM '"+tableName+"'";
    qry->prepare(Temp);
    if(qry->exec())
    {
        do
        {
            modal->setQuery(*qry);
            qDebug()<<qry->record().value(5)<<"--";
            ui->display_table->setModel(modal);
        }
        while(qry->next());

        //qDebug()<<qry->record().value(5)<<modal->rowCount()<<"--"<<qry->next()<<modal->rowCount()<<"--"<<qry->next()<<modal->rowCount()<<"--"<<qry->next()<<modal->rowCount()<<"--"<<qry->next()<<modal->rowCount()<<"--"<<qry->next()<<"--"<<qry->next()<<"--"<<qry->next()<<"--"<<qry->next()<<"--"<<qry->next();

    }
    conn.connClose();
}

void MainWindow::on_btn_open_ImgDialog_clicked() //Open ImageDialog Page (Point Selection Window)
{

    QString image_folder_path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                  "C:/",
                                                  QFileDialog::ShowDirsOnly
                                                  | QFileDialog::DontResolveSymlinks);
    if(!image_folder_path.isEmpty())
    {
        imageDialog = new ImageDialog(this);
        imageDialog->init(image_folder_path);
        imageDialog->show();
   }
}

void MainWindow::on_show_points_clicked() //Open Showpoints Page (Results Tab)
{


    QString image_folder_path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                  "C:/",
                                                  QFileDialog::ShowDirsOnly
                                                  | QFileDialog::DontResolveSymlinks);
    if(!image_folder_path.isEmpty())
    {
        showPoints = new ShowPoints(this);
        showPoints->init(image_folder_path,"");
        showPoints->show();
    }

}


void MainWindow::on_show_point_clicked() //Open Showpoints Page (Point Select Tab)
{
    QString image_folder_path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                  "C:/",
                                                  QFileDialog::ShowDirsOnly
                                                  | QFileDialog::DontResolveSymlinks);
    if(!image_folder_path.isEmpty())
    {

        int col_len = ui->display_table->model()->columnCount();  // Multiple rows can be selected


        QItemSelectionModel *select = ui->display_table->selectionModel();
        QStringList flight, image;
        for (int i=0; i<select->selectedRows().length();i++)
        {
            flight.append(select->selectedRows(col_len-1).value(i).data().toString());
            image.append(select->selectedRows(col_len-2).value(i).data().toString());
            //qDebug()<<flight<<image;
            showPoints = new ShowPoints(this);
            showPoints->init((image_folder_path+"/"+flight.at(i)),image.at(i));
            showPoints->show();
        }
    }
}

void MainWindow::on_detection_results_selectionChanged() //Browse Detection Results File
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select File"), "C:/sqlite/", tr("Result Files (*.xlsx *.csv)"));
    if(!fileName.isEmpty())
    {
    ui->detection_results->setText(fileName);
    }
}

void MainWindow::on_read_det_results_clicked() //Set Detection CSV Filepath
{
    QString fileName = ui->detection_results->text();
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return;
    }

    QString data;
    QStringList wordList;
    data = file.readAll();
    file.close();
    wordList =data.split("\r\n");

    QString temp;
    QStringList tempList;
    QStandardItemModel *model = new QStandardItemModel(15,4, this);
    QTableView *tableView = new QTableView();
    tableView->setModel(model);
    for (int i=0; i<wordList.size();i++)
    {
        temp = wordList.at(i);
        tempList = temp.split(",");
        for(int j=0; j<tempList.length();j++)
        {
            QString celldata;
            celldata = tempList[j];
            if(i==0)
            {
                model->setHorizontalHeaderItem(j, new QStandardItem(celldata) );
            }
            else
            {
                qDebug()<<tempList[j];;
                QStandardItem *item=new QStandardItem(celldata);
                model->setItem((i-1),j,item);
            }
         }
    }
    ui->display_table->setModel(model);
}

void MainWindow::on_lineEdit_database_path_selectionChanged() //DB Path Select
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select Database"), "C:/sqlite/", tr("Database Files (*.db)"));
    if(!fileName.isEmpty())
    {
    ui->lineEdit_database_path->setText(fileName);
    }
}

void MainWindow::global_store_imagefolderpath(QString path)
{
    if(!path.isEmpty())
    {

    qDebug()<<imagePath;
    }
}

void MainWindow::setMessage(QString imagePath){

    qDebug()<<"Recieved"<<imagePath;
}
 QString MainWindow::getDBPath()
{
    return ui->lineEdit_database_path->text();
}


void MainWindow::on_btn_compare_clicked()
{
    if (ui->db_tables->currentText() == "ElipticalWindow")
        Compare_CircularWindow();
    else if(ui->db_tables->currentText() == "RectangularWindow")
        Compare_Rectangle();
    else if(ui->db_tables->currentText() == "Text")
        Compare_Rectangle();
    else if(ui->db_tables->currentText() == "CurvedArrow")
        Compare_Curved_Arrow();
    else if(ui->db_tables->currentText() == "StraightArrow")
        Compare_Straight_Arrow();
    else if(ui->db_tables->currentText() == "DoorHandle")
        Compare_Door_Handle();
    else if(ui->db_tables->currentText() == "DoorFrameLines")
        Compare_Door_Framelines();
    else if(ui->db_tables->currentText() == "DoorFoorplate")
        Compare_Door_Footplate();
    qDebug()<<ui->db_tables->currentText();

}

void MainWindow::Compare_CircularWindow() //Compare Database & CSV File for Circular Window
{

    QPoint p1,p2;
    double r1,r2;
    QString fileName = ui->detection_results->text(); //Get CSV File Path
    QFile file(fileName); //Open file
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << file.errorString();
        return;
    }

    QString data;
    QStringList wordList;
    data = file.readAll(); //Read CSV Data
    file.close();
    wordList =data.split("\r\n");

    QString temp;
    QStringList tempList;
    QStandardItemModel *model = new QStandardItemModel(15,4, this);
    QTableView *tableView = new QTableView();
    tableView->setModel(model);

    MainWindow conn;
    QString dbpath=ui->lineEdit_database_path->text();
    conn.connOpen(dbpath); //Connect to Database
    QSqlQuery query;

    for (int i=0; i<(wordList.size()-1);i++) //Iterate Through Each Row In CSV File
    {
        temp = wordList.at(i);
        tempList = temp.split(",");
        for(int j=0; j<tempList.length();j++)
        {
            QString celldata;
            celldata = tempList[j];
            if(i==0) //Set TableView Column Headers
            {
                model->setHorizontalHeaderItem(j, new QStandardItem(celldata) );
                if(j==(tempList.length()-1))
                    model->setHorizontalHeaderItem((j+1), new QStandardItem("Result") );
            }
            else
            {
                QStandardItem *item=new QStandardItem(celldata);
                model->setItem((i-1),j,item);
                if((j==(tempList.length()-1))) //Read Database & Compare With CSV File Data
                {
                    //Frame Query String for SQL Database
                    query.prepare("SELECT Center_X,Center_Y,Radius,Flight FROM CircularWindow WHERE ID = (:ID) AND Flight = (:Flight)");
                    query.bindValue(":ID",tempList[3]);
                    query.bindValue(":Flight",tempList[4]);
                    if(query.exec())
                    {
                        QSqlQueryModel *sqlQueryModel = new QSqlQueryModel;
                        sqlQueryModel ->setQuery (query);
                        p1.setX(sqlQueryModel->record(0).value(0).toDouble());
                        p1.setY(sqlQueryModel->record(0).value(1).toDouble());
                        r1 = sqlQueryModel->record(0).value(2).toDouble();
                        p2.setX(tempList[0].toDouble());
                        p2.setY(tempList[1].toDouble());
                        r2 = tempList[2].toDouble();
                        model->setItem((i-1),(j+1),new QStandardItem((TwoCircles(p1, p2, r1, r2)? "Pass" : "Fail")));
                        qDebug() << sqlQueryModel->record(0).value(0).toString()<<sqlQueryModel->record(0).value(1).toString()<<sqlQueryModel->record(0).value(2).toString()<<sqlQueryModel->record(0).value(3).toString();
                    }
                }
            }
         }
    }
    ui->display_table->setModel(model); //Update TableView
    conn.connClose();
 }

bool MainWindow::TwoCircles(QPoint p1, QPoint p2, double radius1, double radius2)
{
    double X1 = p1.x();
    double Y1 = p1.y();
    double X2 = p2.x();
    double Y2 = p2.y();

    double distance = pow((X1 - X2) * (X1 - X2) + (Y1 - Y2) * (Y1 - Y2), 0.5);
    if (radius2 >= radius1 && distance <= (radius2 - radius1)){
        //qDebug()<<"Circle 1 is inside Circle 2.";
        return true;
    }
    else if (radius1 >= radius2 && distance <= (radius1 - radius2) ) {
        //qDebug()<<"Circle 2 is inside Circle 1.";
        return true;
    }
    else if (distance > (radius1 + radius2)){
        //qDebug()<<"Circle 2 does not overlap Circle 1.";
        return false;
    }
    else {
        //qDebug()<<"Circle 2 overlaps Circle 1.";
        return true;
    }
}

void MainWindow::Compare_Rectangle() //Compare Database & CSV File for Rectangles
{
    QPoint p11,p12,p13,p14,p21,p22,p23,p24;


        QString fileName = ui->detection_results->text(); //Get CSV File Path
        QFile file(fileName); //Open file
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << file.errorString();
            return;
        }

        QString data;
        QStringList wordList;
        data = file.readAll(); //Read CSV Data
        file.close();
        wordList =data.split("\r\n");

        QString temp;
        QStringList tempList;
        QStandardItemModel *model = new QStandardItemModel(15,4, this);
        QTableView *tableView = new QTableView();
        tableView->setModel(model);

        MainWindow conn;
        QString dbpath=ui->lineEdit_database_path->text();
        conn.connOpen(dbpath);//Connect to Database
        QSqlQuery query;


        for (int i=0; i<(wordList.size()-1);i++) //Iterate Through Each Row In CSV File
        {
            temp = wordList.at(i);
            tempList = temp.split(",");
            for(int j=0; j<tempList.length();j++)
            {
                QString celldata;
                celldata = tempList[j];
                if(i==0)
                {
                    model->setHorizontalHeaderItem(j, new QStandardItem(celldata) );
                    if(j==(tempList.length()-1))
                        model->setHorizontalHeaderItem((j+1), new QStandardItem("Result") );
                }
                else
                {
                    QStandardItem *item=new QStandardItem(celldata);
                    model->setItem((i-1),j,item);
                    if((j==(tempList.length()-1)))
                    {

                        query.prepare("SELECT 4 Points,Flight FROM RectangleWindow WHERE ID = (:ID) AND Flight = (:Flight)");
                        query.bindValue(":ID",tempList[3]);
                        query.bindValue(":Flight",tempList[4]);
                        if(query.exec())
                        {
                            QSqlQueryModel *sqlQueryModel = new QSqlQueryModel;
                            sqlQueryModel ->setQuery (query);

                            p11.setX(sqlQueryModel->record(0).value(0).toDouble());
                            p11.setY(sqlQueryModel->record(0).value(1).toDouble());
                            p12.setX(sqlQueryModel->record(1).value(0).toDouble());
                            p12.setY(sqlQueryModel->record(1).value(1).toDouble());
                            p13.setX(sqlQueryModel->record(2).value(0).toDouble());
                            p13.setY(sqlQueryModel->record(2).value(1).toDouble());
                            p14.setX(sqlQueryModel->record(3).value(0).toDouble());
                            p14.setY(sqlQueryModel->record(3).value(1).toDouble());

                            //r1 = sqlQueryModel->record(0).value(2).toDouble();
                            p21.setX(tempList[0].toDouble());
                            p21.setY(tempList[1].toDouble());
                            p22.setX(tempList[0].toDouble());
                            p22.setY(tempList[1].toDouble());
                            p23.setX(tempList[0].toDouble());
                            p23.setY(tempList[1].toDouble());
                            p24.setX(tempList[0].toDouble());
                            p24.setY(tempList[1].toDouble());

                           // r2 = tempList[2].toDouble();
                            model->setItem((i-1),(j+1),new QStandardItem((TwoRectangles(p11,p12,p13,p14,p21,p22,p23,p24)? "Pass" : "Fails")));
                            qDebug() << sqlQueryModel->record(0).value(0).toString()<<sqlQueryModel->record(0).value(1).toString()<<sqlQueryModel->record(0).value(2).toString()<<sqlQueryModel->record(0).value(3).toString();

                        }
                    }
                }
             }
        }
        ui->display_table->setModel(model);
        conn.connClose();

}

bool MainWindow::TwoRectangles(QPoint p11, QPoint p12, QPoint p13, QPoint p14,QPoint p21,QPoint p22,QPoint p23,QPoint p24)
{
    double X11 = p11.x();
    double Y11 = p11.y();
    double X12 = p12.x();
    double Y12 = p12.y();
    double X13 = p13.x();
    double Y13 = p13.y();
    double X14 = p14.x();
    double Y14 = p14.y();
    double X21 = p21.x();
    double Y21 = p21.y();
    double X22 = p22.x();
    double Y22 = p22.y();
    double X23 = p23.x();
    double Y23 = p23.y();
    double X24 = p24.x();
    double Y24 = p24.y();



    double lenght1 = pow((X12 - X11) * (X12 - X11) + (Y12 - Y11) * (Y12 - Y11), 0.5);
    double width1 =  pow((X13 - X12) * (X13 - X12) + (Y13 - Y12) * (Y13 - Y12), 0.5);

    double lenght2 = pow((X12 - X11) * (X12 - X11) + (Y12 - Y11) * (Y12 - Y11), 0.5);
    double width2 =  pow((X13 - X12) * (X13 - X12) + (Y13 - Y12) * (Y13 - Y12), 0.5);

    double area1=lenght1*width1;
    double area2=lenght2*width2;


    if (area1>area2){
        //qDebug()<<"Circle 1 is inside Circle 2.";
        return true;
    }
    else if (area1=area2 ) {
        //qDebug()<<"Circle 2 is inside Circle 1.";
        return true;
    }
    else if (area1<area2){
        //qDebug()<<"Circle 2 does not overlap Circle 1.";
        return false;
    }
    else {
        //qDebug()<<"Circle 2 overlaps Circle 1.";
        return true;
    }
}

void MainWindow::Compare_Curved_Arrow() //Compare Database & CSV File for Curved Arrow
{
    //Write Code to Read CSV File (Refer "Compare_CircularWindow()")

        //Iterate through each element in CSV & Compare the corresponding item with the element in "Curved Arrow table" in
        //Database
}

void MainWindow::Compare_Straight_Arrow() //Compare Database & CSV File for Straight Arrow
{
   /* QPoint p11,p21,p31,p41,p12,p22,p32,p42;


        QString fileName = ui->detection_results->text(); //Get CSV File Path
        QFile file(fileName); //Open file
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << file.errorString();
            return;
        }

        QString data;
        QStringList wordList;
        data = file.readAll(); //Read CSV Data
        file.close();
        wordList =data.split("\r\n");

        QString temp;
        QStringList tempList;
        QStandardItemModel *model = new QStandardItemModel(15,4, this);
        QTableView *tableView = new QTableView();
        tableView->setModel(model);

        MainWindow conn;
        QString dbpath=ui->lineEdit_database_path->text();
        conn.connOpen(dbpath);//Connect to Database
        QSqlQuery query;


        for (int i=0; i<(wordList.size()-1);i++) //Iterate Through Each Row In CSV File
        {
            temp = wordList.at(i);
            tempList = temp.split(",");
            for(int j=0; j<tempList.length();j++)
            {
                QString celldata;
                celldata = tempList[j];
                if(i==0)
                {
                    model->setHorizontalHeaderItem(j, new QStandardItem(celldata) );
                    if(j==(tempList.length()-1))
                        model->setHorizontalHeaderItem((j+1), new QStandardItem("Result") );
                }
                else
                {
                    QStandardItem *item=new QStandardItem(celldata);
                    model->setItem((i-1),j,item);
                    if((j==(tempList.length()-1)))
                    {

                        query.prepare("SELECT Point11_X,Point11_Y,Point21_X,Point21_Y,Point31_X,Point31_Y,Point41_X,Point41_Y,Flight FROM Straight_Arrow WHERE ID = (:ID) AND Flight = (:Flight)");
                        query.bindValue(":ID",tempList[3]);
                        query.bindValue(":Flight",tempList[4]);
                        if(query.exec())
                        {
                            QSqlQueryModel *sqlQueryModel = new QSqlQueryModel;
                            sqlQueryModel ->setQuery (query);

                            p11.setX(sqlQueryModel->record(0).value(0).toDouble());
                            p11.setY(sqlQueryModel->record(0).value(1).toDouble());
                            p21.setX(sqlQueryModel->record(1).value(0).toDouble());
                            p21.setY(sqlQueryModel->record(1).value(1).toDouble());
                            p31.setX(sqlQueryModel->record(2).value(0).toDouble());
                            p31.setY(sqlQueryModel->record(2).value(1).toDouble());
                            p41.setX(sqlQueryModel->record(3).value(0).toDouble());
                            p41.setY(sqlQueryModel->record(3).value(1).toDouble());
                           // r1 = sqlQueryModel->record(0).value(2).toDouble();
                            p12.setX(tempList[0].toDouble());
                            p12.setY(tempList[1].toDouble());
                            p22.setX(tempList[0].toDouble());
                            p22.setY(tempList[1].toDouble());
                            p32.setX(tempList[0].toDouble());
                            p32.setY(tempList[1].toDouble());
                            p42.setX(tempList[0].toDouble());
                            p42.setY(tempList[1].toDouble());
                            //r2 = tempList[2].toDouble();
                            model->setItem((i-1),(j+1),new QStandardItem((TwoCircles(p1, p2, r1, r2)? "Pass" : "Fails")));
                            qDebug() << sqlQueryModel->record(0).value(0).toString()<<sqlQueryModel->record(0).value(1).toString()<<sqlQueryModel->record(0).value(2).toString()<<sqlQueryModel->record(0).value(3).toString();

                        }
                    }
                }
             }
        }
        ui->display_table->setModel(model);
        conn.connClose();*/
}

void MainWindow::Compare_Door_Handle() //Compare Database & CSV File for Door Handle
{
    //Write Code to Read CSV File (Refer "Compare_CircularWindow()")

        //Iterate through each element in CSV & Compare the corresponding item with the element in "Door Handle table" in
        //Database
}

void MainWindow::Compare_Door_Framelines() //Compare Database & CSV File for Door Framelines
{
    //Write Code to Read CSV File (Refer "Compare_CircularWindow()")

        //Iterate through each element in CSV & Compare the corresponding item with the element in "Door Framelines table" in
        //Database
}

void MainWindow::Compare_Door_Footplate() //Compare Database & CSV File for Door Footplate
{
    //Write Code to Read CSV File (Refer "Compare_CircularWindow()")

        //Iterate through each element in CSV & Compare the corresponding item with the element in "Doorfootplate table" in
        //Database
}







#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include<QtDebug>
#include<QFileInfo>
#include<imagedialog.h>
#include<showpoints.h>
#include<QKeyEvent>
#include<QSqlQuery>
//#include<QDialog>
//#include<QtCore>
//include<QtGui>

//CSV File
#include <QFile>
#include <QStringList>
#include <QFileDialog>
#include<QStandardItemModel>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString imagePath;
    void init();
    QSqlDatabase mydb;
    void connClose()
    {
        QString connection;
        connection = mydb.connectionName();
        mydb.close();
        mydb = QSqlDatabase();
        mydb.removeDatabase(connection);
    }

    bool connOpen(QString dbpath)
    {
        mydb = QSqlDatabase::addDatabase("QSQLITE");
        qDebug() << dbpath;
        mydb.setDatabaseName(dbpath);

        if(!mydb.open())
        {
            qDebug()<<("Database Connection Failed");
            return false;
        }
        else
        {
            qDebug()<<("Connected To Database....." + mydb.databaseName());
            return true;
        }
    }
    QString getDBPath();
    void setMessage(QString imagePath);
    void global_store_imagefolderpath(QString path);

    void ElipticalWindow();
    void Compare_CircularWindow();
    void Compare_Rectangle();
    void Compare_Curved_Arrow();
    void Compare_Straight_Arrow();
    void Compare_Door_Handle();
    void Compare_Door_Framelines();
    void Compare_Door_Footplate();
    bool TwoCircles(QPoint p1, QPoint p2, double radius1, double radius2);
    bool TwoRectangles(QPoint p11, QPoint p12, QPoint p13, QPoint p14,QPoint p21,QPoint p22,QPoint p23,QPoint p24);


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_btn_connect_clicked();

    void on_btn_read_table_clicked();

    void on_btn_open_ImgDialog_clicked();

    void on_detection_results_selectionChanged();

    void on_read_det_results_clicked();

    void on_lineEdit_database_path_selectionChanged();

    void on_btn_compare_clicked();

    void on_show_points_clicked();

    void on_show_point_clicked();

private:
    Ui::MainWindow *ui;
    ImageDialog *imageDialog;
    ShowPoints *showPoints;
    void keyPressEvent(QKeyEvent *);

};

#endif // MAINWINDOW_H

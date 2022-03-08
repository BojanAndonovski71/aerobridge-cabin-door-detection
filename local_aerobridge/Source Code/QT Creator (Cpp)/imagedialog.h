#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include <QDialog>
#include<QtDebug>
#include<QMouseEvent>
#include<QKeyEvent>

#include<QLabel>
#include<QImage>
#include<QGraphicsScene>
#include<QGraphicsSceneMouseEvent>

#include<QPainter>
#include<QPen>
#include<QPoint>


#include<qmath.h>
#include<QSqlDatabase>

namespace Ui {
class ImageDialog;
}

class ImageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageDialog(QWidget *parent = 0);
    ~ImageDialog();

    QGraphicsScene *scene;
    QPixmap image;
    QBrush color; //Point Color
    int currentIndex; //Index of Current Image
    int points=0; //No Of Points selected On Image
    int count=0;
    QStringList allFiles; //Contails All Images In Selected Dir
    QString imagePath, imageName, imageFolder;
    QStringList featureDataObj;

    struct featureData{
        QList<int> X_array;
        QList<int> Y_array;
        QString Feature;

    };

    struct featureData EW, RW, Text, Footplate, DH, Framelines, SA, CA;

    void init(QString path);
    QSqlDatabase db;
    void connClose()
    {
        QString connection;
        connection = db.connectionName();
        db.close();
        db = QSqlDatabase();
        db.removeDatabase(connection);
    }

    bool connOpen(QString dbpath)
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbpath);

        if(!db.open())
        {
            qDebug()<<("Database Connection Failed");
            return false;
        }
        else
        {
            qDebug()<<("Connected To Database....." + db.databaseName());
            return true;
        }
    }


private slots:
    void on_prevImage_clicked();

    void on_nextImage_clicked();

    void on_resetImage_clicked();

    void on_feature_currentIndexChanged(int index);

private:
    Ui::ImageDialog *ui;
    void keyPressEvent(QKeyEvent *ev);


protected:
    void mousePressEvent(QMouseEvent *);
    //void paintEvent(QPaintEvent *event);
    void painter(int X, int Y);
    void storeFileinfo(QString imagePath);
    //void keyPressEvent(QKeyEvent *);
    void ElipticalWindow(struct featureData s);
    void four_points(struct featureData s);
    void text_points(struct featureData s);
    void many_points(struct featureData s);
    void WriteToDB(QString s);
    void clear_array(featureData *s);

};

#endif // IMAGEDIALOG_H

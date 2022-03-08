#ifndef SHOWPOINTS_H
#define SHOWPOINTS_H

#include <QDialog>
#include<QtDebug>
#include<QMouseEvent>
#include<QKeyEvent>

#include<QLabel>
#include<QImage>
#include<QGraphicsScene>

#include<QPainter>
#include<QPen>
#include<QPoint>

namespace Ui {
class ShowPoints;
}

class ShowPoints : public QDialog
{
    Q_OBJECT

public:

    QGraphicsScene *scene;
    QPixmap image;
    QBrush color; //Point Color
    int currentIndex; //Index of Current Image
    QString imagePath, imageName, imageFolder;
    QStringList allFiles; //Contails All Images In Selected Dir
    int line_thickness = 4;


    explicit ShowPoints(QWidget *parent = 0);
    ~ShowPoints();

    void init(QString path, QString name);
    void storeFileinfo(QString imagePath);
    void read_and_plot_points();
    void draw_points(QList<int> X_array, QList<int> Y_array, QString Feature);//(QList<int> X_array, QList<int> Y_array, QString Feature);
    void draw_ellipse();
    void draw_rect(QPen pen, QList<int> X, QList<int> Y);
    void draw_line(QPen pen, QList<int> X, QList<int> Y);
    void draw_st_arrow(QPen pen, QList<int> X, QList<int> Y);
    void draw_curved_arrow(QPen pen, QList<int> X, QList<int> Y);
    void draw_handle(QPen pen, QList<int> X, QList<int> Y);
    void connect_points(QPen pen,QList<int> X, QList<int> Y);
    void draw_arrow(QPen pen,QList<int> X, QList<int> Y);

private slots:

    void on_prevImage_clicked();

    void on_nextImage_clicked();
    void on_refreshImage_clicked();

private:
    Ui::ShowPoints *ui;
    void keyPressEvent(QKeyEvent *ev);
};

#endif // SHOWPOINTS_H

﻿#ifndef PRPSSCENE_H
#define PRPSSCENE_H

#include <QGraphicsScene>
#include "IO/Data/data.h"
#include <QTimer>
#include <QGraphicsLineItem>
#include "IO/Data/data.h"

class LineItem : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 15 };

    LineItem(QColor color, QLineF line, QGraphicsItem *parent = 0);

    QColor color() const { return myColor; }
    QLineF line() const { return myLine; }
    int type() const override { return Type;}
    void addNum() { myNum++; }
    int num() { return myNum; }

protected:

private:
    QLineF myLine;
    QColor myColor;
    int myNum;
};



class PRPSScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit PRPSScene(MODE mode, int data_max, QObject *parent = 0);

    void set_color_map(int h, int l){
        high = h;
        low = l;
    }

signals:

public slots:
    void addPRPD(QVector<QPoint> list);     //添加一组PRPD数据

private:
    void axisInit();
    void setText(QString str, QPointF P);
    QPointF transData(QPoint P);

    QPen pen_gray;
    QPen pen_red, pen_yellow, pen_green;

    QPointF P_shadow;     //投影
    QPointF P_amplitude_range;    //最大幅值
    QPointF P0, Px_max, Py_max;

    int data_group_num;

//    QVector<QPointF> test_list;
    MODE mode;
    int data_max;       //显示数据的最大值

    int high, low;      //颜色变化阈值
};

#endif // PRPSSCENE_H

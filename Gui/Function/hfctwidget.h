﻿#ifndef HFCTWIDGET_H
#define HFCTWIDGET_H

#include "channelwidget.h"
#include "Gui/Common/fir.h"
#include "Gui/Algorithm/Bp/bpcable.h"
//#include <dwt.h>
//#include "dwt.h"

namespace Ui {
class HFCTWidget;
}

//using namespace std;
//using namespace splab;


//typedef float   Type;

class HFCTWidget : public ChannelWidget
{
    Q_OBJECT

public:
    explicit HFCTWidget(G_PARA *data, CURRENT_KEY_VALUE *val, MODE mode, int menu_index, QWidget *parent = 0);
    ~HFCTWidget();

public slots:
    void reload(int index);   //重新装载设置
    void save_channel();        //保存通道数据

private slots:
    void fresh_1000ms();
    void fresh_100ms();
    void fresh_1ms();

private:
    Ui::HFCTWidget *ui;
    H_CHANNEL_SQL *hfct_sql;
    Fir *fir;

#ifdef TEST_LAB
    BpCable *bpcable;
#endif

    void do_key_ok();
    void do_key_up_down(int d);
    void do_key_left_right(int d);

    void fresh_setting(void);

    void chart_ini();
    void PRPDReset();
    void do_Spectra_compute();

    //数据流
    QVector<PC_DATA> pclist_100ms;      //100ms的脉冲数据,HFCT分析的基准

    qint32 max_100ms;       //100ms内的最大值
    QVector<int> pulse_list_100ms;
    MODE mode_continuous;

//    DWT<Type> discreteWT("db4");
};

#endif // HFCTWIDGET_H

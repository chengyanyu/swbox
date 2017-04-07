#include "debugset.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>
//#include <QPoint>
#include "ui_debugui.h"


DebugSet::DebugSet(QWidget *parent,G_PARA *g_data) : QFrame(parent),ui(new Ui::DebugUi)
{
    key_val = NULL;

    data = g_data;

    sql_para = sqlcfg->get_para();

    this->resize(455, 185);
    this->move(2, 31);
//    this->setStyleSheet("DebugSet {background-color:lightGray;}");
//    this->setStyleSheet("DebugSet {border-image: url(:/widgetphoto/mainmenu/bk2.png);}");
    ui->setupUi(this);
    ui->tabWidget->setStyleSheet("QTabWidget {background-color:lightGray;}");
    ui->tabWidget->widget(0)->setStyleSheet("QWidget {background-color:lightGray;}");
    ui->tabWidget->widget(1)->setStyleSheet("QWidget {background-color:lightGray;}");
    ui->tabWidget->widget(2)->setStyleSheet("QWidget {background-color:lightGray;}");

    ui->tabWidget->tabBar()->setStyle(new CustomTabStyle);
    iniUi();

    widget = new QWidget(this);
    widget->setStyleSheet("QWidget {background-color:lightGray;}");
    widget->resize(455, 185);
    widget->move(2, 31);


    lab1 = new QLabel(widget);
    lab1->setText(tr("请输入管理员密码，解锁调试模式"));
    lab2 = new QLabel(widget);
//    lab2->move(20,70);
    passwordEdit = new QLineEdit(widget);
//    passwordEdit->move(20,40);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(lab1);
    layout->addWidget(passwordEdit);
    layout->addWidget(lab2);
    widget->setLayout(layout);

    password_set = "UDLR";  //默认密码

    resetPassword();


//    plot = new QwtPlot(ui->scrollArea);
    plot = new QwtPlot;
    ui->scrollArea->setWidget(plot);
    plot->resize(80, 9);
//    plot->setStyleSheet("background:transparent;color:gray;font-family:Moonracer;font-size:10px;");

//    plot->setAxisScale(QwtPlot::xBottom, 0, 360, 90);

    plot->setAxisMaxMinor( QwtPlot::xBottom, 1);

    /* remove gap */
    plot->axisWidget(QwtPlot::xBottom)->setMargin(0);
    plot->axisScaleDraw(QwtPlot::xBottom)->enableComponent(QwtAbstractScaleDraw::Backbone, true);


//    plot->setAxisScale(QwtPlot::yLeft, 0, 20, 5);
    plot->setAxisMaxMinor(QwtPlot::yLeft, 1);
//    plot->axisScaleDraw(QwtPlot::yLeft)->enableComponent(QwtAbstractScaleDraw::Labels, false);
    plot->axisScaleDraw(QwtPlot::yLeft)->enableComponent(QwtAbstractScaleDraw::Labels, true);

    /* remove gap */
    plot->axisWidget(QwtPlot::yLeft)->setMargin(0);


    plot->plotLayout()->setAlignCanvasToScales(true);

    /* display */
    curve = new QwtPlotCurve();
    curve->setPen(QPen(Qt::yellow, 0, Qt::SolidLine, Qt::RoundCap));
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->attach(plot);

    /* insert wave data */
    wave.clear();
    for (int i = 0; i < 360; i++) {
        wave.push_back(QPointF(i, 10 + 10 * qSin(i * 8 * M_PI / 360)));
    }
    curve->setSamples(wave);

    plot->hide();




}

void DebugSet::set_offset_suggest(int a, int b)
{
    ui->lab_offset1->setText(QString("%1").arg(a));
    ui->lab_offset2->setText(QString("%1").arg(b));
}


void DebugSet::iniUi()
{
    ui->spinBox_Tev_Gain->setValue(this->sql_para->tev_gain );
    ui->spinBox_Tev_offset1->setValue(this->sql_para->tev_offset1);
    ui->spinBox_Tev_offset2->setValue(this->sql_para->tev_offset2);

    ui->spinBox_AAStep->setValue(this->sql_para->aa_step);

    ui->lineEdit_AA_offset->setText(QString("%1").arg(sql_para->aa_offset));

    ui->tabWidget->setCurrentIndex(0);

}

void DebugSet::resetPassword()
{
    pass = false;
    password.clear();
    passwordEdit->clear();
    lab2->clear();
    this->widget->show();
    ui->tabWidget->hide();

//    qDebug()<<"password dlg reset!";
}

void DebugSet::working(CURRENT_KEY_VALUE *val)
{
    if (val == NULL) {
        return;
    }
    key_val = val;

    this->show();
    qDebug()<<"show debug ui!";
}

void DebugSet::trans_key(quint8 key_code)
{
    if (key_val == NULL) {
        return;
    }

    if (key_val->grade.val1 != 2) {
        return;
    }
    if(key_val->grade.val2 != 1){
        return;
    }

    switch (key_code) {
    case KEY_OK:
        if(pass){
            if(key_val->grade.val4 == 0 && key_val->grade.val3 == 0){   //初始状态
                key_val->grade.val3 = 2;
                iniUi();
                qDebug()<<"tabWidget reset!";
            }
            else if(key_val->grade.val3 == 3 && key_val->grade.val4 != 0 ){   //录波
                emit startRecWv(key_val->grade.val4-1);
            }
            else{                                                   //保存
                qDebug()<<"debug para saved!";
                sql_para->tev_gain = ui->spinBox_Tev_Gain->value();
                sql_para->tev_offset1 = ui->spinBox_Tev_offset1->value();
                sql_para->tev_offset2 = ui->spinBox_Tev_offset2->value();
                sql_para->aa_step = ui->spinBox_AAStep->value();
                sqlcfg->sql_save(sql_para);

                resetPassword();

                key_val->grade.val4 = 0;
                key_val->grade.val3 = 0;
                key_val->grade.val2 = 0;

                fresh_parent();
            }
        }
        else{
            //检查密码是否匹配
            if(password == password_set){
                qDebug()<<"password accepted!";
                this->widget->hide();
                ui->tabWidget->show();
                pass = true;

                iniUi();
//                lab2->setText("password accepted!");
            }
            else{
                if(passwordEdit->text().isEmpty()){
                    //初始状态不显示
                }
                else{
                    resetPassword();
                    lab2->setText("password error!");
                    qDebug()<<"password error!";
                }
            }
        }

        break;
    case KEY_CANCEL:
        if(key_val->grade.val4 == 0){   //退出Debug设置
            key_val->grade.val3 = 0;
            key_val->grade.val2 = 0;

            resetPassword();

            fresh_parent();
        }
        else{
            key_val->grade.val4 = 0;    //退出三级菜单
        }

        break;
    case KEY_UP:
        if(pass){
            if(key_val->grade.val4 == 0){   //判断在二级菜单
                if(key_val->grade.val3>1){
                    key_val->grade.val3 --;
                }
                else{
                    key_val->grade.val3 = 3;
                }
            }
            else{                           //判断在三级菜单
                switch (key_val->grade.val3) {
                case 0:

                    break;
                case 1:
                    if(key_val->grade.val4 >1 ){
                        key_val->grade.val4 --;
                    }
                    else{
                        key_val->grade.val4 = 3;
                    }
                    break;
                case 2:
                    if(key_val->grade.val4 >1 ){
                        key_val->grade.val4 --;
                    }
                    else{
                        key_val->grade.val4 = 2;
                    }
                    break;
                case 3:
                    if(key_val->grade.val4 < 2){
                        key_val->grade.val4 ++;
                    }
                    else{
                        key_val->grade.val4 = 1;
                    }

                    break;
                default:
                    break;
                }
            }
        }
        else{
            password.append("U");
            passwordEdit->setText(passwordEdit->text()+"*");
        }
        break;
    case KEY_DOWN:
        if(pass){
            if(key_val->grade.val4 == 0){   //判断在二级菜单
                if(key_val->grade.val3<3){
                    key_val->grade.val3 ++;
                }
                else{
                    key_val->grade.val3 = 1;
                }
            }
            else{                           //判断在三级菜单
                switch (key_val->grade.val3) {
                case 0:

                    break;
                case 1:
                    if(key_val->grade.val4 <3){
                        key_val->grade.val4 ++;
                    }
                    else{
                        key_val->grade.val4 = 1;
                    }
                    break;
                case 2:
                    if(key_val->grade.val4 <2){
                        key_val->grade.val4 ++;
                    }
                    else{
                        key_val->grade.val4 = 1;
                    }
                    break;
                case 3:
                    if(key_val->grade.val4 < 2){
                        key_val->grade.val4 ++;
                    }
                    else{
                        key_val->grade.val4 = 1;
                    }
                    break;
                default:
                    break;
                }
            }
        }
        else{
            password.append("D");
            passwordEdit->setText(passwordEdit->text()+"*");
        }
        break;
    case KEY_LEFT:
        if(pass){
            if(key_val->grade.val4 == 0){
            }
            else{
                switch (key_val->grade.val3) {
                case 0:

                    break;
                case 1:
                    if(key_val->grade.val4 == 1){
                        ui->spinBox_Tev_Gain->stepDown();
                    }
                    else if(key_val->grade.val4 == 2){
                        ui->spinBox_Tev_offset1->stepDown();
                    }
                    else if(key_val->grade.val4 == 3){
                        ui->spinBox_Tev_offset2->stepDown();
                    }
                    break;
                case 2:
                    if(key_val->grade.val4 == 1){
                        ui->spinBox_AAStep->stepDown();
                    }
                    else if(key_val->grade.val4 == 2){
                        sql_para->aa_offset--;
                    }
                    break;
                case 3:
//                    if(key_val->grade.val4 < 2){
//                        key_val->grade.val4 ++;
//                    }
//                    else{
//                        key_val->grade.val4 = 1;
//                    }

                    break;
                default:
                    break;
                }
            }
        }
        else{
            password.append("L");
            passwordEdit->setText(passwordEdit->text()+"*");
        }

        break;
    case KEY_RIGHT:
        if(pass){
            if(key_val->grade.val4 == 0 && key_val->grade.val3 != 0){   //必须第三层处于工作状态
                key_val->grade.val4 =1;
            }
            else{
                switch (key_val->grade.val3) {
                case 0:

                    break;
                case 1:
                    if(key_val->grade.val4 == 1){
                        ui->spinBox_Tev_Gain->stepUp();
                    }
                    else if(key_val->grade.val4 == 2){
                        ui->spinBox_Tev_offset1->stepUp();
                    }
                    else if(key_val->grade.val4 == 3){
                        ui->spinBox_Tev_offset2->stepUp();
                    }
                    break;
                case 2:
                    if(key_val->grade.val4 == 1){
                        ui->spinBox_AAStep->stepUp();
                    }
                    else if(key_val->grade.val4 == 2){
                        sql_para->aa_offset++;
                    }
                    break;
                case 3:
//                    if(key_val->grade.val4 < 2){
//                        key_val->grade.val4 ++;
//                    }
//                    else{
//                        key_val->grade.val4 = 1;
//                    }

                    break;
                default:
                    break;
                }
            }
        }
        else{
            password.append("R");
            passwordEdit->setText(passwordEdit->text()+"*");
        }

        break;
    default:
        break;
    }
    fresh();
}

//处理显示录波信号
void DebugSet::showWaveData(qint32 *wv, int len, int mod)
{
    switch (mod) {
    case 0:     //TEV
        ui->lab_recWv->setText(tr("接到录波信号,通道%1").arg("TEV"));
        break;
    case 1:     //AA超声
        ui->lab_recWv->setText(tr("接到录波信号,通道%1").arg("AAUltrasonic"));
        break;
    case 2:

        break;
    case 3:

        break;
    default:
        break;
    }

    wave.clear();
    for(int i=0;i<len;i++){
        wave.append(QPoint(i,wv[i]));
    }

    curve->setSamples(wave);
    plot->show();

}


void DebugSet::fresh()
{
//    printf("\nkey_val->grade.val1 is : %d",key_val->grade.val1);
//    printf("\tkey_val->grade.val2 is : %d",key_val->grade.val2);
//    printf("\tkey_val->grade.val3 is : %d",key_val->grade.val3);
//    printf("\tkey_val->grade.val4 is : %d",key_val->grade.val4);

    if(pass){
        if(key_val->grade.val3){
            ui->tabWidget->setCurrentIndex(key_val->grade.val3-1);
        }
        switch (key_val->grade.val3) {
        case 0:

            break;
        case 1:
            if(key_val->grade.val4 == 0){
                ui->spinBox_Tev_Gain->setStyleSheet("QDoubleSpinBox { background: lightGray }");
                ui->spinBox_Tev_offset1->setStyleSheet("QSpinBox { background: lightGray }");
                ui->spinBox_Tev_offset2->setStyleSheet("QSpinBox { background: lightGray }");
            }
            else if(key_val->grade.val4 == 1){
                ui->spinBox_Tev_Gain->setStyleSheet("QDoubleSpinBox { background: gray }");
                ui->spinBox_Tev_offset1->setStyleSheet("QSpinBox { background: lightGray }");
                ui->spinBox_Tev_offset2->setStyleSheet("QSpinBox { background: lightGray }");
            }
            else if(key_val->grade.val4 == 2){
                ui->spinBox_Tev_Gain->setStyleSheet("QDoubleSpinBox { background: lightGray }");
                ui->spinBox_Tev_offset1->setStyleSheet("QSpinBox { background: gray }");
                ui->spinBox_Tev_offset2->setStyleSheet("QSpinBox { background: lightGray }");
            }
            else if(key_val->grade.val4 == 3){
                ui->spinBox_Tev_Gain->setStyleSheet("QDoubleSpinBox { background: lightGray }");
                ui->spinBox_Tev_offset1->setStyleSheet("QSpinBox { background: lightGray }");
                ui->spinBox_Tev_offset2->setStyleSheet("QSpinBox { background: gray }");
            }
            break;
        case 2:
            if(key_val->grade.val4 == 0){
                ui->spinBox_AAStep->setStyleSheet("QDoubleSpinBox { background: lightGray }");
                ui->lineEdit_AA_offset->setStyleSheet("QLineEdit { background: lightGray }");
                ui->lineEdit_AA_offset->deselect();
            }
            else if(key_val->grade.val4 == 1){
                ui->spinBox_AAStep->setStyleSheet("QDoubleSpinBox { background: gray }");
                ui->lineEdit_AA_offset->setStyleSheet("QLineEdit { background: lightGray }");
                ui->lineEdit_AA_offset->deselect();
            }
            else if(key_val->grade.val4 == 2){
                ui->spinBox_AAStep->setStyleSheet("QDoubleSpinBox { background: lightGray }");
                ui->lineEdit_AA_offset->setStyleSheet("QLineEdit { background: gray }");
                ui->lineEdit_AA_offset->selectAll();
            }
            ui->lineEdit_AA_offset->setText(QString("%1").arg(sql_para->aa_offset));
            break;
        case 3:
            ui->comboBox->setCurrentIndex(key_val->grade.val4 - 1);
            if(key_val->grade.val4 == 0){
                ui->comboBox->setStyleSheet("QComboBox { background: lightGray }");
            }
            else if(key_val->grade.val4 == 1){
                ui->comboBox->setStyleSheet("QComboBox { background: gray }");
            }
            break;
        default:
            break;
        }
    }
    else{

    }


}


















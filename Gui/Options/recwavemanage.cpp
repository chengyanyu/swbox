#include "recwavemanage.h"
#include "ui_voiceplayer.h"
#include <QtDebug>
#include <QMessageBox>
#include <QAbstractButton>

RecWaveManage::RecWaveManage(QWidget *parent) : QFrame(parent), ui(new Ui::Form)
{
    key_val = NULL;

    this->resize(CHANNEL_X, CHANNEL_Y);
    this->move(3, 3);

    tableWidget = new QTableWidget(this);
    tableWidget->resize(CHANNEL_X, CHANNEL_Y);
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode ( QAbstractItemView::SingleSelection); //设置选择模式，选择单行
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->horizontalHeader()->setVisible(false);
    tableWidget->verticalHeader()->setDefaultSectionSize(16);
    tableWidget->horizontalHeader()->setDefaultSectionSize(350);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setGridStyle(Qt::NoPen);
    tableWidget->setStyleSheet("alternate-background-color: lightGray;");

    recWaveForm = new RecWaveForm(5,this);
    connect(this, SIGNAL(send_key(quint8)), recWaveForm, SLOT(trans_key(quint8)));

    contextMenu = new QListWidget(this);        //右键菜单
    contextMenu->setStyleSheet("QListWidget {border-image: url(:/widgetphoto/bk/para_child.png);color:gray;outline: none;}");
    QStringList list;
    list << tr("查看波形") << tr("收藏/取消收藏") << tr("删除当前波形") << tr("删除全部波形") << tr("播放声音");
    contextMenu->addItems(list);
    contextMenu->resize(100, 90);
    contextMenu->move(300,10);
    contextMenu->setSpacing(2);
    contextMenu->hide();

    contextMenu_num = 3;        //显示菜单条目

    dir = QDir(WAVE_DIR);
    dir_favorite = QDir(FAVORITE_DIR);
    dir_sd = QDir("/mmc/sdcard/WaveForm/");

    box = new QMessageBox(QMessageBox::Warning,tr("删除全部文件"),tr("将要删除本机保存的全部波形文件.\n确定要删除吗?"),
                          QMessageBox::Ok | QMessageBox::Cancel,this);

    box->hide();
    box->move(135,100);

    reset_flag = 0;

    player = new QFrame(this);
    ui->setupUi(player);
    player->resize(454,100);
    player->move(2,88);
    player->setStyleSheet("QFrame {background-color:lightGray}");
    player->hide();

    this->hide();
}

RecWaveManage::~RecWaveManage()
{
    delete ui;
}

void RecWaveManage::playVoiceProgress(int p, int all, bool f)
{
    ui->horizontalSlider->setRange(0,all);
    ui->horizontalSlider->setValue(p);
    player->setVisible(f);
    ui->label_name->setText(tableWidget->currentItem()->text());
    ui->label_current->setText(QString::number(p/100.0, 'f', 1) + "s" );
    ui->label_All->setText(QString::number(all/100.0, 'f', 1) + "s" );
}

void RecWaveManage::working(CURRENT_KEY_VALUE *val)
{
    if (val == NULL) {
        return;
    }
    key_val = val;

    reload_tablewidget();
//    listWidget->setCurrentRow(0);
    this->show();
}

void RecWaveManage::reload_tablewidget()
{
    //录波数据列表初始化
    QDir dir = QDir(WAVE_DIR);
    QDir dir_favorite = QDir(FAVORITE_DIR);
//    int r = tableWidget->currentRow();
    tableWidget->clear();

    QStringList filters;
    filters << "*.DAT" ;
    dir.setNameFilters(filters);
    dir_favorite.setNameFilters(filters);

    QStringList list = dir.entryList(QDir::Files);
    QStringList list_favorite = dir_favorite.entryList(QDir::Files);

    list.replaceInStrings(".DAT", "");
    list_favorite.replaceInStrings(".DAT", "");

    QFileInfoList list_file_info = dir.entryInfoList(QDir::Files);
    QFileInfoList list_favorite_file_info = dir_favorite.entryInfoList(QDir::Files);

    tableWidget->setRowCount(list_file_info.length() + list_favorite_file_info.length());
    tableWidget->setColumnCount(2);

    for (int i = 0; i < list_favorite_file_info.length(); ++i) {
        tableWidget->setItem(i, 0, new QTableWidgetItem(QString("%1☆").arg(list_favorite.at(i))) );
        tableWidget->setItem(i, 1, new QTableWidgetItem(QString("%1 KB").arg(list_favorite_file_info.at(i).size() / 1000) ) );
    }

    for (int i = 0; i < list_file_info.length(); ++i) {
        tableWidget->setItem(i + list_favorite_file_info.length(), 0, new QTableWidgetItem(list.at(i)) );
        tableWidget->setItem(i + list_favorite_file_info.length(), 1,
                             new QTableWidgetItem(QString("%1 KB").arg(list_file_info.at(i).size() / 1000) ) );
    }

    refresh();

//    tableWidget->setCurrentCell(r,0);       //保存选择状态
}

void RecWaveManage::trans_key(quint8 key_code)
{
    if (key_val == NULL) {
        return;
    }

    if (key_val->grade.val1 != 3) {
        return;
    }

    if(player->isVisible()){
        emit stop_play_voice();
        return;
    }

    if (key_val->grade.val5 != 0) {
        emit send_key(key_code);
        return;
    }


    switch (key_code) {
    case KEY_OK:
        if(reset_flag == 0){
            if(tableWidget->currentRow()>=0){
                reset_flag = 0;
                contextMenu->hide();
                switch (key_val->grade.val4) {
                case 0:             //查看波形
                case 1:             //查看波形
                    key_val->grade.val5 = 1;
                    key_val->grade.val4 = 0;
                    recWaveForm->working(key_val,tableWidget->currentItem()->text());
                    break;
                case 2:
                    key_val->grade.val4 = 0;
                    do_favorite();
                    break;
                case 3:             //删除当前波形
                    key_val->grade.val4 = 0;
                    deleteCurrent();
                    break;
                case 4:             //删除全部波形
                    key_val->grade.val4 = 0;
                    reset_flag = 1;
                    break;
                case 5:             //播放声音
                    key_val->grade.val4 = 0;
                    readVoiceData();
                    qDebug()<<"play voice";
                    break;
                default:
                    break;
                }
            }
        }
        else if(reset_flag == 1){
            reset_flag = 0;
        }
        else if(reset_flag == 2){
            reset_flag = 0;
            deleteAll();        //确定删除
            reload_tablewidget();
        }

        break;
    case KEY_CANCEL:        
        if(reset_flag > 0){
            reset_flag = 0;
        }
        else if(key_val->grade.val4 >0){     //小退
            key_val->grade.val4 = 0;
        }
        else{                           //大退
            key_val->grade.val2 = 0;
            key_val->grade.val3 = 0;
            key_val->grade.val5 = 0;
            this->hide();
            emit fresh_parent();
        }

        break;
    case KEY_UP:
        if(reset_flag == 0){
            if(key_val->grade.val4 > 0){
                if(key_val->grade.val4 > 1){
                    key_val->grade.val4 --;
                }
                else{
                    key_val->grade.val4 = contextMenu_num;
                }

            }
            else if(key_val->grade.val3 > 1){
                key_val->grade.val3 --;
            }
            else if(key_val->grade.val3 == 1){
                key_val->grade.val3 = tableWidget->rowCount();
            }
        }
        break;
    case KEY_DOWN:
        if(reset_flag == 0){
            if(key_val->grade.val4 > 0){
                if(key_val->grade.val4 < contextMenu_num){
                    key_val->grade.val4 ++;
                }
                else{
                    key_val->grade.val4 = 1;
                }

            }
            else if(key_val->grade.val3 < tableWidget->rowCount()){
                key_val->grade.val3 ++;
            }
            else if(key_val->grade.val3 == tableWidget->rowCount()){
                key_val->grade.val3 = 1;
            }
        }
        break;
    case KEY_LEFT:
        if(reset_flag == 1){
            reset_flag = 2;
        }
        else if(reset_flag == 2){
            reset_flag = 1;
        }
        else{
            contextMenu->hide();
        }
        break;
    case KEY_RIGHT:
        if(reset_flag == 1){
            reset_flag = 2;
        }
        else if(reset_flag == 2){
            reset_flag = 1;
        }
        else{
            if(key_val->grade.val3){
                if(key_val->grade.val4 == 0){
                    key_val->grade.val4 ++;
                    contextMenu->show();
                }
//                qDebug()<<contextMenu->visualItemRect(contextMenu->currentItem());
            }
        }

        break;
    default:
        break;
    }

    refresh();
}


void RecWaveManage::do_favorite()
{
    QString filename = tableWidget->currentItem()->text();
    QString filepath, newpath;
    if(filename.contains(QString("☆")) ){
        filepath = FAVORITE_DIR"/" + filename.remove(QString("☆") );
        newpath = WAVE_DIR"/" + filename.remove(QString("☆") );
    }
    else {
        filepath = WAVE_DIR"/" + filename;
        newpath = FAVORITE_DIR"/" + filename;
    }

    QFile file;
    file.setFileName(filepath + ".DAT");
    file.rename(newpath + ".DAT");

    file.setFileName(filepath + ".CFG");
    file.rename(newpath + ".CFG");

    file.setFileName(filepath + ".mp3");
    file.rename(newpath + ".mp3");

    system( "sync");
    reload_tablewidget();

}

//删除全部文件
void RecWaveManage::deleteAll()
{
    qDebug()<<"delete All";
    QStringList list = dir.entryList(QDir::Files);
//    QStringList list = dir.entryList();
    qDebug()<<"to be delete number:"<<list.length();
//    qDebug()<<"to be delete number:"<<dir.entryInfoList().length();

    foreach (QString l, list) {
        dir.remove(l);
    }

    list = dir_sd.entryList(QDir::Files);
    foreach (QString l, list) {
        dir_sd.remove(l);
    }

    system( "sync");
}

void RecWaveManage::deleteCurrent()
{
    dir.remove(tableWidget->currentItem()->text() + ".DAT");
    dir.remove(tableWidget->currentItem()->text() + ".CFG");
    dir.remove(tableWidget->currentItem()->text() + ".mp3");

    dir_sd.remove(tableWidget->currentItem()->text() + ".DAT");
    dir_sd.remove(tableWidget->currentItem()->text() + ".CFG");
    dir_sd.remove(tableWidget->currentItem()->text() + ".mp3");

    if(tableWidget->currentItem()->text().contains(QString("☆"))){
        dir_favorite.remove(tableWidget->currentItem()->text().remove(QString("☆")) + ".DAT");
        dir_favorite.remove(tableWidget->currentItem()->text().remove(QString("☆")) + ".CFG");
        dir_favorite.remove(tableWidget->currentItem()->text().remove(QString("☆")) + ".mp3");
    }

    system( "sync");
    reload_tablewidget();
}

void RecWaveManage::readVoiceData()
{
    QFile file;
    file.setFileName(WAVE_DIR"/"+tableWidget->currentItem()->text()+".DAT");

    if (!file.open(QIODevice::ReadOnly)){
        qDebug()<<"file open failed!";
        return;
    }

    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);

    quint32 t1,t2;
    qint16 v;
    VectorList list;
    while (!in.atEnd()) {
        in >> t1 >> t2 >> v;
        list.append((qint32)(v));
    }

    emit play_voice(list);


    file.close();

}

void RecWaveManage::refresh()
{
    tableWidget->setCurrentCell(key_val->grade.val3 - 1, 0);

    contextMenu->setCurrentRow(key_val->grade.val4 - 1);
    if(key_val->grade.val4 == 0){
        contextMenu->setVisible(false);
    }
    if(tableWidget->currentRow()>=0){            //根据条目改变菜单
        if(tableWidget->currentItem()->text().contains(("AAUltrasonic"))){
            contextMenu->item(4)->setHidden(false);
            contextMenu_num = 5;
            contextMenu->resize(100 + 10, 19*contextMenu_num);
        }
        else{
            contextMenu->item(4)->setHidden(true);
            contextMenu_num = 4;
            contextMenu->resize(100 + 10, 19*contextMenu_num);
        }
    }

    if(reset_flag){
        box->show();
        if(reset_flag == 1){
            box->button(QMessageBox::Ok)->setStyleSheet("");
            box->button(QMessageBox::Cancel)->setStyleSheet("QPushButton {background-color:gray;}");
            box->setDefaultButton(QMessageBox::Cancel);
        }
        else if(reset_flag == 2){
            box->button(QMessageBox::Ok)->setStyleSheet("QPushButton {background-color:gray;}");
            box->button(QMessageBox::Cancel)->setStyleSheet("");
            box->setDefaultButton(QMessageBox::Ok);
        }

    }
    else{
        box->hide();
    }
}


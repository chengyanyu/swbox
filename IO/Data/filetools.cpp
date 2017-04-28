#include "filetools.h"
#include <QtDebug>
#include <QFile>
#include <QDataStream>
#include <QDateTime>
#include <QDir>


//还没有开启冲突控制
FileTools::FileTools()
{
    this->start();      //开启线程
    work = false;
}

void FileTools::saveWaveToFile(VectorList data, MODE mode)
{
    _data = data;
    _mode = mode;
    filepath = getFilePath();       //得到文件保存的路径
    qDebug()<<filepath;
    work = true;
}

void FileTools::run()
{
    while(true){
        if(work){
            saveDataFile();     //保存数据文件
            saveCfgFile();      //生成对应的配置文件
            system ("sync");

            if(_mode == AA_Ultrasonic){
                saveWavFile();      //生成声音文件
            }

            //这里插入mp3转换代码

            qDebug()<<"new thread save completed!";

            work = false;
        }
        else{
            sleep(1);
        }
    }
    this->terminate();  //停止线程
    this->wait();
}

void FileTools::saveDataFile()
{
    QFile file;
    bool flag;

    //保存文本文件
    file.setFileName(filepath + ".txt");
    flag = file.open(QIODevice::WriteOnly | QIODevice::Text);
    if(flag){
        QTextStream out(&file);
        short a;
        for(int i=0;i<_data.length();i++){
            a = (qint16)_data[i];
            out << a <<",";
        }
        qDebug()<<file.fileName()<<"    "<<_data.length()<<" points";
        file.close();
    }
    else
        qDebug()<<"file open failed!";


    //保存二进制文件（小端）
    file.setFileName(filepath + ".DAT");
    flag = file.open(QIODevice::WriteOnly);
    if(flag){
        QDataStream out(&file);
        out.setByteOrder(QDataStream::LittleEndian);
        short a;
        for(int i=0;i<_data.length();i++){
            out << (quint32)(i+1);
            out << (quint32)0xFF;
            a = (qint16)_data[i];
            out << a;
        }
        //        size = file.size();        //保存文件大小
        file.close();
    }
    else
        qDebug()<<"file open failed!";



}

//实现优先在SD卡建立波形存储文件夹
//如果SD卡未插入，则在内存当前文件夹下建立波形文件夹
//返回波形文件夹路径
QString FileTools::getFilePath()
{
    QString filename;
    filename = QString("%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-zzz"));

    switch (_mode) {
    case 0:     //地电波
        filename.prepend("TEV_");
        break;
    case 1:     //AA超声
        filename.prepend("AAUltrasonic_");
        break;
    case 2:     //

        break;
    default:
        break;
    }

    //文件夹操作
    QDir dir;
    //    if(dir.exists("/mmc/sdcard/WaveForm/")){
    //        return QString("/mmc/sdcard/WaveForm/" + filename);
    //    }
    //    else if(dir.mkdir("/mmc/sdcard/WaveForm/") ){
    //        return QString("/mmc/sdcard/WaveForm/" + filename);
    //    }
    //    else
    if(dir.exists("./WaveForm/") ){
        qDebug()<< "SD card failed!";
        return QString("./WaveForm/" + filename);
    }
    else if(dir.mkdir("./WaveForm/")){
        qDebug()<< "SD card failed!";
        return QString("./WaveForm/" + filename);
    }
    else{
        return QString();       //全失败不太可能
    }
}

void FileTools::saveCfgFile()
{
    QFile file;
    file.setFileName(filepath + ".CFG");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "wave1,,1999" << "\n";
    out << "1,1A,0D" << "\n";
    out << "1,UA,,,V,1.000000,0.000000,0,-32768,32767,1.000000,1.000000,S" << "\n";
    out << "50.000000" << "\n";
    out << "1" << "\n";
    out << "3200," << _data.length() << "\n";     //点数可变
    out << QDateTime::currentDateTime().toString() << "\n";
    out << QDateTime::currentDateTime().toString() << "\n";
    out << "BINARY" << "\n";

    file.close();
}

//保存wav文件
void FileTools::saveWavFile()
{
    wfh1=(WavFormatHeader*)malloc(sizeof(WavFormatHeader));
    //    memset(wfh1,0,sizeof(wfh1));

    int size = _data.length() * 2;
    //    char *Data1 = (char*)malloc(size);
    //    for(int i=0;i<size;i++){
    //        Data1[i] = (char)_data.at(i);
    //    }



    strcpy(wfh1->RIFF,"RIFF");
    wfh1->Filesize = size + sizeof(WavFormatHeader);
    strcpy(wfh1->WavFlags,"WAVE");
    strcpy(wfh1->FMTFlags,"fmt ");
    wfh1->ByteFilter = 16;
    wfh1->Format = 1;
    wfh1->Channel = 1;
    wfh1->SampleFreq = 320000;
    wfh1->PCMBitSize = 16;
    wfh1->ByteFreq   = wfh1->PCMBitSize * wfh1->Channel * wfh1->SampleFreq / 8;     //640000
    wfh1->BlockAlign = wfh1->PCMBitSize * wfh1->Channel / 8;
    strcpy(wfh1->fact,"data");
    wfh1->Datasize = size;


    QFile file("out.wav");
    file.setFileName(filepath + ".wav");
    if (!file.open(QIODevice::WriteOnly))
        return;

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);

    out.writeRawData(wfh1->RIFF,4);
    out << (qint32)wfh1->Filesize;
    out.writeRawData(wfh1->WavFlags,4);
    out.writeRawData(wfh1->FMTFlags,4);
    out << wfh1->ByteFilter;
    out << wfh1->Format;
    out << wfh1->Channel;
    out << (qint32)wfh1->SampleFreq;
    out << (qint32)wfh1->ByteFreq;
    out << wfh1->BlockAlign;
    out << wfh1->PCMBitSize;
    out.writeRawData(wfh1->fact,4);
    out << (qint32)wfh1->Datasize;

    qDebug()<<"wfh1->Datasize :"<< wfh1->Datasize;

    int i;
    //    char c;
    //    int t;
    for(i=0;i<_data.length();i++){
        out << qint16(_data.at(i) * 32);
        //        t = _data.at(i)>>2;
        //        c = char(t);
        //        out.writeRawData(&c,1);
    }
    qDebug()<<"i="<<i;

    file.close();
}




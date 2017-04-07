#include "menu5.h"

Menu5::Menu5(QWidget *parent) : QFrame(parent)
{
    key_val = NULL;

    this->resize(459, 219);
    this->move(10, 28);
    this->setStyleSheet("Menu5 {border-image: url(:/widgetphoto/mainmenu/tab6.png);}");

    /* menu list */
    main_title0 = new QLabel(this);
    main_title0->resize(39, 24);
    main_title0->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m10.png);}");
    main_title0->move(2, 3);

    main_title1 = new QLabel(this);
    main_title1->resize(main_title0->width(), main_title0->height());
    main_title1->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m20.png);}");
    main_title1->move(main_title0->x() + 45, main_title0->y());

    main_title2 = new QLabel(this);
    main_title2->resize(main_title0->width(), main_title0->height());
    main_title2->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m30.png);}");
    main_title2->move(main_title1->x() + 45, main_title0->y());

    main_title3 = new QLabel(this);
    main_title3->resize(main_title0->width(), main_title0->height());
    main_title3->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m40.png);}");
    main_title3->move(main_title2->x() + 45, main_title0->y());

    main_title4 = new QLabel(this);
    main_title4->resize(main_title0->width(), main_title0->height());
    main_title4->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m50.png);}");
    main_title4->move(main_title3->x() + 45, main_title0->y());

    main_title5 = new QLabel(this);
    main_title5->resize(main_title0->width(), main_title0->height());
    main_title5->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m61.png);}");
    main_title5->move(main_title4->x() + 45, main_title0->y());

    main_title6 = new QLabel(this);
    main_title6->resize(main_title0->width(), main_title0->height());
    main_title6->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m70.png);}");
    main_title6->move(main_title5->x() + 45, main_title0->y());

    /* grade II menu image */
    load_data_lab = new QLabel(this);
    load_data_lab->resize(174, 42);
    load_data_lab->move(150, 50);
    load_data_lab->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/grade2_bk.png);}");

    /* grade II menu text */
    load_data_txt_lab = new QLabel(load_data_lab);
    load_data_txt_lab->resize(153, 21);
    load_data_txt_lab->move(11, 7);
    load_data_txt_lab->setAttribute(Qt::WA_TranslucentBackground, true);
    load_data_txt_lab->setStyleSheet("QLabel {color:gray;}");
    load_data_txt_lab->setAlignment(Qt::AlignCenter);
    load_data_txt_lab->setText(tr("载入数据"));

    /* grade II menu image */
    env_recd_lab = new QLabel(this);
    env_recd_lab->resize(load_data_lab->width(), load_data_lab->height());
    env_recd_lab->move(load_data_lab->x(), load_data_lab->y() + 27);
    env_recd_lab->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/grade2_bk.png);}");

    /* grade II menu text */
    env_recd_txt_lab = new QLabel(env_recd_lab);
    env_recd_txt_lab->resize(153, 21);
    env_recd_txt_lab->move(11, 7);
    env_recd_txt_lab->setAttribute(Qt::WA_TranslucentBackground, true);
    env_recd_txt_lab->setStyleSheet("QLabel {color:gray;}");
    env_recd_txt_lab->setAlignment(Qt::AlignCenter);
    env_recd_txt_lab->setText(tr("环境记录"));

    /* grade II menu image */
    back_recd_lab = new QLabel(this);
    back_recd_lab->resize(env_recd_lab->width(), env_recd_lab->height());
    back_recd_lab->move(env_recd_lab->x(), env_recd_lab->y() + 27);
    back_recd_lab->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/grade2_bk.png);}");

    /* grade II menu text */
    back_recd_txt_lab = new QLabel(back_recd_lab);
    back_recd_txt_lab->resize(153, 21);
    back_recd_txt_lab->move(11, 7);
    back_recd_txt_lab->setAttribute(Qt::WA_TranslucentBackground, true);
    back_recd_txt_lab->setStyleSheet("QLabel {color:gray;}");
    back_recd_txt_lab->setAlignment(Qt::AlignCenter);
    back_recd_txt_lab->setText(tr("背景记录"));

    /* grade II menu image */
    test_pos_lab = new QLabel(this);
    test_pos_lab->resize(back_recd_lab->width(), back_recd_lab->height());
    test_pos_lab->move(back_recd_lab->x(), back_recd_lab->y() + 27);
    test_pos_lab->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/grade2_bk.png);}");

    /* grade II menu text */
    test_pos_txt_lab = new QLabel(test_pos_lab);
    test_pos_txt_lab->resize(153, 21);
    test_pos_txt_lab->move(11, 7);
    test_pos_txt_lab->setAttribute(Qt::WA_TranslucentBackground, true);
    test_pos_txt_lab->setStyleSheet("QLabel {color:gray;}");
    test_pos_txt_lab->setAlignment(Qt::AlignCenter);
    test_pos_txt_lab->setText(tr("测试部位"));
#if 0
    load_data_lab->hide();
    env_recd_lab->hide();
    back_recd_lab->hide();
    test_pos_lab->hide();
#endif
}

void Menu5::working(CURRENT_KEY_VALUE *val)
{
    if (val == NULL) {
        return;
    }
    key_val = val;
    if (!key_val->grade.val1) {
        //amplitude->hide();
        //pulse->hide();
    }
    fresh_table();
    this->show();
}

void Menu5::trans_key(quint8 key_code)
{
    if (key_val == NULL) {
        return;
    }
    if (key_val->grade.val0 != 5) {                                             //not current menu
        return;
    }

    switch (key_code) {
    case KEY_OK:
        if (!key_val->grade.val1) {
            key_val->grade.val1 = 1;
            fresh_grade1();
        } else if (key_val->grade.val1 == 1) {
            //key_val->grade.val2 = 1;
            //amplitude->working(key_val);
            //pulse->hide();
        } else if (key_val->grade.val1 == 2) {
            //key_val->grade.val2 = 1;
            //amplitude->hide();
            //pulse->working(key_val);
        } else if (key_val->grade.val1 == 3) {
            //key_val->grade.val2 = 1;
            //amplitude->working(key_val);
            //pulse->hide();
        } else if (key_val->grade.val1 == 4) {
            //key_val->grade.val2 = 1;
            //amplitude->hide();
            //pulse->working(key_val);
        }
        break;
    case KEY_CANCEL:
        if (!key_val->grade.val2) {
            key_val->grade.val1 = 0;
            fresh_grade1();
        }
        break;
    case KEY_UP:
        if (!key_val->grade.val2) {
            if (key_val->grade.val1 > 1) {
                key_val->grade.val1--;
            } else {
                key_val->grade.val1 = 4;
            }
            fresh_grade1();
        }
        break;
    case KEY_DOWN:
        if (!key_val->grade.val2) {
            if (key_val->grade.val1 < 4) {
                key_val->grade.val1++;
            } else {
                key_val->grade.val1 = 1;
            }
            fresh_grade1();
        }
        break;
    case KEY_LEFT:

        break;

    case KEY_RIGHT:

        break;
    default:
        break;
    }
    fresh_table();
    emit send_key(key_code);
}

void Menu5::fresh_grade1(void)
{
    //amplitude->hide();                                                          //hide before gui
    //pulse->hide();
    this->show();                                                               //show current gui

    switch (key_val->grade.val1) {
    case 0:
        load_data_txt_lab->setStyleSheet("QLabel {color:gray;}");
        env_recd_txt_lab->setStyleSheet("QLabel {color:gray;}");
        back_recd_txt_lab->setStyleSheet("QLabel {color:gray;}");
        test_pos_txt_lab->setStyleSheet("QLabel {color:gray;}");
        break;
    case 1:
        load_data_txt_lab->setStyleSheet("QLabel {color:white;}");
        env_recd_txt_lab->setStyleSheet("QLabel {color:gray;}");
        back_recd_txt_lab->setStyleSheet("QLabel {color:gray;}");
        test_pos_txt_lab->setStyleSheet("QLabel {color:gray;}");
        break;
    case 2:
        load_data_txt_lab->setStyleSheet("QLabel {color:gray;}");
        env_recd_txt_lab->setStyleSheet("QLabel {color:white;}");
        back_recd_txt_lab->setStyleSheet("QLabel {color:gray;}");
        test_pos_txt_lab->setStyleSheet("QLabel {color:gray;}");
        break;
    case 3:
        load_data_txt_lab->setStyleSheet("QLabel {color:gray;}");
        env_recd_txt_lab->setStyleSheet("QLabel {color:gray;}");
        back_recd_txt_lab->setStyleSheet("QLabel {color:white;}");
        test_pos_txt_lab->setStyleSheet("QLabel {color:gray;}");
        break;
    case 4:
        load_data_txt_lab->setStyleSheet("QLabel {color:gray;}");
        env_recd_txt_lab->setStyleSheet("QLabel {color:gray;}");
        back_recd_txt_lab->setStyleSheet("QLabel {color:gray;}");
        test_pos_txt_lab->setStyleSheet("QLabel {color:white;}");
        break;
    default:
        break;
    }
    emit send_title_val(*key_val);
}

void Menu5::fresh_table(void)
{
    if (key_val->grade.val0 == 5 && !key_val->grade.val1) {
        main_title0->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m12.png);}");
        main_title1->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m22.png);}");
        main_title2->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m32.png);}");
        main_title3->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m42.png);}");
        main_title4->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m52.png);}");
        main_title5->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m61.png);}");
        main_title6->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m72.png);}");
    } else if (key_val->grade.val0 == 5 && key_val->grade.val1) {
        main_title0->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m10.png);}");
        main_title1->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m20.png);}");
        main_title2->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m30.png);}");
        main_title3->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m40.png);}");
        main_title4->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m50.png);}");
        main_title5->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m62.png);}");
        main_title6->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/m70.png);}");
    }
}
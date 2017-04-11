#include "battery.h"

Battery::Battery()
{
    init_battery_power (&battery_power);
}

int Battery::battValue()
{
    check_battery_power (&battery_power);
    return battery_power.power;
}

int Battery::adm1191_conv_init()
{
    return gpio_open (ADM1191_CONV_PIN, "out");
}

int Battery::adm1191_conv_set(int value)
{
    return gpio_set (ADM1191_CONV_PIN, value);
}

int Battery::i2c_adm1191_init(unsigned char addr)
{
    int fd, ret;
    struct i2c_rdwr_ioctl_data i2c_data;
    struct i2c_msg msg;
    unsigned char msg_buf;

    fd = open (I2C_DEV_0, O_RDWR);
    if (fd < 0) {
        printf ("open i2c dev error\n");
        return -1;
    }
    ioctl (fd, I2C_TIMEOUT, 100);
    ioctl (fd, I2C_M_TEN, 0);
    ioctl (fd, I2C_RETRIES, 2);

    msg_buf = 0x01 | 0x04;		/* V_CONT | I_CONT | VRANGE = 26.52V */

    i2c_data.nmsgs = 1;
    i2c_data.msgs = &msg;

    i2c_data.msgs [0].addr = addr;
    i2c_data.msgs [0].flags = 0;			/* write */
    i2c_data.msgs [0].len = 1;
    i2c_data.msgs [0].buf = &msg_buf;

    ret = ioctl (fd, I2C_RDWR, (unsigned long) &i2c_data);

    close (fd);

    return ret;
}

int Battery::i2c_adm1191_read(unsigned char addr, unsigned short *voltage, unsigned short *current)
{
    int fd, ret;
    struct i2c_rdwr_ioctl_data i2c_data;
    struct i2c_msg msg;
    unsigned char msg_buf [3];

    fd = open (I2C_DEV_0, O_RDWR);
    if (fd < 0) {
        printf ("open i2c dev error\n");
        return -1;
    }
    ioctl (fd, I2C_TIMEOUT, 100);
    ioctl (fd, I2C_M_TEN, 0);
    ioctl (fd, I2C_RETRIES, 2);

    i2c_data.nmsgs = 1;
    i2c_data.msgs = &msg;

    i2c_data.msgs [0].addr = addr;
    i2c_data.msgs [0].flags = 1;			/* read */
    i2c_data.msgs [0].len = 3;
    i2c_data.msgs [0].buf = msg_buf;

    ret = ioctl (fd, I2C_RDWR, (unsigned long) &i2c_data);

    close (fd);

    if (ret >= 0) {
        * voltage = (msg_buf [0] << 4) | (msg_buf [2] >> 4);
        * current = (msg_buf [1] << 4) | (msg_buf [2] & 0xf);
    }

    return ret;
}

int Battery::i2c_adm1191_probe(Battery::battery_power_t *bp)
{
    unsigned char addr;
    int ret;

    if (adm1191_conv_init () != 0) {
        printf ("adm1191_conv_init failled\n");
    }

    if (adm1191_conv_set (1) != 0) {
        printf ("adm1191_conv_set failed\n");
    }

    for (addr = ADM1191_START_ADDR; addr <= ADM1191_END_ADDR; addr++) {
        //printf ("probe adm 0x%x...\n", addr);
        if ((ret = i2c_adm1191_init (addr)) >= 0) {
            //printf ("adm1191 probe ok at 0x%x\n", addr);
            break;
        }
        usleep (100000);
    }

    bp->adm1191_addr = addr;

    if (addr > ADM1191_END_ADDR) {
        return -1;
    }
    else {
        return 0;
    }
}

int Battery::conv_vol(unsigned short vol, float *vol_f, int hi)
{
    if (hi) {
        * vol_f = ADM_VOL_FULLSCALE_HI * vol / 4096.0f;
    }
    else {
        * vol_f = ADM_VOL_FULLSCALE_LO * vol / 4096.0f;
    }

    return 0;
}

int Battery::conv_cur(unsigned short cur, float *cur_f)
{
    * cur_f = (ADM_CUR_FULLSCALE * cur) / (4096.0f * ADM_SENSE_RES);

    return 0;
}

int Battery::calc_bat_pwr_percent(Battery::battery_power_t *bp)
{
    int i;

    for (i = 0; i < BAT_PWR_PER_NUM; i++) {
        if (bp->vol > bat_pwr_percent [i]) {
            break;
        }
    }

    bp->power = 100 - i * 10;

    if (bp->power <= BAT_PWR_ALARM_PER) {
        bp->lo_pwr_alarm = 1;
    }
    else {
        bp->lo_pwr_alarm = 0;
    }

    if (bp->power <= BAT_PWR_LOSS_PER) {
        bp->pwr_loss_alarm = 1;
    }
    else {
        bp->pwr_loss_alarm = 0;
    }

    return 0;
}

int Battery::init_battery_power(Battery::battery_power_t *bp)
{
    bp->cur = 400.0f;			/* 400ma */
    bp->vol = 8.2f;				/* 8.2v */
    bp->power = 100;			/* 100% */
    bp->lo_pwr_alarm = 0;		/* 低电压告警 */
    bp->pwr_loss_alarm = 0;

    if (i2c_adm1191_probe (bp) < 0) {
        bp->adm1191_addr = ADM1191_ADDR;
        printf ("failed to probe adm\n");

        return -1;
    }
    else {
        printf ("adm1191 is probed at 0x%x\n", bp->adm1191_addr);

        return 0;
    }
}

int Battery::check_battery_power(Battery::battery_power_t *bp)
{
    unsigned short vol, cur;

    if (i2c_adm1191_read (bp->adm1191_addr, &vol, &cur) >= 0) {
        conv_vol (vol, &bp->vol, 1);
        conv_cur (cur, &bp->cur);
        calc_bat_pwr_percent (bp);
        printf ("vol %1.2f; cur %3.2f; pwr %d; lo pwr %d; loss pwr %d\n",
                bp->vol, bp->cur, bp->power, bp->lo_pwr_alarm, bp->pwr_loss_alarm);
    }

    return 0;
}

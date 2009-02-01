/*
 * stbx25xx_tuner.c - DVB Front-end/tuner driver
 * for digital TV devices equipped with IBM STBx25xx SoC
 *
 * Copyright (C) 2009 Tomasz Figa <tomasz.figa@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stbx25xx_common.h"
#include "stx0288.h"
#include "ix2476.h"
#include <linux/i2c.h>
#include <linux/gpio.h>

static u8 stx0288_inittab[] = {
	0x00, 0x00,
	0x01, 0x15,
	0x02, 0x20,
	0x03, 0x8e,
	0x04, 0x8e,
	0x05, 0x12,
	0x06, 0x00,
	0x07, 0x20,
	0x08, 0x00,
	0x09, 0x00,
	0x0a, 0x04,
	0x0b, 0x00,
	0x0c, 0x00,
	0x0d, 0x00,
	0x0e, 0xc1,
	0x0f, 0x54,
	0x10, 0xf2,
	0x11, 0x7a,
	0x12, 0x03,
	0x13, 0x48,
	0x14, 0x84,
	0x15, 0xc5,
	0x16, 0xb8,
	0x17, 0x9c,
	0x18, 0x00,
	0x19, 0xa6,
	0x1a, 0x88,
	0x1b, 0x8f,
	0x1c, 0xf0,
	0x1e, 0x80,
	0x1f, 0x1a,
	0x20, 0x0b,
	0x21, 0x54,
	0x22, 0xff,
	0x23, 0x01,
	0x24, 0x9a,
	0x25, 0x7f,
	0x26, 0x00,
	0x27, 0x00,
	0x28, 0x46,
	0x29, 0x66,
	0x2a, 0x90,
	0x2b, 0xfa,
	0x2c, 0xd9,
	0x2d, 0x02,
	0x2e, 0xb1,
	0x2f, 0x00,
	0x30, 0x00,
	0x31, 0x1e,
	0x32, 0x14,
	0x33, 0x0f,
	0x34, 0x09,
	0x35, 0x0c,
	0x36, 0x05,
	0x37, 0x2f,
	0x38, 0x16,
	0x39, 0xbd,
	0x3a, 0x00,
	0x3b, 0x12,
	0x3c, 0x01,
	0x3d, 0x30,
	0x3e, 0x00,
	0x3f, 0x00,
	0x40, 0x63,
	0x41, 0x04,
	0x42, 0x60,
	0x43, 0x00,
	0x44, 0x00,
	0x45, 0x00,
	0x46, 0x00,
	0x47, 0x00,
	0x48, 0x00,
	0x49, 0x00,
	0x4a, 0x00,
	0x4b, 0xd1,
	0x4c, 0x33,
	0x4d, 0x00,
	0x4e, 0x00,
	0x4f, 0x00,
	0x50, 0x10,
	0x51, 0x36,
	0x52, 0x21,
	0x53, 0x94,
	0x54, 0xb2,
	0x55, 0x29,
	0x56, 0x64,
	0x57, 0x2b,
	0x58, 0x54,
	0x59, 0x86,
	0x5a, 0x00,
	0x5b, 0x9b,
	0x5c, 0x08,
	0x5d, 0x7f,
	0x5e, 0xff,
	0x5f, 0x8d,
	0x60, 0x82,
	0x61, 0x82,
	0x62, 0x82,
	0x63, 0x02,
	0x64, 0x02,
	0x65, 0x02,
	0x66, 0x82,
	0x67, 0x82,
	0x68, 0x82,
	0x69, 0x82,
	0x6a, 0x38,
	0x6b, 0x0c,
	0x6c, 0x00,
	0x6d, 0x00,
	0x6e, 0x00,
	0x6f, 0x00,
	0x70, 0x00,
	0x71, 0x00,
	0x72, 0x00,
	0x73, 0x00,
	0x74, 0x00,
	0x75, 0x00,
	0x76, 0x00,
	0x77, 0x00,
	0x78, 0x00,
	0x79, 0x00,
	0x7a, 0x00,
	0x7b, 0x00,
	0x7c, 0x00,
	0x7d, 0x00,
	0x7e, 0x00,
	0x7f, 0x00,
	0x80, 0x00,
	0x81, 0x00,
	0x82, 0x3f,
	0x83, 0x3f,
	0x84, 0x00,
	0x85, 0x00,
	0x86, 0x00,
	0x87, 0x00,
	0x88, 0x00,
	0x89, 0x00,
	0x8a, 0x00,
	0x8b, 0x00,
	0x8c, 0x00,
	0x8d, 0x00,
	0x8e, 0x00,
	0x8f, 0x00,
	0x90, 0x00,
	0x91, 0x00,
	0x92, 0x00,
	0x93, 0x00,
	0x94, 0x1c,
	0x95, 0x00,
	0x96, 0x00,
	0x97, 0x00,
	0x98, 0x00,
	0x99, 0x00,
	0x9a, 0x00,
	0x9b, 0x00,
	0x9c, 0x00,
	0x9d, 0x00,
	0x9e, 0x00,
	0x9f, 0x00,
	0xa0, 0x48,
	0xa1, 0x00,
	0xa2, 0x00,
	0xa3, 0x00,
	0xa4, 0x00,
	0xa5, 0x00,
	0xa6, 0x00,
	0xa7, 0x00,
	0xa8, 0x00,
	0xa9, 0x00,
	0xaa, 0x00,
	0xab, 0x00,
	0xac, 0x00,
	0xad, 0x00,
	0xae, 0x00,
	0xaf, 0x00,
	0xb0, 0xb8,
	0xb1, 0x3a,
	0xb2, 0x10,
	0xb3, 0x82,
	0xb4, 0x80,
	0xb5, 0x82,
	0xb6, 0x82,
	0xb7, 0x82,
	0xb8, 0x20,
	0xb9, 0x00,
	0xba, 0x00,
	0xbb, 0x00,
	0xbc, 0x00,
	0xbd, 0x00,
	0xbe, 0x00,
	0xbf, 0x00,
	0xc0, 0x00,
	0xc1, 0x00,
	0xc2, 0x00,
	0xc3, 0x00,
	0xc4, 0x00,
	0xc5, 0x00,
	0xc6, 0x00,
	0xc7, 0x00,
	0xc8, 0x00,
	0xc9, 0x00,
	0xca, 0x00,
	0xcb, 0x00,
	0xcc, 0x00,
	0xcd, 0x00,
	0xce, 0x00,
	0xcf, 0x00,
	0xd0, 0x00,
	0xd1, 0x00,
	0xd2, 0x00,
	0xd3, 0x00,
	0xd4, 0x00,
	0xd5, 0x00,
	0xd6, 0x00,
	0xd7, 0x00,
	0xd8, 0x00,
	0xd9, 0x00,
	0xda, 0x00,
	0xdb, 0x00,
	0xdc, 0x00,
	0xdd, 0x00,
	0xde, 0x00,
	0xdf, 0x00,
	0xe0, 0x00,
	0xe1, 0x00,
	0xe2, 0x00,
	0xe3, 0x00,
	0xe4, 0x1c,
	0xe5, 0x00,
	0xe6, 0x00,
	0xe7, 0x00,
	0xe8, 0x00,
	0xe9, 0x00,
	0xea, 0x00,
	0xeb, 0x00,
	0xec, 0x00,
	0xed, 0x00,
	0xee, 0x00,
	0xef, 0x00,
	0xf0, 0x00,
	0xf1, 0x00,
	0xf2, 0xc0,
	0xf3, 0x00,
	0xf4, 0x00,
	0xf5, 0x00,
	0xf6, 0x00,
	0xf7, 0x00,
	0xf8, 0x00,
	0xf9, 0x00,
	0xfa, 0x00,
	0xfb, 0x00,
	0xfc, 0x00,
	0xfd, 0x00,
	0xfe, 0x00,
	0xff, 0xff,
};

static struct stx0288_config tm9101_config = {
	.demod_address	= 0x68,
	.min_delay_ms 	= 100,
	.inittab	= stx0288_inittab,
};

static int tm9101_set_voltage(struct dvb_frontend *fe, fe_sec_voltage_t voltage)
{
	switch(voltage) {
		case SEC_VOLTAGE_OFF:
			info("SEC_VOLTAGE_OFF");
			gpio_set_value(231, 1);
			msleep(20);
			break;
		case SEC_VOLTAGE_18:
			info("SEC_VOLTAGE_18");
			gpio_set_value(229, 0);
			gpio_set_value(231, 0);
			msleep(20);
			break;
		case SEC_VOLTAGE_13:
			info("SEC_VOLTAGE_13");
			gpio_set_value(229, 1);
			gpio_set_value(231, 0);
			msleep(20);
			break;
	}

	return 0;
}

static void stx0288_reset(void)
{
	gpio_set_value(228, 0);
	
	msleep(2);
	
	gpio_set_value(228, 1);
	
	msleep(20);
}

int stbx25xx_frontend_init(struct stbx25xx_dvb_dev *dvb)
{
	int ret;
	
	gpio_direction_output(228, 1);
	
	stx0288_reset();

	dvb->i2c = i2c_get_adapter(0);
	if(!dvb->i2c) {
		dev_err(dvb->dev, "could not get i2c adapter\n");
		return -ENODEV;
	}
	
	if (!dvb->fe) {
		dvb->fe = dvb_attach(stx0288_attach, &tm9101_config, dvb->i2c);
		if (dvb->fe) {
			dvb->fe->ops.set_voltage = tm9101_set_voltage;
			dvb_attach(ix2476_attach, dvb->fe, 0x60,
					dvb->i2c);
		}
	}

	if (!dvb->fe) {
		dev_err(dvb->dev, "could not attach frontend\n");
		return -ENODEV;
	}

	ret = dvb_register_frontend(&dvb->dvb_adapter, dvb->fe);
	if (ret < 0) {
		if (dvb->fe->ops.release)
			dvb->fe->ops.release(dvb->fe);
		dvb->fe = NULL;
		return ret;
	}
	
	gpio_direction_output(231, 1);
	gpio_direction_output(229, 0);

	return 0;
}

void stbx25xx_frontend_exit(struct stbx25xx_dvb_dev *dvb)
{
	if(dvb->fe)
		dvb_unregister_frontend(dvb->fe);
}

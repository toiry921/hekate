/*
* Copyright (c) 2018 naehrwert
*
* This program is free software; you can redistribute it and/or modify it
* under the terms and conditions of the GNU General Public License,
* version 2, as published by the Free Software Foundation.
*
* This program is distributed in the hope it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "btn.h"
#include "i2c.h"
#include "gpio.h"
#include "t210.h"
#include "util.h"

u32 btn_read()
{
	u32 res = 0;
	if (!gpio_read(GPIO_PORT_X, GPIO_PIN_7))
		res |= BTN_VOL_DOWN;
	if (!gpio_read(GPIO_PORT_X, GPIO_PIN_6))
		res |= BTN_VOL_UP;
	if (i2c_recv_byte(4, 0x3C, 0x15) & 0x4)
		res |= BTN_POWER;
	return res;
}

u32 btn_wait()
{
	u32 res = 0, btn = btn_read();
	int pwr = 0;

	//Power button down, raise a filter.
	if (btn & BTN_POWER)
	{
		pwr = 1;
		btn &= ~BTN_POWER;
	}

	do
	{
		res = btn_read();
		//Power button up, remove filter.
		if (!(res & BTN_POWER) && pwr)
			pwr = 0;
		else if (pwr) //Power button still down.
			res &= ~BTN_POWER;
	} while (btn == res);

	return res;
}

u32 btn_wait_timeout(u32 time_ms)
{
	u32 timeout = get_tmr() + (time_ms * 1000);
	u32 res = btn_read();
	u32 btn = res;

	do
	{
		//Keep the new value until timeout is reached
		if (btn == res)
			res = btn_read();
	} while (get_tmr() < timeout);

	return res;
}

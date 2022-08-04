#include "utc.h"

NmeaTime_t UTCToBeijing(NmeaTime_t time)
{
	uint16_t year = time.year + 2000;
	time.hour += 8;

	if (time.month == 1 || time.month == 3 || time.month == 5 || time.month == 7 || time.month == 8 || time.month == 10 || time.month == 12)//1,3,5,7,8,9,12月每月为31天
	{
		if (time.hour >= 24)
		{
			time.hour -= 24; time.day += 1;//如果超过24小时，减去24小时，后再加上一天
			if (time.day > 31) { time.day -= 31; time.month += 1; }//如果超过31一天，减去31天，后加上一个月
			if (time.month > 12) { time.month -= 12; year++; }//如果超过12月,应该是1月,然后加上一年
		}
	}
	else if (time.month == 4 || time.month == 6 || time.month == 9 || time.month == 11)//4，6，9，11月每月为30天
	{
		if (time.hour >= 24)
		{
			time.hour -= 24; time.day += 1;//如果超过24小时，减去24小时，后再加上一天
			if (time.day > 30) { time.day -= 30; time.month += 1; }//如果超过30一天，减去30天，后加上一个月
		}
	}
	else//剩下为2月，闰年为29天，平年为28天
	{
		if (time.hour >= 24)
		{
			time.hour -= 24; time.day += 1;
			if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))//判断是否为闰年，年号能被400整除或年号能被4整除，而不能被100整除为闰年
			{
				if (time.day > 29) { time.day -= 29; time.month += 1; }
			}//为闰年
			else { if (time.day > 28) { time.day -= 28; time.month += 1; } }//为平年
		}
	}
	time.year = year - 2000;
	return time;
}

NmeaTime_t BeijingToUTC(NmeaTime_t time)
{
	uint16_t year = time.year + 2000;
	if (time.hour >= 8)//如果小时大于等于8点,则减去小时即可
	{
		time.hour -= 8;
	}
	else
	{
		time.hour = time.hour + 24 - 8; time.day -= 1;//如果时间小于8点,则时间是前一天,然后日减一
		if (time.month == 1 || time.month == 2 || time.month == 4 || time.month == 6 || time.month == 8 || time.month == 9 || time.month == 11)//1,2,4,6,8,9,11月改成前一个月是31天
		{
			if (time.day == 0) { time.day = 31; time.month -= 1; }//如果日是0,则应该是上个月最后一天,日改为31,月减一
			if (time.month == 0) { time.month = 12; year--; }//如果月为0,月就是12月,年减一
		}
		else if (time.month == 5 || time.month == 7 || time.month == 10 || time.month == 12)//5,7,10,12上个月是30天
		{
			if (time.day == 0) { time.day = 30; time.month -= 1; }//如果日是0,则应该是上个月最后一天,日改为30,月减一
		}
		else//剩下为3月，上个月闰年为29天，平年为28天
		{
			if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))//判断是否为闰年，年号能被400整除或年号能被4整除，而不能被100整除为闰年
			{//为闰年
				if (time.day == 0)
				{
					time.day = 29;
					time.month -= 1;
				}
			}
			else
			{//为平年
				if (time.day == 0)
				{
					time.day = 28;
					time.month -= 1;
				}
			}
		}
	}
	time.year = year - 2000;
	return time;
}




#include "Watchy_Star_Field.h"

// DARKMODE
RTC_DATA_ATTR bool DARKMODE = false;
// HOUR_SET, change it to switch to 12-hour
RTC_DATA_ATTR bool HOUR_SET = true;
RTC_DATA_ATTR uint16_t FRONT_COLOR = GxEPD_BLACK;
RTC_DATA_ATTR uint16_t BACKG_COLOR = GxEPD_WHITE;

moonPhaser moonP;

void WatchyStarField::handleButtonPress()
{
    if (guiState == WATCHFACE_STATE)
    {
        uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
        if (wakeupBit & UP_BTN_MASK)
        {
            HOUR_SET = !HOUR_SET;
            RTC.read(currentTime);
            showWatchFace(true);
        }
        else if (wakeupBit & DOWN_BTN_MASK)
        {
            HOUR_SET = !HOUR_SET;
            RTC.read(currentTime);
            showWatchFace(true);
        }
        else if (wakeupBit & BACK_BTN_MASK)
        {
            DARKMODE = !DARKMODE;
            FRONT_COLOR = DARKMODE ? GxEPD_WHITE : GxEPD_BLACK;
            BACKG_COLOR = DARKMODE ? GxEPD_BLACK : GxEPD_WHITE;
            RTC.read(currentTime);
            showWatchFace(true);
        }
        else if (wakeupBit & MENU_BTN_MASK)
        {
            Watchy::handleButtonPress();
        }
    }
    else {Watchy::handleButtonPress();}
}

void WatchyStarField::drawWatchFace()
{
    display.fillScreen(BACKG_COLOR);
    display.setTextColor(FRONT_COLOR);
    display.drawBitmap(0, 0, field, 200, 200, FRONT_COLOR);
    drawTime();
    drawDate();
    drawSteps();
    drawBattery();

    display.drawBitmap(118, 168, WIFI_CONFIGURED ? wifi : wifioff, 25, 18, FRONT_COLOR);
    drawMoon();
    drawSun();
}

void WatchyStarField::drawTime()
{
    display.setFont(&DSEG7_Classic_Bold_53);
    display.setCursor(6, 53+5);
    const long ss = currentTime.Hour * 60 + currentTime.Minute;
    int sh = ss / 60;

    if (HOUR_SET == false)
    {
        display.fillRect(7, 60, 25, 9, BACKG_COLOR);
        if (sh >= 12)
        {
            display.drawBitmap(7, 60, pm, 25, 9, FRONT_COLOR);

            if (sh > 12)
            {
                sh -= 12;
            }
        }
        else
        {
            display.drawBitmap(7, 60, am, 25, 9, FRONT_COLOR);
        }
    }
    const int8_t sm = ss % 60;
    const int8_t a = sh >= 10 ? sh / 10 : 0;
    const int8_t b = sh % 10;
    const int8_t c = sm >= 10 ? sm / 10 : 0;
    const int8_t d = sm % 10;

    display.drawBitmap(11,  5, getFdImg(a), 33, 53, FRONT_COLOR);
    display.drawBitmap(55,  5, getFdImg(b), 33, 53, FRONT_COLOR);
    display.drawBitmap(111, 5, getFdImg(c), 33, 53, FRONT_COLOR);
    display.drawBitmap(155, 5, getFdImg(d), 33, 53, FRONT_COLOR);
}

void WatchyStarField::drawDate()
{
    display.setFont(&Seven_Segment10pt7b);

    int16_t  x1, y1;
    uint16_t w, h;

    String dayOfWeek = dayStr(currentTime.Wday);
    dayOfWeek = dayOfWeek.substring(0, dayOfWeek.length() - 3);
    display.getTextBounds(dayOfWeek, 5, 85, &x1, &y1, &w, &h);
    if(currentTime.Wday == 4)
    {
        w = w - 5;
    }
    display.setCursor(76 - w, 86);
    display.println(dayOfWeek);

    String month = monthShortStr(currentTime.Month);
    display.getTextBounds(month, 60, 110, &x1, &y1, &w, &h);
    display.setCursor(79 - w, 110);
    display.println(month);

    const int da = currentTime.Day;
    int ye = currentTime.Year + 1970;

    const int8_t a = da / 10;
    const int8_t b = da % 10;
    const int8_t c = ye / 1000;
    ye = ye % 1000;
    const int8_t d = ye / 100;
    ye = ye % 100;
    const int8_t e = ye / 10;
    const int8_t f = ye % 10;

    display.drawBitmap(8,  95,  getDdImg(a), 16, 25, FRONT_COLOR);
    display.drawBitmap(29, 95,  getDdImg(b), 16, 25, FRONT_COLOR);
    display.drawBitmap(8,  129, getDdImg(c), 16, 25, FRONT_COLOR);
    display.drawBitmap(29, 129, getDdImg(d), 16, 25, FRONT_COLOR);
    display.drawBitmap(50, 129, getDdImg(e), 16, 25, FRONT_COLOR);
    display.drawBitmap(71, 129, getDdImg(f), 16, 25, FRONT_COLOR);
}

void WatchyStarField::drawSteps()
{
    // reset step counter at midnight
    if (currentTime.Hour == 0 && currentTime.Minute == 0)
    {
        sensor.resetStepCounter();
    }
    uint32_t stepCount = sensor.getCounter();

    uint32_t l5 = 61 * stepCount / 10000;

    if (l5 > 61)
    {
        l5 = 61;
    }

    display.fillRect(131, 148, l5, 9, FRONT_COLOR);

    const int8_t a = stepCount / 10000;
    stepCount = stepCount % 10000;
    const int8_t b = stepCount / 1000;
    stepCount = stepCount % 1000;
    const int8_t c = stepCount / 100;
    stepCount = stepCount % 100;
    const int8_t d = stepCount / 10;
    const int8_t e = stepCount % 10;

    display.drawBitmap(8,  165, getDdImg(a), 16, 25, FRONT_COLOR);
    display.drawBitmap(29, 165, getDdImg(b), 16, 25, FRONT_COLOR);
    display.drawBitmap(50, 165, getDdImg(c), 16, 25, FRONT_COLOR);
    display.drawBitmap(71, 165, getDdImg(d), 16, 25, FRONT_COLOR);
    display.drawBitmap(92, 165, getDdImg(e), 16, 25, FRONT_COLOR);
}

void WatchyStarField::drawBattery()
{
    int8_t batteryLevel = 0;
    float VBAT = getBatteryVoltage();
    if(VBAT > 4.1)
    {
        batteryLevel = 37;
    }
    else if(VBAT > 4.05 && VBAT <= 4.1)
    {
        batteryLevel = 33;
    }
    else if(VBAT > 4 && VBAT <= 4.05)
    {
        batteryLevel = 28;
    }
    else if(VBAT > 3.95 && VBAT <= 4)
    {
        batteryLevel = 23;
    }
    else if(VBAT > 3.9 && VBAT <= 3.95)
    {
        batteryLevel = 18;
    }
    else if(VBAT > 3.85 && VBAT <= 3.9)
    {
        batteryLevel = 13;
    }
    else if(VBAT > 3.8 && VBAT <= 3.85)
    {
        batteryLevel = 8;
    }
    else if(VBAT > 3.75 && VBAT <= 3.8)
    {
        batteryLevel = 4;
    }

    display.fillRect(155, 169, batteryLevel, 15, FRONT_COLOR);
}

void WatchyStarField::drawMoon() {
    constexpr int16_t x = 131;
    constexpr int16_t y = 74;
    constexpr int16_t w = 61;
    constexpr int16_t r = w / 2;

    int year = currentTime.Year + 1970;
    int32_t month = currentTime.Month;
    int32_t day = currentTime.Day;
    double hour = currentTime.Hour + 0.1;

    moonData_t moon = moonP.getPhase(year, month, day, hour);

    if (!DARKMODE)
    {
        display.fillCircle(x + r, y + r, r + 1, GxEPD_BLACK);
    }

    display.drawBitmap(x, y, getLunaImg(&moon), w, w, GxEPD_WHITE);
}

void WatchyStarField::drawSun() {
    weatherData currentWeather = getWeatherData();
    int tz = settings.gmtOffset / 3600l + 1; // + 1 for DST - TODO discover this somehow

    int rh = currentWeather.sunrise.Hour + tz;
    int rm = currentWeather.sunrise.Minute;
    int sh = currentWeather.sunset.Hour + tz;
    int sm = currentWeather.sunset.Minute;

    const int sr = rh * 60 + rm;
    const int ss = sh * 60 + sm;

    long k = (currentTime.Hour + tz - 1) * 60 + currentTime.Minute;
    int tk = (k - sr) * 60 / (ss - sr);
    if (k > ss)
        tk = 60;
    else if (k < sr)
        tk = 0;
    display.drawBitmap(110, 132 - tk, arr, 3, 5, FRONT_COLOR);

    // only adjust 12h/24h display after calculating arrow offset
    if (HOUR_SET == false) {
        if (rh > 12)
        {
            rh -= 12;
        }

        if (sh > 12)
        {
            sh -= 12;
        }
    }

    const int8_t a = sh >= 10 ? sh / 10 : 0;
    const int8_t b = sh % 10;
    const int8_t c = sm >= 10 ? sm / 10 : 0;
    const int8_t d = sm % 10;
    const int8_t e = rh >= 10 ? rh / 10 : 0;
    const int8_t f = rh % 10;
    const int8_t g = rm >= 10 ? rm / 10 : 0;
    const int8_t h = rm % 10;

    display.drawBitmap(116, 67,  getNumImg(a), 3, 5, FRONT_COLOR);
    display.drawBitmap(120, 67,  getNumImg(b), 3, 5, FRONT_COLOR);
    display.drawBitmap(128, 67,  getNumImg(c), 3, 5, FRONT_COLOR);
    display.drawBitmap(132, 67,  getNumImg(d), 3, 5, FRONT_COLOR);
    display.drawBitmap(116, 137, getNumImg(e), 3, 5, FRONT_COLOR);
    display.drawBitmap(120, 137, getNumImg(f), 3, 5, FRONT_COLOR);
    display.drawBitmap(128, 137, getNumImg(g), 3, 5, FRONT_COLOR);
    display.drawBitmap(132, 137, getNumImg(h), 3, 5, FRONT_COLOR);
}

const unsigned char *WatchyStarField::getFdImg(int8_t digit)
{
    switch(digit)
    {
        case 1:
            return fd_1;
        case 2:
            return fd_2;
        case 3:
            return fd_3;
        case 4:
            return fd_4;
        case 5:
            return fd_5;
        case 6:
            return fd_6;
        case 7:
            return fd_7;
        case 8:
            return fd_8;
        case 9:
            return fd_9;
        default:
            return fd_0;
    }
}

const unsigned char *WatchyStarField::getDdImg(int8_t digit)
{
    switch(digit)
    {
        case 1:
            return dd_1;
        case 2:
            return dd_2;
        case 3:
            return dd_3;
        case 4:
            return dd_4;
        case 5:
            return dd_5;
        case 6:
            return dd_6;
        case 7:
            return dd_7;
        case 8:
            return dd_8;
        case 9:
            return dd_9;
        default:
            return dd_0;
    }
}

const unsigned char *WatchyStarField::getNumImg(int8_t digit)
{
    switch(digit)
    {
        case 1:
            return num_1;
        case 2:
            return num_2;
        case 3:
            return num_3;
        case 4:
            return num_4;
        case 5:
            return num_5;
        case 6:
            return num_6;
        case 7:
            return num_7;
        case 8:
            return num_8;
        case 9:
            return num_9;
        default:
            return num_0;
    }
}

const unsigned char *WatchyStarField::getLunaImg(const moonData_t *moon)
{
    // Waxing: 0-180
    // Waning: 180-360
    const double percentLit = moon->percentLit;
    if (percentLit < 0.1)
        return luna1;
    else if (percentLit < 0.25)
        return moon->angle <= 180 ? luna12 : luna2;
    else if (percentLit < 0.4)
        return moon->angle <= 180 ? luna11 : luna3;
    else if (percentLit < 0.6)
        return moon->angle <= 180 ? luna10 : luna4;
    else if (percentLit < 0.75)
        return moon->angle <= 180 ? luna9  : luna5;
    else if (percentLit < 0.9)
        return moon->angle <= 180 ? luna8  : luna6;
    else
        return luna7;
}
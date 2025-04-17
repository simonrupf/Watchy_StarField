#ifndef WATCHY_7_SEG_H
#define WATCHY_7_SEG_H
#include <math.h>
#include <Watchy.h>
#include "moonPhaser.h"
#include "Seven_Segment10pt7b.h"
#include "icons.h"

class Watchy7SEG : public Watchy{
    using Watchy::Watchy;
    public:
        void drawWatchFace();
        void drawTime();
        void drawDate();
        void drawSteps();
        void drawBattery();
        void drawMoon();

        void drawSun();
        virtual void handleButtonPress();

        static const unsigned char *getFdImg(int8_t digit);
        static const unsigned char *getDdImg(int8_t digit);
        static const unsigned char *getNumImg(int8_t digit);

};

#endif

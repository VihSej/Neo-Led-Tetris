
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cmath>
#define PIN 2
#define PA 23
#define PB 26
#define PSLIDE A15

Adafruit_NeoMatrix pixels = Adafruit_NeoMatrix(16, 16, 1, 2, PIN,
                                               NEO_TILE_TOP + NEO_TILE_LEFT + NEO_TILE_COLUMNS + NEO_TILE_ZIGZAG +
                                                   NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                                               NEO_GRB + NEO_KHZ800);
unsigned char curBlock[16][8];
unsigned char grid[16][8];
unsigned long curColor = 0xFFFFFF;
bool reachEnd;
bool reachTop;
int curCenterX;
int curCenterY;
int slideState;
unsigned long _lastIncReadTime = micros();
unsigned long _lastDecReadTime = micros();
volatile int counter = 0;
int lastCounter = 0;
int rotateNum = 0;
int slideNum = 0;
int randNum;
int slidePotVal;
bool rowClear = true;
bool isRotValid = false;
int stateIndex = 0;


void setup()
{
    Serial.begin(230400);
    pixels.begin();
    pixels.setBrightness(20);
    reachEnd = false;
    reachTop = false;
    curCenterX = 0;
    curCenterY = 0;
    slideState = 0;

    pinMode(PA, INPUT_PULLUP);
    pinMode(PB, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PA), readDial, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PB), readDial, CHANGE);

    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            curBlock[i][j] = 0;
        }
    }

    newBlock(); // Spawns the First Block
}

void lightUp(unsigned char curBlock[][8], unsigned long color)
{
    for (int j = 0; j < 16; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            if (curBlock[j][i] == 0)
                continue;
            else
            {
                pixels.drawPixel(i * 2, j * 2, color);
                pixels.drawPixel(i * 2 + 1, j * 2, color);
                pixels.drawPixel(i * 2, j * 2 + 1, color);
                pixels.drawPixel(i * 2 + 1, j * 2 + 1, color);
            }
        }
    }
    pixels.show();
}

void falling()
{
    pixels.show();
    for (int j = 15; j >= 0; j--)
    {
        for (int i = 7; i >= 0; i--)
        {
            if (curBlock[j][i] == 0)
                continue;
            else
            {
                curBlock[j + 1][i] = curBlock[j][i];
                pixels.drawPixel(i * 2, j * 2, pixels.Color(0, 0, 0));
                pixels.drawPixel(i * 2 + 1, j * 2, pixels.Color(0, 0, 0));
                pixels.drawPixel(i * 2, j * 2 + 1, pixels.Color(0, 0, 0));
                pixels.drawPixel(i * 2 + 1, j * 2 + 1, pixels.Color(0, 0, 0));
                curBlock[j][i] = 0;
                grid[j + 1][i] = grid[j][i];
                grid[j][i] = 0;
            }
        }
    }
    lightUp(curBlock, curColor);
    for (int j = 15; j >= 0; j--)
    {
        for (int i = 7; i >= 0; i--)
        {
            if (curBlock[j][i] == 0)
            {
                continue;
            }
            else
            {
                if ((grid[j + 1][i] != 0 && curBlock[j + 1][i] == 0) || j == 15)
                {
                    reachEnd = true;
                }
                if (i <= 2 && (grid[j + 1][i] != 0 && curBlock[j + 1][i] == 0))
                    reachTop = true;
            }
        }
    }
    curCenterY++;
    delay(300);
}

void newBlock()
{
    reachEnd = false;
    srand((unsigned int)time(NULL));
    randNum = rand() % 7 + 1;
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            curBlock[i][j] = 0;
        }
    }
    Serial.println(randNum);
    switch (randNum)
    {
    case 1:
        // CYAN BLOCK
        for (int i = 2; i < 6; i++)
        {
            curBlock[0][i] = randNum;
            grid[0][i] = randNum;
        }
        curColor = pixels.Color(0, 255, 220);
        curCenterX = 3;
        curCenterY = 0;
        break;
    case 2:
        // BLUE BLOCK
        curBlock[0][3] = randNum;
        grid[0][3] = randNum;
        for (int i = 3; i < 6; i++)
        {
            curBlock[1][i] = randNum;
            grid[1][i] = randNum;
        }
        curColor = pixels.Color(0, 0, 255);
        curCenterX = 4;
        curCenterY = 1;
        break;
    case 3:
        // ORANGE BLOCK
        curBlock[0][5] = randNum;
        grid[0][5] = randNum;
        for (int i = 3; i < 6; i++)
        {
            curBlock[1][i] = randNum;
            grid[1][i] = randNum;
        }
        curColor = pixels.Color(255, 112, 0);
        curCenterX = 4;
        curCenterY = 1;
        break;
    case 4:
        // YELLOW BLOCK
        curBlock[0][3] = randNum;
        curBlock[0][4] = randNum;
        curBlock[1][3] = randNum;
        curBlock[1][4] = randNum;

        grid[0][3] = randNum;
        grid[0][4] = randNum;
        grid[1][3] = randNum;
        grid[1][4] = randNum;

        curColor = pixels.Color(255, 255, 0);
        curCenterX = 3;
        curCenterY = 1;
        break;
    case 5:
        // GREEN BLOCK
        curBlock[1][3] = randNum;
        curBlock[1][4] = randNum;
        curBlock[0][4] = randNum;
        curBlock[0][5] = randNum;

        grid[1][3] = randNum;
        grid[1][4] = randNum;
        grid[0][4] = randNum;
        grid[0][5] = randNum;

        curColor = pixels.Color(0, 255, 0);
        curCenterX = 4;
        curCenterY = 1;
        break;
    case 6:
        // PURPLE BLOCK
        curBlock[0][4] = randNum;
        curBlock[1][3] = randNum;
        curBlock[1][4] = randNum;
        curBlock[1][5] = randNum;

        grid[0][4] = randNum;
        grid[1][3] = randNum;
        grid[1][4] = randNum;
        grid[1][5] = randNum;

        curColor = pixels.Color(128, 0, 128);
        curCenterX = 4;
        curCenterY = 1;
        break;
    case 7:
        // RED BLOCK
        curBlock[0][3] = randNum;
        curBlock[0][4] = randNum;
        curBlock[1][4] = randNum;
        curBlock[1][5] = randNum;

        grid[0][3] = randNum;
        grid[0][4] = randNum;
        grid[1][4] = randNum;
        grid[1][5] = randNum;

        curColor = pixels.Color(255, 0, 0);
        curCenterX = 4;
        curCenterY = 1;
        break;
    }
    lightUp(curBlock, curColor);
}

void displayGameOver()
{
    pixels.setBrightness(30);
    pixels.fillScreen(pixels.Color(70, 70, 70));
    pixels.setCursor(3, 1);
    pixels.setTextColor(pixels.Color(255, 0, 0));
    pixels.setTextSize(1);
    pixels.print("OVER");

    for (int j = 8; j < 16; j++)
    {
        for (int i = 0; i < 16; i++)
        {
            pixels.drawPixel(i, j, pixels.Color(70, 70, 70));
        }
    }

    pixels.drawPixel(3, 9, pixels.Color(255, 0, 0));
    pixels.drawPixel(4, 9, pixels.Color(255, 0, 0));
    pixels.drawPixel(5, 9, pixels.Color(255, 0, 0));
    pixels.drawPixel(6, 9, pixels.Color(255, 0, 0));
    pixels.drawPixel(7, 9, pixels.Color(255, 0, 0));
    pixels.drawPixel(9, 9, pixels.Color(255, 0, 0));
    pixels.drawPixel(10, 9, pixels.Color(255, 0, 0));
    pixels.drawPixel(11, 9, pixels.Color(255, 0, 0));
    pixels.drawPixel(12, 9, pixels.Color(255, 0, 0));
    pixels.drawPixel(12, 9, pixels.Color(255, 0, 0));

    pixels.drawPixel(3, 10, pixels.Color(255, 0, 0));
    pixels.drawPixel(9, 10, pixels.Color(255, 0, 0));
    pixels.drawPixel(13, 10, pixels.Color(255, 0, 0));

    pixels.drawPixel(3, 11, pixels.Color(255, 0, 0));
    pixels.drawPixel(9, 11, pixels.Color(255, 0, 0));
    pixels.drawPixel(13, 11, pixels.Color(255, 0, 0));

    pixels.drawPixel(3, 12, pixels.Color(255, 0, 0));
    pixels.drawPixel(4, 12, pixels.Color(255, 0, 0));
    pixels.drawPixel(5, 12, pixels.Color(255, 0, 0));
    pixels.drawPixel(6, 12, pixels.Color(255, 0, 0));
    pixels.drawPixel(7, 12, pixels.Color(255, 0, 0));
    pixels.drawPixel(9, 12, pixels.Color(255, 0, 0));
    pixels.drawPixel(10, 12, pixels.Color(255, 0, 0));
    pixels.drawPixel(11, 12, pixels.Color(255, 0, 0));
    pixels.drawPixel(12, 12, pixels.Color(255, 0, 0));
    pixels.drawPixel(12, 12, pixels.Color(255, 0, 0));

    pixels.drawPixel(3, 13, pixels.Color(255, 0, 0));
    pixels.drawPixel(9, 13, pixels.Color(255, 0, 0));
    pixels.drawPixel(11, 13, pixels.Color(255, 0, 0));

    pixels.drawPixel(3, 14, pixels.Color(255, 0, 0));
    pixels.drawPixel(9, 14, pixels.Color(255, 0, 0));
    pixels.drawPixel(12, 14, pixels.Color(255, 0, 0));

    pixels.drawPixel(3, 15, pixels.Color(255, 0, 0));
    pixels.drawPixel(4, 15, pixels.Color(255, 0, 0));
    pixels.drawPixel(5, 15, pixels.Color(255, 0, 0));
    pixels.drawPixel(6, 15, pixels.Color(255, 0, 0));
    pixels.drawPixel(7, 15, pixels.Color(255, 0, 0));
    pixels.drawPixel(9, 15, pixels.Color(255, 0, 0));
    pixels.drawPixel(13, 15, pixels.Color(255, 0, 0));

    pixels.show();
}

void cyanStates(int state, int x, int y)
{
    switch (state)
    {
    case 1:

        break;
    }
}

void loop()
{
    if (counter != lastCounter)
    {
        if (counter > lastCounter)
            rotateNum = 1;
        if (counter < lastCounter)
            rotateNum = -1;
        lastCounter = counter;
    }
    else
    {
        rotateNum = 0;
    }

    if (reachTop)
    {
        displayGameOver();
        return;
    }

    if (rotateNum != 0)
    {
        if (rotateNum == 1)
        {
            rotateRight();
        }
        else
            rotateLeft();
    }

    slidePotVal = round((7 * analogRead(PSLIDE)) / 4095);
    if (slidePotVal != slideState)
    {
        if (slidePotVal > slideState)
        {
            for (int i = 0; i < (slidePotVal - slideState); i++)
            {
                rightShift();
            }
        }
        else
        {
            for (int i = 0; i < (slideState - slidePotVal); i++)
            {
                leftShift();
            }
        }
        slideState = slidePotVal;
    }

    if (!reachEnd)
    {
        falling();
    }
    else
    {
        checkRowClear();
        newBlock();
    }
}

void checkRowClear()
{

    for (int j = 15; j >= 0; j--)
    {
        rowClear = true;
        for (int i = 7; i >= 0; i--)
        {
            if (grid[j][i] == 0)
                rowClear = false;
        }
        if (rowClear)
        {
            for (int i = 0; i < 8; i++)
            {
                pixels.drawPixel(i * 2, j * 2, pixels.Color(255, 255, 255));
                pixels.drawPixel(i * 2 + 1, j * 2, pixels.Color(255, 255, 255));
                pixels.drawPixel(i * 2, j * 2 + 1, pixels.Color(255, 255, 255));
                pixels.drawPixel(i * 2 + 1, j * 2 + 1, pixels.Color(255, 255, 255));
            }
            pixels.show();
            delay(300);
            for (int i = 0; i < 8; i++)
            {
                pixels.drawPixel(i * 2, j * 2, 0);
                pixels.drawPixel(i * 2 + 1, j * 2, 0);
                pixels.drawPixel(i * 2, j * 2 + 1, 0);
                pixels.drawPixel(i * 2 + 1, j * 2 + 1, 0);
            }
            pixels.show();
            delay(300);
            fallDown(j);
        }
    }
}

void fallDown(int y)
{
    for (int j = y; j > 0; j--)
    {
        for (int i = 0; i < 8; i++)
        {
            int temp = grid[j - 1][i];
            grid[j][i] = temp;
            unsigned long colorUsed = giveColor(temp);
            pixels.drawPixel(i * 2, j * 2, colorUsed);
            pixels.drawPixel(i * 2 + 1, j * 2, colorUsed);
            pixels.drawPixel(i * 2, j * 2 + 1, colorUsed);
            pixels.drawPixel(i * 2 + 1, j * 2 + 1, colorUsed);
        }
    }
    for (int i = 0; i < 8; i++)
    {
        grid[0][i] = 0;
        pixels.drawPixel(i * 2, 0, 0);
        pixels.drawPixel(i * 2 + 1, 0, 0);
        pixels.drawPixel(i * 2, 1, 0);
        pixels.drawPixel(i * 2 + 1, 1, 0);
    }
    pixels.show();
}

unsigned long giveColor(unsigned char c)
{
    switch (c)
    {
    case 0:
        return 0;
    case 1:
        return (pixels.Color(0, 255, 220));
    case 2:
        return (pixels.Color(0, 0, 255));
    case 3:
        return (pixels.Color(255, 112, 0));
    case 4:
        return (pixels.Color(255, 255, 0));
    case 5:
        return (pixels.Color(0, 255, 0));
    case 6:
        return (pixels.Color(128, 0, 128));
    case 7:
        return (pixels.Color(255, 0, 0));
    }
}

void rotateLeft()
{
    int transX = 0;
    int transY = 0;
    int rotatedX = 0;
    int rotatedY = 0;
    int newX = 0;
    int newY = 0;

    for (int j = 0; j < 15; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            if (curBlock[j][i] != 0)
            {
                transX = i - curCenterX;
                transY = -1 * (j - curCenterY);

                rotatedX = transY;
                rotatedY = -1 * transX;

                newX = rotatedX + curCenterX;
                newY = -1 * (rotatedY + curCenterY);
                if ((newX >= 0 && newX < 8) && (newY >= 0 && newY < 16))
                {
                    curBlock[j][i] = 0;
                    pixels.drawPixel(i * 2, j * 2, 0);
                    pixels.drawPixel(i * 2 + 1, j * 2, 0);
                    pixels.drawPixel(i * 2, j * 2 + 1, 0);
                    pixels.drawPixel(i * 2 + 1, j * 2 + 1, 0);

                    curBlock[newY][newX] = curColor;
                    pixels.drawPixel(newX * 2, newY * 2, curColor);
                    pixels.drawPixel(newX * 2 + 1, newY * 2, curColor);
                    pixels.drawPixel(newX * 2, newY * 2 + 1, curColor);
                    pixels.drawPixel(newX * 2 + 1, newY * 2 + 1, curColor);
                }
            }
        }
    }
}

void rotateRight()
{
    int transX = 0;
    int transY = 0;
    int rotatedX = 0;
    int rotatedY = 0;
    int newX = 0;
    int newY = 0;
    int xStor[4];
    int yStor[4];
    int index = 0;

    switch (randNum)
    {
    case 1:
    }
    for (int j = 0; j < 15; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            if (curBlock[j][i] != 0)
            {
                transX = i - curCenterX;
                transY = j - curCenterY;

                rotatedX = -1 * transY;
                rotatedY = transX;

                newX = rotatedX + curCenterX;
                newY = rotatedY + curCenterY;
                xStor[index] = newX;
                yStor[index] = newY;
                index++;
                if ((newX >= 0 && newX < 8) && (newY >= 0 && newY < 16))
                {
                    curBlock[j][i] = 0;
                    pixels.drawPixel(i * 2, j * 2, 0);
                    pixels.drawPixel(i * 2 + 1, j * 2, 0);
                    pixels.drawPixel(i * 2, j * 2 + 1, 0);
                    pixels.drawPixel(i * 2 + 1, j * 2 + 1, 0);
                }
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        curBlock[yStor[i]][xStor[i]] = curColor;
        pixels.drawPixel(xStor[i] * 2, yStor[i] * 2, curColor);
        pixels.drawPixel(xStor[i] * 2 + 1, yStor[i] * 2, curColor);
        pixels.drawPixel(xStor[i] * 2, yStor[i] * 2 + 1, curColor);
        pixels.drawPixel(xStor[i] * 2 + 1, yStor[i] * 2 + 1, curColor);
    }
}

void leftShift()
{
    for (int j = 0; j < 16; j++)
    {
        if (curBlock[j][0] != 0)
            return;
    }
    for (int j = 0; j < 16; j++)
    {
        for (int i = 1; i < 8; i++)
        {
            if ((curBlock[j][i] != 0 && curBlock[j][i - 1] == 0) && grid[j][i - 1] != 0)
                return;
        }
    }
    for (int j = 0; j < 16; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            if (curBlock[j][i] != 0)
            {
                curBlock[j][i - 1] = curBlock[j][i];
                grid[j][i - 1] = grid[j][i];
                curBlock[j][i] = 0;
                grid[j][i] = 0;
                pixels.drawPixel(i * 2, j * 2, 0);
                pixels.drawPixel(i * 2 + 1, j * 2, 0);
                pixels.drawPixel(i * 2, j * 2 + 1, 0);
                pixels.drawPixel(i * 2 + 1, j * 2 + 1, 0);

                pixels.drawPixel((i - 1) * 2, j * 2, curColor);
                pixels.drawPixel((i - 1) * 2 + 1, j * 2, curColor);
                pixels.drawPixel((i - 1) * 2, j * 2 + 1, curColor);
                pixels.drawPixel((i - 1) * 2 + 1, j * 2 + 1, curColor);
            }
        }
    }
    curCenterX--;
}

void rightShift()
{
    for (int j = 0; j < 16; j++)
    {
        if (curBlock[j][7] != 0)
            return;
    }
    for (int j = 0; j < 16; j++)
    {
        for (int i = 0; i < 7; i++)
        {
            if ((curBlock[j][i] != 0 && curBlock[j][i + 1] == 0) && grid[j][i + 1] != 0)
                return;
        }
    }
    for (int j = 0; j < 16; j++)
    {
        for (int i = 7; i >= 0; i--)
        {
            if (curBlock[j][i] != 0)
            {
                curBlock[j][i + 1] = curBlock[j][i];
                grid[j][i + 1] = grid[j][i];
                curBlock[j][i] = 0;
                grid[j][i] = 0;
                pixels.drawPixel(i * 2, j * 2, 0);
                pixels.drawPixel(i * 2 + 1, j * 2, 0);
                pixels.drawPixel(i * 2, j * 2 + 1, 0);
                pixels.drawPixel(i * 2 + 1, j * 2 + 1, 0);

                pixels.drawPixel((i + 1) * 2, j * 2, curColor);
                pixels.drawPixel((i + 1) * 2 + 1, j * 2, curColor);
                pixels.drawPixel((i + 1) * 2, j * 2 + 1, curColor);
                pixels.drawPixel((i + 1) * 2 + 1, j * 2 + 1, curColor);
            }
        }
    }
    curCenterX++;
}

void readDial()
{
    static uint8_t x = 3;
    static int8_t y = 0;
    static const int8_t s[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
    x <<= 2;
    if (digitalRead(PA))
        x |= 0x02;
    if (digitalRead(PB))
        x |= 0x01;
    y += s[(x & 0x0f)];
    if (y > 3)
    {
        counter = counter + 1;
        y = 0;
    }
    else if (y < -3)
    {
        counter = counter - 1;
        y = 0;
    }
}

// EXTRA CODE

// #define BUTTON0 34
// #define BUTTON1 0
// pinMode(BUTTON0, INPUT_PULLUP);
// pinMode(BUTTON1, INPUT_PULLUP);
// attachInterrupt(BUTTON0, click0, FALLING);
// attachInterrupt(BUTTON1, click1, FALLING);

// volatile int x = 0;
// volatile int y = 0;
// unsigned long lastclick_time0 = 0;
// unsigned long lastclick_time1 = 0;

// void IRAM_ATTR click0() { // ISR
//   if (millis() > lastclick_time0 + 100) {
//     pixels.drawPixel(x, y, pixels.Color(0, 0, 0));
//     x++; x=x%16;
//     lastclick_time0 = millis();
//   }
// }

// void IRAM_ATTR click1() { // ISR
//   if (millis() > lastclick_time1 + 100) {
//     pixels.drawPixel(x, y, pixels.Color(0, 0, 0));
//     y++; y=y%32;
//     lastclick_time1 = millis();
//   }
// }

//  for (int y = 0; y < 32; y++) {
//    for (int x = 0; x < 16; x++) {
//      pixels.drawPixel(x, y, pixels.Color(255, 255, 255));
//      pixels.show();
//      delay(50);
//      pixels.drawPixel(x, y, pixels.Color(0, 0, 0));
//    }
//  }

//    newBlock();
//    for (int i = 0; i < 14; i++) {
//      falling();
//      }

//  pixels.drawPixel(x, y, pixels.Color(255, 255, 255));
//  pixels.show();
//  Serial.print(x); Serial.print(", "); Serial.println(y);

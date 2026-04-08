#include <Arduino.h>
#include <esp_sleep.h>
#include <SD_MMC.h>
#include <FS.h>

#define pause 34
#define go 35
#define back 32 // pullup
#define next 33 // pullup
#define voldown 36
#define volup 39
  //
#define battery null
#define charging null

#define null 0
#define longPress 3000
  //
#define startScr 0
#define playScr 1
#define settScr 2
#define errScr 3

bool btnValue[6];   // необработанные данные, есть сигнал со всех кнопок, не учитывая дребезг
volatile uint8_t btnData[3]; // обработанные данные, какая кнопка(1/2 кнопки) нажаты
volatile uint8_t charge;
volatile float voltage;
volatile uint8_t currDisp = 0;
volatile bool isCharging;
volatile bool isSD;

bool isbPause = false;
bool isbGo = false;
bool isbBack = false;
bool isbNext = false;
bool isbVolup = false;
bool isbVoldown = false;

File root;
File musicFile;

void setup() {
    // пины со встроенной подтяжкой (резисторы не нужны)
    pinMode(back, INPUT_PULLUP);
    pinMode(next, INPUT_PULLUP);

    // пины БЕЗ встроенной подтяжки (нужны внешние резисторы 10кОм к 3.3V)
    pinMode(pause, INPUT); 
    pinMode(go, INPUT);
    pinMode(voldown, INPUT); // VP
    pinMode(volup, INPUT);   // VN

    pinMode(battery, INPUT);
    pinMode(charging, INPUT);

      //

    esp_sleep_enable_ext0_wakeup((gpio_num_t)go, 0);

      //

    firstBattGet();
    checkCharging();
    if (charge < 2 && !isCharging) { esp_deep_sleep_start(); }
    
    currDisp = startScr;

    uint8_t cardType = SD_MMC.cardType();
    if ((cardType == CARD_NONE) || (!SD_MMC.begin("/sdcard", true))) { 
        isSD = false; 
        currDisp = errScr;
        errDisp();
        delay(3000);
        esp_deep_sleep_start();
    }
    else { isSD = true; }


}
    
    /////////////////////////

void checkCharging() {
    uint32_t rawSum = 0;
    for (uint8_t i = 0; i < 5; i++) {
        rawSum = rawSum + analogRead(charging);
        delay(5);
    }
    if (rawSum / 5 > 1000) { isCharging = true; }
    else { isCharging = false; firstBattGet(); }

    delay(2000);
}

void firstBattGet() {
    float voltgSum = 0;
    for (uint8_t i = 0; i < 20; i++) {
        int raw = analogRead(battery);
        voltgSum = voltgSum + (raw / 4095.0) * 3.3 * 2.0 * 1.1;
        delay(10);
    }
    voltage = voltgSum / 20;
    
    if (voltage >= 4.2) { charge = 100; }
    else if (voltage <= 3.3) { charge = 0; }
    else { charge = (voltage - 3.3) / (4.2 - 3.3) * 100; }

}

void battGet() {
    float voltgSum = 0;
    uint8_t chrgtmp;
    for (uint8_t i = 0; i < 10; i++) {
        int raw = analogRead(battery);
        voltgSum = voltgSum + (raw / 4095.0) * 3.3 * 2.0 * 1.1;
        delay(5);
    }
    voltage = voltgSum / 10;
    
    if (voltage >= 4.2) { chrgtmp = 100; }
    else if (voltage <= 3.3) { chrgtmp = 0; }
    else { chrgtmp = (voltage - 3.3) / (4.2 - 3.3) * 100; }

    float smothChrg = (charge * 0.9) + (chrgtmp * 0.1);
    uint8_t newchrg = (uint8_t)smothChrg;

    if ((isCharging && charge < newchrg) || (!isCharging && charge > newchrg)) { charge = newchrg; }
    
    delay(20000);
}

void actPlayerBtn(uint8_t btn, uint8_t btn2, bool isLong) {
    if (btn == pause && btn2 == null) { /* play / pause */}
    else if (btn == go && btn2 == null && !isLong) { /* next frame */ }
    else if (btn == back && btn2 == null && !isLong) { /* back */ }
    else if (btn == next && btn2 == null && !isLong) { /* next */ }
    else if (btn == voldown && btn2 == null && !isLong) { /* volume down */ }
    else if (btn == volup && btn2 == null && !isLong) { /* volume up */ }
    else if (btn == go && btn2 == null && isLong) { /* light sleep */ }
    else if (((btn == go && btn2 == pause) || (btn == pause && btn2 == go)) && isLong) { /* deep sleep */ }
}

void actSettBtn(uint8_t btn, uint8_t btn2, bool isLong) {
    if (btn == pause && btn2 == null) { /* click */}
    else if (btn == go && btn2 == null && !isLong) { /* next frame */ }
    else if ((btn == back && btn2 == null && !isLong) || (btn == voldown && btn2 == null && !isLong)) { /* down */ }
    else if ((btn == next && btn2 == null && !isLong) || (btn == volup && btn2 == null && !isLong)) { /* up */ }
    else if (btn == go && btn2 == null && isLong) { /* light sleep */ }
    else if (((btn == go && btn2 == pause) || (btn == pause && btn2 == go)) && isLong) { /* deep sleep */ }
}

void btnRead() {
    bool butPause = !digitalRead(pause);
    bool butGo = !digitalRead(go);
    bool butBack = !digitalRead(back);
    bool butNext = !digitalRead(next);
    bool butVolup = !digitalRead(volup);
    bool butVoldown = !digitalRead(voldown);
    bool butAll = (butPause || butBack || butNext || butVolup || butVoldown || butGo);

    btnValue[0] = butPause;
    btnValue[1] = butGo;
    btnValue[2] = butBack;
    btnValue[3] = butNext;
    btnValue[4] = butVolup;
    btnValue[5] = butVoldown;
}

void btnDecode() {
    uint8_t btn = null;
    uint8_t btn2 = null;
    for (uint8_t i = 0; i < 6; i++) {
        if (btnValue[i]) {
            if (btn == 0) {
                switch(i) {
                    case 0: btn = pause; break;
                    case 1: btn = go; break;
                    case 2: btn = back; break;
                    case 3: btn = next; break;
                    case 4: btn = volup; break;
                    case 5: btn = voldown; break;
                }
            }
            else {
                switch(i) {
                    case 0: btn2 = pause; break;
                    case 1: btn2 = go; break;
                    case 2: btn2 = back; break;
                    case 3: btn2 = next; break;
                    case 4: btn2 = volup; break;
                    case 5: btn2 = voldown; break;
                }
            }
        }
    }
    btnData[0] = btn;
    btnData[1] = btn2;
}

void btnGet() {
    btnRead();
    uint8_t btnSum = (btnValue[0] + btnValue[1] + btnValue[2] + btnValue[3] + btnValue[4] + btnValue[5]);
    if ((btnSum == 0) || ((btnSum != 1) && (btnSum != 2))) {
        btnData[0] = 0;
        btnData[1] = 0;
        btnData[2] = 0;
        return;
    }
    uint8_t btn = null;
    uint8_t btn2 = null;
    bool isPress = true;
    bool isLong = false;
    uint8_t btnNewSum;
    
    btnDecode();

    unsigned long startTime = millis(); // засекаем время начала нажатия
    unsigned long lastSeenTime = millis();
    delay(50); // отсекаем дребезг

    while (isPress) {
        btnRead();

        // eсли нажато 2 кнопки, проверяем, чтобы хотя бы одна осталась
        btnNewSum = (btnValue[0] + btnValue[1] + btnValue[2] + btnValue[3] + btnValue[4] + btnValue[5]);

        if (btnNewSum == btnSum) { lastSeenTime = millis(); }
        else if ((btnNewSum == 0) && (millis() - lastSeenTime > 50)) {
            isPress = false; isLong = false;
        }
        else if ((btnNewSum < btnSum) && (millis() - lastSeenTime > 50)) {
            btnSum = btnNewSum;
            startTime = millis(); // Сброс основного таймера (3 сек)
            lastSeenTime = startTime;
            btnDecode();
            btn = btnData[0];
            btn2 = btnData[1];
        }
        else if ((btnNewSum > btnSum) && (millis() - lastSeenTime > 50)) {
            btnSum = btnNewSum;
            btnDecode();
            btn = btnData[0];
            btn2 = btnData[1];
            startTime = millis();
        }
        
        // проверка на "Долгое нажатие" прямо в процессе удержания(если прошло 3 сек с начала)
        if (millis() - startTime > longPress) { isPress = false; isLong = true; }

        delay(10);
    }

    btnData[0] = btn;
    btnData[1] = btn2;
    btnData[2] = isLong;
}

void errDisp() {
    if (!isSD) {
        // Error! SD cant be mounted or SD is missing.
    }
}

void loop() {
    
}
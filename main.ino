#define pause 32
#define go 33
#define back 34
#define next 35
#define volup 36
#define voldown 39

#define null 0
#define longPress 3000

bool btnValue[6];   // необработанные данные, есть сигнал со всех кнопок, не учитывая дребезг
uint8_t btnData[3]; // обработанные данные, какая кнопка(1/2 кнопки) нажаты

void setup() {
    // пины со встроенной подтяжкой (резисторы не нужны)
    pinMode(pause, INPUT_PULLUP);
    pinMode(go, INPUT_PULLUP);

    // пины БЕЗ встроенной подтяжки (нужны внешние резисторы 10кОм к 3.3V)
    pinMode(volup, INPUT); 
    pinMode(voldown, INPUT);
    pinMode(back, INPUT); // VP
    pinMode(next, INPUT); // VN
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
    for (uint8_t i = 0; i < 7; i++) {
        if (btnValue[i]) {
            if (btn == 0) {
                switch(i) {
                    case 1: btn = pause; break;
                    case 2: btn = go; break;
                    case 3: btn = back; break;
                    case 4: btn = next; break;
                    case 5: btn = volup; break;
                    case 6: btn = voldown; break;
                }
            }
            else {
                switch(i) {
                    case 1: btn2 = pause; break;
                    case 2: btn2 = go; break;
                    case 3: btn2 = back; break;
                    case 4: btn2 = next; break;
                    case 5: btn2 = volup; break;
                    case 6: btn2 = voldown; break;
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

void loop() {
    
}
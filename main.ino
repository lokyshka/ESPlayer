#define pause 32
#define go 33
#define volup 34
#define voldown 35
#define back 36
#define next 39

#define null 0
#define longPress 3000

void setup() {
    // Пины со встроенной подтяжкой (резисторы не нужны)
    pinMode(pause, INPUT_PULLUP);
    pinMode(go, INPUT_PULLUP);

    // Пины БЕЗ встроенной подтяжки (нужны внешние резисторы 10кОм к 3.3V)
    pinMode(volup, INPUT); 
    pinMode(voldown, INPUT);
    pinMode(back, INPUT); // VP
    pinMode(next, INPUT); // VN
}

void actionBtn(uint8_t btn, uint8_t btn2, bool isLong) {
    if (btn == pause && btn2 == null) { /* play / pause */}
    else if (btn == go && btn2 == null && !isLong) { /* next frame */ }
    else if (btn == back && btn2 == null && !isLong) { /* back */ }
    else if (btn == next && btn2 == null && !isLong) { /* next */ }
    else if (btn == volup && btn2 == null && !isLong) { /* volume up */ }
    else if (btn == voldown && btn2 == null && !isLong) { /* volume down */ }
    else if (btn == go && btn2 == null && isLong) { /* light sleep */ }
    else if (((btn == go && btn2 == pause) || (btn == pause && btn2 == go)) && isLong) { /* deep sleep */ }
}

void loop() {

}
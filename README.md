# ESPlayer
ESPlayer — DIY mp3-player project on ESP32. At the moment, the project is under development: hardware and software thinking. To create such a player, you need:
  1. ESP32-WROVER(in my version, we need about 8 megabytes of RAM, but you can edit code and use ESP32 with less RAM(with extended RAM!))
  2. battery(i advise to take a 2000 mAh or better battery)
  3. 6 buttons
  4. charging card
  5. SD-cardreader(preferably without Logic Level Shifter)
  6. TFT-display(1.8 inches, 160px × 128px) (maybe!)
  7. DAC-decoder with AUX
  8. body for all of this
  9. some wires, resistors(10kOm, 100kOm...), few transistors and diodes...
  10. ...(please wait updates)

I use this set:
1. non-official MagSafe Battery body(96mm × 64mm × 12mm)
2. ESP32-WROVER(without leads, 8mb RAM) — 650rub — ozon.ru/t/sSymcpe
3. 6 buttons — 130rub — ozon.ru/t/OLwPGph (link to product in the amount of 10)
4. usb-c charging card — 85rub — ozon.ru/t/hogITDG
5. DAC-decoder PCM5102(with aux) — 170rub — ozon.ru/t/q4iR9TL (delivery not from Russia)
6. SD-cardreader — 250rub — ozon.ru/t/p5RyK07
7. TFT-display — 350rub(240rub + 110rub delivery) — aliexpress.ru/item/1005008045403404.html?sku_id=12000043411859573&spm=a2g2w.productlist.search_results.4.70a01942yqLHBt (maybe!)
8. 2000mah battery(maybe!)

Now, we have used some libraries(this list will be updated in future):
1. esp_sleep -- library in ESP32-core
2. SDMMC -- library in ESP32-core
3. FS -- library in ESP32-core

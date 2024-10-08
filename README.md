# LolinS3MiniPRO

https://www.wemos.cc/en/latest/s3/s3_mini_pro.html

Ukázkový kód pro ArduinoIDE.

Přečte data ze senzoru QMI8658 (gyroskop, akcelerometr, teploměr), stavy tlačítek a podle toho vypisuje infromace na displej GC9A01A. Podle stavu tlačítek se nastaví příslušná barva pro RGB LED i pro text na displeji.

*Zatím chybí IR.*

Použité knihovny 3. stran:

RGB LED
https://github.com/adafruit/Adafruit_NeoPixel

QMI8658
https://github.com/lewisxhe/SensorLib

TFT Displej 
https://github.com/adafruit/Adafruit_GC9A01A

V souboru *Adafruit_GC9A01A.h* je třeba změnit rozlišení displeje na 132x132. Displej je ve skutečnosti 128x128, ale na mých dvou kusech je nějaký "posunutý" a mám pak okolo 4px barevný rámeček. Navíc pak všechna grafika musí být posunuta o 4 px, takže je třeba udělat *tft.SetCursor(4, 4)* jinak se vykresluje částečně "za rohem"
```
#define GC9A01A_TFTWIDTH 132  ///< Display width in pixels
#define GC9A01A_TFTHEIGHT 132 ///< Display height in pixels
```

Pokud se chcete na displej dívat tak, že budou pod ním tlačítka (jako na obrázcích níže), je nutné v souboru *Adafruit_GC9A01A.cpp* najít v metodě *setRotation* na řádku 229 tento kód
```
m = (MADCTL_MX | MADCTL_BGR);
```

a nahradit ho tímto, **jinak bude text přezrcadlený**

```   
m = (MADCTL_BGR);
```


![image](https://github.com/user-attachments/assets/0afe9842-d2e0-41c2-bb28-8c5e1fc40fd2)

![image](https://github.com/user-attachments/assets/e4630cda-2c9a-41d4-8066-4f3337769cc2)












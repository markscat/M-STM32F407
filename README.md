# M-STM32F407
我在ＳＴＭ３２Ｆ４０７上開發的程式……好吧！還有Deepseek、chargpt和Gemini

2025/10/10

 目前的進度：
 - 板上LED的操作
 - printf和scanf的重寫
 - TCA555,I2C擴充IO的寫入;但讀取有待測試
 - TM1638的寫入;讀取有待測試
 - 完成EEPROM AT24C32以及AT24C02的讀寫
 - 完成OLED 螢幕開啟以及關閉的功能
 - 完成ADS1115程式應用
 
  #IO setup:<br/>
 I2C1 GPIO Configuration<br/>
  PB6     ------> I2C1_SCL<br/>
 	PB7     ------> I2C1_SDA<br/>
 USART2 GPIO Configuration<br/>
 	PA2     ------> USART2_TX<br/>
 	PA3     ------> USART2_RX<br/>
 TM1638 GPIO Configuration<br/>
 	PC4     ------> SEG_CLK<br/>
 	PC5     ------> SEG_DIO<br/>
 	PB0     ------> SEG_STB<br/>
 
 SPI GPIO Configuration<br/>
   VCC
   PE10 	------> SPI1_CS_Pin<br/>
   PA7		------> SPI1_MOSI_Pin<br/>
   PA5		------> SPI1_SCK_Pin<br/>
   PA6		------> SPI1_MISO_Pin<br/>
   
 SPI Panel ili9225
   VCC
   GND
   GND
   NC
   NC
   NC
   PA5		------> SPI1_SCK_Pin<br/>
   PA6		------> SPI1_MISO_Pin<br/>
   PE7		------> 9552_RS_Pin<br/>
   PE8		------> 9552_RTS_Pin<br/>
   PE9		------> 9552_CS_Pin<br/>
 
  I2C Address:
  AT24C02				0x57
  AT24C32				0x50
  SH1106				0x3C
  RTC(DS3231/ DS1307) 	0x68
  ADS1115(ADC)			0x78

  目前為止,我是基於STM32F407的開發版,找到什麼外部設備就寫什麼
  並沒有一定的專案規劃；這也是我一直想要去尋找的東西。
  我是硬體工程師，我希望做出的是一個產品，一個可以對實際有意義的東西。
  而不是成天埋在程式碼裡面去寫驅動程式。
  我問過AI，他給我不少想法，但我都沒興趣。
  
  目前我已經可以用C#寫電腦端的程式,MCU也可以用STM32系列的mcu開發實體層的控制
  我自己本身也可以做硬體設計。
  看來缺的是機構了……
  拜託……不要逼我去學3D設計……

05/01
  修正DS1307相關問題
  就是很多問題，一時記不清楚  
04/13
-修改scanf()接收不到訊號的問題

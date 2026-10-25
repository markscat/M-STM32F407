我在ＳＴＭ３２Ｆ４０７上開發的程式……好吧！還有Deepseek、chargpt和Gemini
<br>

2026/10/20<br>
1. 新增I2C擴充TCA9548相關程式
2. EEPPROM還沒整理,如果沒必要就不用了
5. to do<br> 
    - io擴充TCA555(ok)<br>
    - LCM<br>
    - I2C擴充<br>
    - DAC<br>
    - 顯示ADC的數值在OLED上<br>
    - 硬體上必須要有個轉接板;或是直接做一塊周邊板
    - 周邊板應該有戲,因為最主要的用途就是把所有的I2C的模組放在一起,只要有I2C的MCU，
    　幾乎都可以接<br>
    <br>
 

2026/10/20<br>
1. 新增I2C_APP.C/h<br>
2. 目的是為了要把應用層的東西放在這裡,這樣main()程式就會變得很整潔，把應用層和驅動層隔離<br>
3.　已經完成項目<br>
    -　ADC TSC1551<br>
    -  RTC<br>
    -  EEPROM <br>
4. OLED可以不用改,副程式寫得很完整<br>
    - 但是可以考慮寫個測試程式<br>
5. to do<br> 
    - io擴充TCA555<br>
    - LCM<br>
    - I2C擴充<br>
    - DAC<br>
    - 顯示ADC的數值在OLED上<br>
    <br>

2026/10/19<br>
    1. 整理main(),把應用程式模組化,然後獨立出來,這樣主程式才會比較乾淨<br>
    2. 新增TCA9548,不過現在還沒完成<br>
<br>
2626/10/04<br>
    1. 同步程式碼;備份後刪除舊有資料夾<br>
    2. 增加註解I2C.C的註解,了解自己到底寫了些什麼東西<br>
    3. 預計要做的：<br>
        - 想著接下來該做什麼,主要我是希望能做類似ＰＬＣ的東西;有IO,adc,dac,通訊<br>
        - PLC在操作上會很靈活,但不一定要作到可以相容其他的PLC語言<br>
        - 找機會把main<br>
    
2026/10/01<br>
一年沒動了。<br>
1.　因為兩塊外接版的EEPROM的位址不同,所以會出現Ａ板可以被初始化，Ｂ板卻會出現『初始化錯誤』的問題<br>
2.　歷史遺留問題：Comport的程本來就沒有傳送Commind ID的資料,導致分析錯誤<br>

2025/10/10<br><br>

目前的進度：<br><br>

板上LED的操作<br>
printf和scanf的重寫<br>
TCA555,I2C擴充IO的寫入;但讀取有待測試<br>
TM1638的寫入;讀取有待測試<br>
完成EEPROM AT24C32以及AT24C02的讀寫<br>
完成OLED 螢幕開啟以及關閉的功能<br>
完成ADS1115程式應用<br>
#IO setup:<br>
I2C1 GPIO Configuration<br>
PB6 ------> I2C1_SCL<br>
PB7 ------> I2C1_SDA<br>
USART2 GPIO Configuration<br>
PA2 ------> USART2_TX<br>
PA3 ------> USART2_RX<br>
TM1638 GPIO Configuration<br>
PC4 ------> SEG_CLK<br>
PC5 ------> SEG_DIO<br>
PB0 ------> SEG_STB<br>
<br>
SPI GPIO Configuration<br>
VCC PE10 ------> SPI1_CS_Pin<br>
PA7 ------> SPI1_MOSI_Pin<br>
PA5 ------> SPI1_SCK_Pin<br>
PA6 ------> SPI1_MISO_Pin<br>
<br>
SPI Panel ili9225 VCC GND GND NC NC NC PA5 ------> SPI1_SCK_Pin<br>
PA6 ------> SPI1_MISO_Pin<br>
PE7 ------> 9552_RS_Pin<br>
PE8 ------> 9552_RTS_Pin<br>
PE9 ------> 9552_CS_Pin<br>
<br>
I2C Address:<br>
AT24C02--------------------> 0x57<br>
AT24C32--------------------> 0x50<br>
SH1106---------------------> 0x3C<br>
RTC(DS3231/ DS1307)--------> 0x68<br>
ADS1115(ADC)---------------> 0x78<br>

目前為止,我是基於STM32F407的開發版,找到什麼外部設備就寫什麼<br>
並沒有一定的專案規劃；這也是我一直想要去尋找的東西。<br>
我是硬體工程師，我希望做出的是一個產品，一個可以對實際有意義的東西。<br>
而不是成天埋在程式碼裡面去寫驅動程式。<br>
我問過AI，他給我不少想法，但我都沒興趣。<br>

目前我已經可以用C#寫電腦端的程式,MCU也可以用STM32系列的mcu開發實體層的控制<br>
我自己本身也可以做硬體設計。<br>
看來缺的是機構了……<br>
拜託……不要逼我去學3D設計……<br>

05/01 修正DS1307相關問題 就是很多問題，一時記不清楚
04/13 -修改scanf()接收不到訊號的問題

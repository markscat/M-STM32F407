目前所有IO的設定

    /**
     * I2C1 GPIO Configuration
     * PB6     ------> I2C1_SCL
     * PB7     ------> I2C1_SDA
    */
    
    /**
     * TM1638 GPIO Configuration
     * PC4     ------> SEG_CLK
     * PC5     ------> SEG_DIO
     * PB0     ------> SEG_STB
    * /
    
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */

0406 modify list：
    1. 修正主程式因為沒有while(1){}迴圈，導致程式無法結束甚至卡死的問題．
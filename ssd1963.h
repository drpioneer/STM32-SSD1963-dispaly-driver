#ifndef _SSD1963_H_
#define _SSD1963_H_

#include "stm32f4xx.h"

#define HDP                     (799)                                           // Количество строк дисплея
#define VDP                     (479)                                           // Количество столбцов дисплея

#define LCD_REG   		        ((uint32_t)0x60000000)	                        // Адрес для работы с командами
#define LCD_DATA    	        ((uint32_t)0x60020000)                          // Адрес для работы с данными

static __inline void SSD1963_SendCmd (uint8_t index)                            // Функция отправки команд в SSD1963
{
    *(uint16_t *) (LCD_REG) = index;    
}

static __inline void SSD1963_WriteData (uint16_t data)                          // Функция отправки данных в SSD1963
{   
    *(uint16_t *) (LCD_DATA) = data;     
}

void    SSD1963_Init            (void);
void    SSD1963_SetWorkArea     (uint16_t x,  uint16_t y, uint16_t length, uint16_t width);

#endif

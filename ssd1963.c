// https://itnan.ru/post.php?c=1&p=278967
/*           *** Модуль контроллера дисплея на базе SSD1963 ***
 *******************************************************************************
 * Урезанный программный модуль, в котором использован код из примера:
 * https://hubstub.ru/display/137-podklyuchenie-tft-displeya-po-fsmc-k-stm32-na-
 * primere-ssd1963-.html
 *
 * Контроллер SSD1963  - универсальный  контроллер  TFT  дисплеев с  разрешением
 * до  864 х 480 пикс., глубиной цвета  до 24 бит/пиксель, оснащёный  встроенной
 * статической  памятью  объёмом 1215 кБ,  ФАПЧ  генераторами  развёртки и т.п.,
 * что позволяет обходиться минимумом внешней обвязки. Внешний интерфейс SSD1963
 * бывает двух видов: 6800 и 8080. В данном проекте задействован интерфейс 8080,
 * т.к. STM32F4xx поддерживает его аппаратно. Ширина данных внешнего  интерфейса
 * может быть 8/16/24 бит. От ширины интерфейса  не зависит  настройка  дисплея,
 * поэтому инициализация разных дисплеев происходит абсолютно  одинаково. Однако
 * в процессе инициализации  необходимо  настроить  ширину  данных  изображения,
 * которые будет принимать контроллер дисплея от МК в процессе  работы.  Дисплей
 * Winstar 'WF70HTIFGDBN0#' использует внешний интерфейс шириной 16 бит. 
 * Для реализации параллельного интерфейса  8080  в  МК  STM32F4xx  задействован
 * FMC (Flexible memory controller -  коммутируемый контроллер памяти),  который
 * позволяет  МК  работать  как  со  статической,  NOR и NAND  памятью,  так и с
 * параллельной шиной,  используя  свойства  непосредственного  проецирования на
 * адресное  пространство  для  сокращения  кода  управления и ускорения  режима
 * работы. При  настройке  FMC  очень важно указать правильно тайминги,  т.к.  в
 * параллельной шине, работающей на большой скорости, в проводниках разной длины
 * время прохода сигнала разное. Неверная настройка FMC приведет к  проблемам  в
 * работе TFT-дисплея вплоть до его полной неработоспособности.
 * Настройки FMC описаны в модуле FMC.C 
 * Этот программный модуль лишь обеспечивает минимально необходимые  функции для
 * вывода изображения.  Основная  работа  по  формированию картинки возложена на
 * Light & Versatile Graphic Library (https://lvgl.io/)
 *******************************************************************************
*/

#include "ssd1963.h"                                                            // Библиотека драйвер дисплея

#define _SSD1963_USE_FREERTOS   1                                               // =1 -> работа в многозадачной среде
#if (_SSD1963_USE_FREERTOS == 1)
    #include "cmsis_os.h"
    #define _SSD1963_DELAY(x)   osDelay(x)
#else
    #define _SSD1963_DELAY(x)   HAL_Delay(x)
#endif

void SSD1963_Init (void)                                                        // Функция инициализации дисплея. 
// Для инициализации дисплея достаточно ширины шины данных =8 бит.
// В контроллер дисплея передаётся список команд с параметрами для приведения дисплея к рабочему состоянию.
{
    GPIOD->BSRR                 |=  GPIO_BSRR_BR12;                             // Аппаратный сброс дисплея -> pin PD12
    _SSD1963_DELAY              (100);
    GPIOD->BSRR                 |=  GPIO_BSRR_BS12;

    // Инициализация WF70HTIFGDBN0# найденная на просторах интернета
    SSD1963_SendCmd             (0xE2);                                         // Настройка множителя PLL
    SSD1963_WriteData           (0x23);                                         // set PLL clock to 120M. 0x36 for 6.5M, 0x23 for 10M crystal.
    SSD1963_WriteData           (0x02);
    SSD1963_WriteData           (0x54);

    SSD1963_SendCmd             (0xE0);                                         // Запуск PLL
    SSD1963_WriteData           (0x01);
    _SSD1963_DELAY              (5);

    SSD1963_SendCmd             (0xE0);
    SSD1963_WriteData           (0x03);                                         // now, use PLL output as system clock
    _SSD1963_DELAY              (5);

    SSD1963_SendCmd             (0x01);                                         // Программный сброс дисплея
    _SSD1963_DELAY              (5);

    SSD1963_SendCmd             (0xE6);                                         // Установка PLL для PCLK, зависит от разрешения
    SSD1963_WriteData           (0x03);
    SSD1963_WriteData           (0x33);
    SSD1963_WriteData           (0x33); 

    SSD1963_SendCmd             (0xB0);                                         // Установка режимов работы LCD панели
    SSD1963_WriteData           (0x00);                                         // 18 bit TFT panel (!!!) (было = 20 -> 24bit, искажались цвета. https://www.cyberforum.ru/avr/thread2085474.html )
    SSD1963_WriteData           (0x80);                                         // Hsync + Vsync + DE mode, TFT mode
    SSD1963_WriteData           (HDP >> 8);                                     // Set HDP
    SSD1963_WriteData           (HDP);
    SSD1963_WriteData           (VDP >> 8);                                     // Set VDP
    SSD1963_WriteData           (VDP);
    SSD1963_WriteData           (0x00);

    SSD1963_SendCmd             (0x36);                                         // Конфигурация адресации данных
    SSD1963_WriteData           (0x00);                                         // При = 0xC0 все области экрана были перевернуты!!!

    SSD1963_SendCmd             (0x3A);                                         // Set the current pixel format for RGB image data
    SSD1963_WriteData           (0x50);                                         // 16-bit/pixel

    SSD1963_SendCmd             (0xF0);                                         // Pixel Data Interface Format
    SSD1963_WriteData           (0x03);                                         // 16-bit(565 format) data 

    SSD1963_SendCmd             (0xB4);                                         // Настройка HSYNC
    SSD1963_WriteData           (0x04);                                         // Set HT
    SSD1963_WriteData           (0x1F);
    SSD1963_WriteData           (0x00);                                         // Set HPS
    SSD1963_WriteData           (0xD2);
    SSD1963_WriteData           (0x00);                                         // Set HPW
    SSD1963_WriteData           (0x00);                                         // Set HPS
    SSD1963_WriteData           (0x00);
    SSD1963_WriteData           (0x00);

    SSD1963_SendCmd             (0xB6);                                         // Настройка VSYNC
    SSD1963_WriteData           (0x02);                                         // Set VT
    SSD1963_WriteData           (0x0C);
    SSD1963_WriteData           (0x00);                                         // Set VPS
    SSD1963_WriteData           (0x22);
    SSD1963_WriteData           (0x00);                                         // Set VPW
    SSD1963_WriteData           (0x00);                                         // Set FPS
    SSD1963_WriteData           (0x00);

    SSD1963_SendCmd             (0x29);                                         // Разрешение отображения картинки
}
   
void SSD1963_SetX (uint16_t start_x, uint16_t end_x)                            // Функция установки рабочей области по X
{
    SSD1963_SendCmd             (0x2A);
    SSD1963_WriteData           (start_x >> 8);
    SSD1963_WriteData           (start_x &  0xFF);
    SSD1963_WriteData           (end_x   >> 8);
    SSD1963_WriteData           (end_x   &  0xFF);
}

void SSD1963_SetY (uint16_t start_y, uint16_t end_y)                            // Функция установки рабочей области по Y
{
    SSD1963_SendCmd             (0x2B);
    SSD1963_WriteData           (start_y >> 8);
    SSD1963_WriteData           (start_y &  0xFF);
    SSD1963_WriteData           (end_y   >> 8);
    SSD1963_WriteData           (end_y   &  0xFF);
}

void SSD1963_SetWorkArea (uint16_t start_x, uint16_t end_x, uint16_t start_y, uint16_t end_y) // Функция установки координат рабочей области
{
    SSD1963_SetX                (start_x, end_x);
    SSD1963_SetY                (start_y, end_y);
    SSD1963_SendCmd             (0x2C);
}

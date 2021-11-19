# ssd1963

           *** Модуль контроллера дисплея на базе SSD1963 ***

Урезанный программный модуль, в котором использован код из примера:
https://hubstub.ru/display/137-podklyuchenie-tft-displeya-po-fsmc-k-stm32-na-primere-ssd1963-.html
Контроллер SSD1963  - универсальный  контроллер  TFT  дисплеев с  разрешением
до  864 х 480 пикс., глубиной цвета  до 24 бит/пиксель, оснащёный  встроенной
статической  памятью  объёмом 1215 кБ,  ФАПЧ  генераторами  развёртки и т.п.,
что позволяет обходиться минимумом внешней обвязки. Внешний интерфейс SSD1963
бывает двух видов: 6800 и 8080. В данном проекте задействован интерфейс 8080,
т.к. STM32F4xx поддерживает его аппаратно. Ширина данных внешнего  интерфейса
может быть 8/16/24 бит. От ширины интерфейса  не зависит  настройка  дисплея,
поэтому инициализация разных дисплеев происходит абсолютно  одинаково. Однако
в процессе инициализации  необходимо  настроить  ширину  данных  изображения,
которые будет принимать контроллер дисплея от МК в процессе  работы.  Дисплей
Winstar 'WF70HTIFGDBN0#' использует внешний интерфейс шириной 16 бит. 
Для реализации параллельного интерфейса  8080  в  МК  STM32F4xx  задействован
FMC (Flexible memory controller -  коммутируемый контроллер памяти),  который
позволяет  МК  работать  как  со  статической,  NOR и NAND  памятью,  так и с
параллельной шиной,  используя  свойства  непосредственного  проецирования на
адресное  пространство  для  сокращения  кода  управления и ускорения  режима
работы. При  настройке  FMC  очень важно указать правильно тайминги,  т.к.  в
параллельной шине, работающей на большой скорости, в проводниках разной длины
время прохода сигнала разное. Неверная настройка FMC приведет к  проблемам  в
работе TFT-дисплея вплоть до его полной неработоспособности.
Настройки FMC описаны в модуле FMC.C 
Этот программный модуль лишь обеспечивает минимально необходимые  функции для
вывода изображения.  Основная  работа  по  формированию картинки возложена на
Light & Versatile Graphic Library (https://lvgl.io/)

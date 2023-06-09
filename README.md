# libToMcsr
Динамическая библиотека, реализующая пользовательские функции для обработки данных, полученных из мцср. Результат чтения записывается в файл /usr/local/T15result/tdms_*текущееДатаВремя*/*текущееДатаВремя*.tdms 
## Установка 
* git clone https://github.com/farshMagical/libToMcsr.git
* make
## Как работает
Библиотека для своей работы использует два файла конфигурации:
* config.ini (файл конфигурации для мцср, есть по умолчанию, из него парсится кол-во работающих мсцр) - /usr/local/config.ini
* properties.ini - /usr/local/properties.ini
### Пример файла properties.ini
// Файл конфигурации для пользовательской функции для записи данных в tdms формате \
DEVICE T15\
CNAME Ch.\
Shot 123\
Start_delay 0\
FW 0\
T4_delay 0\
Duration 0\
Gain 1\
Offset 0\
Vcoef 1\
Acoef 1\
Aconst 0\
RATE 2000000\
Uref 4.096\
Dig 16

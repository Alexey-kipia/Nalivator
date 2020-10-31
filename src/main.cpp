//****************************************************************************************************************
#include <Arduino.h>            // Основная библиотека
#include <EEPROM.h>             // Библиотека работы со встроенной памятью EEPROM
#include "LCD_1602_RUS.h"       // Библиотека для работы с LCD
#include "DFPlayerMini_Fast.h"  // Библиотека для работы с MP3 плеером
#include "encUniversalMinim.h"  // Библиотека для работы с Энкодером
#include "Adafruit_TiCoServo.h" // Библиотека для работы с SERVO
#include "timer2Minim.h"        //
#include "Stroki_menu.h"        //
#include "Random.h"             //
#include "microLED.h"           // Библиотека для работы с адресуемыми светодиодами
//****************************************************************************************************************
void setup();                             // Подпрограмма начальных установок
void loop();                              // Основной цикл
void print_lcd(const uint8_t n);          // Вывод на дисплей данных из Stroki_menu.h
void oled_main_screen();                  // Основной экран "НАЛИВАТОР+"
void oled_Auto();                         // Экран режима "АВТО"
void oled_Nastroiki();                    // Экраны меню "НАСТРОЙКИ"
void oled_Nalivau();                      // Экран режима "НАЛИВАЮ"
void oled_Nalito();                       // Экран меню "HАЛИТО"
void oled_Volume(uint8_t subVol);         // Экран меню "ГРОМКОСТЬ"
void oled_Brightness(uint8_t subBrigh);   // Экран меню "ЯРКОСТЬ"
void oled_Promivka(uint8_t subPromivka);  // Экран режима "ПРОМЫВКА"
void oled_Tost();                         // Экраны меню "НАСТРОЙКИ" -> "ТОСТЫ"
void oled_Player(uint8_t subPlay);        // Экран меню "ПЛЕЕР"
void oled_servo();                        // Экраны меню "НАСТРОЙКИ" -> "НАСТРОЙКИ СЕРВО"
void oled_Servo_calibr(uint8_t subServo); // Экран меню НАСТРОЙКИ СЕРВО КАЛИБРОВКА
void oled_Servo_speed(uint8_t subSS);     // Экран меню НАСТРОЙКИ СЕРВО СКОРОСТЬ
void oled_Multirazliv();                  // Экран режима МУЛЬТИРАЗЛИВ
void oled_Setup_Multirazliv();            // Экран настройки МУЛЬТИРАЗЛИВ
void oled_Mushketery();                   // Экраны меню "МУШКЕТЁРЫ"
void play_Mushketery();                   // Режим МУШКЕТЕРЫ
void oled_Kalibr_Pump(uint8_t subPump);   // Экраны меню "НАСТРОЙКИ" -> "КАЛИБРОВКА ПОМПЫ"
void oled_Num_Folder(uint8_t subFolder);  // Настройки -> Тосты -> Папка с треками
void oled_mix_Tost();                     // Экран меню Настройки -> Тосты -> Перемешать треки
void Tost();                              // Экран вывода тостов
void Play_track(uint8_t trackNum);        // Воспроизведение трека
void Button_Tower();                      // Обработка кнопки на башне
void EncTick();                           // Кнопки-крутилки
void flowTick();                          // Наливайка, опрос концевиков
void flowRoutnie();                       // Поиск и заливка
void Servo_move(uint8_t target);          // Управление SERVO
void Energy_Saving();                     // Энергосбережение
void Mon_Battery();                       // Монитор питания и защита батареи
void ret_menu();                          // Выход из некоторых меню
void mix();                               // Перемешиваем тосты
void mix_music();                         // Перемешиваем песни
void play_next();                         // Воспроизведение следующего трека
void oled_Sleep_Time(uint8_t subTime);    // Экран настройки таймер сна
void CvetoMuzik();                        // Светомузыка
void oled_Bar_Man(uint8_t subBarmen);     // Экран меню Настройки -> Бармен-Долив

//****************************************************************************************************************
//======== НАСТРОЙКИ ========
#define DEBUG_UART 0 // отладка, 0 выключено, 1 включено
#define LED_TOWER    // Раcкомментировать если есть светодиоды на башне
#if (DEBUG_UART == 0)
    //#define Button_Tower        // сенсорная кнопка на башне. Раскомментировать если есть
#endif
//#define BAT_MONITOR_ON          // включение в коде всё что звязано с АКБ, закомментировать если нет АКБ вообще.
#ifdef BAT_MONITOR_ON
#define ADC_U_COEFF 0.00472 // коэффициент перевода кода АЦП в напряжение.  1.1 / 1023 / R4 * (R4 + R5).  Условие должно выполняться - R4 / ( R4 + R5 ) = 0.22
#define LOW_BAT_SLEEP_ON    // контроль низкого заряда акб. Закомментировать - выключить для отладки кода, если не подключен пин A3 к мониторингу АКБ, иначе сон
#endif
#define SOUND_THEME 0          // звуковая тема, 0 - по умолчанию, 1 - для танкистов
#define NUM_SHOTS 5            // количество рюмок (оно же кол-во светодиодов и концевиков!), всё делалось под 6 рюмок!!!!
#define SWITCH_LEVEL 1         // концевики 1 - высокий сигнал при замыкании, 0 - низкий
#define PUMP_LEVEL 1           // уровень включения помпы, 1 - высокий уровень, 0 - низкий
#define MAX_DRINK 50           // максимальный объём тары мл, для корректного отображения в менюшках экрана, ставить не более 95
#define PRE_PAUSE 1000UL       // пауза серво перед движением к рюмке
#define POST_PAUSE 500UL       // пауза после остановки сервы до включения помпы
#define MEMORY_ON              // включение записи параметров в память, закомментировано - значит выключено!
#define START_POS_SERVO_GLASS1 // раскомментировать, если нужна начальная позиция серво в первой рюмке
//#define SERVO_CHANGE_DIRECTION    // раскомментировать, отзеркалить движение серво
#ifdef SERVO_CHANGE_DIRECTION
#define INITAL_ANGLE_SERVO 180 // начальный угол на который становится серво, если включен режим зеркало, подбирать в меньшую сторону, если упирается серво.
#endif

// изменить значения если серво не доезжает до нужных углов, не всегда эффективно!
#define SERVO_MIN 544  // уменьшить если не доезжает до 0° (544 по умолчанию)
#define SERVO_MAX 2400 // увеличить если не доезжает до 180° (2400 по умолчанию )
#define ORDER_GRB      // порядок цветов ORDER_GRB / ORDER_RGB / ORDER_BRG

//****************************************************************************************************************
//======== Пины Arduino NANO ========
#if defined(__AVR_ATmega328P__)
#include <SoftwareSerial.h>      // подключаем плеер к программному Serial, обязательно!
SoftwareSerial mySerial(10, 11); // RX, TX; плейер подключаем к D10 и D11

#ifndef SERVO_CHANGE_DIRECTION
const uint8_t SW_pins[] = {A0, A1, A2, 6, 7, 8}; // Пины концевиков для Arduino NANO
#else
const uint8_t SW_pins[] = {8, 7, A3, A2, A1, A0}; // Пины концевиков для Arduino NANO
#endif

#define Lcd_CLK 2     // Энкодер Lcd_CLK для Arduino NANO
#define Lcd_DT 3      // Энкодер Lcd_DT для Arduino NANO
#define Lcd_SW 4      // Энкодер Lcd_SW для Arduino NANO
#define LED1_PIN 5    // Лента 1 для Arduino NANO
#define SERVO_PIN 9   // Servo для Arduino NANO. Можно только 9, 10 Пины!!!!!!!!
#define PUMP_POWER 13 // Помпа для Arduino NANO

#ifdef BAT_MONITOR_ON
#define BAT_PIN A7 // Пин замера напряжения АКБ для Arduino NANO
#endif

#define BUSY_PIN 12 // Пин готовности DF плеера для Arduino NANO

#ifdef LED_TOWER
#define LED2_PIN A3 // Пин второй ленты для Arduino NANO
#endif

#ifdef Button_Tower
#define BUT_TOWER_PIN A6 // Пин кнопки на башне для Arduino NANO
#endif

//****************************************************************************************************************
//======== Пины Arduino MEGA ========
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define SERVO_PIN 2 // Пин серво для Arduino mega. Можно только 2, 3, 5, 6, 7, 8, 11, 12, 13, 44, 45, 46 Пины!!!!!!!!
#ifndef SERVO_CHANGE_DIRECTION
const uint8_t SW_pins[] = {A0, A1, A2, A3, A7, A8}; // Пины концевиков для Arduino mega
#else
const uint8_t SW_pins[] = {A8, A7, A3, A2, A1, A0}; // Пины концевиков для Arduino mega
#endif
#define PUMP_POWER 12 // помпа для Arduino mega
#define LED1_PIN 5    // Пин ленты для Arduino mega
#ifdef BAT_MONITOR_ON
#define BAT_PIN A9 // Пин замера напряжения акб для Arduino mega
#endif
#define BUSY_PIN 10 // Пин готовности DF плеера для Arduino mega
// Пины ЭНКОДЕРА
#define Lcd_CLK 7   // для Arduino mega
#define Lcd_DT 6    // для Arduino mega
#define Lcd_SW 4    // кнопка энкодера для Arduino mega
#ifdef LED_TOWER
#define LED2_PIN 11 // Пин второй ленты для Arduino mega
#endif
#ifdef Button_Tower
#define BUT_TOWER_PIN 22 // Пин кнопки на башне для Arduino mega
#endif
#else
#error "ВЫБРАНА НЕ ВЕРНАЯ ПЛАТА АРДУИНО!!!"
#endif

//****************************************************************************************************************
//********** ИНИЦИАЛИЗАЦИЯ **********
LEDdata Leds[NUM_SHOTS];                    // буфер ленты типа LEDdata (размер зависит от COLOR_DEBTH)
microLED Strip1(Leds, NUM_SHOTS, LED1_PIN); // объект лента
#ifdef LED_TOWER                            //
#define NUMLEDS 16                          // колличество светиков во второй ленте
LEDdata Leds2[NUMLEDS];                     // буфер ленты типа LEDdata (размер зависит от COLOR_DEBTH)
microLED Strip2(Leds2, NUMLEDS, LED2_PIN);  // объект лента
encMinim Enc(Lcd_CLK, Lcd_DT, Lcd_SW, 0);   // пин Lcd_CLK, пин Lcd_DT, пин Lcd_SW, направление (0/1)
LCD_1602_RUS lcd(0x27, 16, 2);              //Адрес дисплея 0x27 или 0x3F, подключение А4-SDA-зеленый, А5-SCL-желтый
DFPlayerMini_Fast myMP3;                    //
Adafruit_TiCoServo servo;                   //

timerMinim TOWERtimer(20); //
bool mig = false;          //
bool rainbow = true;       //
bool clearLed = false;     //
#endif                     //
#ifdef Button_Tower        //
bool stateBut = false;     //
#endif                     //

timerMinim LEDtimer(50);         //
timerMinim FLOWdebounce(20);     //
timerMinim WAITtimer(PRE_PAUSE); //
timerMinim TIMEProcent(2000);    //
timerMinim SAVEtimer(30000);     // таймер спящего режима
timerMinim PAUSEtimer(4000);     // таймер паузы
timerMinim PLAYtimer(2000);      // таймер переключения треков

int16_t time50ml;           // время налива 50 мл
uint8_t shotPos[NUM_SHOTS]; // = 0; // позиция рюмок, в сетапе считается из памяти.
int8_t rumka[NUM_SHOTS];    // массив рандомных позиций рюмок
int16_t address = 0;        // Переменная для хранения адреса памяти
int16_t bright = 0;         // яркость led, считается из памяти
int16_t Procent = 0;
int8_t count = -1;
int8_t curPumping = -1;
int16_t servoPos = 0;
int8_t volume;
int8_t volume2;
uint8_t subNastr = 1;
uint8_t subMush = 1;
uint8_t subMenu = 1;
uint8_t yesGlass = 0;
uint8_t randomGlass = 0;
uint8_t speedSer = 10;
int8_t tracks = 0;
int16_t tracks2 = 0;
uint8_t Menu = 0;
uint8_t MenuFlag = 0;   // Здесь храниться уровень меню.
uint8_t Drink = 20;     // По умолчанию в рюмку наливаем  20 мл.
uint8_t DrinkCount = 0; //счётчик налитых рюмок
uint8_t folder;
uint8_t oldFolder;
uint8_t folder2 = 10;
uint8_t num = 0;
uint8_t num2 = 0;
int8_t sleepTime = 0;
uint8_t ManDrink[] = {0, 0, 0, 0, 0, 0};
uint8_t TostList[100];  // массив номеров треков тостов, в нём перемешаем всё, максимум 100 тостов. нужно ли столько?
uint8_t MusicList[100]; // массив номеров песенок, в нём перемешаем всё, максимум 100
uint8_t ManRum = 1;
uint8_t mixTost = 0;
uint8_t mixMusic = 0;
uint8_t folTra = 1; //
uint8_t oldNum = 1; //
uint8_t barMan;
bool LEDchanged = false;
bool pumping = false;
bool promivka = false;
bool ledShow = true;
bool moving = false;
bool tost = false;
bool returnMenu = false;
bool check = false;
bool save = false;
bool systemON = false;
bool noGlass = true;
bool readyDrink = false; // стопки налиты
bool flag = false;       // флаг что нужно показать меню налива
bool player = false;     //
bool nextTrack = false;
bool playOn = false; // флаг что плеер был включен
bool ledShowOn = true;
bool pause = false;
bool noTost = false;
bool playMush = false;
bool flagMush = false;
uint8_t noDoliv;
bool readySystem = true;
int8_t drift = 0;
#if (SOUND_THEME == 1)
bool muveBack = true;
#endif

enum
{
  NO_GLASS,
  EMPTY,
  IN_PROCESS,
  READY
} shotStates[NUM_SHOTS];
enum
{
  SEARCH,
  MOVING,
  WAIT,
  PUMPING
} systemState;

//********** свои символы **********
#ifdef BAT_MONITOR_ON
int16_t value = 750;                                                                    // значение с аналогового пина подключенного к батарее
bool lowBat = false;                                                                    // флаг низкого заряда акб
uint8_t battery0[8] = {B01110, B11111, B10001, B10001, B10001, B10001, B10001, B11111}; // пустая батарея
uint8_t battery1[8] = {B01110, B11111, B10001, B10001, B10001, B10001, B11111, B11111}; // батарея одно деление
uint8_t battery2[8] = {B01110, B11111, B10001, B10001, B10001, B11111, B11111, B11111}; // батарея два деления
uint8_t battery3[8] = {B01110, B11111, B10001, B10001, B11111, B11111, B11111, B11111}; // батарея три деления
uint8_t battery4[8] = {B01110, B11111, B10001, B11111, B11111, B11111, B11111, B11111}; // батарея четыре деления
uint8_t battery5[8] = {B01110, B11111, B11111, B11111, B11111, B11111, B11111, B11111}; // батарея полная
uint8_t power[8] = {B01010, B01010, B11111, B11111, B11111, B01110, B00100, B00100};    // питание от сети
#endif

//********** МАКРО **********
#if (PUMP_LEVEL == 0)
#define pumpON() digitalWrite(PUMP_POWER, 0)
#define pumpOFF() digitalWrite(PUMP_POWER, 1)
#else
#define pumpON() digitalWrite(PUMP_POWER, 1)
#define pumpOFF() digitalWrite(PUMP_POWER, 0)
#endif

#if (DEBUG_UART == 1)
#define DEBUG(x) Serial.println(x)
#else
#define DEBUG(x)
#endif

//****************************************************************************************************
void setup()
{
  Strip1.clear();
  Strip1.show();
#ifdef LED_TOWER
  Strip2.clear();
  Strip2.show();
#endif
  delay(1000);
#if (DEBUG_UART == 1)
  Serial.begin(9600);
#endif
#ifdef BAT_MONITOR_ON
#if defined(__AVR_ATmega328P__)
  analogReference(INTERNAL);
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  analogReference(INTERNAL1V1);
#endif
#endif

#if defined(__AVR_ATmega328P__)
  mySerial.begin(9600); // инициализируем прграммный Serial порт
  delay(100);
  myMP3.begin(mySerial); // инициализируем Serial порт МП3 плейера
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  Serial3.begin(9600);
  delay(100);
  myMP3.begin(Serial3);          //инициализируем Serial порт МП3 плейера 14 и 15 Пины меги
#endif

  //********** настройка пинов **********
  pinMode(PUMP_POWER, OUTPUT); // Помпа
#if (PUMP_LEVEL == 0)
  digitalWrite(PUMP_POWER, HIGH); // высокий уровень если активный на включение ноль
#else
  digitalWrite(PUMP_POWER, LOW); // низкий уровень если активна на включение единица
#endif

  for (uint8_t i = 0; i < NUM_SHOTS; i++) // Концевики
  {
#if (SWITCH_LEVEL == 0)
    pinMode(SW_pins[i], INPUT_PULLUP); // вход с подтяжкой если активный ноль
#else
    pinMode(SW_pins[i], INPUT);  // иначе просто вход
#endif
  }

  pinMode(BUSY_PIN, INPUT); // вход сигнала окончания трека от MP3 плеера

#ifdef Button_Tower              // если есть кнопка на башне
  pinMode(BUT_TOWER_PIN, INPUT); // включить как вход
#endif

  //Procent = 5; // используем переменную счётчика, потом приведём её в нужное значение, она учавствует в другом месте.
  for (uint8_t i = 0; i < NUM_SHOTS; i++)
  {
    EEPROM.get(address, shotPos[i]); // считываем из памяти положение для сервы
    if (shotPos[i] < 1 || shotPos[i] > 180)
      shotPos[i] = Procent; // если ячейки памяти не в интервале, то ставим начальные значения
    address++;
    Procent += 180 / (NUM_SHOTS - 1);
  }

  address = 10;
  for (uint8_t i = 0; i < NUM_SHOTS; i++)
  {
    EEPROM.get(address, ManDrink[i]); // считываем из памяти, дриньки для мультиразлива
    if (ManDrink[i] < 20 || ManDrink[i] > MAX_DRINK)
      ManDrink[i] = 20; // если ячейки памяти не в интервале, то ставим начальные значения
    address++;
  }

  address = 20;
  EEPROM.get(address, volume); // считываем из памяти громкость
  if (volume < 0 || volume > 30)
    volume = 10;

  address = 30;
  EEPROM.get(address, bright); // считываем из памяти яркость led
  if (bright < 0 || bright > 255)
    bright = 100;

  address = 40;
  EEPROM.get(address, speedSer); // считываем из памяти скорость сервы
  if (speedSer < 5 || speedSer > 30)
    speedSer = 10;

  address = 50;
  EEPROM.get(address, sleepTime); // считываем из памяти время таймера сна
  if (sleepTime <= 0 || sleepTime > 20)
    sleepTime = 0;
  else
    SAVEtimer.setInterval(sleepTime * 30000UL);

  address = 60;
  EEPROM.get(address, time50ml); // считываем из памяти время налива 50 мл.
  if (time50ml < 1000 || time50ml > 10000)
    time50ml = 5000;

  address = 70;
  EEPROM.get(address, mixTost); // считываем из памяти флаг перемешивания тостов
  if (mixTost > 1)
    mixTost = 0;

  address = 80;
  EEPROM.get(address, folder); // считываем из памяти номер папки треков тостов
  if (folder < 1 || folder > 9)
    folder = 1;
  oldFolder = folder;

  address = 90;
  EEPROM.get(address, noDoliv); // считываем из памяти флаг перемешивания тостов
  if (noDoliv > 1)
    noDoliv = 1;

  address = 100;
  EEPROM.get(address, volume2); // считываем из памяти громкость для плеера
  if (volume2 < 0 || volume2 > 30)
    volume2 = 10;

  address = 110;
  EEPROM.get(address, mixMusic); // считываем из памяти флаг перемешивания песен
  if (mixMusic > 1)
    mixMusic = 1;

  address = 120;
  EEPROM.get(address, barMan); // считываем из памяти флаг режима бармен
  if (barMan > 1)
    barMan = 1;

  Procent = 0;

  delay(100); //Между двумя командами необходимо делать задержку 100 миллисекунд, в противном случае некоторые команды могут работать не стабильно.
  myMP3.EQSelect(EQ_NORMAL);
  delay(100);
  tracks = myMP3.numTracksInFolder(folder); // считываем колличество треков в папке 01-09 в корне флешки, не больше 100
  if (tracks > 100)
    tracks = 100;
  delay(100);
  myMP3.volume(volume);
  delay(100);
#if (SOUND_THEME == 1)
  Play_track(50); //танк завёлся
#else
  Play_track(1);                 // Проигрываем "mp3/0001.mp3"(0001_get started!.mp3)
#endif
  lcd.init(); // Инициализация дисплея
  lcd.backlight();
  lcd.setCursor(7, 0);
  print_lcd(0); // НУ,
  lcd.setCursor(1, 1);
  print_lcd(1); // НАА-ЧАА-ЛИИИИ!
  Strip1.setBrightness(bright);
#ifdef LED_TOWER
  Strip2.setBrightness(bright);
#endif
  //servo.attach(SERVO_PIN);
  servo.attach(SERVO_PIN, SERVO_MIN, SERVO_MAX);
#ifdef START_POS_SERVO_GLASS1
#ifdef SERVO_CHANGE_DIRECTION
  servo.write(INITAL_ANGLE_SERVO - shotPos[0]);
#else
  servo.write(shotPos[0]);
#endif
#else
#ifdef SERVO_CHANGE_DIRECTION
  servo.write(INITAL_ANGLE_SERVO);
#else
  servo.write(0);
#endif
#endif
  delay(2000);
  oled_main_screen();
  mix();
}

void loop()
{
#ifdef BAT_MONITOR_ON
  if (!lowBat)
  {
#endif
    EncTick();
    flowTick();
    CvetoMuzik();
    play_Mushketery();
    Energy_Saving();
    Tost();
    ret_menu();
    play_next();
#ifdef Button_Tower
    Button_Tower();
#endif
#ifdef BAT_MONITOR_ON
  }
  Mon_Battery();
#endif
}

//****************************************************************************************************
//********** Воспроизведение трека **********
void Play_track(uint8_t trackNum)
{
  if (!player && volume != 0)
    myMP3.playFromMP3Folder(trackNum);
}

//********** Наливайка, опрос концевиков **********
void flowTick()
{
  if (FLOWdebounce.isReady())
  {
    for (uint8_t i = 0; i < NUM_SHOTS; i++)
    {
      bool swState = !digitalRead(SW_pins[i]) ^ SWITCH_LEVEL;
      if (shotStates[i] == NO_GLASS && swState && readySystem)
      {                        // поставили пустую рюмку
        shotStates[i] = EMPTY; // флаг на заправку
#ifndef SERVO_CHANGE_DIRECTION
        Strip1.setLED(i, mRGB(255, 0, 0)); // подсветили красный
#else
        Strip1.setLED(NUM_SHOTS - 1 - i, mRGB(255, 0, 0)); // подсветили красный
#endif
        LEDchanged = true;
        //DEBUG("set glass");
        //DEBUG(i);
        if (!systemON && !save && !playMush)
          Play_track(i + 20); //  трек в папке mp3, с 20 по 25
        yesGlass++;
        SAVEtimer.reset();
        if (save)
        {
          Play_track(17); // звук просыпания
          save = false;
          Enc.rst();
          lcd.backlight();
          check = true; //  проверяем рюмки
          delay(2000);
        }
        if (barMan == 1 && (MenuFlag == 1 || MenuFlag == 2) && !systemON)
        {
          oled_Nalivau();
          systemON = true;
          if (ledShowOn)
          {
            if (ledShow)
              check = true;
#ifdef LED_TOWER
            TOWERtimer.setInterval(50);
            if (mig || rainbow)
            {
              mig = false;
              rainbow = false;
              clearLed = true;
              TOWERtimer.reset();
            }
#endif
          }
        }
      }
      if (shotStates[i] != NO_GLASS && !swState)
      {                           // убрали пустую/полную рюмку
        shotStates[i] = NO_GLASS; // статус - нет рюмки
        if (!ledShow)
        {
#ifndef SERVO_CHANGE_DIRECTION
          Strip1.setLED(i, mRGB(0, 0, 0)); // чёрный
#else
          Strip1.setLED(NUM_SHOTS - 1 - i, mRGB(0, 0, 0)); // чёрный
#endif
          LEDchanged = true;
        }
        if (i == curPumping)
        {                     // убрали во время заправки!
          systemState = WAIT; // режим работы - ждать
          WAITtimer.reset();
          pumpOFF(); // помпу выкл
          moving = false;
          if (MenuFlag == 20 || promivka)
            curPumping = -1;
          else
            Play_track(2); // трек в папке mp3,  кто то снял рюмку при наливе
        }
        //DEBUG("take glass");
        // DEBUG(i);
        yesGlass--;
      }
    }
  }
  if (systemON)
    flowRoutnie(); // если активны - ищем рюмки и всё такое
}

//********** поиск и заливка **********
void flowRoutnie()
{
  switch (systemState)
  {
  case SEARCH: // если поиск рюмки
    if (!moving)
    {
      noGlass = true;
      for (int8_t i = 0; i < NUM_SHOTS; i++)
      {
        if (shotStates[i] == EMPTY && i != curPumping)
        { // поиск рюмки
          if (MenuFlag == 6)
          {
            for (int8_t j = randomGlass; j < NUM_SHOTS; j++)
            {
              if (rumka[j] != -1)
              {
                randomGlass = j;
                break;
              }
            }
            if (subMush == 1)
            {
              if (i == rumka[randomGlass])
              {
                noGlass = false; // один за всех
              }
            }
            else if (subMush == 2)
            {
              if (i == rumka[randomGlass])
              {
                if (randomGlass < NUM_SHOTS - 1)
                  randomGlass++;
                noGlass = false; // все за одного
              }
            }
          }
          else
          {
            noGlass = false;
          }

          if (!noGlass)
          { // нашли хоть одну рюмку
#if (SOUND_THEME == 1)
            if (digitalRead(BUSY_PIN) && !promivka && MenuFlag != 20)
            {
              Play_track(53); //  трек в папке mp3 ,  движения башни танка
            }
#else
            if ((DrinkCount == 0 || digitalRead(BUSY_PIN)) && !promivka && MenuFlag != 20)
            {
              Play_track(18); // трек в папке mp3 , мелодия во время разлива
            }
#endif
            curPumping = i;                      // запоминаем выбор
            systemState = MOVING;                // режим - движение
            shotStates[curPumping] = IN_PROCESS; // стакан в режиме заполнения
            break;
          }
        }
      }
    }
    if (noGlass)
    { // если не нашли ни одной рюмки
#if (SOUND_THEME == 1)
      if (muveBack)
      {
        Play_track(53); // трек в папке mp3 ,  движения башни танка
        muveBack = false;
      }

#endif
      readySystem = false;
#ifdef START_POS_SERVO_GLASS1
      Servo_move(shotPos[0]); // цель серво - первая рюмка
#else
      Servo_move(0);                                              // цель серво - 0
#endif
      if (!moving)
      { // едем до упора
        PAUSEtimer.setInterval(4000);
        PAUSEtimer.reset();
        if (DrinkCount > 0 && !promivka && MenuFlag != 20)
        {
          oled_Nalito(); // Выводим на экран налито ...
          tost = true;
          if (player && volume != 0 && tracks != -1)
          {
            playOn = true;
            player = false;
            myMP3.stop();
          }
          else if (!player && volume != 0)
          {
            myMP3.stop();
          }
          if (ledShowOn)
          {
            ledShow = true;
#ifdef LED_TOWER
            TOWERtimer.setInterval(20);
            rainbow = true;
            clearLed = true;
            TOWERtimer.reset();
#endif
          }
        }
        else
        {
          lcd.clear();
          if (yesGlass == 0 || promivka || MenuFlag == 20)
          {
            lcd.setCursor(3, 1);
            print_lcd(5); // НЕТ РЮМОК!
          }
          else
          {
            lcd.setCursor(1, 1);
            print_lcd(6); // СНИМИТЕ РЮМКИ!
          }
          returnMenu = true;
          Play_track(3); // трек в папке mp3,  типа предупреждение нет рюмок

#ifdef LED_TOWER
          if (ledShowOn)
          {
            TOWERtimer.setInterval(50);
            mig = true;
            rainbow = false;
            clearLed = true;
            TOWERtimer.reset();
          }
#endif
        }
        DrinkCount = 0;
        systemON = false;
        curPumping = -1; // снимаем выбор рюмки
#if (SOUND_THEME == 1)
        muveBack = true;
#endif
        //DEBUG("no glass");
      }
    }
    break;

  case MOVING: // движение к рюмке
    if (flag)
    {
      if (!promivka)
        oled_Nalivau();
      else
        oled_Promivka(1);
      flag = false;
    }
    Servo_move(shotPos[curPumping]);
    if (!moving)
    {                        // если приехали
      systemState = PUMPING; // режим - наливание
      if (!promivka)
        TIMEProcent.setInterval(time50ml / 50); // перенастроили таймер,
      else
        TIMEProcent.setInterval(1);

      Procent = 0;
      TIMEProcent.reset();
      pumpON(); // НАЛИВАЙ!
      if (!promivka && MenuFlag != 20)
      {
        DrinkCount++;
        lcd.setCursor(11, 0);
        if (MenuFlag == 1 || MenuFlag == 6)
          lcd.print(Drink, DEC);
        else if (MenuFlag == 2)
          lcd.print(ManDrink[curPumping], DEC);
        lcd.setCursor(2, 1);
        lcd.print((curPumping + 1), DEC);
        lcd.setCursor(11, 1);
        print_lcd(4); // пробел 3
#if (SOUND_THEME == 1)
        Play_track(51); //выстрел танка
#endif
      }
      //DEBUG("fill glass");
    }
    break;

  case PUMPING: // если качаем
    if (TIMEProcent.isReady())
    {
      Procent++;
      if (!promivka)
      {
        if ((MenuFlag == 1 || MenuFlag == 20 || MenuFlag == 6) && Procent >= Drink)
          readyDrink = true;
        else if (MenuFlag == 2 && Procent >= ManDrink[curPumping])
          readyDrink = true;
        lcd.setCursor(12, 1);
        lcd.print(Procent, DEC);
      }
      else
      {
        if (MenuFlag == 11 && (Procent >= 7000 || !Enc.isHold()))
          readyDrink = true;
#ifdef Button_Tower
        else if (MenuFlag != 11 && (Procent >= 7000 || !stateBut))
          readyDrink = true;
#endif
      }
    }
    if (readyDrink)
    {                                 // если налили
      pumpOFF();                      // помпа выкл
      shotStates[curPumping] = READY; // налитая рюмка, статус: готов
#ifndef SERVO_CHANGE_DIRECTION
      Strip1.setLED(curPumping, mRGB(0, 255, 0)); // подсветили
#else
      Strip1.setLED(NUM_SHOTS - 1 - curPumping, mRGB(0, 255, 0)); // подсветили
#endif
      Strip1.show();
      curPumping = -1;    // снимаем выбор рюмки
      systemState = WAIT; // режим работы - ждать
      WAITtimer.reset();
      readyDrink = false;
      //DEBUG("wait");
    }
    break;

  case WAIT:
    if (WAITtimer.isReady())
    {
      systemState = SEARCH;
      //DEBUG("search");
      if (promivka)
      {
        systemON = false;
        if (MenuFlag == 11)
        {
          oled_Promivka(2);
        }
        else
        {
          do
          {
#ifdef START_POS_SERVO_GLASS1
            Servo_move(shotPos[0]); // цель серво - первая рюмка
#else
            Servo_move(0);                                        // цель серво - 0
#endif
          } while (moving);
          returnMenu = true;
        }
        promivka = false;
      }
      if (MenuFlag == 20)
        systemON = false;
    }
  }
}

//********** Управление SERVO **********
void Servo_move(uint8_t target)
{
  static uint32_t prevServoTime = 0;
#ifdef SERVO_CHANGE_DIRECTION
  target = 180 - target;
#endif
#ifdef START_POS_SERVO_GLASS1
#ifdef SERVO_CHANGE_DIRECTION
  static uint8_t pos = INITAL_ANGLE_SERVO - shotPos[0];
#else
  static uint8_t pos = shotPos[0];
#endif
#else
#ifdef SERVO_CHANGE_DIRECTION
  static uint8_t pos = INITAL_ANGLE_SERVO;
#else
  static uint8_t pos = 0;
#endif
#endif
  static bool deadTime = false;
  if (!moving)
  {
    if (pos != target)
    {
      moving = true;
      deadTime = false;
      prevServoTime = millis();
    }
  }
  if (moving && !deadTime && millis() - prevServoTime >= speedSer)
  {
    prevServoTime = millis();
    if (pos > target)
      pos--;
    else if (pos < target)
      pos++;
    servo.write(pos);
    if (pos == target)
    {
      deadTime = true;
#if (SOUND_THEME == 1)
      if (!playMush && !promivka && Menu != 3)
        Play_track(52); //клик башни
#endif
    }
  }
  if (deadTime && millis() - prevServoTime >= POST_PAUSE)
  { // пауза после остановки сервы до включения помпы
    deadTime = false;
    moving = false;
  }
}

// ********** Энергосбережение **********
void Energy_Saving()
{
  if (sleepTime != 0 && !systemON && !tost && !save && SAVEtimer.isReady())
  {
    Play_track(16);
    lcd.noBacklight(); // Отключить подсветку дисплея
    save = true;       //
    Strip1.clear();    // Отключить светодиоды
    Strip1.show();     //
  }
}

//********** Выход из некоторых меню **********
void ret_menu()
{
  if (returnMenu && PAUSEtimer.isReady())
  {
    if (MenuFlag == 1 || MenuFlag == 6)
      oled_Auto(); // выходим в меню автоналива
    else if (MenuFlag == 2)
      oled_Multirazliv(); // выходим в меню мультиналива
    else if (promivka)
      oled_Promivka(0);
    else if (MenuFlag == 20)
      oled_Kalibr_Pump(0);

    returnMenu = false;
    promivka = false;
    readySystem = true;
  }
}

//********** Монитор питания и защита батареи **********
#ifdef BAT_MONITOR_ON
void Mon_Battery()
{
  static uint32_t prevBatTime = 0;
  static uint8_t flagBat = 0;
  static uint8_t flag = 0;
  if (!systemON && !ledShow && !playMush && millis() - prevBatTime >= 1000UL)
  {
    prevBatTime = millis();
    value = ((value * 4) + analogRead(BAT_PIN)) / 5;
    if (value < 650)
    { // 3 вольта 635
      flagBat = 0;
#ifdef LOW_BAT_SLEEP_ON
      if (!lowBat)
      {
        lowBat = true; // гасим всё
        lcd.noBacklight();
        if (player)
        {
          playOn = false;
          player = false;
          myMP3.stop();
        }
        Strip1.clear();
        Strip1.show();
      }
#endif
    }
    else if (value < 690)
    { // 3.2 вольта 677
      flagBat = 1;
    }
    else if (value < 740)
    { // 3,4 вольта  720
      flagBat = 2;
#ifdef LOW_BAT_SLEEP_ON
      if (lowBat)
      {
        lowBat = false;
        lcd.backlight();
        check = true; // проверяем рюмки
      }
#endif
    }
    else if (value < 780)
    { // 3,6 вольта  762
      flagBat = 3;
    }
    else if (value < 840)
    { // 3,8 вольта  805
      flagBat = 4;
    }
    else if (value < 900)
    { // 4,2 вольта  889
      flagBat = 5;
    }
    else if (value < 1023)
    { // 5 вольт от сети
      flagBat = 6;
    }
    if (!lowBat && flagBat != flag && MenuFlag <= 6 && !tost && !returnMenu)
    {
      flag = flagBat;
      switch (flag)
      {
      case 0:
        lcd.createChar(7, battery0);
        break;
      case 1:
        lcd.createChar(7, battery1);
        break;
      case 2:
        lcd.createChar(7, battery2);
        break;
      case 3:
        lcd.createChar(7, battery3);
        break;
      case 4:
        lcd.createChar(7, battery4);
        break;
      case 5:
        lcd.createChar(7, battery5);
        break;
      case 6:
        lcd.createChar(7, power);
        break;
      }
      lcd.setCursor(15, 0);
      lcd.write(7);
    }
    if (MenuFlag == 18)
    {
      lcd.setCursor(5, 1);
      lcd.print((value * ADC_U_COEFF), 2);
      //lcd.setCursor(12, 1);     // раскомментировать, нужно для отладки заряда АКБ
      //lcd.print(value, DEC);    // раскомментировать, нужно для отладки заряда АКБ
    }
  }
}
#endif

//********** Перемешиваем тосты **********
void mix()
{
  if (tracks != -1)
  {
    for (uint8_t i = 0; i < tracks; i++)
      TostList[i] = i; // заполняем массив тостов, последовательно значениями
    num = 0;
    if (mixTost == 1)
    {
      randomSeed(CreateTrulyRandomSeed()); // инициализирует генератор псевдослучайных чисел
      for (uint8_t i = tracks - 1; i > 0; i--)
      { // перемешиваем значения случайно в массиве так, что они не повторяются
        uint8_t idx = random(i + 1);
        uint8_t t = TostList[i];
        TostList[i] = TostList[idx];
        TostList[idx] = t;
      }
    }
  }
}

//********** Перемешиваем песни **********
void mix_music()
{
  if (tracks2 != -1)
  {
    for (uint8_t i = 0; i < tracks2; i++)
      MusicList[i] = i; // заполняем массив песенок, последовательно значениями
    if (mixMusic == 1)
    {
      randomSeed(CreateTrulyRandomSeed()); // инициализирует генератор псевдослучайных чисел
      for (uint8_t i = tracks2 - 1; i > 0; i--)
      { // перемешиваем значения случайно в массиве так, что они не повторяются
        uint8_t idx = random(i + 1);
        uint8_t t = MusicList[i];
        MusicList[i] = MusicList[idx];
        MusicList[idx] = t;
      }
    }
  }
}

//********** Воспроизведение следующего трека **********
void play_next()
{
  if (player && !pause && PLAYtimer.isReady() && !nextTrack)
  {
    if (digitalRead(BUSY_PIN))
    {
      if (playOn)
      {
        playOn = false;
        myMP3.volume(volume2);
      }
      nextTrack = true;
      num2++;
    }
  }

  if (nextTrack)
  {
    PLAYtimer.reset();
    nextTrack = false;
    pause = false;
    oldNum = num2;
    if (num2 <= tracks2 - 1)
    {
      myMP3.playFolder(folder2, (MusicList[num2] + 1));
      if (!player)
      {
        player = true;
        if (MenuFlag == 3)
          oled_Player(0);
      }
      else
      {
        if (MenuFlag == 3)
          oled_Player(4);
      }
    }
    else
    {
      player = false;
      num2 = 0;
      myMP3.stop();
      delay(100);
      if (MenuFlag == 3)
        oled_Player(0);
      myMP3.volume(volume);
    }
  }
}

//********** Вывод на дисплей данных из Stroki_menu.h **********
void print_lcd(const uint8_t n)
{
  static char buf[32];
  strcpy_P(buf, (char *)pgm_read_word(&(text[n])));
  lcd.print(buf);
}

//********** Обработка кнопки на башне **********
#ifdef Button_Tower
void Button_Tower()
{
  static uint32_t butTowTime = 0;
  static uint8_t clickCount = 0;
  static bool lastStateBut = false;
  static bool flagDubl = false;
  if ((MenuFlag == 1 || MenuFlag == 2 || MenuFlag == 6) && ((!systemON && !returnMenu && !save && !playMush && barMan == 0) || promivka))
  {
    stateBut = digitalRead(BUT_TOWER_PIN);
    if (stateBut != lastStateBut)
    {
      lastStateBut = stateBut;
      if (!lastStateBut)
      {
        if (tost)
        {
          noTost = true;
        }
        else
        {
          clickCount++;
          butTowTime = millis();
          flagDubl = true;
        }
      }
    }
    if (flagDubl && millis() - butTowTime >= 400)
    { // двойной клик на башне
      flagDubl = false;
      if (clickCount >= 2)
      {
        if (MenuFlag != 6)
        {
          systemON = true;
          flag = true;
          if (noDoliv >= 1)
            readySystem = false;
          if (ledShowOn)
          {
            if (ledShow)
              check = true;
#ifdef LED_TOWER
            TOWERtimer.setInterval(50);
            if (mig || rainbow)
            {
              mig = false;
              rainbow = false;
              clearLed = true;
              TOWERtimer.reset();
            }
#endif
          }
        }
        else if (MenuFlag == 6)
        {
          flagMush = true;
        }
        SAVEtimer.reset();
      }
      clickCount = 0;
    }
  }
}
#endif

//********** Режим МУШКЕТЕРЫ **********
void play_Mushketery()
{
  static bool muveServo = false;
  static uint8_t countRand = 0;
  if (flagMush)
  {
    flagMush = false;
    countRand = 0;
    for (uint8_t i = 0; i < NUM_SHOTS; i++)
    {
      if (shotStates[i] == EMPTY)
        countRand++; // подсчёт стопок готовых к наливу
      rumka[i] = i;  // заполняем массив последовательно позициями  рюмок
    }
    if (countRand != 0)
    {
      playMush = true;
      if (volume != 0)
      {
        if (ledShowOn)
        {
          ledShow = true;
#ifdef LED_TOWER
          TOWERtimer.setInterval(20);
          rainbow = true;
#endif
        }
        if (player)
        {
          playOn = true;
          player = false;
          myMP3.stop();
          delay(100);
        }
        myMP3.volume(volume);
        delay(100);
        myMP3.playFromMP3Folder(15); // 0015 трек в папке MP3, рулетка
        lcd.clear();
        lcd.setCursor(2, 0);
        print_lcd(75); // КРУЧУ ВЕРЧУ
        lcd.setCursor(2, 1);
        print_lcd(76); // НАЛИТЬ ХОЧУ
        delay(500);
      }
    }
    else
    {
      systemON = true;
    }
  }

  if (playMush)
  {
    if (!muveServo)
    {
      Servo_move(shotPos[NUM_SHOTS - 1]);
      if (!moving)
        muveServo = true;
    }
    else
    {
      Servo_move(shotPos[0]);
      if (!moving)
        muveServo = false;
    }
    if (digitalRead(BUSY_PIN) || player)
    {
      moving = false;
      muveServo = false;
      systemON = true;
      flag = true; // флаг ,если пустые рюмки найдены показываем меню налива
      readySystem = false;
      countRand = 0;
      randomSeed(CreateTrulyRandomSeed());
      for (int8_t i = NUM_SHOTS - 1; i >= 0; i--)
      {
        int8_t idx = random(i + 1);
        int8_t t = rumka[i];
        rumka[i] = rumka[idx];
        rumka[idx] = t;
        if (shotStates[i] == EMPTY)
          countRand++; // снова подсчёт стопок готовых к наливу, вдруг кто снял во время трека рулетки
      }
      //randomGlass = random(countRand);
      randomGlass = 1;
      for (uint8_t i = 0; i < NUM_SHOTS; i++)
      {
        if (shotStates[rumka[i]] != EMPTY)
          rumka[i] = -1; // убираем позиции из разлива, где нет тары
        if (countRand > 1 && subMush == 2)
        {
          if (i == randomGlass && rumka[i] == -1)
            randomGlass++;
          else if (i == randomGlass)
            rumka[i] = -1; // убираем из разлива одну из установленных рюмок
        }
      }
      playMush = false;
      randomGlass = 0;
      if (ledShowOn)
      {
        if (ledShow)
          check = true;
#ifdef LED_TOWER
        TOWERtimer.setInterval(50);
        if (mig || rainbow)
        {
          mig = false;
          rainbow = false;
          clearLed = true;
          TOWERtimer.reset();
        }
#endif
      }
    }
  }
}

//***********************************************************************************************************
void move_enc(uint8_t *var, int16_t shift, int16_t lowLimit, int16_t upLimit, bool cycle)
{
  int16_t value = (int16_t)*var;
  value += shift;
  if (value > upLimit)
    value = (cycle) ? lowLimit : upLimit;
  if (value < lowLimit)
    value = (cycle) ? upLimit : lowLimit;
  *var = (uint8_t)value;
}

void move_enc(int16_t *var, int16_t shift, int16_t lowLimit, int16_t upLimit, bool cycle)
{
  *var += shift;
  if (*var > upLimit)
    *var = (cycle) ? lowLimit : upLimit;
  if (*var < lowLimit)
    *var = (cycle) ? upLimit : lowLimit;
}

void move_enc(int8_t *var, int16_t shift, int16_t lowLimit, int16_t upLimit, bool cycle)
{
  *var += shift;
  if (*var > upLimit)
    *var = (cycle) ? lowLimit : upLimit;
  if (*var < lowLimit)
    *var = (cycle) ? upLimit : lowLimit;
}

//***********************************************************************************************************
//********** Кнопки-крутилки **********
void EncTick()
{
  if (!returnMenu || promivka)
    Enc.tick();
  if (!save && !systemON && !tost && !playMush)
  {
    if (Enc.isTurn())
    { // если произошло движение энкодера в любую сторону
      SAVEtimer.reset();
      if (Enc.isLeft())
        drift--;
      if (Enc.isRight())
        drift++;
      switch (MenuFlag)
      {
      case 0:                               // главное меню
        move_enc(&Menu, drift, 0, 4, true); // Перемещение  по главному меню
        oled_main_screen();
        break;

      case 1: // меню автоналива
      case 6: // меню налива мушкетёров
        move_enc(&Drink, drift * 5, 20, MAX_DRINK, false);
        oled_Auto();
        break;

      case 3: // меню плеера
        if (folTra == 1)
        {
          move_enc(&folder2, drift, 10, 18, false);
          if (player)
          {
            player = false;
            myMP3.stop();
            oled_Player(0);
            delay(100);
          }
          tracks2 = myMP3.numTracksInFolder(folder2);
          if (tracks2 > 100)
            tracks2 = 100;
          num2 = 0;
          oled_Player(1);
        }
        else if (folTra == 2)
        {
          move_enc(&num2, drift, 0, tracks2, false);
          oled_Player(4);
        }
        else if (folTra == 3)
        {
          if (++mixMusic > 1)
            mixMusic = 0;
          oled_Player(5);
        }
        else if (folTra == 4)
        {
          move_enc(&volume2, drift, 0, 30, false);
          myMP3.volume(volume2);
          oled_Player(2);
        }
        break;

      case 4: // меню настроек
#ifdef BAT_MONITOR_ON
        move_enc(&subNastr, drift, 1, 9, true); // Перемещение  по меню настроек
#else
        move_enc(&subNastr, drift, 1, 8, true);                 // Перемещение  по меню настроек
#endif
        oled_Nastroiki();
        break;

      case 5:                                  // меню выбора игры мушкетёров
        move_enc(&subMush, drift, 1, 2, true); // Перемещение  по мушкетёрам
        oled_Mushketery();
        break;

      case 12: // меню настройки дриньков мультиразлива
        move_enc(&ManDrink[(ManRum - 1)], drift * 5, 20, MAX_DRINK, false);
        oled_Setup_Multirazliv();
        break;

      case 15: // меню настройки яркости led
        move_enc(&bright, drift * 5, 0, 255, false);
        oled_Brightness(1);
        Strip1.setBrightness(bright);
#ifdef LED_TOWER
        Strip2.setBrightness(bright);
#endif
        break;

      case 16: // меню включения-выключения бармена
        if (++barMan > 1)
          barMan = 0;
        oled_Bar_Man(1);
        break;

      case 19: // меню настройки времени таймера сна
        move_enc(&sleepTime, drift, 0, 19, false);
        oled_Sleep_Time(1);
        break;

      case 20: //  меню настройки времени налива 50 мл.
        move_enc(&time50ml, drift * 50, 1000, 9950, false);
        oled_Kalibr_Pump(1);
        break;

      case 30:                                 // меню выбора настройки тостов
        move_enc(&subMenu, drift, 1, 3, true); // Перемещение  по меню тостов
        oled_Tost();
        break;

      case 31: //  меню настройки громкости тостов
        move_enc(&volume, drift, 0, 30, false);
        oled_Volume(1);
        break;

      case 32: // меню включения-выключения перемешивания тостов
        if (++mixTost > 1)
          mixTost = 0;
        oled_mix_Tost();
        break;

      case 33:                                // меню выбора папки тостов
        move_enc(&folder, drift, 1, 9, true); // Перемещение  по меню папок
        tracks = myMP3.numTracksInFolder(folder);
        if (tracks > 100)
          tracks = 100;
        oled_Num_Folder(1);
        break;

      case 40:                                 // меню выбора настроек серво
        move_enc(&subMenu, drift, 1, 2, true); // Перемещение  по меню настроек серво
        oled_servo();
        break;

      case 41: // меню настройки позиции серво над рюмками
        move_enc(&servoPos, drift, 0, 180, false);
        servo.write(servoPos);
        oled_Servo_calibr(2);
        shotPos[count] = servoPos;
        break;

      case 42: // меню настройки скорости серво
        move_enc(&speedSer, -drift, 2, 30, false);
        oled_Servo_speed(1);
        break;
      }
      //Enc.rst();
      drift = 0;
    }

    if (Enc.isDouble())
    { // двойной клик кнопки энкодера
      if (MenuFlag == 3 && tracks2 != -1)
      {
        switch (folTra)
        {
        case 1:
          lcd.setCursor(6, 0);
          break; //  на выборе папки
        case 2:
          lcd.setCursor(10, 1);
          break; // на треках
        case 3:
          lcd.setCursor(8, 0);
          break; //  на рандоме
        case 4:
          lcd.setCursor(14, 0);
          break; //  на громкости
        case 5:
          lcd.setCursor(15, 1);
          break; //  на вкл-выкл play
        }
        print_lcd(79); // пробел
        if (!player)
        {
          (folTra >= 4) ? folTra = 1 : folTra++;
        }
        else
        {
          (folTra >= 5) ? folTra = 1 : folTra++;
          if (folTra == 3)
            folTra++;
        }

        oled_Player(3);
      }
      else
      {
        if (player)
        {
          pause = !pause;
          if (pause)
          {
            myMP3.pause();
          }
          else
          {
            myMP3.resume();
            PLAYtimer.reset();
          }
        }
      }
    }

    if (Enc.isHolded())
    { // длительное нажатие кнопки энкодера
      if (MenuFlag == 11 && !promivka)
      {
        promivka = true;
        systemON = true;
        flag = true;
      }
      else if (MenuFlag != 0 && MenuFlag <= 5)
      { //Выход в главное меню
        oled_main_screen();
        if (MenuFlag == 3)
        { // если были в меню плеера , то обновляем громкость плеера и рандом
#ifdef MEMORY_ON
          address = 100;
          EEPROM.update(address, volume2);
          delay(5);
          address = 110;
          EEPROM.update(address, mixMusic);
#endif
        }
        MenuFlag = 0;
      }
      else if (MenuFlag == 12)
      { //Выход из меню настроек мультиразлива в главное меню настроек
        MenuFlag = 4;
        oled_Nastroiki();
#ifdef MEMORY_ON
        address = 10;
        for (uint8_t i = 0; i < NUM_SHOTS; i++)
        {
          EEPROM.update(address, ManDrink[i]); // обновляем в памяти, дриньки для мультиразлива
          address++;
          delay(5);
        }
#endif
      }
      else if (MenuFlag == 31)
      { //Выход из меню громкости в меню тоста
        MenuFlag = 30;
        oled_Tost();
        myMP3.volume(volume);
#ifdef MEMORY_ON
        address = 20;
        EEPROM.update(address, volume);
#endif
      }
      else if (MenuFlag == 32)
      { //Выход из меню перемешивания  в меню тоста
        MenuFlag = 30;
        oled_Tost();
#ifdef MEMORY_ON
        address = 70;
        EEPROM.update(address, mixTost); // обновляем в памяти флаг перемешивания тостов
#endif
        mix(); // перемешиваем треки, если включено
      }
      else if (MenuFlag == 33)
      { //Выход из меню папок  в меню тоста
        MenuFlag = 30;
        oled_Tost();
        if (oldFolder != folder)
        {
          oldFolder = folder;
          mix(); // перемешиваем треки, если включено
        }
#ifdef MEMORY_ON
        address = 80;
        EEPROM.update(address, folder); // обновляем в памяти выбор папок
#endif
      }
      else if (MenuFlag == 41)
      { //Выход из меню  калибровки  сервы в меню настроек сервы
        MenuFlag = 40;
        count = -1;
        servoPos = 0;
        do
        {
#ifdef START_POS_SERVO_GLASS1
          Servo_move(shotPos[0]); // цель серво - первая рюмка
#else
          Servo_move(0);                                        // цель серво - 0
#endif
        } while (moving);
        oled_servo();
#ifdef MEMORY_ON
        address = 0;
        for (uint8_t i = 0; i < NUM_SHOTS; i++)
        {
          EEPROM.update(address, shotPos[i]); // обновляем в памяти положение для сервы
          address++;
          delay(5);
        }
#endif
      }
      else if (MenuFlag == 15)
      { //Выход из меню настройки яркости led
        MenuFlag = 4;
        oled_Nastroiki();
        check = true;
#ifdef LED_TOWER
        clearLed = true;
        TOWERtimer.reset();
#endif
#ifdef MEMORY_ON
        address = 30;
        EEPROM.put(address, bright); // обновляем в памяти яркость led
#endif
      }
      else if (MenuFlag == 16)
      { // выход из режима включения бармена
        MenuFlag = 4;
        oled_Nastroiki();
#ifdef MEMORY_ON
        address = 90;
        EEPROM.update(address, noDoliv); // обновляем в памяти долив
        delay(5);
        address = 120;
        EEPROM.update(address, barMan); // обновляем в памяти режим бармена
#endif
      }
      else if (MenuFlag == 42)
      { // выход из настроки скорости серво в меню настройки серво
        MenuFlag = 40;
        oled_servo();
#ifdef MEMORY_ON
        address = 40;
        EEPROM.update(address, speedSer); // обновляем в памяти скорость сервы
#endif
#ifdef BAT_MONITOR_ON
      }
      else if (MenuFlag == 18)
      { // выход из меню просмотра напряжения батареи в меню настроек
        MenuFlag = 4;
        oled_Nastroiki();
#endif
      }
      else if (MenuFlag == 19)
      { // выход из меню настройки таймера сна в менюнастроек
        MenuFlag = 4;
        oled_Nastroiki();
#ifdef MEMORY_ON
        address = 50;
        EEPROM.update(address, sleepTime); // обновляем в памяти таймер сна
#endif
        if (sleepTime != 0)
          SAVEtimer.setInterval(sleepTime * 30000UL);
      }
      else if (MenuFlag == 20)
      { // выход из меню настройки помпы в меню настроек
        MenuFlag = 4;
        do
        {
#ifdef START_POS_SERVO_GLASS1
          Servo_move(shotPos[0]); // цель серво - первая рюмка
#else
          Servo_move(0);                                        // цель серво - 0
#endif
        } while (moving);
        oled_Nastroiki();
#ifdef MEMORY_ON
        address = 60;
        EEPROM.put(address, time50ml); // обновляем в памяти время налива 50 мл.
#endif
      }
      else if (MenuFlag == 30 || MenuFlag == 40)
      { // выход из меню настройки тостов и серво в меню настроек
        MenuFlag = 4;
        oled_Nastroiki();
        if (playOn)
          player = true;
      }
      else if (MenuFlag == 6)
      { // выход из меню розлива мушкетёров  в меню мушкетёров
        MenuFlag = 5;
        oled_Mushketery();
      }
    }
  }

  if (Enc.isSingle())
  { // одиночный клик
    SAVEtimer.reset();
    if (save)
    {
      Play_track(17);
      save = false;
      lcd.backlight();
      check = true; //  проверяем рюмки
    }
    else
    {
      if (systemON || playMush)
      { // нажата кнопка во время работы наливатора, то отбой всего
        systemState = SEARCH;
        DrinkCount = 0;
        shotStates[curPumping] = READY;
        curPumping = -1; // снимаем выбор рюмки
        systemON = false;
        playMush = false;
        check = true;
        pumpOFF(); // помпу выкл
        moving = false;
        if (ledShowOn)
        {
          ledShow = false;
#ifdef LED_TOWER
          clearLed = true;
          rainbow = false;
          mig = false;
          TOWERtimer.reset();
#endif
        }
        lcd.clear();
        lcd.setCursor(5, 1);
        print_lcd(70); //  ОТБОЙ!
        Play_track(4); //  трек в папке mp3,

        do
        {
#ifdef START_POS_SERVO_GLASS1
          Servo_move(shotPos[0]); // цель серво - первая рюмка
#else
          Servo_move(0);                                        // цель серво - 0
#endif
        } while (moving);
        delay(3000);
        if (playOn)
        {
          player = true;
          PLAYtimer.reset();
          pause = false;
        }
        returnMenu = true;
      }
      else if (tost)
      { // если во время произношения тоста нажата кнопка, то отменяем воспроизведение
        if (!digitalRead(BUSY_PIN))
        {
          noTost = true;
          if (ledShowOn)
          {
            ledShow = false;
            check = true;
#ifdef LED_TOWER
            rainbow = false;
            clearLed = true;
            TOWERtimer.reset();
#endif
          }
        }
      }
      else if (Menu == 0 && MenuFlag == 0)
      { //Нажатие кнопки меню авто
        MenuFlag = 1;
        oled_Auto();
      }
      else if (MenuFlag == 1 || MenuFlag == 2)
      { //Начинается автоматический разлив
        if (barMan == 0 && readySystem)
        {
          systemON = true;
          flag = true; // флаг ,если пустые рюмки найдены показываем меню налива
          if (noDoliv == 1)
            readySystem = false;
#ifdef Button_Tower
          if (stateBut)
            promivka = true;
#endif
          if (ledShowOn)
          {
            if (ledShow)
              check = true;
#ifdef LED_TOWER
            TOWERtimer.setInterval(50);
            if (mig || rainbow)
            {
              mig = false;
              rainbow = false;
              clearLed = true;
              TOWERtimer.reset();
            }
#endif
          }
        }
      }
      else if (Menu == 1 && MenuFlag == 0)
      { // меню мультиразлив
        MenuFlag = 2;
        oled_Multirazliv();
      }
      else if (Menu == 2 && MenuFlag == 0)
      { // меню плеера
        if (!player)
        {
          tracks2 = myMP3.numTracksInFolder(folder2);
          if (tracks2 > 100)
            tracks2 = 100;
        }
        if (tracks2 == -1)
          folTra = 1; // галочка на выборе папок
        MenuFlag = 3;
        oled_Player(0);
      }
      else if (Menu == 2 && MenuFlag == 3)
      { // вошли в меню плеера
        if (!player && tracks2 != -1)
        {
          mix_music();
          nextTrack = true;
          myMP3.volume(volume2);
          folTra = 2;
        }
        else
        {
          if (folTra == 2)
          {
            if (oldNum == num2)
              num2++;
            nextTrack = true;
          }
          else if (folTra == 5)
          {
            player = false;
            myMP3.stop();
            folTra = 1;
            oled_Player(0);
            delay(100);
            myMP3.volume(volume);
          }
        }
      }
      else if (Menu == 3 && MenuFlag == 0)
      { // меню настроек
        MenuFlag = 4;
        oled_Nastroiki();
      }
      else if (Menu == 3 && MenuFlag == 4)
      { // вошли в меню настроек
        switch (subNastr)
        {
        case 1: // промывка
          MenuFlag = 11;
          oled_Promivka(0);
          break;
        case 2: //мультиразлив
          MenuFlag = 12;
          oled_Setup_Multirazliv();
          break;
        case 3: //   меню тосты
          MenuFlag = 30;
          if (player)
          {
            player = false;
            pause = false;
            myMP3.stop();
            playOn = true;
          }
          subMenu = 1;
          oled_Tost();
          break;
        case 4: // серво
          MenuFlag = 40;
          subMenu = 1;
          oled_servo();
          break;
        case 5: // яркость led
          MenuFlag = 15;
          oled_Brightness(0);
          break;
        case 6: // режим бармен
          MenuFlag = 16;
          oled_Bar_Man(0);
          break;
        case 7: // калибровка помпы
          MenuFlag = 20;
          oled_Kalibr_Pump(0);
          break;
        case 8: // таймер сна
          MenuFlag = 19;
          oled_Sleep_Time(0);
          break;
#ifdef BAT_MONITOR_ON
        case 9: // напряжение батареи
          MenuFlag = 18;
          oled_Bat_Volt();
          break;
#endif
        }
      }
      else if (Menu == 4 && MenuFlag == 0)
      { // меню мушкетёры
        MenuFlag = 5;
        oled_Mushketery();
      }
      else if (Menu == 4 && MenuFlag == 5)
      { // вошли в меню мушкетёры
        //barMan = 0;
        MenuFlag = 6;
        oled_Auto();
      }
      else if (MenuFlag == 6)
      { // начинаем розлив мушкетёров
        if (readySystem)
          flagMush = true;
      }
      else if (MenuFlag == 30 && subMenu == 1)
      { // вошли из меню тост в настройку громкости
        MenuFlag = 31;
        oled_Volume(0);
      }
      else if (MenuFlag == 30 && subMenu == 2)
      { // вошли из меню тост в настройку перемешивания треков
        MenuFlag = 32;
        oled_mix_Tost();
      }
      else if (MenuFlag == 30 && subMenu == 3)
      { // вошли из меню тост в настройку   папки проигрывания
        MenuFlag = 33;
        oled_Num_Folder(0);
      }
      else if (MenuFlag == 40 && subMenu == 1)
      { // вошли из меню сервы в настройку положения сервы над рюмками
        MenuFlag = 41;
        oled_Servo_calibr(0);
      }
      else if (MenuFlag == 40 && subMenu == 2)
      { // вошли из меню сервы в настройку  скорости сервы
        MenuFlag = 42;
        oled_Servo_speed(0);
      }
      else if (MenuFlag == 11)
      { // меню настройки промывка
        do
        {
#ifdef START_POS_SERVO_GLASS1
          Servo_move(shotPos[0]); // цель серво - первая рюмка
#else
          Servo_move(0);                                        // цель серво - 0
#endif
        } while (moving);
        oled_Nastroiki();
        MenuFlag = 4;
      }
      else if (MenuFlag == 12)
      { // меню настройки мультиналива
        if (++ManRum > 6)
          ManRum = 1;
        oled_Setup_Multirazliv();
      }
      else if (MenuFlag == 15)
      { // меню настройки яркости
        ledShowOn = !ledShowOn;
        oled_Brightness(2);
      }
      else if (MenuFlag == 16)
      { // меню настройки долив-бармен
        if (++noDoliv > 1)
          noDoliv = 0;
        oled_Bar_Man(2);
      }
      else if (MenuFlag == 31)
      { //меню настройки громкости
        if (volume != 0)
          tost = true; // проверяем громкость
      }
      else if (MenuFlag == 42)
      { // двигаем серво туда-обратно для наглядности установленной скорости
        do
        {
          Servo_move(180);
        } while (moving);

        do
        {
#ifdef START_POS_SERVO_GLASS1
          Servo_move(shotPos[0]); // цель серво - первая рюмка
#else
          Servo_move(0);                                        // цель серво - 0
#endif
        } while (moving);
      }
      else if (MenuFlag == 20)
      { // наливаем в рюмку для проверки величины налива, после изменения времени налива
        Drink = 50;
        systemON = true;
        oled_Kalibr_Pump(1);
      }

      if (MenuFlag == 41)
      { //Калибровка серво
        if (++count >= NUM_SHOTS)
          count = 0;

        servoPos = shotPos[count];
        oled_Servo_calibr(1);
        do
        {
          Servo_move(servoPos);
        } while (moving);
      }
    }
  }
}

//********** Основной экран "НАЛИВАТОР+" **********
void oled_main_screen()
{
  lcd.clear();
  lcd.setCursor(3, 0);
  print_lcd(7); // НАЛИВАТОР+
#ifdef BAT_MONITOR_ON
  lcd.setCursor(15, 0);
  lcd.write(7);
#endif
  lcd.setCursor(0, 1);
  print_lcd(66); // >
  lcd.setCursor(15, 1);
  print_lcd(65); // <
  switch (Menu)
  {
  case 0:
    lcd.setCursor(6, 1);
    print_lcd(8); //  АВТО
    break;
  case 1:
    lcd.setCursor(2, 1);
    print_lcd(9); //  МУЛЬТИРАЗЛИВ
    break;
  case 2:
    lcd.setCursor(5, 1);
    print_lcd(10); //  ПЛЕЕР
    break;
  case 3:
    lcd.setCursor(3, 1);
    print_lcd(11); //  НАСТРОЙКИ
    break;

  case 4:
    lcd.setCursor(3, 1);
    print_lcd(71); //  МУШКЕТЁРЫ
    break;
  }
}

//********** Экраны меню "НАСТРОЙКИ" **********
void oled_Nastroiki()
{
  lcd.clear();
  lcd.setCursor(3, 0);
  print_lcd(11); // НАСТРОЙКИ
#ifdef BAT_MONITOR_ON
  lcd.setCursor(15, 0);
  lcd.write(7);
#endif
  lcd.setCursor(0, 1);
  print_lcd(66); // >
  lcd.setCursor(15, 1);
  print_lcd(65); // <
  switch (subNastr)
  {
  case 1:
    lcd.setCursor(4, 1);
    print_lcd(12); // ПРОМЫВКА
    break;
  case 2:
    lcd.setCursor(2, 1);
    print_lcd(9); // МУЛЬТИРАЗЛИВ
    break;
  case 3:
    lcd.setCursor(5, 1);
    print_lcd(13); // ТОСТЫ
    break;

  case 4:
    lcd.setCursor(2, 1);
    print_lcd(14); // МЕНЮ  СЕРВО
    break;

  case 5:
    lcd.setCursor(2, 1);
    print_lcd(15); // ЯРКОСТЬ  LED
    break;

  case 6:
    lcd.setCursor(2, 1);
    print_lcd(16); // БАРМЕН-ДОЛИВ
    break;

  case 7:
    lcd.setCursor(2, 1);
    print_lcd(19); // КАЛИБР.ПОМПЫ
    break;

  case 8:
    lcd.setCursor(3, 1);
    print_lcd(18); // ТАЙМЕР СНА
    break;
#ifdef BAT_MONITOR_ON
  case 9:
    lcd.setCursor(1, 1);
    print_lcd(17); // НАПРЯЖЕНИЕ АКБ
    break;
#endif
  }
}

//********** Экран режима "АВТО" **********
void oled_Auto()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  print_lcd(20); // HАЛИTЬ
  lcd.setCursor(7, 0);
  print_lcd(29); //  ПО    ml
  lcd.setCursor(10, 0);
  lcd.print(Drink, DEC);
#ifdef BAT_MONITOR_ON
  lcd.setCursor(15, 0);
  lcd.write(7);
#endif
  lcd.setCursor(0, 1);
  if (Drink < 25)
    print_lcd(21); // HИ O ЧEM
  else if (Drink < 30)
    print_lcd(22); // ПO ЧУTЬ - ЧУTЬ
  else if (Drink < 40)
    print_lcd(23); // B CAMЫЙ PAЗ
  else if (Drink < 45)
    print_lcd(24); // ПО  ПОЛНОЙ
  else
    print_lcd(25); //  ДО КРАЕВ

  if (barMan == 1)
  {
    lcd.setCursor(15, 1);
    print_lcd(38); //  B
  }
  if (MenuFlag == 6)
  {
    lcd.setCursor(15, 1);
    print_lcd(74); // M
  }
}

//********** Экран режима "НАЛИВАЮ" **********
void oled_Nalivau()
{
  lcd.clear();
  lcd.setCursor(2, 0);
  print_lcd(26); //  НАЛИВАЮ     ml
  lcd.setCursor(0, 1);
  print_lcd(27); //  В  -Ю РЮМКУ   ml
}

//********** Экран режима "HАЛИТО" **********
void oled_Nalito()
{
  lcd.clear();
  if (MenuFlag == 2)
  {
    lcd.setCursor(5, 0);
  }
  else
  {
    lcd.setCursor(7, 0);
    print_lcd(29); //  ПО    ml
    lcd.setCursor(10, 0);
    lcd.print(Drink, DEC);
    lcd.setCursor(0, 0);
  }
  print_lcd(28); // HАЛИТО
  lcd.setCursor(0, 1);
  print_lcd(30); // В   РЮМ
  lcd.setCursor(5, 1);
  lcd.print(DrinkCount, DEC);
  lcd.setCursor(10, 1);
  if (DrinkCount == 1)
    print_lcd(31); // КУ
  else if (DrinkCount <= 4)
    print_lcd(32); // КИ
  else
    print_lcd(33); // ОК
}

// ********** Экран меню НАСТРОЙКИ СЕРВО КАЛИБРОВКА **********
void oled_Servo_calibr(uint8_t subServo)
{
  if (subServo == 0)
  {
    lcd.clear();
    lcd.setCursor(4, 0);
    print_lcd(34); // -я рюмка
    lcd.setCursor(3, 1);
    print_lcd(35); // поз.:
  }
  if (subServo == 1)
  {
    lcd.setCursor(3, 0);
    lcd.print((count + 1), DEC);
  }
  if (subServo == 2 || subServo == 1)
  {
    lcd.setCursor(10, 1);
    print_lcd(4); // пробел 3
    lcd.setCursor(9, 1);
    lcd.print(servoPos, DEC);
  }
}

//********** Экран режима МУЛЬТИРАЗЛИВ **********
void oled_Multirazliv()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  print_lcd(36); //  1-   2-   3-
#ifdef BAT_MONITOR_ON
  lcd.setCursor(15, 0);
  lcd.write(7);
#endif
  lcd.setCursor(2, 0);
  lcd.print(ManDrink[0], DEC);
  lcd.setCursor(7, 0);
  lcd.print(ManDrink[1], DEC);
  lcd.setCursor(12, 0);
  lcd.print(ManDrink[2], DEC);
  lcd.setCursor(0, 1);
  print_lcd(37); //   4-   5-   6-
  lcd.setCursor(2, 1);
  lcd.print(ManDrink[3], DEC);
  lcd.setCursor(7, 1);
  lcd.print(ManDrink[4], DEC);
  lcd.setCursor(12, 1);
  lcd.print(ManDrink[5], DEC);
  if (barMan == 1)
  {
    lcd.setCursor(15, 1);
    print_lcd(38); //  B
  }
}

//********** Экран настройки МУЛЬТИРАЗЛИВ **********
void oled_Setup_Multirazliv()
{
  lcd.clear();
  lcd.setCursor(0, 1);
  if (ManDrink[(ManRum - 1)] < 25)
    print_lcd(21); // НИ О ЧЕМ
  else if (ManDrink[(ManRum - 1)] < 30)
    print_lcd(22); // ПО ЧУТЬ - ЧУТЬ
  else if (ManDrink[(ManRum - 1)] < 40)
    print_lcd(23); // В САМЫЙ  РАЗ
  else if (ManDrink[(ManRum - 1)] < 45)
    print_lcd(24); // ПО  ПОЛНОЙ
  else
    print_lcd(25); // ДО КРАЕВ

  lcd.setCursor(0, 0);
  lcd.print(ManRum, DEC);
  lcd.setCursor(1, 0);
  print_lcd(39); // -Я РЮМКА:   ml
  lcd.setCursor(10, 0);
  lcd.print(ManDrink[(ManRum - 1)], DEC);
}

//********** Экраны меню "НАСТРОЙКИ" -> "ТОСТЫ" **********
void oled_Tost()
{
  lcd.clear();
  lcd.setCursor(5, 0);
  print_lcd(13); // ТОСТЫ
  switch (subMenu)
  {
  case 1:
    lcd.setCursor(3, 1);
    print_lcd(40); // ГР.ОЗВУЧКИ
    break;

  case 2:
    lcd.setCursor(0, 1);
    print_lcd(41); // ПЕРЕМЕШАТЬ ТРЕКИ
    break;

  case 3:
    lcd.setCursor(0, 1);
    print_lcd(42); // ПАПКА С ТРЕКАМИ
    break;
  }
}

//********** Настройки -> Тосты -> Папка с треками **********
void oled_Num_Folder(uint8_t subFolder)
{
  if (subFolder == 0)
  {
    lcd.clear();
    lcd.setCursor(3, 0);
    print_lcd(43); // ДИР:
    lcd.setCursor(2, 1);
    print_lcd(44); // ТРЕКОВ:
  }
  if (subFolder == 1 || subFolder == 0)
  {
    lcd.setCursor(11, 0);
    lcd.print(folder, DEC);
    lcd.setCursor(11, 1);
    if (tracks == -1)
    {
      print_lcd(45); // НЕТ
    }
    else
    {
      if (subFolder != 0)
        print_lcd(4); // пробел 3
      lcd.setCursor(11, 1);
      lcd.print(tracks, DEC);
    }
  }
}

//********** Экран меню "ПЛЕЕР" **********
void oled_Player(uint8_t subPlay)
{
  if (subPlay == 0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    print_lcd(43); // ДИР:
    lcd.setCursor(9, 0);
    print_lcd(67); // ГР:
#ifdef BAT_MONITOR_ON
    lcd.setCursor(15, 0);
    lcd.write(7);
#endif
    lcd.setCursor(0, 1);
    print_lcd(68); // ТР:
    lcd.setCursor(11, 1);
    if (player)
      print_lcd(46); // PLAY
  }
  if (subPlay == 1 || subPlay == 0)
  {
    lcd.setCursor(4, 0);
    lcd.print(folder2, DEC);
    lcd.setCursor(3, 1);
    if (tracks2 == -1)
    {
      print_lcd(45); // НЕТ
    }
    else
    {
      if (subPlay != 0)
      {
        print_lcd(4); // тройной пробел
      }
      lcd.setCursor(3, 1);
      lcd.print(tracks2, DEC);
    }
  }
  if (subPlay == 2 || subPlay == 0)
  {
    if (subPlay != 0)
    {
      lcd.setCursor(13, 0);
      print_lcd(79); // пробел
    }
    lcd.setCursor(12, 0);
    lcd.print(volume2, DEC);
  }
  if (subPlay == 3 || subPlay == 0)
  {
    switch (folTra)
    {
    case 1:
      lcd.setCursor(6, 0);
      break; // галочка на выборе папки
    case 2:
      lcd.setCursor(10, 1);
      break; // галочка на треках
    case 3:
      lcd.setCursor(8, 0);
      break; // галочка на вкл-выкл рандом
    case 4:
      lcd.setCursor(14, 0);
      break; // галочка на громкости
    case 5:
      lcd.setCursor(15, 1);
      break; // галочка на вкл-выкл play
    }
    print_lcd(65); // <
  }
  if (subPlay == 4 || subPlay == 1 || subPlay == 0)
  {
    if (subPlay != 0)
    {
      lcd.setCursor(7, 1);
      print_lcd(4); // тройной пробел
    }
    if (tracks2 != -1)
    {
      lcd.setCursor(7, 1);
      lcd.print((MusicList[num2] + 1), DEC);
    }
  }
  if (subPlay == 5 || subPlay == 0)
  {
    lcd.setCursor(7, 0);
    if (mixMusic == 1)
      print_lcd(80); // R
    else
      print_lcd(81); // L
  }
}

//********** Экраны меню "НАСТРОЙКИ" -> "НАСТРОЙКИ СЕРВО" **********
void oled_servo()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  print_lcd(47); // НАСТРОЙКИ  СЕРВО
  switch (subMenu)
  {
  case 1:
    lcd.setCursor(3, 1);
    print_lcd(48); // КАЛИБРОВКА
    break;

  case 2:
    lcd.setCursor(4, 1);
    print_lcd(49); // СКОРОСТЬ
    break;
  }
}

//********** Экран меню "ГРОМКОСТЬ" **********
void oled_Volume(uint8_t subVol)
{
  if (subVol == 0)
  {
    lcd.clear();
    lcd.setCursor(0, 1);
    print_lcd(50); // ГРОМКОСТЬ
  }
  if (subVol == 1 || subVol == 0)
  {
    if (subVol != 0)
    {
      lcd.setCursor(12, 1);
      print_lcd(4); // пробел 3
    }
    lcd.setCursor(13, 1);
    lcd.print(volume, DEC);
  }
}

//********** Экран меню "ЯРКОСТЬ" **********
void oled_Brightness(uint8_t subBrigh)
{
  if (subBrigh == 0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    print_lcd(51); // ЯРКОСТЬ LED
    lcd.setCursor(10, 0);
    print_lcd(69); // SHOW
  }
  if (subBrigh == 1 || subBrigh == 0)
  {
    lcd.setCursor(2, 1);
    print_lcd(4); // пробел 3
    lcd.setCursor(2, 1);
    lcd.print((map(bright, 0, 254, 0, 99)), DEC);
  }
  if (subBrigh == 2 || subBrigh == 0)
  {
    lcd.setCursor(10, 1);
    if (ledShowOn)
      print_lcd(54); // ВКЛ.
    else
      print_lcd(55); // ВЫКЛ.
  }
}

//********** Экран режима "ПРОМЫВКА" **********
void oled_Promivka(uint8_t subPromivka)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  print_lcd(52); // П Р О М Ы В К А
  if (subPromivka == 1)
  {
    lcd.setCursor(2, 1);
    print_lcd(53); // >>>>>>>>>>>>
  }
  else if (subPromivka == 2)
  {
    lcd.setCursor(5, 1);
    lcd.print(Procent, DEC);
    lcd.setCursor(10, 1);
    print_lcd(63); // мс
  }
}

//********** Экран меню Настройки -> Бармен-Долив **********
void oled_Bar_Man(uint8_t subBarmen)
{
  if (subBarmen == 0)
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    print_lcd(78); // БАРМЕН
    lcd.setCursor(10, 0);
    print_lcd(77); // ДОЛИВ
  }
  if (subBarmen == 1 || subBarmen == 0)
  {
    lcd.setCursor(1, 1);
    if (barMan == 1)
      print_lcd(54); // ВКЛ.
    else
      print_lcd(55); // ВЫКЛ.
  }
  if (subBarmen == 2 || subBarmen == 0)
  {
    lcd.setCursor(10, 1);
    if (noDoliv == 1)
      print_lcd(55); // ВЫКЛ.
    else
      print_lcd(54); // ВКЛ
  }
}

//********** Экран меню Настройки -> Тосты -> Перемешать треки **********
void oled_mix_Tost()
{
  lcd.clear();
  lcd.setCursor(6, 1);
  if (mixTost == 1)
    print_lcd(54); // ВКЛ.
  else
    print_lcd(55); // ВЫКЛ.
}

//********** Экран меню НАСТРОЙКИ -> МЕНЮ СЕРВО -> СКОРОСТЬ **********
void oled_Servo_speed(uint8_t subSS)
{
  if (subSS == 0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    print_lcd(49); //  СКОРОСТЬ
  }
  if (subSS == 1 || subSS == 0)
  {
    if (subSS != 0)
    {
      lcd.setCursor(12, 0);
      print_lcd(4); // пробел 3
    }
    lcd.setCursor(12, 0);
    lcd.print((map(speedSer, 2, 30, 30, 2)), DEC);
  }
}

//********** Экран меню состояния батареи **********
#ifdef BAT_MONITOR_ON
void oled_Bat_Volt()
{
  lcd.clear();
  lcd.setCursor(3, 0);
  print_lcd(56); //  НАПРЯЖЕНИЕ
  lcd.setCursor(5, 1);
  lcd.print((value * ADC_U_COEFF), 2);
  lcd.setCursor(10, 1);
  print_lcd(57); // V
}
#endif


//********** Экран настройки таймер сна **********
void oled_Sleep_Time(uint8_t subTime)
{
  if (subTime == 0)
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    print_lcd(58); // ЗАСНУТЬ ЧЕРЕЗ
  }
  if (subTime == 1 || subTime == 0)
  {
    if (sleepTime == 0)
    {
      if (subTime != 0)
      {
        lcd.setCursor(10, 1);
        print_lcd(4); // пробел 3
      }
      lcd.setCursor(5, 1);
      print_lcd(55); // ВЫКЛ.
    }
    else
    {
      lcd.setCursor(5, 1);
      lcd.print((map(sleepTime, 0, 20, 0, 10)), DEC);
      lcd.setCursor(6, 1);
      if (sleepTime % 2 != 0)
        print_lcd(59); // .30
      else
        print_lcd(60); // .00

      lcd.setCursor(9, 1);
      print_lcd(61); // МИН.
    }
  }
}


//********** Экраны меню "НАСТРОЙКИ" -> "КАЛИБРОВКА ПОМПЫ" **********
void oled_Kalibr_Pump(uint8_t subPump)
{
  if (subPump == 0)
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    print_lcd(62); // НАЛИВ 50ml ЗА:
    lcd.setCursor(8, 1);
    print_lcd(63); // мс
  }
  if (subPump == 1 || subPump == 0)
  {
    lcd.setCursor(3, 1);
    lcd.print(time50ml, DEC);
    lcd.setCursor(12, 1);
    print_lcd(64); // ml
  }
}


//********** Экраны меню "МУШКЕТЁРЫ" **********
void oled_Mushketery()
{
  lcd.clear();
  lcd.setCursor(3, 0);
  print_lcd(71); // МУШКЕТЁРЫ
#ifdef BAT_MONITOR_ON
  lcd.setCursor(15, 0);
  lcd.write(7);
#endif
  switch (subMush)
  {
  case 1:
    lcd.setCursor(2, 1);
    print_lcd(72); // ОДИН ЗА ВСЕХ
    break;
  case 2:
    lcd.setCursor(1, 1);
    print_lcd(73); // ВСЕ ЗА ОДНОГО
    break;
  }
}

//********** Светомузыка **********
void CvetoMuzik()
{
  static uint8_t led = 0;
  static uint8_t count = 0;
  static int16_t col = 0;
#ifdef LED_TOWER
  static bool flag = false;
  static uint8_t count2 = 0;
  static uint8_t count3 = 0;
  static uint8_t count4 = 0;
#endif
  if ((MenuFlag == 15 || ledShow || systemState == PUMPING) && LEDtimer.isReady())
  {
    if (systemState == PUMPING)
    {
      if (++count > 3)
      {
        count = 0;
#ifndef SERVO_CHANGE_DIRECTION
        Strip1.setLED(curPumping, mWHEEL(col)); // зажгли цвет
#else
        Strip1.setLED(NUM_SHOTS - 1 - curPumping, mWHEEL(col)); // зажгли цвет
#endif
        Strip1.show();
        col += 450;
        if (col > 1350)
          col = 0;
      }
    }
    else
    {
#ifndef SERVO_CHANGE_DIRECTION
      Strip1.setLED(led, mWHEEL(col));
#else
      Strip1.setLED(NUM_SHOTS - 1 - led, mWHEEL(col));
#endif
      Strip1.show();
      if (++led >= NUM_SHOTS)
      {
        led = 0;
        col += 450;
        if (col > 1350)
        {
          col = 0;
          if (++count == 6)
          { // колличество проходов иллюминации после налива
            count = 0;
            if (MenuFlag != 15 && !playMush)
              check = true; // если не в меню настроек, то проверяем рюмки
          }
        }
      }
    }
  }
  if (check)
  { // проверка рюмок
    for (led = 0; led < NUM_SHOTS; led++)
    {
#ifndef SERVO_CHANGE_DIRECTION
      if (shotStates[led] == READY)
        Strip1.setLED(led, mRGB(0, 255, 0)); // налитая рюмка, статус: готов
      else if (shotStates[led] == EMPTY)
        Strip1.setLED(led, mRGB(255, 0, 0));
      else if (shotStates[led] == NO_GLASS)
        Strip1.setLED(led, mRGB(0, 0, 0));
#else
      if (shotStates[led] == READY)
        Strip1.setLED(NUM_SHOTS - 1 - led, mRGB(0, 255, 0)); // налитая рюмка, статус: готов
      else if (shotStates[led] == EMPTY)
        Strip1.setLED(NUM_SHOTS - 1 - led, mRGB(255, 0, 0));
      else if (shotStates[led] == NO_GLASS)
        Strip1.setLED(NUM_SHOTS - 1 - led, mRGB(0, 0, 0));
#endif
    }
    LEDtimer.reset();
    LEDchanged = true;
    check = false;
    ledShow = false;
    led = 0;
    col = 0;
    count = 0;
  }
  // отрисовка светодиодов по флагу
  if (!ledShow && LEDchanged && LEDtimer.isReady())
  {
    LEDchanged = false;
    Strip1.show();
  }
  // Башня LED
#ifdef LED_TOWER
  if (mig && !clearLed && TOWERtimer.isReady())
  {
    if (flag)
    {
      Leds2[count2] = mRGB(255, 0, 0);
      Leds2[count2 + (NUMLEDS / 2)] = mRGB(0, 0, 255);
    }
    else
    {
      Leds2[count2 + NUMLEDS / 2] = mRGB(255, 0, 0);
      Leds2[count2] = mRGB(0, 0, 255);
    }
    Strip2.show();
    if (++count2 >= NUMLEDS / 2)
    {
      count2 = 0;
      flag = !flag;
    }
    if (++count3 > 90)
    { // продолжительность мигалки у.е.
      mig = false;
      clearLed = true;
      TOWERtimer.reset();
    }
  }
  if ((MenuFlag == 15 || rainbow) && !clearLed && TOWERtimer.isReady())
  {
    Leds2[count2] = mHSV((uint8_t)(count3 + count2 * (255 / NUMLEDS)), 255, 255); //
    count3 += 4;
    Strip2.show();
    if (++count2 >= NUMLEDS)
    {
      count2 = 0;
      if (!playMush && MenuFlag != 15 && ++count4 > 25)
      { // продолжительность радуги у.е.
        rainbow = false;
        clearLed = true;
        TOWERtimer.reset();
      }
    }
  }
  if (systemON && ledShowOn && !clearLed && !promivka && MenuFlag != 20 && TOWERtimer.isReady())
  {
    if (++count2 >= 7)
      count2 = 0;
    if (++count4 >= 21)
      count4 = 0;
    Strip2.clear();
    for (count3 = count2; count3 < NUMLEDS; count3 += 7)
    {
      if (count4 < 7)
      {
        Strip2.setRGB(count3, 255, 0, 0);
      }
      else if (count4 < 14)
      {
        Strip2.setRGB(count3, 0, 255, 0);
      }
      else if (count4 < 21)
      {
        Strip2.setRGB(count3, 0, 0, 255);
      }
    }
    Strip2.show();
  }
  if (clearLed && TOWERtimer.isReady())
  {
    clearLed = false;
    count2 = 0;
    count3 = 0;
    count4 = 0;
    Strip2.clear();
    Strip2.show();
    TOWERtimer.reset();
  }
#endif
}

//********** Экран вывода тостов **********
void Tost()
{
  static bool readyTost = false;
  if (tost)
  {
    if (!readyTost && PAUSEtimer.isReady())
    {
      if (volume != 0 && tracks != -1)
      {
        myMP3.volume(volume);
        lcd.clear();
        lcd.setCursor(4, 0);
        print_lcd(2); // ТРЕК -
        lcd.setCursor(11, 0);
        lcd.print((TostList[num] + 1), DEC);
        lcd.setCursor(2, 1);
        print_lcd(3); // !!! ТОСТ !!!
        myMP3.playFolder(folder, (TostList[num] + 1));
        if (++num >= tracks)
          num = 0;
      }
      readyTost = true;
      PAUSEtimer.setInterval(500);
      PAUSEtimer.reset();
    }
    if ((readyTost && ((PAUSEtimer.isReady() && digitalRead(BUSY_PIN)) || noTost)) || volume == 0 || tracks == -1)
    {
      tost = false;
      if (noTost)
      {
        noTost = false;
        myMP3.stop();
      }
      SAVEtimer.reset();
      readyTost = false;
      if (MenuFlag == 31)
      {
        oled_Volume(0);
      }
      else
      {
        if (playOn)
        {
          player = true;
          PLAYtimer.reset();
          pause = false;
        }
        returnMenu = true;
      }
    }
  }
}
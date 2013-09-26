//---------------------------------------------------------------------------

/*

Процедура адаптивного арифметического кодирования последовательно вызывает
функции инициализации адаптивной модели void start_model (void),
инициализации побитового вывода void start_outputing_bits (void)
инициализации регистров границ и кода перед началом сжатия void start_encoding (void).
Далее в цикле ведется посимвольное считывание потока, для каждого символа
вызывается процедура кодирования очередного символа
void encode_symbol (int symbol) и сразу после этого процедура
обновления модели очередным символом void update_model (int symbol).
Считывание ведётся до тех пор, пока не встретится
символ конца файла EOF. Далее кодируется сам символ EOF.
После этого выполняется очистка побитового вывода
void done_encoding (void) и очистка буфера побитового вывода
void done_outputing_bits (void). Процедура кодирования очередного
символа void encode_symbol (int symbol) включает в себя собственно
реализацию алгоритма арифметического кодирования отдельного символа с
пересчётом значений границ и выдвиганием очередных битов, а также вывод
указанного бита и отложенных ранее в файл
void output_bit_plus_follow ( int bit) и void output_bit ( int bit).
Процедура адаптивного арифметического декодирования
производится аналогично процедуре кодирования.
Процедура декодирования очередного символа int decode_symbol (void)
определяет текущий масштаба частот, масштабирует значения в регистре кода,
ищет соответствующего символа в таблице частот, делает пересчет границ и
производит удаление очередного символа из входного потока int input_bit (void).
Процедура обновления модели void update_model ( int symbol)
обновляет значений в таблицах частот, перед этим проверяя
счетчик частоты на переполнение и масштабируя частоты при переполнении

*/



#include <vcl.h>
#include <stdio.h>
#include <process.h>
#include <time.h>

#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;


AnsiString fileopen = 0;
AnsiString filesave = 0;

// Количество битов в регистре
#define BITS_IN_REGISTER 16

// Максимально возможное значение в регистре
#define TOP_VALUE (((long) 1 << BITS_IN_REGISTER) - 1)

// Диапазоны
#define FIRST_QTR (TOP_VALUE / 4 + 1)
#define HALF      (2 * FIRST_QTR)
#define THIRD_QTR (3 * FIRST_QTR)

// Количество символов алфавита
#define NO_OF_CHARS 256
// Специальный символ КонецФайла
#define EOF_SYMBOL    (NO_OF_CHARS + 1)
// Всего символов в модели
#define NO_OF_SYMBOLS (NO_OF_CHARS + 1)

// Порог частоты для масштабирования
#define MAX_FREQUENCY 16383

// Таблицы перекодировки
unsigned char index_to_char [NO_OF_SYMBOLS];
int char_to_index [NO_OF_CHARS];

// Таблицы частот
int cum_freq [NO_OF_SYMBOLS + 1];
int freq [NO_OF_SYMBOLS + 1];

// Регистры границ и кода
long low, high;
long value;

// Поддержка побитлвых операций с файлами
long bits_to_follow;
int buffer;
int bits_to_go;
int garbage_bits;

// Обрабатываемые файлы

FILE *in, *out;

// Инициализация адаптивной модели

void start_model (void)
{
  int i;

  for ( i = 0; i < NO_OF_CHARS; i++)
  {
    char_to_index [i] = i + 1;
    index_to_char [i + 1] = i;
  }
  for ( i = 0; i <= NO_OF_SYMBOLS; i++)
  {
    freq [i] = 1;
    cum_freq [i] = NO_OF_SYMBOLS - i;
  }
  freq [0] = 0;
}

//------------------------------------------------------------
// Обновление модели очередным символом

void update_model ( int symbol)
{
  int i;
  int ch_i, ch_symbol;
  int cum;

  /*
  Проверка на переполнение счетчика частоты производится при обновлении
  модели очередным символом в функции void update_model ( int symbol).
  При переполнении производится масштабирование частот (все частоты делятся на 2)
  */
  // проверка на переполнение счетчика частоты
  if (cum_freq [0] == MAX_FREQUENCY)
  {
    cum = 0;
    // масштабирование частот при переполнении
    for ( i = NO_OF_SYMBOLS; i >= 0; i--)
    {
      freq [i] = (freq [i] + 1) / 2;
      cum_freq [i] = cum;
      cum += freq [i];
    }
  }

  /*
  символ перемещается по таблице частот выше символов с
  такой же частотой, чтобы его новая частота
  (увеличенная на единицу старая) не нарушала сортировку таблицы
  */
  for ( i = symbol; freq [i] == freq [i - 1]; i--);
  if (i < symbol)
  {
    ch_i = index_to_char [i];
    ch_symbol  = index_to_char [symbol];
    index_to_char [i]  = ch_symbol;
    index_to_char [symbol] = ch_i;
    char_to_index [ch_i] = symbol;
    char_to_index [ch_symbol] = i;
  }

  // обновление значений в таблицах частот
  //Для каждого символа проводится обновление значений в таблицах частот
  freq [i] += 1;
  while (i > 0)
  {
    i -= 1;
    cum_freq [i] += 1;
  }
}

//------------------------------------------------------------
// Инициализация побитового ввода

void start_inputing_bits (void)
{
  bits_to_go = 0;
  garbage_bits = 0;
}

//------------------------------------------------------------
// Ввод очередного бита сжатой информации

int input_bit (void)
{
  int t;

  if (bits_to_go == 0)
  {
    buffer = getc (in);
    if (buffer == EOF)
    {
      garbage_bits += 1;
      if (garbage_bits > BITS_IN_REGISTER - 2)
      {
	printf ("Ошибка в сжатом файле\n");
	exit (-1);
      }
    }
    bits_to_go = 8;
  }
  t = buffer & 1;
  buffer >>= 1;
  bits_to_go -= 1;
  return t;
}

//------------------------------------------------------------
// Инициализация побитового вывода

void start_outputing_bits (void)
{
  buffer = 0;
  bits_to_go = 8;
}

//------------------------------------------------------------
// Вывод очередного бита сжатой информации

void output_bit ( int bit)
{
  buffer >>= 1;
  if (bit)
    buffer |= 0x80;
  bits_to_go -= 1;
  if (bits_to_go == 0)
  {
    putc ( buffer, out);
    bits_to_go = 8;
  }
}

//------------------------------------------------------------
// Очистка буфера побитового вывода

void done_outputing_bits (void)
{
  putc ( buffer >> bits_to_go, out);
}

//------------------------------------------------------------
// Вывод указанного бита и отложенных ранее

void output_bit_plus_follow ( int bit)
{
  output_bit (bit);
  while (bits_to_follow > 0)
  {
    output_bit (!bit);
    bits_to_follow--;
  }
}

//------------------------------------------------------------
// Инициализация регистров границ и кода перед началом сжатия

void start_encoding (void)
{
  low = 0l;
  high = TOP_VALUE;
  bits_to_follow = 0l;
}

//------------------------------------------------------------
// Очистка побитового вывода

void done_encoding (void)
{
  bits_to_follow++;
  if (low < FIRST_QTR)
    output_bit_plus_follow (0);
  else
    output_bit_plus_follow (1);
}

//------------------------------------------------------------
/* Инициализация регистров перед декодированием.
   Загрузка начала сжатого сообщения
*/

void start_decoding (void)
{
  int i;

  value = 0l;
  for ( i = 1; i <= BITS_IN_REGISTER; i++)
    value = 2 * value + input_bit ();
  low = 0l;
  high = TOP_VALUE;
}

//------------------------------------------------------------
// Кодирование очередного символа

void encode_symbol ( int symbol)        //включает в себя собственно реализацию алгоритма арифметического кодирования отдельного символа
{
  long range;

  // пересчет значений границ
  /*
  high и low - это переменные, обозначающие верхнюю и нижнюю
  границы текущего интервала соответственно, а range - это длина этого интервала
  */
  range = (long) (high - low) + 1;
  high = low + (range * cum_freq [symbol - 1]) / cum_freq [0] - 1;
  low = low + (range * cum_freq [symbol]) / cum_freq [0];
  // выдвигание очередных битов
  /*
  в цикле происходит определение числа,
  которое следует записать в выходной файл,
  и которое будет обозначать данный кодируемый символ
  */
  for (;;)
  {
    if (high < HALF)
      output_bit_plus_follow (0);
    else if (low >= HALF)
    {
      output_bit_plus_follow (1);
      low -= HALF;
      high -= HALF;
    }
    else if (low >= FIRST_QTR && high < THIRD_QTR)
    {
      bits_to_follow += 1;
      low -= FIRST_QTR;
      high -= FIRST_QTR;
    }
    else
      break;
/*
если верхняя граница интервала будет меньше половины диапазона, то двоичное представление
числа будет иметь вид …0….
Аналогично, если нижняя граница интервала будет больше половины
диапазона, то двоичное представление числа будет иметь вид …1….
Если же low >= FIRST_QTR && high < THIRD_QTR, то это означает, что
первые цифры границ отличаются, а вторые равняются 9 и 0 (в десятичном представлении).
В этом случае вторые цифры "отбрасываются", а на самом деле этот бит "откладывается".
Эго значение будет зависеть от последующих битов.
Если эти условия не выполняются, то это значит, что первые цифры границ совпадают и кодирование символа завершено
*/
    // сдвиг влево с "втягиванием" очередного бита
    low = 2 * low;
    high = 2 * high + 1;
  }
}

//------------------------------------------------------------
// Декодирование очередного символа

int decode_symbol (void)
{
  long range;
  int cum, symbol;

  // определение текущего масштаба частот
  range = (long) (high - low) + 1;
  // масштабирование значения в регистре кода
  cum = (int)
    ((((long) (value - low) + 1) * cum_freq [0] - 1) / range);
  // поиск соответствующего символа в таблице частот
  for (symbol = 1; cum_freq [symbol] > cum; symbol++);
  // пересчет границ
  high = low + (range * cum_freq [symbol - 1]) / cum_freq [0] - 1;
  low = low + (range * cum_freq [symbol]) / cum_freq [0];
  // удаление очередного символа из входного потока
  for (;;)
  {
    if (high < HALF)
    {
    }
    else if (low >= HALF)
    {
      value -= HALF;
      low -= HALF;
      high -= HALF;
    }
    else if (low >= FIRST_QTR && high < THIRD_QTR)
    {
      value -= FIRST_QTR;
      low -= FIRST_QTR;
      high -= FIRST_QTR;
    }
    else
      break;
    // сдвиг влево с "втягиванием очередного бита
    low = 2 * low;
    high = 2 * high + 1;
    value = 2 * value + input_bit ();
  }
  return symbol;
}

//------------------------------------------------------------
// Собственно адаптивное арифметическое кодирование

void encode ( char *infile, char *outfile)
{
  int ch, symbol;

  in = fopen ( infile, "r+b");
  out = fopen ( outfile, "w+b");
  if (in == NULL || out == NULL)
    return;
  start_model ();      //функции инициализации адаптивной модели
  start_outputing_bits ();     //инициализации побитового вывода
  start_encoding ();     //инициализации регистров границ и кода перед началом сжатия
  for (;;)   //цикле ведется посимвольное считывание потока
  {
    ch = getc (in);
    if (ch == EOF)
      break;      //Считывание ведётся до тех пор, пока не встретится символ конца файла EOF
    symbol = char_to_index [ch];

    /*
    Для каждого символа вызывается процедура кодирования очередного символа
    void encode_symbol (int symbol) и
    сразу после этого процедура обновления модели
    очередным символом void update_model ( int symbol)
    */
    
    encode_symbol (symbol);
    update_model (symbol);
  }
  encode_symbol (EOF_SYMBOL); //кодируется сам символ EOF

  /*
  выполняется очистка побитового вывода void done_encoding (void)
  и очистка буфера побитового вывода void done_outputing_bits (void)
  */

  done_encoding ();
  done_outputing_bits ();

  fclose (in);
  fclose (out);
}

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button1Click(TObject *Sender)
{
        if(fileopen == 0 || filesave == 0)
        {
                Application->MessageBox("Выбирите файлы!", "Внимание", MB_OK);
                return;
        }

        char * charfileopen = fileopen.c_str();
        char * charfilesave = filesave.c_str();

        clock_t start_time, end_time;

        start_time = clock();

        encode (charfileopen, charfilesave);

        end_time = clock( );
        double timeofwork = static_cast< double >( end_time - start_time ) / CLOCKS_PER_SEC;
        Edit1->Text = timeofwork;
        Application->MessageBox("Файл закодирован!", "Внимание", MB_OK);
        
        /*
        AnsiString s;
        char * a = new char[s.Length()];
        strcpy(a,s.c_str());
        fputs(s);
        */
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click(TObject *Sender)
{
        Close();        
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender)
{
        if(OpenDialog1->Execute())
        {
                fileopen = OpenDialog1->FileName;
                Edit2->Text = fileopen;
                if(filesave != 0) Button1->Enabled = true;
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button4Click(TObject *Sender)
{
        if(SaveDialog1->Execute())
        {
                filesave = SaveDialog1->FileName;
                Edit3->Text = filesave;
                if(fileopen != 0) Button1->Enabled = true;
        }
}
//---------------------------------------------------------------------------


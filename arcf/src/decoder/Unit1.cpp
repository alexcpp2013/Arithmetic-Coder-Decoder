//---------------------------------------------------------------------------

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

// ���������� ����� � ��������
#define BITS_IN_REGISTER 16

// ����������� ��������� �������� � ��������
#define TOP_VALUE (((long) 1 << BITS_IN_REGISTER) - 1)

// ���������
#define FIRST_QTR (TOP_VALUE / 4 + 1)
#define HALF      (2 * FIRST_QTR)
#define THIRD_QTR (3 * FIRST_QTR)

// ���������� �������� ��������
#define NO_OF_CHARS 256
// ����������� ������ ����������
#define EOF_SYMBOL    (NO_OF_CHARS + 1)
// ����� �������� � ������
#define NO_OF_SYMBOLS (NO_OF_CHARS + 1)

// ����� ������� ��� ���������������
#define MAX_FREQUENCY 16383

// ������� �������������
unsigned char         index_to_char [NO_OF_SYMBOLS];
int		      char_to_index [NO_OF_CHARS];

// ������� ������
int                   cum_freq [NO_OF_SYMBOLS + 1];
int		      freq [NO_OF_SYMBOLS + 1];

// �������� ������ � ����
long                  low, high;
long		      value;

// ��������� ��������� �������� � �������
long		      bits_to_follow;
int		      buffer;
int		      bits_to_go;
int 		      garbage_bits;

// �������������� �����

FILE *in, *out;

// ������������� ���������� ������

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
// ���������� ������ ��������� ��������

void update_model ( int symbol)
{
  int i;
  int ch_i, ch_symbol;
  int cum;

  // �������� �� ������������ �������� �������
  if (cum_freq [0] == MAX_FREQUENCY)
  {
    cum = 0;
    // ��������������� ������ ��� ������������
    for ( i = NO_OF_SYMBOLS; i >= 0; i--)
    {
      freq [i] = (freq [i] + 1) / 2;
      cum_freq [i] = cum;
      cum += freq [i];
    }
  }
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
  // ���������� �������� � �������� ������
  freq [i] += 1;
  while (i > 0)
  {
    i -= 1;
    cum_freq [i] += 1;
  }
}

//------------------------------------------------------------
// ������������� ���������� �����

void start_inputing_bits (void)
{
  bits_to_go = 0;
  garbage_bits = 0;
}

//------------------------------------------------------------
// ���� ���������� ���� ������ ����������

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
	printf ("������ � ������ �����\n");
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
// ������������� ���������� ������

void start_outputing_bits (void)
{
  buffer = 0;
  bits_to_go = 8;
}

//------------------------------------------------------------
// ����� ���������� ���� ������ ����������

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
// ������� ������ ���������� ������

void done_outputing_bits (void)
{
  putc ( buffer >> bits_to_go, out);
}

//------------------------------------------------------------
// ����� ���������� ���� � ���������� �����

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
// ������������� ��������� ������ � ���� ����� ������� ������

void start_encoding (void)
{
  low            = 0l;
  high           = TOP_VALUE;
  bits_to_follow = 0l;
}

//------------------------------------------------------------
// ������� ���������� ������

void done_encoding (void)
{
  bits_to_follow++;
  if (low < FIRST_QTR)
    output_bit_plus_follow (0);
  else
    output_bit_plus_follow (1);
}

//------------------------------------------------------------
/* ������������� ��������� ����� ��������������.
   �������� ������ ������� ���������
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
// ����������� ���������� �������

void encode_symbol ( int symbol)
{
  long range;

  // �������� �������� ������
  range = (long) (high - low) + 1;
  high = low + (range * cum_freq [symbol - 1]) / cum_freq [0] - 1;
  low = low + (range * cum_freq [symbol]) / cum_freq [0];
  // ���������� ��������� �����
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
    // ����� ����� � "�����������" ���������� ����
    low = 2 * low;
    high = 2 * high + 1;
  }
}

//------------------------------------------------------------
// ������������� ���������� �������

int decode_symbol (void)
{
  long range;
  int cum, symbol;

  // ����������� �������� �������� ������
  range = (long) (high - low) + 1;
  // ��������������� �������� � �������� ����
  cum = (int)
    ((((long) (value - low) + 1) * cum_freq [0] - 1) / range);
  // ����� ���������������� ������� � ������� ������
  for (symbol = 1; cum_freq [symbol] > cum; symbol++);
  // �������� ������
  high = low + (range * cum_freq [symbol - 1]) / cum_freq [0] - 1;
  low = low + (range * cum_freq [symbol]) / cum_freq [0];
  // �������� ���������� ������� �� �������� ������
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
    // ����� ����� � "����������� ���������� ����
    low = 2 * low;
    high = 2 * high + 1;
    value = 2 * value + input_bit ();
  }
  return symbol;
}

//------------------------------------------------------------
// ���������� ���������� �������������� �������������

void decode ( char *infile, char *outfile)
{
  int ch, symbol;

  in = fopen ( infile, "r+b");
  out = fopen ( outfile, "w+b");
  if (in == NULL || out == NULL)
    return;
  start_model ();
  start_inputing_bits ();
  start_decoding ();
  for (;;)
  {
    symbol = decode_symbol ();
    if (symbol == EOF_SYMBOL)
      break;
    ch = index_to_char [symbol];
    putc ( ch, out);
    update_model (symbol);
  }
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
                Application->MessageBox("�������� �����!", "��������", MB_OK);
                return;
        }

        /*
        char * charfileopen = new char[fileopen.Length()];
        strcpy(charfileopen,fileopen.c_str());
        char * charfilesave = new char[filesave.Length()];
        strcpy(charfilesave,filesave.c_str());
        */
        char * charfileopen = fileopen.c_str();
        char * charfilesave = filesave.c_str();

        clock_t start_time, end_time;

        start_time = clock();

        decode (charfileopen, charfilesave);

        end_time = clock( );
        double timeofwork = static_cast< double >( end_time - start_time ) / CLOCKS_PER_SEC;
        Edit1->Text = timeofwork;
        Application->MessageBox("���� ������������!", "��������", MB_OK);
        
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

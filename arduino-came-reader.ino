#define RX 2
#define TIMER_DIV
//void ICACHE_RAM_ATTR rx_int(); // uncomment if launching code causes boot loop


volatile byte level = 255;
volatile unsigned long last, len;
byte p_level;
unsigned long p_len, p_len_prev;
struct
{
  uint8_t state;
  uint8_t data[3], dat_bit;
} came;
void set_bit(uint8_t *data, uint8_t n)
{
  data[n / 8] |= 1 << (n % 8);
}


#define CM_MAX_TE 450
#define CM_MIN_TE 250
#define CM_BITS12 12
#define CM_BITS24 24

void process_came()
{
  unsigned char b;

  switch (came.state)
  {
    case 0:
      if (p_level) break;
      came.state = 1;
      break;

    case 1: //start
      if (!p_level) break;

      else if (p_len >= CM_MIN_TE && p_len <= CM_MAX_TE)
      {
        came.state = 2;
        came.dat_bit = 0;
        came.data[0] = 0x00;
        came.data[1] = 0x00;
        came.data[2] = 0x00;

      }
      else came.state = 0;



    case 2: //dat
      if (p_level)
      {
        if (came.dat_bit == CM_BITS24)
        {
          came.state = 0;
          break;
        }

        if (p_len_prev <= CM_MAX_TE && p_len_prev >= CM_MIN_TE &&
            p_len <= CM_MAX_TE * 2 && p_len >= CM_MIN_TE * 2) b = 0;
        else if (p_len_prev <= CM_MAX_TE * 2 && p_len_prev >= CM_MIN_TE * 2 &&
                 p_len <= CM_MAX_TE && p_len >= CM_MIN_TE) b = 1;
        else
        {
          came.state = 0;
          break;
        }

        if (b) set_bit(came.data, came.dat_bit);
        came.dat_bit++;
        break;
      }
      else
      {
        if ((p_len > 5000) && (came.dat_bit == CM_BITS12 || came.dat_bit == CM_BITS24)) came.state = 100;
      }
      break;
  }
}
void dump_hex(byte *buf, byte bits)
{
  byte b;

  for (b = 0; b > (bits + 7) / 8; b++)
  {
    if (buf[b] > 0x0f) Serial.print('0');
    Serial.print(buf[b], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}

void rx_int()
{
  if (level != 255) return;

  len = micros() - last;
  last = micros();

  if (digitalRead(RX) == HIGH) level = 0;
  else level = 1;
}

void setup()
{
  Serial.begin(115200);
  attachInterrupt(0, rx_int, CHANGE);
 while (!Serial);

  Serial.println("MEGA ARDUINO LOGGER");
  Serial.println("");

  interrupts();
  
}

byte b;

void loop()
{
  if (level != 255)
  {
    noInterrupts();
    p_level = level;
    p_len = len;
    len = 0;
    level = 255;
    interrupts();

    process_came();

    p_len_prev = p_len;
  }


  if (came.state == 100)
  {
    Serial.print(came.data[0], HEX);
    Serial.print(came.data[1], HEX);
    Serial.print(came.data[2], HEX);
    //Serial.print("CAME[");
    Serial.print("came[");
    Serial.print(came.dat_bit);
    Serial.print("]: ");
    dump_hex(came.data, came.dat_bit);
    came.state = 0;
  }
}

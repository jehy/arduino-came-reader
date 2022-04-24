#define pinRX 2

#define CM_MAX_TE 450
#define CM_MIN_TE 250
#define CM_BITS12 12
#define CM_BITS24 24
#define Te 320

volatile byte level = 255;
volatile unsigned long last;
volatile unsigned long len;
byte p_level;
unsigned long p_len;
unsigned long p_len_prev;
struct
{
    uint8_t state;
    uint8_t data[3], dat_bit;
} came;


void setup()
{
    Serial.begin(9600);
    while (!Serial);
    attachInterrupt(0, pinRX_int, CHANGE);
    interrupts();
}

void loop()
{
    ////receive
    RfReceive();
}

void RfReceive()
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
        for (int i = 0; i < sizeof(came.data) - (came.dat_bit == CM_BITS12 ? 1 : 0); i++) {
            if (i > 0) {
                Serial.print("_");
            }
            Serial.print(stringWithPrefix(String(came.data[i], BIN), came.dat_bit == CM_BITS12 ? 6 : 8, '0'));
        }
        came.state = 0;
        Serial.println();
    }
}


void pinRX_int()
{
    if (level != 255) return;
    len = micros() - last;
    last = micros();
    if (digitalRead(pinRX) == HIGH) level = 0;
    else level = 1;
}

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

            for (int i = 0; i < sizeof(came.data); i++) {
                came.data[i] = 0x00;
            }

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


void set_bit(uint8_t *data, uint8_t n)
{
    data[n / 8] |= 1 << (n % 8);
}

String stringWithPrefix(String line, int len, char prefix)
{
    String addon = "";
    int n = len - line.length();
    for (int i = 0; i < n; i++) {
        addon += prefix;
    }
    return addon + line;
}

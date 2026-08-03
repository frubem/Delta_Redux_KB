/* 
   Derived from the MM5740 equivalent clone, copyright (C) 2022 Tube Time.
   Changes by Hkz in 2025 to support DELTA Redux	

   This work is licensed under the Creative Commons Attribution-ShareAlike 4.0
   International License. To view a copy of this license, visit
   http://creativecommons.org/licenses/by-sa/4.0/ or send a letter to
   Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
*/

#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>


/* Port assignments:
    PORTA = External data port (outputs)
    PORTB
        0 - X9
        1 - X8
        2 - X7
        3 - X6
        4 - X5
        5 - X4
        6 - X3
        7 - X2
    PORTC
        0 - Y2
        1 - Y3
        2 - Y4
        3 - Y5
        4 - Y6
        5 - Y7
        6 - Y8
        7 - Y9
    PORTD
        0 - X1
        1 - SRB (strobe output)
        2 - Repeat clock in
        3 - AKD (any key down output)
        4 - Control key
        5 - Shift key
        6 - Y10
        7 - Y1
*/

#define KEY_COUNT           90  // Total number of possible keys
#define DEBOUNCE_DELAY     200  // 2ms debounce delay
#define STROBE_TIME         50  // 50us strobe pulse
#define ROW_SETTLING_TIME   50  // 50us settling time

#define STR_BUFFER_SIZE		20	// Buffer size we'll use to copy PROGMEM strings into memory

bool keyflags[KEY_COUNT] = {0};
bool repeat_history = false;

static char extra_cmd_buffer[STR_BUFFER_SIZE];
static const char extra_key_PR3[] PROGMEM = "PR#3\r";
static const char extra_key_PR6[] PROGMEM = "PR#6\r";
static const char extra_key_CAT[] PROGMEM = "CATALOG\r";
static const char extra_key_MON[] PROGMEM = "CALL 65385\r";
static const char extra_key_RES[] PROGMEM = "03F4:00\rFA69G\r";

PGM_P const extra_key_commands[] PROGMEM = {
    extra_key_PR3,	// Extra 1
    extra_key_PR6,	// Extra 2 
    extra_key_CAT,	// Extra 3
    extra_key_MON,	// Extra 4
    extra_key_RES,	// Extra 5
};

static const uint8_t extra_key_rom[] PROGMEM = {
// Row
// 1     2     3     4     5     6     7     8     9    10
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Col 1
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Col 2
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Col 3
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Col 4
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Col 5
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Col 6
0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Col 7
0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Col 8
0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x02, 0x01, 0x00, 0x00, // Col 9
};


static const uint8_t key_rom[] PROGMEM = {
// Row
// 1     2     3     4     5     6     7     8     9   10
// Unshifted ("alone")
 '8', '\\',  '5',  '1',  '2',  '/',  '0',  '2',  '*',0x7F, // Col 1
 ']', 0x0D, 0x0B, 0x0A, 0x15,  '0', 0x20,  '3', 0x08, '=', // Col 2
 '0',  '[',  'p', 0x7f, '\'',  '1',  '/',  '4',  'o', '-', // Col 3
 '9',  'i',  'O',  'K',  ';',  ',',  '.',  'l',  'k', '8', // Col 4
 '6',  'u',  'y',  'j',  'h',  'm',  'n',  '5',  '-', '7', // Col 5
 '5',  'r',  't',  'f',  'g',  'v',  'b',  '6',  '+', '4', // Col 6
0x12,  'e', 0x13,  'd', 0x14,  'c', 0x7F,  '7',  '.', '3', // Col 7
0x05,  'w', 0x06,  's', 0x07,  'x',  '9',  '8', 0x0D, '2', // Col 8
0x09,  'q', 0x1B,  'a', 0x01,  'z', 0x02, 0x01,  '~', '1', // Col 9

// Shifted
 '8',  '|',  '5',  '1',  '2',  '/',  ')',  '2',  '*',0x7F, // Col 1
 '}', 0x0D, 0x0B, 0x0A, 0x15,  '0', 0x20,  '3', 0x08, '+', // Col 2
 '0',  '{',  'P', 0x7f,  '"',  '1',  '?',  '4',  'O', '_', // Col 3
 '(',  'I',  '_',  '[',  ':',  '<',  '>',  'L',  'K', '*', // Col 4
 '^',  'U',  'Y',  'J',  'H',  'M',  'N',  '5',  '-', '&', // Col 5
 '%',  'R',  'T',  'F',  'G',  'V',  'B',  '6',  '+', '$', // Col 6
0x12,  'E', 0x13,  'D', 0x14,  'C', 0x7F,  '7',  '.', '#', // Col 7
0x05,  'W', 0x06,  'S', 0x07,  'X',  '9',  '8', 0x0D, '@', // Col 8
0x09,  'Q', 0x1B,  'A', 0x01,  'Z', 0x02, 0x01,  '~', '!', // Col 9

// Control
 '8', 0x1C,  '5',  '1',  '2',  '/',  '0',  '2',  '*',0x7F, // Col 1
0x1D, 0x0D, 0x0B, 0x0A, 0x15,  '0', 0x20,  '3', 0x08, '=', // Col 2
 '0', 0x1B, 0x10, 0x7f, '\'',  '1',  '/',  '4', 0x0F,0x1F, // Col 3
 '9', 0x09, 0x0f, 0x0b,  ';',  ',',  '.', 0x0C, 0x0B, '8', // Col 4
0x1E, 0x15, 0x19, 0x0A, 0x08, 0x0D, 0x0E,  '5',  '-', '7', // Col 5
 '5', 0x12, 0x14, 0x06, 0x07, 0x16, 0x02,  '6',  '+', '4', // Col 6
0x12, 0x05, 0x13, 0x04, 0x14, 0x03, 0x7F,  '7',  '.', '3', // Col 7
0x05, 0x17, 0x06, 0x13, 0x07, 0x18,  '9',  '8', 0x0D,0x00, // Col 8
0x09, 0x11, 0x1B, 0x01, 0x01, 0x1A, 0x02, 0x01,  '~', '1', // Col 9

// Shift + Control
 '8', 0x1C,  '5',  '1',  '2',  '/',  ')',  '2',  '*',0x7F, // Col 1
0x1D, 0x0D, 0x0B, 0x0A, 0x15,  '0', 0x20,  '3', 0x08, '+', // Col 2
 '0', 0x1B, 0x10, 0x7f,  '"',  '1',  '?',  '4', 0x0F,0x1F, // Col 3
 '(', 0x09, 0x1f, 0x1b,  ':',  '<',  '>', 0x0C, 0x0B, '*', // Col 4
0x1E, 0x15, 0x19, 0x0A, 0x08, 0x0D, 0x0E,  '5',  '-', '&', // Col 5
 '%', 0x12, 0x14, 0x06, 0x07, 0x16, 0x02,  '6',  '+', '$', // Col 6
0x12, 0x05, 0x13, 0x04, 0x14, 0x03, 0x7F,  '7',  '.', '#', // Col 7
0x05, 0x17, 0x06, 0x13, 0x07, 0x18,  '9',  '8', 0x0D,0x00, // Col 8
0x09, 0x11, 0x1B, 0x01, 0x01, 0x1A, 0x02, 0x01,  '~', '!', // Col 9


};

// Output Bit changes for ROM:
// 1, 2, 3, 4,  ,  ,  ,
//             5, 6, 7, 8    - changes for ctrl and shift


// Disables the drive signal on a X column
void disable_column(uint8_t col)
{
    // Disable the current column
    if (col == 0) {
        DDRD &= ~_BV(0);
    } else {
        DDRB &= ~_BV(8 - col);
    }
}

// Enables the drive signal on a X column
void enable_column(uint8_t col)
{
    // Enable the next column
    if (col == 0) {
        DDRD |= _BV(0);
    } else {
        DDRB |= _BV(8 - col);
    }
}

// Read the contents of the Y rows.
uint16_t read_rows()
{
    return ~((((uint16_t)PIND & 0x40) << 3) |
              ((uint16_t)PINC << 1) |
              ((uint16_t)PIND >> 7));
}

void set_akd(uint8_t state)
{
	if(state) PORTD |= _BV(3);
	else PORTD &= ~_BV(3);
}

// Pulse the strobe output
void strobe()
{
    PORTD |= _BV(1);
    _delay_us(STROBE_TIME);
    PORTD &= ~_BV(1);
}

// Checks for falling edge on the key repeat signal
bool is_repeat()
{
    bool repeat = (PIND & _BV(2));
    if (!repeat && repeat_history) {
        repeat_history = repeat;
        return true;
    } else {
        repeat_history = repeat;
        return false;
    }
}

// Checks a key in a row to ensure it stays down
bool debounce_key(uint8_t row)
{
    int debounce;
    for (debounce = 0; debounce < DEBOUNCE_DELAY; debounce++) {
        _delay_us(10);
        if (!(read_rows() & _BV(row))) {
            return false;
        }
    }
    return true;
}

// Check if there's an "extra" key associated to this scancode
uint8_t extra_key_lookup(uint8_t scancode) {
	return pgm_read_byte(&(extra_key_rom[scancode]));
}

// Look up the key in the ROM, with shift and control
uint8_t key_lookup(uint8_t scancode)
{
    uint16_t shift = (PIND & _BV(5)) ? 90 : 0;
    uint16_t control = (PIND & _BV(4)) ? 0: 180; // Control is inverted (GND when pressed)
    return pgm_read_byte(&(key_rom[(uint16_t)scancode + shift + control]));
}

void send_extra_key_command(uint8_t cmd_idx) {
	// Retrieve the string
	strcpy_P(extra_cmd_buffer, (char *)pgm_read_ptr(&(extra_key_commands[cmd_idx])));
	
	// We'll leave the AKD signal disabled for this: it's not needed
	set_akd(false);
	for(uint8_t idx = 0; extra_cmd_buffer[idx]; idx++) {
		PORTA = extra_cmd_buffer[idx];
		_delay_us(50);
		strobe();
		_delay_ms(65);
		
		if(extra_cmd_buffer[idx] == '\r') _delay_ms(500);
	}
}	

// Used to keep track of the last key pressed and to
// set the output data to another code if another key was 
// pressed previously and is still pressed.
static bool key_resend = false;
static uint8_t last_key_sent = 0xFF;

// Scan the keyboard matrix
bool scan_keys()
{
    uint8_t x, y, rom_addr, debounce;
	uint8_t extra_key;
    uint16_t row_data;
    bool keydown = false;

    rom_addr = 0;
    for (x = 0; x < 9; x++) {
        enable_column(x);
        _delay_us(ROW_SETTLING_TIME);
        row_data = read_rows();
        // Sense logic also uses a counter.
        for (y = 0; y < 10; y++) {
            if (row_data & _BV(y)) {
                if ((!keyflags[rom_addr] | key_resend) & debounce_key(y)) {
					last_key_sent = rom_addr;
					key_resend = keyflags[rom_addr]; // If the key was not pressed before this, we'll have to trigger the strobe signal
					keyflags[rom_addr] = true;  // Key is down, update memory so we don't emit it again (unless it's a resend)
					
					extra_key = extra_key_lookup(rom_addr);
					if(!extra_key) {
						PORTA = key_lookup(rom_addr);
						if(!key_resend) strobe(); // Strobe unless this is a resend
						else _delay_us(STROBE_TIME);
					} else if (!key_resend) { // We need to send the sequence corresponding to the extra key, unless we're resending an old code
						send_extra_key_command(extra_key - 1);
					}
					
                    // Continue scanning for more keys
                }
					
				key_resend = false;
            } else {
				// If this key was the last one pressed, and it is pressed no longer,
				// flag this condition to send the code of another pressed key on the next loop
				if(rom_addr == last_key_sent) key_resend = true;
				
                keyflags[rom_addr] = false;  // Key is not down, update memory
            }
            // Detect if any key is down
            if (keyflags[rom_addr]) keydown = true;
            rom_addr++;
        }
        disable_column(x);
    }
	
	set_akd(keydown);
	
    return keydown;
}

// Note: Emit strobe pulse for one clock period (around 50us).

void main(void)
{
    // Set up ports
    DDRA = 0xff; // Output port
    PORTA = 0x00; // No character

    DDRB = 0x00; // Column drivers (X) are off by default
    PORTB = 0x00; // No pullup resistors for column drivers

    DDRC = 0x00; // Row connections (Y) are inputs
    PORTC = 0xff; // Pullup resistors turned on

    DDRD = _BV(1) | _BV(3); // SRB and AKD are outputs
    PORTD = _BV(2) | _BV(6) | _BV(7); // Pullups for REPEAT, Y10 and Y1

    while(1) {
        if (scan_keys() && is_repeat()) {
			set_akd(false);
			
            strobe(); // pulse strobe again
			
			set_akd(true);
        }
    }
}

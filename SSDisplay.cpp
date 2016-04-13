/**
 * @file "SSDisplay.cpp"
 * flexible Seven-Segment Display class
 *
 * @authot     Jaime Martínez-Figueroa <jmf@charliefoxtrot.cl>
 * @date       April 2016
 * @copyright  2016 Charlie Foxtrot Electronics, LGPL
 */

#include "SSDisplay.h"

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/**
 * Constructor method
 * 
 * @param a_pin        output pin for the A segment
 * @param b_pin        output pin for the B segment
 * @param c_pin        output pin for the C segment
 * @param d_pin        output pin for the D segment
 * @param e_pin        output pin for the E segment
 * @param f_pin        output pin for the F segment
 * @param g_pin        output pin for the G segment
 * @param h_pin        output pin for the H segment (decimal point)
 * @param segmentMode  output mode for the segments POSITIVE for common cathode NEGATIVE for common anode
 * @param digitMode    output mode for the segments NEGATIVE for common cathode POSITIVE for common anode
 */
SSDisplay::SSDisplay(
  uint8_t a_pin,
  uint8_t b_pin,
  uint8_t c_pin,
  uint8_t d_pin,
  uint8_t e_pin,
  uint8_t f_pin,
  uint8_t g_pin,
  uint8_t h_pin,
  SSDisplayMode_e segmentMode,
  SSDisplayMode_e digitMode)
{
  _seg_pins[0] = a_pin; 
  _seg_pins[1] = b_pin;
  _seg_pins[2] = c_pin;
  _seg_pins[3] = d_pin;
  _seg_pins[4] = e_pin;
  _seg_pins[5] = f_pin;
  _seg_pins[6] = g_pin;
  _seg_pins[7] = h_pin;

  if(segmentMode == POSITIVE) {
    _on = HIGH;
    _off = LOW;
  }
  else {
    _on = LOW;
    _off = HIGH;
  }

  int i;

  for(i = 0; i < 8; i++) {
    pinMode(_seg_pins[i], OUTPUT);
  }

  _digitMode = digitMode;
}

/**
 * Destructor method (does nothing)
 */
SSDisplay::~SSDisplay() {
  // does nothing
}

/**
 * Updates the state of the segments output
 * 
 * @param content  the content intended for the SSDisplay, from '0' to '9' plus 'A' to 'F' and '-' 
 * @param dot      true for displaying the decimal point
 */
void SSDisplay::updateCurrent(char content, bool dot) {
  int i;

  if(content >= 'a' && content <= 'z') {
    content = content - 'a' + 'A';
  }

  for(i = 0; i < 8; i++) {
    _seg_cont[i] = _on;
  }

  if(content == '1' || 
     content == '4' || 
     content == 'B' || 
     content == 'D' || 
     content == ' ' ||
     content == '-')
  {
    _seg_cont[SEG_A] = _off;
  }

  if(content == '5' || 
     content == '6' || 
     content == 'B' || 
     content == 'C' || 
     content == 'E' || 
     content == 'F' || 
     content == ' ' ||
     content == '-')
  {
    _seg_cont[SEG_B] = _off;
  }

  if(content == '2' || 
     content == 'C' || 
     content == 'E' || 
     content == 'F' || 
     content == ' ' ||
     content == '-')
  {
    _seg_cont[SEG_C] = _off;
  }

  if(content == '1' || 
     content == '4' || 
     content == '7' || 
     content == 'A' || 
     content == 'F' || 
     content == ' ' ||
     content == '-')
  {
    _seg_cont[SEG_D] = _off;
  }

  if(content == '1' || 
     (content >= '3' && content <= '5') || 
     content == '7' || 
     content == '9' || 
     content == ' ' ||
     content == '-')
  {
    _seg_cont[SEG_E] = _off;
  }

  if((content >= '1' && content <= '3') || 
     content == '7' || 
     content == 'D' || 
     content == ' ' ||
     content == '-')
  {
    _seg_cont[SEG_F] = _off;
  }

  if(content == '0' || 
     content == '1' || 
     content == '7' || 
     content == 'C' || 
     content == ' ')
  {
    _seg_cont[SEG_G] = _off;
  }

  if(!dot) {
    _seg_cont[SEG_H] = _off;
  }

  _display();
}

/**
 * Blanks the current display
 */
void SSDisplay::blankCurrent() {
  updateCurrent(' ', false);
}

/**
 * Adds a digit controller
 * 
 * @param pin  The pin to where the commmon pin is connected
 */
void SSDisplay::addDigit(uint8_t pin) {
  SSDisplay_digit_s * temp;

  if(_first == NULL) {
    _first = new SSDisplay_digit_s;

    temp = _first;
  }
  else {
    temp = _first;

    while(temp->next != NULL) {
      temp = temp->next;
    }

    temp->next = new SSDisplay_digit_s;

    temp = temp->next;
  }

  temp->pin = pin;
  temp->content = ' ';
  temp->dot = false;

  pinMode(pin, OUTPUT);

  if(_digitMode == POSITIVE) {
    digitalWrite(pin, LOW);
  }
  else {
    digitalWrite(pin, HIGH);
  }

  _digits++;
}

/**
 * Refreshes all the digits in order, better used inside loop()
 */
void SSDisplay::refreshAll() {
  if(_first == NULL) return;

  SSDisplay_digit_s * selected = _first;

  do {
    if(_digitMode == POSITIVE) {
      digitalWrite(selected->pin, HIGH);
    }
    else {
      digitalWrite(selected->pin, LOW);
    }

    updateCurrent(selected->content, selected->dot);

    blankCurrent();

    if(_digitMode == POSITIVE) {
      digitalWrite(selected->pin, LOW);
    }
    else {
      digitalWrite(selected->pin, HIGH);
    }

    selected = selected->next;
  } while (selected != NULL);
}

/**
 * Refreshes one digit at a time, better used inside a timer interrupt
 */
void SSDisplay::refreshNext() {
  if(_first == NULL) return;
  if(_next == NULL) _next = _first;

  blankCurrent();

  if(_digitMode == POSITIVE) {
    digitalWrite(_next->pin, LOW);
  }
  else {
    digitalWrite(_next->pin, HIGH);
  }

  _next = _next->next;
  
  if(_next == NULL) {
    _next = _first;
  }

  if(_digitMode == POSITIVE) {
    digitalWrite(_next->pin, HIGH);
  }
  else {
    digitalWrite(_next->pin, LOW);
  }

  updateCurrent(_next->content, _next->dot);
}

/**
 * Displays a signed decimal integer, note that the minus sign uses one space.
 * In case of overflow it will show "  ..." or "-  ..."
 * 
 * @param number  the number to be displayed
 */
void SSDisplay::printDecInt(long int number) {
  long int tempNum = 1;
  int i;
  SSDisplay_digit_s * selected = _first;

  for(i = 0; i < _digits; i++) {
    tempNum *= 10;
  }
  tempNum--;

  if(number >= 0) {
    if(number > tempNum) { // positive overflow (prints "   ...")
      i = 1;

      do {
        selected->content = ' ';

        if(i <= 3) {
          selected->dot = true;
        }
        else {
          selected->dot = false;
        }

        selected = selected->next;
        i++;
      } while(selected != NULL);
    }
    else { // positive numbers
      tempNum = number;
      i = 1;

      do {
        if(tempNum > 0 || i == 1) {
          selected->content = '0' + tempNum%10;
        }
        else {
          selected->content = ' ';
        }

        selected->dot = false;
        tempNum /= 10;
        selected = selected->next;
        i++;
      } while(selected != NULL);

      _first->dot = true;
    }
  }
  else {
    tempNum /= 10;

    if(number < -tempNum) { // negative overflow (prints "-  ...")
      i = 1;

      do {

        if(i <= 3) {
          selected->dot = true;
        }
        else {
          selected->dot = false;
        }

        if(i == _digits) {
          selected->content = '-';
        }
        else {
          selected->content = ' ';
        }

        selected = selected->next;
        i++;
      } while(selected != NULL);
    }
    else { // positive numbers
      tempNum = -number;
      i = 1;

      do {
        if(i == _digits) {
          selected->content = '-';
        }
        else {
          if(tempNum > 0) {
            selected->content = '0' + tempNum%10;
          }
          else {
            selected->content = ' ';
          }
        }

        selected->dot = false;
        tempNum /= 10;
        selected = selected->next;
        i++;
      } while(selected != NULL);

      _first->dot = true;
    }
  }
}

/**
 * Displays an unsigned hexadecimal integer. In case of overflow will ignore 
 * extra content.
 * 
 * @param long  the number to be displayed
 */
void SSDisplay::printHexInt(unsigned long int number) {
  unsigned long int tempNum = 1, tempDigit;
  int i = 1;
  SSDisplay_digit_s * selected = _first;
   
  tempNum = number;

  do {
    tempDigit = tempNum%16;

    if(tempNum > 0 || i <= 1) {
      if(tempDigit < 0xA) {
        selected->content = '0' + tempDigit;
      }
      else {
        selected->content = 'A' + tempDigit - 0xA;
      }
    }
    else {
      selected->content = ' ';
    }

    tempNum /= 16;

    selected->dot = false;

    selected = selected->next;
    i++;
  } while (selected != NULL);
}

// privates

/**
 * Updates the status of the outputs from _seg_cont[]
 */
void SSDisplay::_display() {
  int i;

  for(i = 0; i < 8; i++) {
    digitalWrite(_seg_pins[i], _seg_cont[i]);
  }
}

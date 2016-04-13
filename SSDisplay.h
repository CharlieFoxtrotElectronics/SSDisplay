/**
 * @file "SSDisplay.h"
 * flexible Seven-Segment Display class
 *
 * @authot     Jaime Martínez-Figueroa <jmf@charliefoxtrot.cl>
 * @date       April 2016
 * @copyright  2016 Charlie Foxtrot Electronics, LGPL
 */

#ifndef SSDisplay_h
#define SSDisplay_h

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/**
 * Internal digit structure, it's a single linked list
 */
struct SSDisplay_digit_s {
  char content;                    ///< the content of the display as a character
  bool dot;                        ///< flag for the decimal point
  uint8_t pin;                     ///< Arduino pin for the selection of the digit
  SSDisplay_digit_s * next = NULL; ///< Pointer to the next digit
};

/**
 * The pin-modes available
 */
enum SSDisplayMode_e {
  POSITIVE, ///< Positive mode for common cathode 
  NEGATIVE  ///< Negative mode for common anode
};

/**
 * The segments 
 */
enum SSDisplaySegments_e {
  SEG_A = 0,
  SEG_B,
  SEG_C,
  SEG_D,
  SEG_E,
  SEG_F,
  SEG_G,
  SEG_H
};

/**
 * flexible Seven-Segment Display class
 */
class SSDisplay {
  public:
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
    SSDisplay(
      uint8_t a_pin,
      uint8_t b_pin,
      uint8_t c_pin,
      uint8_t d_pin,
      uint8_t e_pin,
      uint8_t f_pin,
      uint8_t g_pin,
      uint8_t h_pin,
      SSDisplayMode_e segmentMode=NEGATIVE,
      SSDisplayMode_e digitMode=NEGATIVE);

    /**
     * Destructor method (does nothing)
     */
    ~SSDisplay();

    /**
     * Updates the state of the segments output
     * 
     * @param content  the content intended for the SSDisplay, from '0' to '9' plus 'A' to 'F' and '-' 
     * @param dot      true for displaying the decimal point
     */
    void updateCurrent(char content, bool dot);

    /**
     * Blanks the current display
     */
    void blankCurrent();

    /**
     * Adds a digit controller
     * 
     * @param pin  The pin to where the commmon pin is connected
     */
    void addDigit(uint8_t pin);

    /**
     * Refreshes all the digits in order, better used inside loop()
     */
    void refreshAll();

    /**
     * Refreshes one digit at a time, better used inside a timer interrupt
     */
    void refreshNext();

    /**
     * Displays a signed decimal integer, note that the minus sign uses one space.
     * In case of overflow it will show "  ..." or "-  ..."
     * 
     * @param number  the number to be displayed
     */
    void printDecInt(long int number);

    /**
     * Displays an unsigned hexadecimal integer. In case of overflow will ignore 
     * extra content.
     * 
     * @param long  the number to be displayed
     */
    void printHexInt(unsigned long int number);

  private:
    uint8_t _seg_pins[8]; ///< Segment output pins
    uint8_t _seg_cont[8]; ///< Segment output status
    uint8_t _on = HIGH;   ///< On status for the segments
    uint8_t _off = LOW;   ///< Off status for the segments

    SSDisplay_digit_s * _first = NULL;          ///< First digit in the linked list
    volatile SSDisplay_digit_s * _next = NULL;  ///< Selected digit in the linked list

    SSDisplayMode_e _digitMode;  ///< Mode for the digits
    int _digits = 0;             ///< Number of digits added
    
    /**
     * Updates the status of the outputs from _seg_cont[]
     */
    void _display();
};

#endif

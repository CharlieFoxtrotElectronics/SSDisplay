/**
 * @file "SevenSDtest.ino"
 * Seven segment display driver test
 *
 * @authot     Jaime Martínez-Figueroa <jmf@charliefoxtrot.cl>
 * @date       Feb 2016
 * @copyright  2016 Charlie Foxtrot Electronics, LGPL
 */

#include <SSDisplay.h>
#include <TimerOne.h>

#define TEST_STEP 12345

long int i = 0;
unsigned long int j = 0;

SSDisplay myDisplay(2, 3, 4, 5, 6, 7, 8, 9);

void setup() {
  myDisplay.addDigit(A5);
  myDisplay.addDigit(A4);
  myDisplay.addDigit(A3);
  myDisplay.addDigit(A2);

  myDisplay.addDigit(A1);
  myDisplay.addDigit(A0);
  myDisplay.addDigit(11);
  myDisplay.addDigit(12);

  Timer1.initialize(1000);
  Timer1.attachInterrupt(flickDisplay);

  myDisplay.printDecInt(0);
}

void loop() {
  for(i = -20000000; i < 110000000; i+=random(1,TEST_STEP)) {
    myDisplay.printDecInt(i);
    // myDisplay.refreshAll(); // "manual" refresh of all the digits
    delay(10);
  }

  for(j = 0; j < 0xFFFFFFFF; j+=random(1,TEST_STEP)) {
    myDisplay.printHexInt(j);
    // myDisplay.refreshAll(); // "manual" refresh of all the digits
    delay(10);
  }
}

void flickDisplay() {
  myDisplay.refreshNext(); // refresh one digit at a time
}

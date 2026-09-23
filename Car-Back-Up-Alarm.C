// ReverseBeeper.C

// Global Variables
unsigned long int TIME, TIME0, TIME1, dT;

unsigned char PIXEL @ 0x000;

const unsigned char MSG0[20] = "Reverse Beeper      ";
 
// Subroutine Declarations
#include <pic18.h>
#include "lcd_portd.c"
 
void NeoPixel_Display(unsigned char RED,
            unsigned char GREEN, unsigned char BLUE)
{
   PIXEL = GREEN; asm("  call Pixel_8 ");
   PIXEL = RED;         asm("  call Pixel_8 ");
   PIXEL = BLUE;  asm("  call Pixel_8 ");
 
   asm("    return");
 
#asm
Pixel_8:
    call    Pixel_1
    call    Pixel_1
    call    Pixel_1
    call    Pixel_1
    call    Pixel_1
    call    Pixel_1
    call    Pixel_1
    call    Pixel_1
    return
 
Pixel_1:
      bsf ((c:3969)),0   ; PORTB,0
    nop
      btfss   ((c:0000)),7
      bcf ((c:3969)),0
      rlncf   ((c:0000)),F
    nop
    nop
    bcf ((c:3969)),0
    return
 
#endasm
   }
 
// Interrupt Service Routine
void interrupt IntServe(void)
{
   // Timer0 = ultrasonic transmit on RC0
   if (TMR0IF) {
      RC0 = !RC0;
      TMR0IF = 0;
   }
 
   // Timer1 overflow = extend time base
   if (TMR1IF) {
      TIME = TIME + 0x10000;
      TMR1IF = 0;
   }
 
   // CCP1 capture = measure echo width
   if (CCP1IF) {
      if (CCP1CON == 0x05) {      // rising edge
         TIME0 = TIME + CCPR1;
         CCP1CON = 0x04;          // next falling edge
      }
      else {                      // falling edge
         TIME1 = TIME + CCPR1;
         dT = TIME1 - TIME0;
         CCP1CON = 0x05;          // next rising edge
      }
      CCP1IF = 0;
   }
}
 
void Beep(void)
{
   unsigned int i, j;
 
   for(i=0; i<20; i++) {
      RA1 = !RA1;
      for(j=0; j<1558; j++);
   }
 
   RA1 = 0;
}
 
// Main Routine
void main(void)
{
   int mm;
   unsigned int i, N, Zone;
   int LastZone;
   unsigned char RED, GREEN, BLUE;
 
   TRISA = 0x00;   // RA1 = speaker / H-bridge input
   TRISB = 0xFE;
   TRISC = 0x04;   // RC2 = CCP1 input, RC0 = ultrasonic transmit
   TRISD = 0x00;
   TRISE = 0x00;
   ADCON1 = 0x0F;
 
   RA1 = 0;
   RC0 = 0;
 
   LCD_Init();
   LCD_Move(0,0);  for(i=0; i<20; i++) LCD_Write(MSG0[i]);
 
   TIME = 0;
   TIME0 = 0;
   TIME1 = 0;
   dT = 0;
 
   // Clear interrupt flags
   TMR0IF = 0;
   TMR1IF = 0;
   CCP1IF = 0;
 
   // Timer0 = ultrasonic transmit
   T0CS = 0;
   T0CON = 0x81;
   TMR0ON = 1;
   TMR0IE = 1;
   TMR0IP = 1;
 
   // Timer1 = ultrasonic timing
   TMR1CS = 0;
   T1CON = 0x81;
   TMR1ON = 1;
   TMR1IE = 1;
   TMR1IP = 1;
 
   // CCP1 = echo capture
   TRISC2 = 1;
   CCP1CON = 0x05;
   CCP1IE = 1;
 
   PEIE = 1;
   GIE = 1;
 
   while(1) {
      // distance in 1/10 mm
      mm = (dT * 1715) / 10000;
      Zone = -1;
      LastZone = -1;
 
      LCD_Move(1,0);  LCD_Write('D'); LCD_Write('i'); LCD_Write('s');
  LCD_Write('t'); LCD_Write('a'); LCD_Write('n'); LCD_Write('c');
  LCD_Write('e'); LCD_Write(':');
      LCD_Out(mm, 5, 1);
 
      // Reverse car beeper behavior
      if (mm < 2000) {            // under 199.9 mm
         Beep();
             Zone = 1;
      }
      else if (mm < 3000) {       // 200.0 to 299.9 mm
         Beep();
             Zone = 2;
         Wait_ms(50);
      }
      else if (mm < 4000) {       // 300.0 to 399.9 mm
         Beep();
             Zone = 2;
         Wait_ms(120);
      }
      else if (mm < 7000) {       // 400.0 to 699.9 mm
         Beep();
             Zone = 3;
         Wait_ms(250);
      }
      else if (mm < 10000) {      // 700.0 to 999.9 mm
         Beep();
         Zone = 3;
         Wait_ms(450);
      }
      else {
         RA1 = 0;
             Zone = 0;
         Wait_ms(50);
      }
            if (Zone != LastZone) {
                  LastZone = Zone;
                  if (Zone == 1) {
                        NeoPixel_Display(100, 0, 0);
                        NeoPixel_Display(100, 0, 0);
                        NeoPixel_Display(100, 0, 0);
                        NeoPixel_Display(100, 0, 0);
                        NeoPixel_Display(100, 0, 0);
                        NeoPixel_Display(100, 0, 0);
                        NeoPixel_Display(100, 0, 0);
                        NeoPixel_Display(100, 0, 0);
                  } else if (Zone == 2) {
                        NeoPixel_Display(100, 100, 0);
                        NeoPixel_Display(100, 100, 0);
                        NeoPixel_Display(100, 100, 0);
                        NeoPixel_Display(100, 100, 0);
                        NeoPixel_Display(100, 100, 0);
                        NeoPixel_Display(100, 100, 0);
                        NeoPixel_Display(100, 100, 0);
                        NeoPixel_Display(100, 100, 0);
                  } else if (Zone == 3) {
                        NeoPixel_Display(0, 100, 0);
                        NeoPixel_Display(0, 100, 0);
                        NeoPixel_Display(0, 100, 0);
                        NeoPixel_Display(0, 100, 0);
                        NeoPixel_Display(0, 100, 0);
                        NeoPixel_Display(0, 100, 0);
                        NeoPixel_Display(0, 100, 0);
                        NeoPixel_Display(0, 100, 0);
                  } else {
                        NeoPixel_Display(0, 0, 0);
                        NeoPixel_Display(0, 0, 0);
                        NeoPixel_Display(0, 0, 0);
                        NeoPixel_Display(0, 0, 0);
                        NeoPixel_Display(0, 0, 0);
                        NeoPixel_Display(0, 0, 0);
                        NeoPixel_Display(0, 0, 0);
                        NeoPixel_Display(0, 0, 0);
                  }
          }
   }

// STM103 version Feb 2017
// This code demonstrates Rx and Tx using SPI 1
// Struggled long time with MISO data value not arriving in receive buffer.
// Turns out you must use HSE clock (i.e. XTAL). If you use HSI, MISO signal is never read. Silicon bug.
// Liam Goudge Mar 2017

#include "../Foundation/STM32F103.h"


void wait (void) {
	int i=0;
	for (i=0;i<500;i++);
}

void flash (void) // Flash LED on MDFLY STM32F103 board
  {
	int i=0;

	 GPIOC_BSRR=0x2000; // Set PC13
	 for (i=0;i<0x8000;i++);
	 GPIOC_BSRR=0x20000000; // Clear PC13
	 for (i=0;i<0x8000;i++);
      }

int main(void)
{
	RCC_CR=0x10000; // HSE on.
					// Use HSE crystal clock source in SYSCLK mux.

	//Set the Clock config register to output SYSCLK clock at MCO pin 29 and select HSE clock source
	RCC_CFGR=0x4000001;

// Set up power to SPI1 and allocate pins PA4-7
// SPI1 is on APB2, SPI2 is on APB1
// Also enable GPIO port C for LED

	RCC_APB2ENR= 0x101D; // Start clock to SPI1, GPIO port C (for LED), GPIO port B (for SPI2), GPIO port A (for SPI) and AFIO

// Set up GPIO for PC13 to drive LED
	GPIOC_CRH = 0x200000; 	// PC13 output push pull
  	  	  	  	  	  	  	// GPIO output 10Mhz

// Set up pins for SPI1 on GPIOA_CRL
// Pins PA4-7. for SPI1
// PA7: MOSI. AF output. 2Mhz.
// PA6: MISO. Input. Pull up-down.
// PA5: SCLK. AF output. 2Mhz
// PA4. CS. GP Output. 2Mhz

	GPIOA_CRL = 0xB4B30000;
	GPIOA_ODR=0x40; // Set PA6 to pull-up input for MISO

// Set up SPI1
// SPI_CR1: SSM&SSI set to allow software control of chip select NSS, MSB First, SPE enable, Fclk/32 baud rate (250KHz), MASTER, CPOL, CPHA
  	  SPI1_CR1 = 0x364;

  	  char dataout = 0xD0; // ID register command for BME280 external pressure sensor
  	  char datain[4];

  	  while (1) {

	  // Set cs to low (enable)
	  GPIOA_BSRR = 0x100000;

	  while (!(SPI1_SR & 0x2)); // wait for TX buffer to clear
	  SPI1_DR = dataout;

	  while (!(SPI1_SR & 0x2)); // wait for TX buffer to clear
	  SPI1_DR = 0; // Send dummy packet

	  while (!(SPI1_SR & 0x2)); // wait for Tx buffer to clear
	  while (!(SPI1_SR & 0x1)); // wait while RX empty RXNE

	  datain[0] = SPI1_DR;

	  while ((SPI1_SR & 0x80)); // wait while BUSY is still set

	  GPIOA_BSRR=0x10; // set CS hi

  	  }

}







/*


	  // Chip select lo (reset)
	  //GPIOA_BSRR = GPIOA_BSRR | 0x100000;
  	GPIOA_BSRR=0x10; // set CS hi

	  //Generate 74+ clock ticks
	  int i=0;


	  for (i=0;i<10;i++) {
		  while (!(SPI_SR & 0x2));
		  SPI_DR=0xff; // send 0xFF while waiting for data receive

	  }


	  while (SPI_SR & 0x80); // wait for busy flag to clear 1. When here, overflowing in TX. Not waited for busy to end. Understand


	  // now cs to low
	  GPIOA_BSRR = 0x100000; //2 Overflow still set from before (1) ? Rx has something in it.


// ---- RESET

	  int data[10];
	  int status[10];


	  data[0] = SPI_DR;
	  status[0] = SPI_SR;


	  // send CMD0 (card reset command)
	  SPI_DR=0x40; // CMD number 0 3. Still overflow. But Rx now empty as I read it.
	  while (!(SPI_SR & 0x2)); // wait for TX buffer to clear

	  data[1] = SPI_DR;
	  status[1] = SPI_SR;

	  SPI_DR=0x0; //4
	  while (!(SPI_SR & 0x2)); // wait for TX buffer to clear

	  data[2] = SPI_DR;
	  status[2] = SPI_SR;

	  SPI_DR=0x0; //5
	  while (!(SPI_SR & 0x2)); // wait for TX buffer to clear

	  data[3] = SPI_DR;
	  status[3] = SPI_SR;

	  SPI_DR=0x0;
	  while (!(SPI_SR & 0x2)); // wait for TX buffer to clear

	  data[4] = SPI_DR;
	  status[4] = SPI_SR;

	  SPI_DR=0x0; //6
	  while (!(SPI_SR & 0x2)); // wait for TX buffer to clear

	  data[5] = SPI_DR;
	  status[5] = SPI_SR;

	  SPI_DR=0x95; // CRC
	  while (!(SPI_SR & 0x2)); // wait for TX buffer to clear

	  data[6] = SPI_DR;
	  status[6] = SPI_SR;

	  // ==============


	  while (!(SPI_SR & 0x1)); // wait for RXNE RX buffer is ready

	  data[7] = SPI_DR;
	  status[7] = SPI_SR;



	  for (i=0;i<20;i++) {
		  //while (!(SPI_SR & 0x80)); // wait while BUSY is still set
		  SPI_DR=0xff;

	  }



	  data[8] = SPI_DR;
	  status[8] = SPI_SR;




	  // send CMD58 (Read OCR. command 8 does not work with this card)
	  SPI_DR=0x7a; // CMD number 58
	  while (!(SPI_SR & 0x2)); // wait for TX buffer to clear
	  SPI_DR=0x0;
	  while (!(SPI_SR & 0x2)); // wait for TX buffer to clear
	  SPI_DR=0x0;
	  while (!(SPI_SR & 0x2)); // wait for TX buffer to clear
	  SPI_DR=0x0;
	  while (!(SPI_SR & 0x2)); // wait for TX buffer to clear
	  SPI_DR=0x0;

	  while (!(SPI_SR & 0x2)); // wait for TX buffer to clear
	  SPI_DR=0x75; // CRC
/*
	  for (i=0;i<10;i++) {
		  while (!(SPI_SR & 0x2));
		  SPI_DR=0xff;

	  }


	  while(1);

	  while (!(SPI_SR & 0x1)) {
		  SPI_DR=0xff; // send 0xFF while waiting for data receive
	  }





	  int count=0;
	  for (count=0;count<2;count++)
	  {

	  // send ACMD41 = CMD55 then CMD41
	  SPI_DR=0x77; // CMD55
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;

	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x75; // CRC

	  for (i=0;i<5;i++) {
		  while (!(SPI_SR & 0x2));
		  SPI_DR=0xff;

	  }

	  // send ACMD41 = CMD55 then CMD41
	  SPI_DR=0x69; // CMD41
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;

	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x75; // CRC

	  for (i=0;i<15;i++) {
		  while (!(SPI_SR & 0x1));
		  SPI_DR=0xff;

	  }




	  }
/*
	  // send CMD10 (CID)
	  SPI_DR=0x4A; // CMD10
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;

	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x75; // CRC

	  for (i=0;i<100;i++) {
		  while (!(SPI_SR & 0x2));
		  SPI_DR=0xff;

	  }



	  // send CMD17 (Read 512B from address)
	  SPI_DR=0x51; // CMD17
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;
	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x0;

	  while (SPI_SR & 0x80); // wait for busy flag to clear
	  SPI_DR=0x75; // CRC

	  for (i=0;i<100;i++) {
		  while (!(SPI_SR & 0x2));
		  SPI_DR=0xff;

	  }

	  // Chip select hi (set)
	  //GPIOA_BSRR=GPIOA_BSRR | 0x10;



  while (1) {
	  flash();
  }

} */





#include "hw_gpio.h"
#include "stdio.h"
//#include "stdint.h"
// Define uint8_t as an alias for unsigned char
#define uint8_t unsigned char

 //Define buffer size as 16
#pragma DATA_ALIGN(controlTable, 1024)


int values[16];
static int controlTable[1024];

// Define receive buffer as array of unsigned characters with buffer size

// Define DMA control table as an array of unsigned characters with size 1024 and initialize a pointer to its base address


// Define UDMA_CTL_TABLE_R as a volatile unsigned long pointer to the DMA control table base address

#define DELAY_VALUE  4000


// Delay function that waits for a fixed amount of time
void Delay(void);

// Function to initialize UART0
void UART_Init(void);

// Function to print a single character over UART0
void UART_print_char(char c);

// Function to print a null-terminated string over UART0
void UART_print_string(char* s);

// Function to initialize DMA for UART0 transmission
void DMA_Init(void);

// Function to transmit data using DMA on UART0
void UART_DMA_Transmit(void);


int main(void)

{
    // Initialize UART and DMA
    UART_Init();
    DMA_Init();

    // Print a message over UART
    char startMessage[]="welcome from UART0 \n";
    UART_print_string(startMessage);
    // Recieve Data from UART (16 Bytes) Wait for DMA transmission to complete (polling method)

    // Loop to transmit 16 characters over UART using DMA
    while(1){
        UART_DMA_Transmit();
    }

    // Infinite loop to prevent the program from exiting
    while(1){

    }
return 0;
}

//This is a simple delay function that creates a software delay in the program
void Delay(void)

{

volatile unsigned long i;

for(i=0;i<DELAY_VALUE;i++){};

}

void UART_Init(void)
{
    SYSCTL_RCGCGPIO_R|=1;
        while((SYSCTL_PRGPIO_R &0x01)==0);
        GPIO_PORTA_AFSEL_R |=(1<<0) | (1<<1);
        GPIO_PORTA_PCTL_R |=0x11;
        GPIO_PORTA_AMSEL_R &= ~0x03;
        GPIO_PORTA_DEN_R |= 0x03;


    SYSCTL_RCGCUART_R|=1;
        UART0_CTL_R &=~1;
        UART0_IBRD_R|= 104;        // 104;
        UART0_FBRD_R |=11;        //11;
        UART0_LCRH_R|=(1<<5) | (1<<6);
         UART0_IFLS_R &=~0x38;
        UART0_CTL_R |=0x0301;
        UART0_DMACTL_R|= (1<<0) |(1<<1);
    
}

/**
 * Sends a single character over UART0.
 *
 * @param c The character to be sent.
 */
void UART_print_char(char c){

    // Wait until Ready to transmit
    while((UART0_FR_R & (1<<5)) != 0);

    // Send the character to the data register
    UART0_DR_R = c;
}


/**
 * Sends a string of characters over UART0.
 *
 * @param s A pointer to the first character of the string to be sent.
 */
void UART_print_string(char* s){

    // Iterate through the string and send each character
    while(*s){

        // Send the current character
        UART_print_char(*(s++));
    }
}

void UART0_Handler( void )
    {

     //Add your Code Here

}

void DMA_Init(void)
{

        SYSCTL_RCGCDMA_R|=1;
        Delay();
        UDMA_CFG_R|=1;
        UDMA_CTLBASE_R|=(volatile uint32_t)controlTable;
        UDMA_ALTCLR_R= (1<<8);
        UDMA_USEBURSTCLR_R|=(1<<8);
        UDMA_REQMASKCLR_R |=(1<<8);
        controlTable[32]=(volatile uint32_t)&UART0_DR_R;
        controlTable[33] |= (volatile uint32_t) values;
        controlTable[34] |= 1|(3<<26)|(0<<30)|(0<<28);
        UDMA_ENASET_R |= (1<<8);

        UDMA_ALTCLR_R= (1<<9);
        UDMA_USEBURSTCLR_R|=(1<<9);
        UDMA_REQMASKCLR_R |=(1<<9);
        controlTable[32]=(volatile uint32_t)values;
        controlTable[33] |= (volatile uint32_t)&UART0_DR_R;
        controlTable[34] |= 1|(0<<26)|(0<<30)|(0<<28)|(3<<30);
        UDMA_ENASET_R |= (1<<9);




}

uint8_t UART0_Available(){
  return ((UART0_FR_R &UART_FR_RXFE)==UART_FR_RXFE)? 0:1;
}
void UART_DMA_Transmit( void )
    {
    char c;

    while(UART0_Available() !=1){};
                UDMA_ENASET_R = (1 << 0);

            // Wait for the DMA transfer to complete
           // while((UDMA_CHIS_R & (1 << 0)) == 0);

            // Clear the DMA transfer complete flag
            UDMA_CHIS_R = (1 << 0);

        c=(UART0_DR_R & 0xFF);

        while((UART0_FR_R & UART_FR_TXFF)==UART_FR_TXFF){};
        UDMA_ENASET_R = (1 << 0);
        UDMA_CHIS_R = (1 << 0);
            UART0_DR_R = c;
    

}

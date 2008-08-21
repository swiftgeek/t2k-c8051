<<<<<<< .mine
/*****************************************************************************
/*
  Name:         AT25160A.h
  Created by:   Bahman Sotoodian  								Feb/28/2008


  Contents:     EEPROM user interface

  
\****************************************************************************/

#ifndef  _EEPROM_H_
#define  _EEPROM_H_

// SPI Protocol Signals
sbit EEP_SCK  = MSCB_SPI_SCK;
sbit EEP_MOSI = MSCB_SPI_MISO;
sbit EEP_MISO = MSCB_SPI_MOSI;

sbit RAM_CSn  = RAM_CHIP_SELECT;     // The Chip Select Signal to select the chip
sbit RAM_HLDn = RAM_HOLD_DOWN;		 // The Hold Down Signal to puase the serial communication			
sbit RAM_WPn  = RAM_WRITE_PROTECT;   // The Write Protection signal to enable/disable write to 
												 // the status register

// The Instruction Set for the AT25160A Device 
#define AT2516_WREN  0x06  						//Set Write Enable Latch
#define AT2516_WRDI  0x04  						//Reset Write Enable Latch
#define AT2516_RDSR  0x05  						//Read Status Register
#define AT2516_WRSR  0x01  						//Write Status Register
#define AT2516_READ  0x03  						//Read Data from Memory Array
#define AT2516_WRITE 0x02  						//Write Data to Memory Array

#define WP_START_ADDR	0x0600
#define WP_FINAL_ADDR	0x07FF

#define AT2516_MAX_BYTE 32
#define AT2516_PROTECTION 0x84					//Setting the bits of Status register: 
												//Setting the BP0, WEN and WPEN to 1 and BP1 to 0.

//unsigned int page_address[10] =0;

// Defing the Status register's bits
#define AT2516_WEN   0x02
#define AT2516_WPEN  0x80

#define ERROR -1

signed char ExtEEPROM_Init (unsigned char npages, unsigned char page_size);
signed char ExtEEPROM_Read (unsigned int ReadPage, unsigned char *destination, 
unsigned char page_size);
signed char ExtEEPROM_Write_Clear(unsigned int WritePage, unsigned char *source, 
unsigned char page_size, unsigned char clear);
signed char ExtEEPROM_WriteProtect (unsigned char *source,unsigned char page_size);
void ExtEEPROM_WriteEnable(void);
void ExtEEPROM_WriteStatusReg(unsigned char status);
unsigned char ExtEEPROM_Status(void);
void ExtEEPROM_Wait(void);

#endif=======
��/ * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 / * 
     N a m e :                   A T 2 5 1 6 0 A . h 
     C r e a t e d   b y :       B a h m a n   S o t o o d i a n     	 	 	 	 	 	 	 	 F e b / 2 8 / 2 0 0 8 
 
 
     C o n t e n t s :           E E P R O M   u s e r   i n t e r f a c e 
 
     
 \ * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * / 
 
 # i f n d e f     _ E E P R O M _ H _ 
 # d e f i n e     _ E E P R O M _ H _ 
 
 / /   S P I   P r o t o c o l   S i g n a l s 
 s b i t   E E P _ S C K     =   M S C B _ S P I _ S C K ; 
 s b i t   E E P _ M O S I   =   M S C B _ S P I _ M I S O ; 
 s b i t   E E P _ M I S O   =   M S C B _ S P I _ M O S I ; 
 
 s b i t   R A M _ C S n     =   R A M _ C H I P _ S E L E C T ;           / /   T h e   C h i p   S e l e c t   S i g n a l   t o   s e l e c t   t h e   c h i p 
 s b i t   R A M _ H L D n   =   R A M _ H O L D _ D O W N ; 	 	   / /   T h e   H o l d   D o w n   S i g n a l   t o   p u a s e   t h e   s e r i a l   c o m m u n i c a t i o n 	 	 	 
 s b i t   R A M _ W P n     =   R A M _ W R I T E _ P R O T E C T ;       / /   T h e   W r i t e   P r o t e c t i o n   s i g n a l   t o   e n a b l e / d i s a b l e   w r i t e   t o   
 	 	 	 	 	 	 	 	 	 	 	 	   / /   t h e   s t a t u s   r e g i s t e r 
 
 / /   T h e   I n s t r u c t i o n   S e t   f o r   t h e   A T 2 5 1 6 0 A   D e v i c e   
 # d e f i n e   A T 2 5 1 6 _ W R E N     0 x 0 6     	 	 	 	 	 	 / / S e t   W r i t e   E n a b l e   L a t c h 
 # d e f i n e   A T 2 5 1 6 _ W R D I     0 x 0 4     	 	 	 	 	 	 / / R e s e t   W r i t e   E n a b l e   L a t c h 
 # d e f i n e   A T 2 5 1 6 _ R D S R     0 x 0 5     	 	 	 	 	 	 / / R e a d   S t a t u s   R e g i s t e r 
 # d e f i n e   A T 2 5 1 6 _ W R S R     0 x 0 1     	 	 	 	 	 	 / / W r i t e   S t a t u s   R e g i s t e r 
 # d e f i n e   A T 2 5 1 6 _ R E A D     0 x 0 3     	 	 	 	 	 	 / / R e a d   D a t a   f r o m   M e m o r y   A r r a y 
 # d e f i n e   A T 2 5 1 6 _ W R I T E   0 x 0 2     	 	 	 	 	 	 / / W r i t e   D a t a   t o   M e m o r y   A r r a y 
 
 # d e f i n e   W P _ S T A R T _ A D D R 	 0 x 0 6 0 0 
 # d e f i n e   W P _ F I N A L _ A D D R 	 0 x 0 7 F F 
 
 # d e f i n e   A T 2 5 1 6 _ M A X _ B Y T E   3 2 
 # d e f i n e   A T 2 5 1 6 _ P R O T E C T I O N   0 x 8 4 	 	 	 	 	 / / S e t t i n g   t h e   b i t s   o f   S t a t u s   r e g i s t e r :   
 	 	 	 	 	 	 	 	 	 	 	 	 / / S e t t i n g   t h e   B P 0 ,   W E N   a n d   W P E N   t o   1   a n d   B P 1   t o   0 . 
 
 / / u n s i g n e d   i n t   p a g e _ a d d r e s s [ 1 0 ]   = 0 ; 
 
 / /   D e f i n g   t h e   S t a t u s   r e g i s t e r ' s   b i t s 
 # d e f i n e   A T 2 5 1 6 _ W E N       0 x 0 2 
 # d e f i n e   A T 2 5 1 6 _ W P E N     0 x 8 0 
 
 # d e f i n e   E R R O R   - 1 
 
 s i g n e d   c h a r   E x t E E P R O M _ I n i t   ( u n s i g n e d   c h a r   n p a g e s ,   u n s i g n e d   c h a r   p a g e _ s i z e ) ; 
 s i g n e d   c h a r   E x t E E P R O M _ R e a d   ( u n s i g n e d   i n t   R e a d P a g e ,   u n s i g n e d   c h a r   * d e s t i n a t i o n ,   
 u n s i g n e d   c h a r   p a g e _ s i z e ) ; 
 s i g n e d   c h a r   E x t E E P R O M _ W r i t e _ C l e a r ( u n s i g n e d   i n t   W r i t e P a g e ,   u n s i g n e d   c h a r   * s o u r c e ,   
 u n s i g n e d   c h a r   p a g e _ s i z e ,   u n s i g n e d   c h a r   c l e a r ) ; 
 s i g n e d   c h a r   E x t E E P R O M _ W r i t e P r o t e c t   ( u n s i g n e d   c h a r   * s o u r c e , u n s i g n e d   c h a r   p a g e _ s i z e ) ; 
 v o i d   E x t E E P R O M _ W r i t e E n a b l e ( v o i d ) ; 
 v o i d   E x t E E P R O M _ W r i t e S t a t u s R e g ( u n s i g n e d   c h a r   s t a t u s ) ; 
 u n s i g n e d   c h a r   E x t E E P R O M _ S t a t u s ( v o i d ) ; 
 v o i d   E x t E E P R O M _ W a i t ( v o i d ) ; 
 
 # e n d i f >>>>>>> .r320

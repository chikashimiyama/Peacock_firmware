#include "Peacock.h"

USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = 0,
				.DataINEndpoint           =
					{
						.Address          = CDC_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};

volatile uint8_t gCommand = 0;

ISR(INT0_vect)
{
	_delay_ms(15);
	EIFR |= (1 << INTF1);
	gCommand |= 0x01;
	toggleLED(false);
}

ISR(INT1_vect)
{
	_delay_ms(15);
	EIFR |= (1 << INTF2);
	gCommand |= 0x02;
	toggleLED(false);
}

ISR(INT2_vect)
{
	_delay_ms(15);
	EIFR |= (1 << INTF3); 
	gCommand |= 0x04; 
	toggleLED(false);
}

ISR(INT3_vect)
{
	_delay_ms(10);
	EIFR |= (1 << INTF4);
	gCommand |= 0x08;
	toggleLED(false);
}

void setupHardware(void){
	MCUSR &= ~(1 << WDRF); // no watch dog
	wdt_disable();
	clock_prescale_set(clock_div_1); // no command
	USB_Init();// USB init
}

void setupLED(void){
	LED_DDR |= (1 << LED_PIN); // LED pin set to out
	LED_PORT |= (1 << LED_PIN); // turn the LED on
}

void toggleLED(bool sw){
	if(sw){
		LED_PORT |= (1 << LED_PIN);
	}else{
		LED_PORT &= ~(1 << LED_PIN);	
	}
}

void setupButtons(void){
	
	BUTTON_BT0_PORT |= (1 << BUTTON_BT0_PIN); // pull up
	EICRA |= (1 << ISC01); // falling edge of INT0 generates interrupt	
	EIMSK |= (1 << INT0 );// enable external interrupt 0

	BUTTON_BT1_PORT |= (1 << BUTTON_BT1_PIN); // pull up
	EICRA |= (1 << ISC11); // falling edge of INT1 generates interrupt
	EIMSK |= (1 << INT1 );// enable external interrupt 1
	
	BUTTON_BT2_PORT |= (1 << BUTTON_BT2_PIN); // pull up
	EICRA |= (1 << ISC21); // falling edge of INT2 generates interrupt
	EIMSK |= (1 << INT2 );// enable external interrupt 2
	
    BUTTON_BT3_PORT |= (1 << BUTTON_BT3_PIN); // pull up
 	EICRA |= (1 << ISC31 ); // falling edge of INT3 generates interrupt
	EIMSK |= (1 << INT3 );// enable external interrupt 3
}

void setupSPI(void){
	
	SPI_DDR |= (1 << SPI_MOSI | 1 << SPI_SCK);
	
	//Chip select output
	SPI_CS0_DDR  |= ( 1 << SPI_CS0_PIN);
	SPI_CS1_DDR  |= ( 1 << SPI_CS1_PIN);
	SPI_CS2_DDR  |= ( 1 << SPI_CS2_PIN);
	SPI_CS3_DDR  |= ( 1 << SPI_CS3_PIN);
	SPI_CS4_DDR  |= ( 1 << SPI_CS4_PIN);
	
	//init with HI
	SPI_CS0_PORT |= ( 1 << SPI_CS0_PIN);
	SPI_CS1_PORT |= ( 1 << SPI_CS1_PIN);
	SPI_CS2_PORT |= ( 1 << SPI_CS2_PIN);
	SPI_CS3_PORT |= ( 1 << SPI_CS3_PIN);
	SPI_CS4_PORT |= ( 1 << SPI_CS4_PIN);
	
	SPCR = ( 1 << SPE ) | ( 1 << MSTR ) | (1 << SPR0); // enable SPI as Master at 1/16 speed
}

uint8_t spiReadWrite(uint8_t out){
	SPDR = out;
	while (!(SPSR & (1<<SPIF)));
	return SPDR;
}

uint8_t clip(uint8_t in){
	if(in > DATA_MAX){
		in = DATA_MAX;
	}
	return in;
}

void getRowData(volatile uint8_t *targetPort, uint8_t targetPin, uint8_t *buffer){
	uint8_t command, rvalue;

	for(int i = 0; i < COLUMN; i++){
		*targetPort &= ~(1 << targetPin); //pin LO - start exchange
		command = (SPI_SINGLE_MODE_MASK | i) << 2; // adjust to 8 bit
		spiReadWrite(command);
		rvalue = spiReadWrite(0);
		*targetPort |= 1 << targetPin; //pin HI - end exchange
		buffer[i] = clip(rvalue);
		_delay_us(100);
	}
}


int main(void)
{
	uint8_t buffer[NUM_SENSORS + 3];
	uint8_t cmd[5];	
	

	setupHardware();
	setupLED();
	setupSPI();
	setupButtons();
	
	_delay_ms(100);
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	lcd_puts("Peacock ver. 0.2\n");
	lcd_puts("by C.Miyama");
	
	_delay_ms(1000);
	GlobalInterruptEnable();

	while(1)
	{
		lcd_clrscr();
		lcd_puts("fuck");
		_delay_ms(10);
		
		if(gCommand > 0){
			cmd[0] = START_DELIMETER;
			cmd[1] = COMMAND_PACKET;
			cmd[2] = gCommand;
			cmd[3] = END_DELIMETER;
			gCommand = 0;
			CDC_Device_SendData(&VirtualSerial_CDC_Interface, (void*)cmd, 4);
		}
		

		buffer[0] = START_DELIMETER;
		buffer[1] = DATA_PACKET;
		getRowData(&SPI_CS0_PORT, SPI_CS0_PIN, &buffer[2] );
		getRowData(&SPI_CS1_PORT, SPI_CS1_PIN, &buffer[COLUMN + 2] );
		getRowData(&SPI_CS2_PORT, SPI_CS2_PIN, &buffer[COLUMN *2 + 2] );
		getRowData(&SPI_CS3_PORT, SPI_CS3_PIN, &buffer[COLUMN *3 + 2] );
		getRowData(&SPI_CS4_PORT, SPI_CS4_PIN, &buffer[COLUMN *4 + 2] );
		buffer[37] = END_DELIMETER;
		CDC_Device_SendData(&VirtualSerial_CDC_Interface, (void*)buffer, 38); // 35x data 2x delimeter 1x type 1x checksum
		
		CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
		_delay_ms(10);
		
	}
}

void EVENT_USB_Device_Connect(void)
{

}

void EVENT_USB_Device_Disconnect(void)
{

}

void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;
	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}

void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}


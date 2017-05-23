#include "usbkeyboard2.h"

#ifdef ONEKEY
uchar  reportBuffer[2];
static uint8_t    idleRate;

int setup(void){
	
	DDRB &= ~(1<<0);
	DDRB |= (1<<1);
	DDRB &= ~(1<<2);

	PORTB|= (1<<0);
	PORTB&= ~(1<<1);
	PORTB|= (1<<2);
	
	usb_init();
	uint8_t sign0=0;
	while (1)
	{
		if((PINB&(1<<0))==0)
		{
			if(sign0==0)keyPrintWord2();
			sign0=0x20;
		}
		if(sign0>0)sign0--;
		usbPoll();
	}
	return 0;
}
const PROGMEM char usbHidReportDescriptor[] = { /* USB report descriptor */
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x09, 0x06,                    // USAGE (Keyboard)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
	0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
	0x75, 0x01,                    //   REPORT_SIZE (1)
	0x95, 0x08,                    //   REPORT_COUNT (8)
	0x81, 0x02,                    //   INPUT (Data,Var,Abs)
	0x95, 0x01,                    //   REPORT_COUNT 1(simultaneous keystrokes)
	0x75, 0x08,                    //   REPORT_SIZE (8)
	0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
	0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
	0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
	0xc0                           // END_COLLECTION
};


void usb_init()
{
	usbInit();
	/* enforce USB re-enumerate: */
	usbDeviceDisconnect();  /* do this while interrupts are disabled */
	//wdt_reset();
	/* fake USB disconnect for > 250 ms */
	_delay_ms(300);
	usbDeviceConnect();
	sei();
}
static void keyPrintChar2(uint8_t data)
{
	while(1){usbPoll();
		if(usbConfiguration && usbInterruptIsReady()){
			reportBuffer[0] = (data >> 7) ? 0x20 : 0x00;//shift加了128
			reportBuffer[1] =data & 0b01111111;//abs删除正负号
			usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
			break;
		}
	}
	while(1){usbPoll();
		if(usbConfiguration && usbInterruptIsReady()){
			reportBuffer[0] = 0;
			reportBuffer[1] =0;
			usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
			break;
		}
	}usbPoll();
}
void keyPrintWord2(){
	reportBuffer[0] = 0;
	reportBuffer[1] =0;
	uint8_t i=0;	
	uint8_t len=eeprom_read_byte((uchar *)0);
		for(i=0;i<len;i++){
			while(1){
				if(usbConfiguration && usbInterruptIsReady()){
					uint8_t data = eeprom_read_byte((uchar *)1 +i);
					keyPrintChar2(data);
					break;
				}usbPoll();
			}
		}
}

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t    *rq = (usbRequest_t *)((void *)data);
	usbMsgPtr = reportBuffer; //
	if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
		if (rq->bRequest == USBRQ_HID_GET_REPORT) {return 0;}
		else if (rq->bRequest == USBRQ_HID_GET_IDLE) {return 0;}
		else if (rq->bRequest == USBRQ_HID_SET_IDLE) {idleRate = rq->wValue.bytes[1];}
	}
	if ((rq->bmRequestType & USBRQ_TYPE_MASK) ==USBRQ_TYPE_VENDOR) {
		if (rq->bRequest == 0x02) {
			eeprom_busy_wait();
			eeprom_write_byte ((uchar *)0 +rq->wIndex.bytes[0],rq->wValue.bytes[0]);
			PORTB|=(1<<1);
		return 0;}
		if (rq->bRequest == 0x03) {
			//结束写
			eeprom_busy_wait();
			PORTB&= ~(1<<1);
		return 0;}
		if (rq->bRequest == 0x01) {
			//开始写
			eeprom_is_ready();
			PORTB&= ~(1<<1);
		return 0;}
	}
	return 0;
}
const uint8_t ascii_to_scan_code_table[] PROGMEM = {
	// /* ASCII:   0 */
	0,
	// /* ASCII:   1 */
	0,
	// /* ASCII:   2 */
	0,
	// /* ASCII:   3 */
	0,
	// /* ASCII:   4 */
	0,
	// /* ASCII:   5 */
	0,
	// /* ASCII:   6 */
	0,
	// /* ASCII:   7 */
	0,
	/* ASCII:   8 */ 42,
	/* ASCII:   9 */ 43,
	/* ASCII:  10 */ 40,
	/* ASCII:  11 */ 0,
	/* ASCII:  12 */ 0,
	/* ASCII:  13 */ 0,
	/* ASCII:  14 */ 0,
	/* ASCII:  15 */ 0,
	/* ASCII:  16 */ 0,
	/* ASCII:  17 */ 0,
	/* ASCII:  18 */ 0,
	/* ASCII:  19 */ 0,
	/* ASCII:  20 */ 0,
	/* ASCII:  21 */ 0,
	/* ASCII:  22 */ 0,
	/* ASCII:  23 */ 0,
	/* ASCII:  24 */ 0,
	/* ASCII:  25 */ 0,
	/* ASCII:  26 */ 0,
	/* ASCII:  27 */ 41,
	/* ASCII:  28 */ 0,
	/* ASCII:  29 */ 0,
	/* ASCII:  30 */ 0,
	/* ASCII:  31 */ 0,
	/* ASCII:  32 */ 44,
	/* ASCII:  33 */ 158,
	/* ASCII:  34 */ 180,
	/* ASCII:  35 */ 160,
	/* ASCII:  36 */ 161,
	/* ASCII:  37 */ 162,
	/* ASCII:  38 */ 164,
	/* ASCII:  39 */ 52,
	/* ASCII:  40 */ 166,
	/* ASCII:  41 */ 167,
	/* ASCII:  42 */ 165,
	/* ASCII:  43 */ 174,
	/* ASCII:  44 */ 54,
	/* ASCII:  45 */ 45,
	/* ASCII:  46 */ 55,
	/* ASCII:  47 */ 56,
	/* ASCII:  48 */ 39,
	/* ASCII:  49 */ 30,
	/* ASCII:  50 */ 31,
	/* ASCII:  51 */ 32,
	/* ASCII:  52 */ 33,
	/* ASCII:  53 */ 34,
	/* ASCII:  54 */ 35,
	/* ASCII:  55 */ 36,
	/* ASCII:  56 */ 37,
	/* ASCII:  57 */ 38,
	/* ASCII:  58 */ 179,
	/* ASCII:  59 */ 51,
	/* ASCII:  60 */ 182,
	/* ASCII:  61 */ 46,
	/* ASCII:  62 */ 183,
	/* ASCII:  63 */ 184,
	/* ASCII:  64 */ 159,
	/* ASCII: A 65 */ 132,
	/* ASCII:  66 */ 133,
	/* ASCII:  67 */ 134,
	/* ASCII:  68 */ 135,
	/* ASCII:  69 */ 136,
	/* ASCII:  70 */ 137,
	/* ASCII:  71 */ 138,
	/* ASCII:  72 */ 139,
	/* ASCII:  73 */ 140,
	/* ASCII:  74 */ 141,
	/* ASCII:  75 */ 142,
	/* ASCII:  76 */ 143,
	/* ASCII:  77 */ 144,
	/* ASCII:  78 */ 145,
	/* ASCII:  79 */ 146,
	/* ASCII:  80 */ 147,
	/* ASCII:  81 */ 148,
	/* ASCII:  82 */ 149,
	/* ASCII:  83 */ 150,
	/* ASCII:  84 */ 151,
	/* ASCII:  85 */ 152,
	/* ASCII:  86 */ 153,
	/* ASCII:  87 */ 154,
	/* ASCII:  88 */ 155,
	/* ASCII:  89 */ 156,
	/* ASCII: Z 90 */ 157,
	/* ASCII:  91 */ 47,
	/* ASCII:  92 */ 49,
	/* ASCII:  93 */ 48,
	/* ASCII:  94 */ 163,
	/* ASCII:  95 */ 173,
	/* ASCII:  96 */ 53,
	/* ASCII: a 97 */ 4,
	/* ASCII:  98 */ 5,
	/* ASCII:  99 */ 6,
	/* ASCII: 100 */ 7,
	/* ASCII: 101 */ 8,
	/* ASCII: 102 */ 9,
	/* ASCII: 103 */ 10,
	/* ASCII: 104 */ 11,
	/* ASCII: 105 */ 12,
	/* ASCII: 106 */ 13,
	/* ASCII: 107 */ 14,
	/* ASCII: 108 */ 15,
	/* ASCII: 109 */ 16,
	/* ASCII: 110 */ 17,
	/* ASCII: 111 */ 18,
	/* ASCII: 112 */ 19,
	/* ASCII: 113 */ 20,
	/* ASCII: 114 */ 21,
	/* ASCII: 115 */ 22,
	/* ASCII: 116 */ 23,
	/* ASCII: 117 */ 24,
	/* ASCII: 118 */ 25,
	/* ASCII: 119 */ 26,
	/* ASCII: 120 */ 27,
	/* ASCII: 121 */ 28,
	/* ASCII: 122 */ 29,
	/* ASCII: 123 */ 175,
	/* ASCII: 124 */ 177,
	/* ASCII: 125 */ 176,
	/* ASCII: 126 */ 181
};
#endif
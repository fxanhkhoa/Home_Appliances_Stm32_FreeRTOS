#include "DS1307.h"

void decodeTime(const uint8_t *data, time *s_time) {
    // decode seconds
    uint8_t msd = 0, lsd = 0;
    uint8_t /*am_pm = -1,*/_12h_mode = -1;

    lsd = (data[0] & 0x0F); // 00001111
    msd = (data[0] & 0x70) >> 4; // 0b01110000
    s_time->seconds = lsd + 10 * msd;

    lsd = (data[1] & 0x0F); // 0b00001111
    msd = (data[1] & 0x70) >> 4;
    s_time->minutes = lsd + 10 * msd;

    // If 1, then 12-hour mode is enabled, 0 - 24-hour mode
    _12h_mode = (data[2] & 0x40) >> 6; // 0b01000000

    // When 12-hour mode enabled, PM = 1, AM = 0, otherwise first bit of
    // hour_msd
    if (_12h_mode) {
        //am_pm = (data[2] & 0x20) >> 5; // 00100000
        msd = (data[2] & 0x10) >> 4; // 00010000
    } else {
        msd = (data[2] & 0x30) >> 4; // 00110000
    }
    lsd = (data[2] & 0x0F); // 00001111
    s_time->hours = lsd + 10 * msd;

    s_time->day_of_week = (data[3] & 0x07); //00000111

    lsd = (data[4] & 0x0F); // 00001111
    msd = (data[4] & 0x30) >> 4; // 00110000
    s_time->date = lsd + 10 * msd;

    lsd = (data[5] & 0x0F); // 00001111
    msd = (data[5] & 0x10) >> 4; // 00010000
    s_time->month = lsd + 10 * msd;

    lsd = (data[6] & 0x0F);
    msd = (data[6] & 0xF0) >> 4; // 11110000
    s_time->year = lsd + 10 * msd;

    s_time->clock_halt = (data[0] & 0x80) >> 7; // 10000000
    s_time->out = (data[7] & 0x80) >> 7;
    s_time->sqwe = (data[7] & 0x10) >> 4; // 00010000
    s_time->rs1 = (data[7] & 0x02) >> 1; // 00000010
    s_time->rs0 = (data[7] & 0x01);
}

// Each number is represented in BCD format, according to documentation
uint8_t* encodeData(const time *s_time) {
	uint8_t *data = calloc(TIME_STRUCT_SIZE + 1, sizeof(uint8_t));
	uint8_t msd, lsd;

	data[0] = 0;

	// 0x00 Clock halt and seconds
	msd = s_time->seconds / 10;
	lsd = s_time->seconds - msd * 10;
	data[1] = (s_time->clock_halt << 7) | (msd << 4) | (lsd);

	// 0x01 Minutes
	msd = s_time->minutes / 10;
	lsd = s_time->minutes - msd * 10;
	data[2] = (msd << 4) | (lsd);

	// 0x02 Hours
	msd = s_time->hours / 10;
	lsd = s_time->hours - msd * 10;
	data[3] = (0 << 6 /*24h mode*/) | (msd << 4) | (lsd);

	// 0x03 Day of week
	data[4] = s_time->day_of_week;

	// 0x04 Date (day of month)
	msd = s_time->date / 10;
	lsd = s_time->date - msd * 10;
	data[5] = (msd << 4) | (lsd);

	// 0x05 Month
	msd = s_time->month / 10;
	lsd = s_time->month - msd * 10;
	data[6] = (msd << 4) | (lsd);

	// 0x06 Year
	msd = s_time->year / 10;
	lsd = s_time->year - msd * 10;
	data[7] = (msd << 4) | (lsd);

	// 0x07 Control part:
	// OUT, SQWE, RS1 and RS0
	data[8] = (s_time->out << 7) | (s_time->sqwe << 4) | (s_time->rs1 << 1)
					| (s_time->rs0);

	return data;
}

void printTime(const time *s_time) {
	int len;
	char str[25];
	len = sprintf(str,"%2d:%2d:%2d\n\r", s_time->hours, s_time->minutes, s_time->seconds);
	HAL_UART_Transmit(&huart1,(uint8_t*) str, len, 1000);
	switch (s_time->day_of_week) {
	case 1:
//			printf("Monday\n\r");
		HAL_UART_Transmit(&huart1,"Monday\n\r", 8, 1000);
			break;
	case 2:
//			printf("Tuesday\n\r");
		HAL_UART_Transmit(&huart1,"Tuesday\n\r", 9, 1000);
			break;
	case 3:
//			printf("Wednesday\n\r");
			break;
	case 4:
//			printf("Thursday\n\r");
			break;
	case 5:
//			printf("Friday\n\r");
			break;
	case 6:
//			printf("Saturday\n\r");
			break;
	case 7:
//			printf("Sunday\n\r");
			break;

	default:
//			printf("BROKEN\n\r");
			break;
	}

//	printf("%2d.%2d.%2d\n\r", s_time->date, s_time->month, s_time->year);
//	printf("Config: CH: %d OUT: %d SQWE: %d RS1: %d RS0: %d\n\r",
//					s_time->clock_halt, s_time->out, s_time->sqwe, s_time->rs1,
//					s_time->rs0);
}

void get_Day_Of_Week(char* str, uint8_t index){
	switch (index) {
	case 1:
//			printf("Monday\n\r");
		str[0] = 'M';
		str[1] = 'o';
		str[2] = 'n';
		str[3] = '\0';
	break;
	case 2:
//			printf("Tuesday\n\r");
		str[0] = 'T';
		str[1] = 'u';
		str[2] = 'e';
		str[3] = '\0';
	break;
	case 3:
//			printf("Wednesday\n\r");
		str[0] = 'W';
		str[1] = 'e';
		str[2] = 'd';
		str[3] = '\0';
	break;
	case 4:
//			printf("Thursday\n\r");
		str[0] = 'T';
		str[1] = 'h';
		str[2] = 'u';
		str[3] = '\0';
	break;
	case 5:
//			printf("Friday\n\r");
		str[0] = 'F';
		str[1] = 'r';
		str[2] = 'i';
		str[3] = '\0';
	break;
	case 6:
//			printf("Saturday\n\r");
		str[0] = 'S';
		str[1] = 'a';
		str[2] = 't';
		str[3] = '\0';
	break;
	case 7:
//			printf("Sunday\n\r");
		str[0] = 'S';
		str[1] = 'u';
		str[2] = 'n';
		str[3] = '\0';
	break;

	default:
//			printf("BROKEN\n\r");
		  str[0] = 'S';
			str[1] = 'a';
			str[2] = 't';
		break;
	}
}

void printByte(const uint8_t n) {
//    unsigned i;
//    for (i = 1 << 7; i > 0; i = i / 2)
//        (n & i) ? printf("1") : printf("0");
//    printf("\n\r");
}

void printRawData(const uint8_t *data, const uint8_t size) {
	if (data == 0) {
			return;
	}

	uint8_t idx;

	for (idx = 0; idx < size; ++idx) {
			printByte(data[idx]);
	}
}

time getTime(void) {
    uint8_t* data = readTime();
    time s_time;
    decodeTime(data, &s_time);
    free(data);
    return s_time;
}

uint8_t* readTime(void) {
	uint8_t *received_data = calloc(TIME_STRUCT_SIZE, sizeof(uint8_t));

	received_data = read(0x00, TIME_STRUCT_SIZE, false);

	return received_data;
}

void writeTime(const time *s_time) {
	uint8_t *data = encodeData(s_time);
//    printRawData(data, TIME_STRUCT_SIZE);

	write(0x00, TIME_STRUCT_SIZE + 1, false, data);

	free(data);
}

uint8_t* read(const uint8_t start_address, const uint8_t allocated_bytes, const bool ram) {

	uint8_t *received_data = calloc(allocated_bytes, sizeof(uint8_t));
	uint8_t a = 0;
	HAL_I2C_Master_Transmit(&DS1307_I2C_PORT, SLAVE_ADDRESS<<1, &a, 1, 100);
	
	HAL_I2C_Master_Receive(&DS1307_I2C_PORT, SLAVE_ADDRESS<<1, received_data, allocated_bytes, 100);
//	for (int i = 0; i < 8; i++)
//		buf[i] += 48;
	//HAL_UART_Transmit(&huart1, received_data, 8, 1000);
//	I2C_start(I2C1, SLAVE_ADDRESS << 1, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
//	if (ram) {
//			I2C_write(I2C1, start_address + TIME_STRUCT_SIZE); // set pointer to read (with offset)
//	} else {
//			I2C_write(I2C1, start_address); //
//	}
//	I2C_stop(I2C1); // stop the transmission
//	I2C_start(I2C1, SLAVE_ADDRESS << 1, I2C_Direction_Receiver); // start a transmission in Master receiver mode

//	for (index = 0; index < allocated_bytes; ++index) {
//			if (index + 1 < allocated_bytes) {
//					received_data[index] = I2C_read_ack(I2C1); // read one byte, send ack and request another byte
//			} else {
//					received_data[index] = I2C_read_nack(I2C1); // read one byte and don't request another byte, stop transmission
//			}
//	}

	return received_data;
}

void write(const uint8_t start_address, const uint8_t bytes, const bool ram, uint8_t* data) {
		
	//HAL_UART_Transmit(&huart1, "LET WRITE", 9, 1000);
	//HAL_UART_Transmit(&huart1, data, 8, 1000);
	HAL_I2C_Master_Transmit(&DS1307_I2C_PORT, SLAVE_ADDRESS<<1, data, bytes, 100);
	
//    I2C_start(I2C1, SLAVE_ADDRESS << 1, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode

//    if (ram) {
//        I2C_write(I2C1, start_address + TIME_STRUCT_SIZE); // set pointer to read (with offset)
//    } else {
//        I2C_write(I2C1, start_address); //
//    }

//    u8 index = 0;
//    for (index = 0; index <= bytes; index += 1) {
//        I2C_write(I2C1, data[index]); // write one byte to the slave
//    }
//    I2C_stop(I2C1); // stop the transmission
}

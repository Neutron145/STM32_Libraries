/*
 *******************************************************************************
 * 	@file			BMP280_hal.h
 *	@brief			This file provides parser for NMEA messages from GPS.
 *					GPS transmits following NMEA messages: GPGGA, GPGSA, GPGSV, GPGLL, GPRMC, GPVTG. 
 *					We will use GPVTG and GGA to get speed, altitude, longitude, latitude, hdop, number of sattelites and course
 * 
 *					All data is read on computer, not in the microcontroller
 * 
 *					Data reading algorithm:
 *					1. Get NMEA message 
 *					2. Calculate checksum
 *					3. If checksum correct, process message before parsing
 *					4. Extract necessary data from template
 *
 *	@author			Rafael Abeldinov
 *  @created 		04.12.2023
 *******************************************************************************
 */

#include "GPS_parser.h"

GGA_data GGA;
VTG_data VTG;

/*
 * @brief	Auxiliary function for insert char in message
 * @param	dest		- Line to insert char into
 * @param	source		- Char to be inserted into the line
 */
void __insert(char* &dest, char source, int pos) {
	char* cpy = new char[strlen(dest) + 1];
	strcpy(cpy, dest);
	for (int i = strlen(dest); i >= pos; i--) {
		cpy[i + 1] = cpy[i];
	}
	cpy[pos] = source;
	dest = _strdup(cpy);
}

/*
 * @brief	Internal function to calculate checksum of NMEA message
 * @param	buffer		- NMEA message for which checksum needs to be calculated
 * @retval	status:		- 1		Valid checksum
 *						- 0		Invalid checksum
 */
int __checksum(char* &buffer) {
	char* b = buffer;
	uint8_t i_xor = 0;
	while (*b++ != '$');
	buffer = b - 1;
	while (*b != '*') {
		i_xor ^= *b++;
	}
	char s_xor[3];
	sprintf(s_xor, "%x", i_xor);
	if (!strcmp(s_xor, b+1)) {
		return 1;
	}
	return 0;
}

/*
 * @brief	Internal function to process message before parse
 * @param	buffer		- NMEA message to process
 */
void __line_process(char* &buffer) {
	int pos = 0;
	while (buffer[pos++] != '*') {
		if (buffer[pos + 1] == ',' && buffer[pos] == ',') {
			__insert(buffer, '0', pos + 1);
		}
	}
}

/*
 * @brief	Main function to extract data from NMEA message into GGA and VTG structs
 * @param	buffer		- NMEA message
 * @retval	status:		- 2		Parse VTG message
 *						- 1		Parse GGA message
 *						- 0		Other NMEA message
 *						- -1	Invalid checksum
 *						- -2	Uncorrect data in NMEA
 */
int parse(char* buffer) {
	if (!__checksum(buffer)) return -1;
	__line_process(buffer);
	buffer += 3;
	char type[4];
	strncpy(type, buffer, 3);
	type[3] = 0;
	buffer += 4;
	if (strncmp(type, "GGA", 4) == 0) {
		int solve_type;
		float f;
		if (sscanf(buffer, "%f,%f,%c,%f,%c,%i,%i,%f,%f,M,%f,M,0", &GGA.date, &GGA.latitude, &GGA.NS, &GGA.longitude, &GGA.EW, &solve_type, &GGA.sats, &GGA.HDOP, &GGA.altitude, &f) != 10) {
			return -2;
		}
		if (solve_type == 0) return -2;
		return 1;
	}
	else if (strncmp(type, "VTG", 4) == 0) {
		char solve_type;
		char true_course;
		if (sscanf(buffer, "%f,%c,0,M,%f,N,%f,K,%c", &VTG.course, &true_course, &VTG.speed_kn, &VTG.speed_mph, &solve_type) != 5) {
			return -2;
		}
		if (solve_type == 'N' || true_course == 'F') return -2;

		return 2;
	}
	else return 0;
}
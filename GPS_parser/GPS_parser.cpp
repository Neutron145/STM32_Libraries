
#include "GPS_parser.h"

GGA_data GGA;
VTG_data VTG;


void __insert(char* &dest, char source, int pos) {
	char* cpy = new char[strlen(dest) + 1];
	strcpy(cpy, dest);
	for (int i = strlen(dest); i >= pos; i--) {
		cpy[i + 1] = cpy[i];
	}
	cpy[pos] = source;
	dest = _strdup(cpy);
}


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


void __line_process(char* &buffer) {
	int pos = 0;
	while (buffer[pos++] != '*') {
		if (buffer[pos + 1] == ',' && buffer[pos] == ',') {
			__insert(buffer, '0', pos + 1);
		}
	}
}


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
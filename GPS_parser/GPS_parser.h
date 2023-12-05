/*
 *******************************************************************************
 * 	@file			GPS_parser.h
 *	@brief			Header file for GPS_parser.c
 *
 *	@author			Rafael Abeldinov
 *  @created 		04.12.2023
 *******************************************************************************
 */

#ifndef GPS_PARSER_H
#define GPS_PARSER_H

#define _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include "fstream"

/* Data from GGA message ------------------------------------------------------*/
typedef struct {
	float date;
	float longitude;
	char EW;
	float latitude;
	char NS;
	float altitude;
	int solve_type;
	float HDOP;
	int sats;
} GGA_data;

/* Data from VTG message ------------------------------------------------------*/
typedef struct {
	float course;
	float speed_kn;
	float speed_mph;
} VTG_data;

/* Instances for storing data from recent messages ----------------------------*/
extern GGA_data GGA;
extern VTG_data VTG;

/* Auxiliary function for insert char in message ------------------------------*/
void __insert(char* &dest, char source, int pos);
/* Internal function for calculate checksum of message ------------------------*/
int __checksum(char* &buffer);
/* Internal function for processing lines before parse ------------------------*/
void __line_process(char* &buffer);
/* Main function for extract data from NMEA messages --------------------------*/
int parse(char* buffer);

#endif /* GPS_PARSER_H */
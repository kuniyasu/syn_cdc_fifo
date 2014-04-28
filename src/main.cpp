/*
 * main.cpp
 *
 *  Created on: 2014/04/29
 *      Author: Kuniyasu
 */

#include <systemc.h>
#include "TOP.h"

TOP* top;

int sc_main(int argc, char* argv[]){
	top = new TOP("TOP");

	return 0;
}


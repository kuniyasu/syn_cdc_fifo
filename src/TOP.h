/*
 * TOP.h
 *
 *  Created on: 2014/04/29
 *      Author: Kuniyasu
 */

#include <systemc.h>

class TOP:public sc_module{
public:
	sc_clock clk;
	sc_signal<bool> nrst;

	SC_HAS_PROCESS(TOP);

	TOP(const sc_module_name& name):sc_module(name),clk("clk",10, SC_NS, 0.5),nrst("nrst"){
		SC_THREAD(reset_thread);

		end_module();
	}

	void reset_thread(){
		nrst = false;
		wait(100, SC_NS);

		nrst = true;
		wait(100, SC_MS);

	}
};




/*
 * TOP.h
 *
 *  Created on: 2014/04/29
 *      Author: Kuniyasu
 */

#include <systemc.h>
#include "syn_cdc_dff.h"
#include "syn_cdc_fifo.h"

class Producer:public sc_module{
public:
	sc_in<bool> clk;
	sc_in<bool> nrst;

	typedef sc_uint<8> DT;

	sc_fifo_out<DT> fifo_out;
	syn_cdc_fifo_put<DT,4,2> out;

	SC_HAS_PROCESS(Producer);
	Producer(const sc_module_name& name=sc_gen_unique_name("Producer")):sc_module(name){
		SC_CTHREAD(main_thread,clk.pos());
		reset_signal_is(nrst,false);

		end_module();
	}

	void main_thread(){
		out.w_reset();
		wait();

		while( true ){

			wait();
		}
	}
};

class Consumer:public sc_module{
public:
	sc_in<bool> clk;
	sc_in<bool> nrst;

	typedef sc_uint<8> DT;

	sc_fifo_in<DT> fifo_in;
	syn_cdc_fifo_get<DT,4,2> in;

	SC_HAS_PROCESS(Consumer);
	Consumer(const sc_module_name& name=sc_gen_unique_name("Consumer")):sc_module(name){
		SC_CTHREAD(main_thread,clk.pos());
		reset_signal_is(nrst,false);
		end_module();
	}

	void main_thread(){
		in.r_reset();
		wait();

		while( true ){

			wait();
		}
	}


};

class TOP:public sc_module{
public:
	sc_clock clk;
	sc_signal<bool> nrst;
	typedef sc_uint<8> DT;

	sc_fifo<DT> fifo;
	syn_cdc_fifo_channel<DT,4,2> cdc_fifo;

	Producer producer;
	Consumer consumer;

	SC_HAS_PROCESS(TOP);

	TOP(const sc_module_name& name):sc_module(name),clk("clk",10, SC_NS, 0.5),nrst("nrst"){
		SC_THREAD(reset_thread);

		producer.clk(clk);
		producer.nrst(nrst);
		producer.fifo_out(fifo);
		producer.out(cdc_fifo);

		consumer.clk(clk);
		consumer.nrst(nrst);
		consumer.fifo_in(fifo);
		consumer.in(cdc_fifo);

		end_module();
	}

	void reset_thread(){
		nrst = false;
		wait(100, SC_NS);

		nrst = true;
		wait(100, SC_MS);

	}
};




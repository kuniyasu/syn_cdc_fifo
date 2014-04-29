/*
 * syn_cdc_dff.h
 *
 *  Created on: 2014/04/29
 *      Author: Kuniyasu
 */

#ifndef SYN_CDC_DFF_H_
#define SYN_CDC_DFF_H_

#include<systemc.h>
#define TR_NAME(_name) (std::string(name())+"_"+std::string(_name)).c_str()
#define PIN_NAME(bname,_name_) (std::string(bname)+"_"+std::string(_name_)).c_str()

template<class DT> class syn_cdc_dff:public sc_module{
public:
	sc_in<bool> wclk;
	sc_in<bool> rclk;
	sc_in<bool> nrst;

	sc_signal<DT> PUT_SIG;
	sc_signal<DT> GET_SIG[3];

	SC_HAS_PROCESS(syn_cdc_dff);

	syn_cdc_dff(const sc_module_name& name=sc_gen_unique_name("syn_cdc_dff")):sc_module(name){
		SC_METHOD(get_method);
		sensitive << wclk.pos() << nrst.neg();
		end_module();
	}

	void get_method(){
		if(nrst.read() ==  false){
			GET_SIG[0] = DT();
			GET_SIG[1] = DT();
			GET_SIG[2] = DT();
		}else{
			GET_SIG[0] = PUT_SIG.read();
			GET_SIG[1] = GET_SIG[0].read();
			GET_SIG[2] = GET_SIG[1].read();

		}
	}

	bool nb_get(DT& dt){
		bool flag = false;

		flag = ( GET_SIG[2].read() == GET_SIG[1].read() );
		dt = GET_SIG[2].read();

		return flag;
	}

	void b_get(DT& dt){
		while(( GET_SIG[2].read() != GET_SIG[1].read() ) != 0)wait();
		dt = GET_SIG[2].read();
	}

	void put(const DT& dt){
		PUT_SIG = dt;
		wait();
	}
};

template<> class syn_cdc_dff<bool>:public sc_module{
public:
	sc_in<bool> wclk;
	sc_in<bool> rclk;
	sc_in<bool> nrst;

	sc_signal<bool> PUT_SIG;
	sc_signal<bool> GET_SIG[2];

	SC_HAS_PROCESS(syn_cdc_dff);

	syn_cdc_dff(const sc_module_name& name=sc_gen_unique_name("syn_cdc_dff")):sc_module(name){
		SC_METHOD(get_method);
		sensitive << wclk.pos() << nrst.neg();
		end_module();
	}

	void get_method(){
		if(nrst.read() ==  false){
			GET_SIG[0] = false;
			GET_SIG[1] = false;
		}else{
			GET_SIG[0] = PUT_SIG.read();
			GET_SIG[1] = GET_SIG[0].read();
		}
	}

	bool nb_get(bool& dt){

		dt = GET_SIG[1].read();

		return true;
	}

	void b_get(bool& dt){
		dt = GET_SIG[1].read();
	}

	void put(const bool& dt){
		PUT_SIG = dt;
		wait();
	}

};

#endif /* SYN_CDC_DFF_H_ */

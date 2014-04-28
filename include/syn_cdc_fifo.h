/*
 * syn_cdc_fifo.h
 *
 *  Created on: 2014/04/29
 *      Author: Kuniyasu
 */


#ifndef FIFO_IF_H
#define FIFO_IF_H

#include<systemc.h>
#define TR_NAME(_name) (std::string(name())+"_"+std::string(_name)).c_str()
#define PIN_NAME(bname,_name_) (std::string(bname)+"_"+std::string(_name_)).c_str()

template<class DT>class fifo_get_if;
template<class DT>class fifo_put_if;

template<class DT>class fifo_get_if:public sc_interface{
public:
	virtual void r_reset() = 0;
	virtual bool is_not_empty() = 0;
	virtual bool nb_get(DT& dt) = 0;
	virtual void  b_get(DT& dt) = 0;
};

template<class DT>class fifo_put_if:public sc_interface{
public:
	virtual void w_reset() = 0;
	virtual bool is_not_full() = 0;
	virtual bool nb_put(const DT& dt) = 0;
	virtual void  b_put(const DT& dt) = 0;
};

#endif

#ifndef SYN_CDC_FIFO_H_
#define SYN_CDC_FIFO_H_

#include<systemc.h>
#define TR_NAME(_name) (std::string(name())+"_"+std::string(_name)).c_str()
#define PIN_NAME(bname,_name_) (std::string(bname)+"_"+std::string(_name_)).c_str()

template<class DT, int SIZE, int WIDTH> class syn_cdc_fifo_base_put;
template<class DT, int SIZE, int WIDTH> class syn_cdc_fifo_base_get;
template<class DT, int SIZE, int WIDTH> class syn_cdc_fifo_put;
template<class DT, int SIZE, int WIDTH> class syn_cdc_fifo_get;
template<class DT, int SIZE, int WIDTH> class syn_cdc_fifo_channel;


template<class DT, int SIZE, int WIDTH> class syn_cdc_fifo_channel{
public:
	typedef sc_uint<WIDTH+1> cnt_type;

	sc_signal<cnt_type> wcnt;
	sc_signal<cnt_type>  rcnt;
	sc_signal<DT> fifo_data[SIZE];

	syn_cdc_fifo_channel(const sc_module_name& name=sc_gen_unique_name("syn_cdc_fifo_channel")){}
};

template<class DT, int SIZE, int WIDTH> class syn_cdc_fifo_base_put{
public:
	typedef sc_uint<WIDTH+1> cnt_type;

	sc_out<cnt_type> wcnt;
	sc_in<cnt_type>  rcnt;
	sc_out<DT> fifo_data[SIZE];

	syn_cdc_fifo_base_put(const sc_module_name& name=sc_gen_unique_name("syn_cdc_fifo_base_put")){}

	template<class C> void bind(C& c){
		wcnt(c.wcnt);
		rcnt(c.rcnt);

		for(int i=0; i<SIZE; i++){
			fifo_data[i](c.fifo_data[i]);
		}
	}

	template<class C> void operator()(C& c){
		bind(c);
	}

};

template<class DT, int SIZE, int WIDTH> class syn_cdc_fifo_base_get{
public:
	typedef sc_uint<WIDTH+1> cnt_type;

	sc_in<cnt_type> wcnt;
	sc_out<cnt_type>  rcnt;
	sc_in<DT> fifo_data[SIZE];

	syn_cdc_fifo_base_get(const sc_module_name& name=sc_gen_unique_name("syn_cdc_fifo_base_get")){}

	template<class C> void bind(C& c){
		wcnt(c.wcnt);
		rcnt(c.rcnt);

		for(int i=0; i<SIZE; i++){
			fifo_data[i](c.fifo_data[i]);
		}
	}

	template<class C> void operator()(C& c){
		bind(c);
	}

};

template<class DT, int SIZE, int WIDTH>
class syn_cdc_fifo_put:public sc_module, public syn_cdc_fifo_base_put<DT,SIZE,WIDTH>, public fifo_put_if<DT>{
public:
	typedef sc_uint<WIDTH+1> cnt_type;
	typedef syn_cdc_fifo_base_put<DT,SIZE,WIDTH> base_type;

	sc_in<bool> clk;
	sc_in<bool> nrst;

	sc_signal<cnt_type> rcnt_sig[2];

	cnt_type _wcnt;

	SC_HAS_PROCESS(syn_cdc_fifo_put);
	syn_cdc_fifo_put(const sc_module_name& name=sc_gen_unique_name("syn_cdc_fifo_put")):sc_module(name){
		SC_METHOD(cnt_sig_ff);
		sensitive << clk.pos() << nrst.neg();

		end_module();
	}

	virtual void w_reset(){
		base_type::wcnt = 0;
		_wcnt = 0;

		for(int i=0; i<SIZE; i++){
			base_type::fifo_data[i] = 0;
		}
	}

	virtual bool is_not_full(){
		bool flag = false;

		cnt_type rcnt = rcnt_sig[1].read();


		return flag;
	}

	virtual bool nb_put(const DT& dt){
		bool flag = false;

		flag = is_not_full();

		if( flag == true ){
			base_type::fifo_data[_wcnt.range(WIDTH-1,0)] = dt;
			_wcnt = _wcnt + 1U;
			base_type::wcnt = binary2gray(_wcnt);
		}

		return false;
	}

	virtual void  b_put(const DT& dt){
		while( is_not_full() == false ) wait();

		base_type::fifo_data[_wcnt.range(WIDTH-1,0)] = dt;
		_wcnt = _wcnt + 1U;
		base_type::wcnt = binary2gray(_wcnt);
	}

	template<class C> void bind(C& c){
		base_type::wcnt(c.wcnt);
		base_type::rcnt(c.rcnt);

		for(int i=0; i<SIZE; i++){
			base_type::fifo_data[i](c.fifo_data[i]);
		}
	}

	template<class C> void operator()(C& c){
		bind(c);
	}

	cnt_type gray2binary(const cnt_type& gray){

		unsigned int mask;
	    for (mask = (gray>>1); gray != 0; gray = (gray>>1) ){
	    	gray = gray ^ mask;
	    }

		return gray;
	}

	cnt_type binary2gray(const cnt_type& binary){
		return ((binary >> 1) ^ binary);
	}

	void cnt_sig_ff(){
		if( nrst.read() ==  false ){
			rcnt_sig[0] = cnt_type();
			rcnt_sig[1] = cnt_type();
		}else{
			rcnt_sig[0] = base_type::rcnt.read();
			rcnt_sig[1] = rcnt_sig[0].read();
		}
	}

};

template<class DT, int SIZE, int WIDTH>
class syn_cdc_fifo_get:public sc_module, public syn_cdc_fifo_base_get<DT,SIZE,WIDTH>, public fifo_get_if<DT>{
public:
	typedef sc_uint<WIDTH+1> cnt_type;
	typedef syn_cdc_fifo_base_put<DT,SIZE,WIDTH> base_type;

	sc_in<bool> clk;
	sc_in<bool> nrst;

	sc_signal<cnt_type> wcnt_sig[2];

	cnt_type _rcnt;

	SC_HAS_PROCESS(syn_cdc_fifo_get);

	syn_cdc_fifo_get(const sc_module_name& name=sc_gen_unique_name("syn_cdc_fifo_get")):sc_module(name),base_type(name){
		SC_METHOD(cnt_sig_ff);
		sensitive << clk.pos() << nrst.neg();

		end_module();
	}

	virtual void r_reset(){
		_rcnt = 0U;
		base_type::rcnt = 0U;
	}

	virtual bool is_not_empty(){
		bool flag = false;

		cnt_type _wcnt = gray2binary(wcnt_sig[1].read());
		if( _rcnt != _wcnt ){
			flag = true;
		}else{
			flag = false;
		}

		return false;
	}

	virtual bool nb_get(DT& dt){
		bool flag = false;

		flag = is_not_empty();
		dt = base_type::fifo_data[_rcnt.range(WIDTH-1,0)].read();

		if( flag == true ){
			_rcnt = _rcnt+1U;
			base_type::rcnt = binary2gray(_rcnt);
		}

		return flag;
	}

	virtual void  b_get(DT& dt){
		while( is_not_empty() == false ) wait();
		dt = base_type::fifo_data[_rcnt.range(WIDTH-1,0)].read();

		_rcnt = _rcnt+1U;
		base_type::rcnt = binary2gray(_rcnt);

	}

	template<class C> void bind(C& c){
		base_type::wcnt(c.wcnt);
		base_type::rcnt(c.rcnt);

		for(int i=0; i<SIZE; i++){
			base_type::fifo_data[i](c.fifo_data[i]);
		}
	}

	template<class C> void operator()(C& c){
		bind(c);
	}

	cnt_type binary2gray(const cnt_type& binary){
		return ((binary >> 1) ^ binary);
	}

	cnt_type gray2binary(const cnt_type& gray){

	    unsigned int mask;
	    for (mask = (gray>>1); gray != 0; gray = (gray>>1) ){
	    	gray = gray ^ mask;
	    }

		return gray;
	}

	void cnt_sig_ff(){
		if( nrst.read() ==  false ){
			wcnt_sig[0] = cnt_type();
			wcnt_sig[1] = cnt_type();
		}else{
			wcnt_sig[0] = base_type::wcnt.read();
			wcnt_sig[1] = wcnt_sig[0].read();
		}
	}

};

#endif /* SYN_CDC_FIFO_H_ */

/*****************************************************************************

    Copyright 2010-2012
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.

    Copyright 2015-2016
    COSEDA Technologies GmbH

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

 *****************************************************************************/

/*****************************************************************************

 sca_tdf_sc_in.h - converter port SystemC -> tdf

 Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

 Created on: 04.03.2009

 SVN Version       :  $Revision: 1960 $
 SVN last checkin  :  $Date: 2016-03-21 16:43:40 +0100 (Mon, 21 Mar 2016) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_tdf_sc_in_bool.h 1960 2016-03-21 15:43:40Z karsten $


 *****************************************************************************/
/*
 LRM clause 4.1.1.11.
 */

/*****************************************************************************/

#ifndef SCA_TDF_SC_IN_BOOL_H_
#define SCA_TDF_SC_IN_BOOL_H_


namespace sca_tdf
{

namespace sca_de
{

//begin implementation specific

template<class T>
class sca_out;

//end implementation specific


template<>
//class sca_in : public implementation-derived-from sca_core::sca_port&lt; sc_core::sc_signal_in_if >
class sca_in<bool>: public sca_core::sca_port< sc_core::sc_signal_in_if<bool> >,
		public sca_tdf::sca_implementation::sca_port_attributes
{
public:
	sca_in();
	explicit sca_in(const char* );

	void set_delay(unsigned long );
	void set_rate(unsigned long );
	void set_timestep(const sca_core::sca_time& );
	void set_timestep(double , sc_core::sc_time_unit );
	void set_max_timestep(const sca_core::sca_time& );
	void set_max_timestep(double , sc_core::sc_time_unit );

	unsigned long get_delay() const;
	unsigned long get_rate() const;
	sca_core::sca_time get_time(unsigned long sample_id = 0) const;
	sca_core::sca_time get_timestep(unsigned long sample_id = 0) const;
	sca_core::sca_time get_max_timestep() const;
	sca_core::sca_time get_last_timestep(unsigned long sample_id=0) const;

	virtual const char* kind() const;

	void initialize(const bool& value, unsigned long sample_id = 0);
	const bool& read_delayed_value(unsigned long sample_id = 0) const;

	bool is_timestep_changed(unsigned long sample_id=0) const;
	bool is_rate_changed() const;
	bool is_delay_changed() const;

	const bool& read(unsigned long sample_id = 0);
	operator const bool&();
	const bool& operator[](unsigned long sample_id) ;

	const sc_core::sc_event& default_event() const;
	const sc_core::sc_event& value_changed_event() const;
	const sc_core::sc_event& posedge_event() const;
	const sc_core::sc_event& negedge_event() const;

	bool event() const;
	bool posedge() const;
	bool negedge() const;

	virtual void bind(sc_core::sc_signal_in_if<bool>&);
	void operator()(sc_core::sc_signal_in_if<bool>&);

	virtual void bind(sc_core::sc_port<sc_core::sc_signal_in_if<bool> >&);
	void operator()(sc_core::sc_port<sc_core::sc_signal_in_if<bool> >&);

	virtual void bind(sc_core::sc_port<sc_core::sc_signal_inout_if<bool> >&);
	void operator()(sc_core::sc_port<sc_core::sc_signal_inout_if<bool> >&);



	/** return the sc_interface for converter ports*/
	virtual sc_core::sc_interface* get_interface();
	virtual const sc_core::sc_interface* get_interface() const;

	/** method of interactive tracing interface, which returns the value
	 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
	 */
	const bool& get_typed_trace_value() const;

	void set_timeoffset(const sca_core::sca_time& toffset); //obsolete in 2.0
	void set_timeoffset(double toffset, ::sc_core::sc_time_unit unit); //obsolete in 2.0

	sca_core::sca_time get_timeoffset() const; //obsolete in 2.0

private:
	// Disabled
	sca_in(const sca_tdf::sca_de::sca_in<bool>&);
	sca_tdf::sca_de::sca_in<bool>& operator= (const sca_tdf::sca_de::sca_in<bool>&);

	void end_of_port_elaboration();

	//begin implementation specific

	//on this way we forbid port-to-port binding to a converter port
	void bind(sca_tdf::sca_de::sca_out<bool>&);
	void operator()(sca_tdf::sca_de::sca_out<bool>&);
	void bind(sca_tdf::sca_de::sca_in<bool>&);
	void operator()(sca_tdf::sca_de::sca_in<bool>&);

	 //overload sc_port_b bind method
	 void bind( sc_core::sc_port_b<sc_core::sc_signal_in_if<bool> >& parent_ );

	sca_core::sca_implementation::sca_sync_value_handle<bool> val_handle;

	//sca time  on which a sample from SystemC time zero has to be read
	sca_core::sca_time sample_delay_time;
	std::vector<bool> delay_buffer;
	bool tmp_delay_val; //as return value for const reference

	void read_sc_signal();

	void construct();

	typedef sca_core::sca_port< ::sc_core::sc_signal_in_if<bool> > base_type;

	void start_of_simulation();

	bool ret_tmp;

	const bool* allow_processing_access_flag;
	bool allow_processing_access_default;

	sca_core::sca_module* pmod;

	virtual bool trace_init(sca_util::sca_implementation::sca_trace_object_data& data);
	virtual void trace(long id, sca_util::sca_implementation::sca_trace_buffer& buffer);

	 //sc trace of sc_core::sc_object to prevent clang warning due overloaded
	 //sca_traceable_object function
	 void trace( sc_core::sc_trace_file* tf ) const;

	bool traces_available;
	std::vector<sca_util::sca_implementation::sca_trace_object_data*> trace_data;

	//method to pass attributes to module after change attributes
	void validate_port_attributes();

	//end implementation specific
};

//begin implementation specific


} // namespace sca_de

//template<class T>
//typedef sca_tdf::sca_de::sca_in<T> sc_in<T> ;

template<>
class sc_in<bool>: public sca_tdf::sca_de::sca_in<bool>
{
public:
	sc_in() :
		sca_tdf::sca_de::sca_in<bool>()
	{
	}
	explicit sc_in(const char* name_) :
		sca_tdf::sca_de::sca_in<bool>(name_)
	{
	}
};


} // namespace sca_tdf


#endif /* SCA_TDF_SC_IN_BOOL_H_ */

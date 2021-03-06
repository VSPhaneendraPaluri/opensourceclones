/*****************************************************************************

    Copyright 2010-2013
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

 sca_tdf_sc_in_sc_logic.cpp - converter port SystemC -> tdf

 Original Author: Karsten Einwich Fraunhofer / COSEDA Technologies

 Created on: 10.04.2012

 SVN Version       :  $Revision: 1960 $
 SVN last checkin  :  $Date: 2016-03-21 16:43:40 +0100 (Mon, 21 Mar 2016) $ (UTC)
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_tdf_sc_in_sc_logic.cpp 1960 2016-03-21 15:43:40Z karsten $


 *****************************************************************************/
/*
 LRM clause 4.1.1.12.
 */

/*****************************************************************************/

#include <systemc-ams>

namespace sca_tdf
{

namespace sca_de
{

sc_core::sc_interface* sca_in<sc_dt::sc_logic>::get_interface()
{
	return this->sc_get_interface();
}

const sc_core::sc_interface* sca_in<sc_dt::sc_logic>::get_interface() const
{
	return this->sc_get_interface();
}


/** method of interactive tracing interface, which returns the value
 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
 */
const sc_dt::sc_logic& sca_in<sc_dt::sc_logic>::get_typed_trace_value() const
{
	const sc_core::sc_interface* scif=this->get_interface();
	const sc_core::sc_signal_inout_if<sc_dt::sc_logic>* sc_sig=
			dynamic_cast<const sc_core::sc_signal_inout_if<sc_dt::sc_logic>*>(scif);

	if(sc_sig==NULL)
	{
		static const sc_dt::sc_logic dummy(sc_dt::SC_LOGIC_X);
		return dummy;
	}

	return sc_sig->read();
}



const sc_core::sc_event& sca_in<sc_dt::sc_logic>::default_event() const
{
	return (*this)->default_event();
}



const sc_core::sc_event& sca_in<sc_dt::sc_logic>::value_changed_event() const
{
	return (*this)->value_changed_event();
}


const sc_core::sc_event& sca_in<sc_dt::sc_logic>::posedge_event() const
{
	return (*this)->posedge_event();
}


const sc_core::sc_event& sca_in<sc_dt::sc_logic>::negedge_event() const
{
	return (*this)->negedge_event();
}



bool sca_in<sc_dt::sc_logic>::event() const
{
	return (*this)->event();
}



bool sca_in<sc_dt::sc_logic>::posedge() const
{
	return (*this)->posedge();
}


bool sca_in<sc_dt::sc_logic>::negedge() const
{
	return (*this)->negedge();
}





//IMRPOVE: use own method called after solver creation (tdf-view eq-setup)

void sca_in<sc_dt::sc_logic>::start_of_simulation()
{
	register_sc_value_trace((*this)->default_event(), val_handle);

	sample_delay_time = this->get_delay_internal() * this->get_propagated_timestep();
	delay_buffer.resize(this->get_delay_internal());

	if(pmod)
	{
		  allow_processing_access_flag=pmod->get_allow_processing_access_flag_ref();
		  if(allow_processing_access_flag==NULL) allow_processing_access_flag=&allow_processing_access_default;
	}


	if(this->get_timeoffset_internal() >= this->get_propagated_timestep())
	{
		std::ostringstream str;
		str << "timeoffset of port: " << this->name() << " : ";
	    str << this->get_timeoffset_internal() << " must be smaller than timestep: ";
	    str << this->get_propagated_timestep();
	    SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}
}



void sca_in<sc_dt::sc_logic>::construct()
{
	base_type::sca_port_type = base_type::SCA_SC_IN_PORT;
	val_handle.set_method(
			this,
			static_cast<sca_core::sca_implementation::sca_sync_value_handle_base::value_method> (&sca_in<
					sc_dt::sc_logic>::read_sc_signal));

	pmod=dynamic_cast<sca_core::sca_module*>(this->get_parent_object());

	if(pmod==NULL)
	{
		std::ostringstream str;
		str << "The sca_tdf::sca_de::sca_out port " << this->name()
			<< " must be instantiated in the context of an sca_tdf::sca_module";
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	allow_processing_access_default=false;
	allow_processing_access_flag=NULL;

	traces_available=false;
}


sca_in<sc_dt::sc_logic>::sca_in() :
	sca_in<sc_dt::sc_logic>::base_type(::sc_core::sc_gen_unique_name("sca_tdf_sc_in")),
	sca_port_attributes(this)
{
	construct();
}


sca_in<sc_dt::sc_logic>::sca_in(const char* name_) : sca_in<sc_dt::sc_logic>::base_type(name_),
		sca_port_attributes(this)
{
	construct();
}


void sca_in<sc_dt::sc_logic>::end_of_port_elaboration()
{
	base_type::m_sca_if=NULL;
}


void sca_in<sc_dt::sc_logic>::read_sc_signal()
{
	val_handle.write((*this)->read());

	if(traces_available)
	{
		//due the tracing is not under control of the TDF scheduler, we must
		//check whether all trace files are initialized
		initialize_all_traces();
		for(unsigned long i=0;i<trace_data.size();i++) trace_data[i]->trace();
	}
}


void sca_in<sc_dt::sc_logic>::set_delay(unsigned long nsamples)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_delay(nsamples);
}


void sca_in<sc_dt::sc_logic>::set_rate(unsigned long rate_)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_rate(rate_);
}


void sca_in<sc_dt::sc_logic>::set_timestep(const sca_core::sca_time& tstep)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_timestep(tstep);

	if(pmod->is_change_attributes_executing() )
	{
	  pmod->set_timestep(tstep*this->get_rate_internal());
	}
}


void sca_in<sc_dt::sc_logic>::set_timestep(double tstep, ::sc_core::sc_time_unit unit)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_timestep(sca_core::sca_time(tstep, unit));
}

////


void sca_in<sc_dt::sc_logic>::set_max_timestep(const sca_core::sca_time& tstep)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_max_timestep(tstep);
}

////


void sca_in<sc_dt::sc_logic>::set_max_timestep(double tstep, sc_core::sc_time_unit unit)
{
	this->set_max_timestep(sca_core::sca_time(tstep,unit));
}


//validate port attributes
void sca_in<sc_dt::sc_logic>::validate_port_attributes()
{
	this->port_attribute_validation();
}




void sca_in<sc_dt::sc_logic>::set_timeoffset(const sca_core::sca_time& toffset)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_timeoffset(toffset);
}


void sca_in<sc_dt::sc_logic>::set_timeoffset(double toffset,
		::sc_core::sc_time_unit unit)
{
	this->sca_tdf::sca_implementation::sca_port_attributes::set_timeoffset(sca_core::sca_time(toffset, unit));
}


unsigned long sca_in<sc_dt::sc_logic>::get_delay() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_delay();
}


unsigned long sca_in<sc_dt::sc_logic>::get_rate() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_rate();
}


sca_core::sca_time sca_in<sc_dt::sc_logic>::get_time(unsigned long sample_id) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_time(sample_id);
}


sca_core::sca_time sca_in<sc_dt::sc_logic>::get_timestep(unsigned long sample_id) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_timestep(sample_id);
}


/////////


sca_core::sca_time sca_in<sc_dt::sc_logic>::get_max_timestep() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_max_timestep();
}

/////////


sca_core::sca_time sca_in<sc_dt::sc_logic>::get_last_timestep(unsigned long sample_id) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_last_timestep(sample_id);
}





sca_core::sca_time sca_in<sc_dt::sc_logic>::get_timeoffset() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::get_timeoffset();
}


const char* sca_in<sc_dt::sc_logic>::kind() const
{
	return "sca_tdf::sca_de::sca_in";
}


void sca_in<sc_dt::sc_logic>::initialize(const sc_dt::sc_logic& value, unsigned long sample_id)
{
	sca_core::sca_module* pmod;
	pmod=dynamic_cast<sca_core::sca_module*>(this->get_parent_object());

	if(!pmod || !(pmod->is_initialize_executing()))
	{
		std::ostringstream str;
	    str << "Port: " << this->name() << " can't be initialized due it is not accessed "
		    << " from the context of the processing method of the parent sca_tdf::sca_module"
		    << " (see LRM clause 4.1.1.6.15)";
	    SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	}

	if (sample_id >= this->get_delay_internal())
	{
		std::ostringstream str;
		str << "Sample id (" << sample_id << ") is greater than delay ("
				<< this->get_delay_internal() << " while initializing port: " << sca_in<sc_dt::sc_logic>::name();
		::sc_core::sc_report_handler::report(::sc_core::SC_ERROR,"SystemC-AMS",
				str.str().c_str(), __FILE__, __LINE__);
	}
	delay_buffer[sample_id] = value;

}



/////


const sc_dt::sc_logic& sca_in<sc_dt::sc_logic>::read_delayed_value(unsigned long sample_id) const
{
	if((pmod==NULL) || !(pmod->is_reinitialize_executing()))
	{
		std::ostringstream str;
	    str << "Port: " << this->name() << " can't execute method read_delayed_value due it is not accessed "
		    << " from the context of the reinitialize method of the parent sca_tdf::sca_module: ";
		if(pmod!=NULL) str << pmod->name();
		str << " (see LRM)";
	    SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
	    return sc_dt::SC_LOGIC_X;
	}

	if (sample_id >= sca_in<sc_dt::sc_logic>::get_delay_internal())
	{
		std::ostringstream str;
		str << "Sample id (" << sample_id << ") is greater than delay ("
				<< sca_in<sc_dt::sc_logic>::get_delay() << " while read_delayed_value port: " << sca_in<sc_dt::sc_logic>::name();
		::sc_core::sc_report_handler::report(::sc_core::SC_ERROR,"SystemC-AMS",
				str.str().c_str(), __FILE__, __LINE__);
	}


	sca_core::sca_time ptimestep   = this->get_propagated_timestep();
	sca_core::sca_time ctime       = this->get_time_internal(sample_id);
	sca_core::sca_time sample_time = this->get_time_internal(0);
	unsigned long      delay       = this->get_delay_internal();

	if (sample_time < sample_delay_time + delay*ptimestep)
	{
		return delay_buffer[sample_id];
	}
	else
	{
		const_cast<sca_in<sc_dt::sc_logic>*>(this)->get_sc_value_on_time(
				ctime - sample_id * ptimestep,
				const_cast<sca_in<sc_dt::sc_logic>*>(this)->val_handle);

		return const_cast<sca_in<sc_dt::sc_logic>*>(this)->val_handle.read();

	}
}


/////


bool sca_in<sc_dt::sc_logic>::is_timestep_changed(unsigned long sample_id) const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::is_timestep_changed(sample_id);
}


/////


bool sca_in<sc_dt::sc_logic>::is_rate_changed() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::is_rate_changed();
}

/////


bool sca_in<sc_dt::sc_logic>::is_delay_changed() const
{
	return this->sca_tdf::sca_implementation::sca_port_attributes::is_delay_changed();
}





const sc_dt::sc_logic& sca_in<sc_dt::sc_logic>::read(unsigned long sample_id)
{
	if((allow_processing_access_flag==NULL) || !(*allow_processing_access_flag))
	{
		if(!sca_ac_analysis::sca_ac_is_running())
		{
			std::ostringstream str;
			str << "Port: " << this->name() << " can't be read due it is not accessed "
		        << " from the context of the processing method of the parent sca_tdf::sca_module"
		        << " (see LRM)";
			SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		}
	}

	sca_core::sca_time sample_time = this->get_time_internal(sample_id);
	sca_core::sca_time ptimestep    = this->get_propagated_timestep();
	unsigned long      rate        = this->get_rate_internal();
	unsigned long      delay       = this->get_delay_internal();

	if (sample_time < sample_delay_time)
	{
		ret_tmp= delay_buffer[(unsigned long)(sample_time.value()/ ptimestep.value())];
		return ret_tmp;
	}
	else
	{
		if (sample_id >= rate)
		{
			std::ostringstream str;
			str << "Sample id (" << sample_id << ") is greater or equal than rate ("
					<< rate << " while reading from port: " << this->name();

			sc_core::sc_report_handler::report(::sc_core::SC_ERROR,"SystemC-AMS",
					str.str().c_str(), __FILE__, __LINE__);
		}
		get_sc_value_on_time(
				sample_time - delay * ptimestep, val_handle);

		return val_handle.read();
	}
}


sca_in<sc_dt::sc_logic>::operator const sc_dt::sc_logic&()
{
	return this->read();
}


const sc_dt::sc_logic& sca_in<sc_dt::sc_logic>::operator[](unsigned long sample_id)
{
	return this->read(sample_id);
}




void sca_in<sc_dt::sc_logic>::bind(sc_core::sc_port<sc_core::sc_signal_inout_if<sc_dt::sc_logic>,1,sc_core::SC_ONE_OR_MORE_BOUND >& oport)
{
	//IMPROVE: hack - find a better solution??
	sc_core::sc_port<sc_core::sc_signal_inout_if<sc_dt::sc_logic> >* base_op=&oport;
	base_type::bind(*(sc_core::sc_port<sc_core::sc_signal_in_if<sc_dt::sc_logic> >*)(base_op));
}


void sca_in<sc_dt::sc_logic>::operator()(sc_core::sc_port<sc_core::sc_signal_inout_if<sc_dt::sc_logic>,1,sc_core::SC_ONE_OR_MORE_BOUND >& oport)
{
	bind(oport);
}


void sca_in<sc_dt::sc_logic>::bind(sc_core::sc_port<sc_core::sc_signal_in_if<sc_dt::sc_logic>,1,sc_core::SC_ONE_OR_MORE_BOUND >& iport)
{
	base_type::bind(iport);
}


void sca_in<sc_dt::sc_logic>::operator()(sc_core::sc_port<sc_core::sc_signal_in_if<sc_dt::sc_logic>,1,sc_core::SC_ONE_OR_MORE_BOUND >& iport)
{
	base_type::bind(iport);
}


void sca_in<sc_dt::sc_logic>::bind(sc_core::sc_signal_in_if<sc_dt::sc_logic>& intf)
{
	base_type::bind(intf);
}



void sca_in<sc_dt::sc_logic>::operator()(sc_core::sc_signal_in_if<sc_dt::sc_logic>& intf)
{
	base_type::bind(intf);
}

//methods private and thus disabled - should not possible to call

void sca_in<sc_dt::sc_logic>::bind(sca_tdf::sca_de::sca_out<sc_dt::sc_logic>&)
{
	SC_REPORT_ERROR("SystemC-AMS","Binding of converter port to converter port not permitted");
}


void sca_in<sc_dt::sc_logic>::operator()(sca_tdf::sca_de::sca_out<sc_dt::sc_logic>& port)
{
	bind(port);
}


void sca_in<sc_dt::sc_logic>::bind(sca_tdf::sca_de::sca_in<sc_dt::sc_logic>&)
{
	SC_REPORT_ERROR("SystemC-AMS","Binding of converter port to converter port not permitted");
}


void sca_in<sc_dt::sc_logic>::operator()(sca_tdf::sca_de::sca_in<sc_dt::sc_logic>& port)
{
	bind(port);
}



//overload sc_port_b bind method
inline void sca_in<sc_dt::sc_logic>::bind( sc_core::sc_port_b<sc_core::sc_signal_in_if<sc_dt::sc_logic> >& parent_ )
{
	sc_core::sc_port<sc_core::sc_signal_in_if<sc_dt::sc_logic> >* port;
	port=dynamic_cast<sc_core::sc_port<sc_core::sc_signal_in_if<sc_dt::sc_logic> >*>(&parent_);

	if(port==NULL)
	{
		std::ostringstream str;
		str << "Port of kind: " << parent_.kind() << " can't bound to";
		str << " port of kind: " << this->kind() << " for port: ";
		str << this->name();
		SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
		return;
	}

	this->bind(*port);
}



bool sca_in<sc_dt::sc_logic>::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
{
	traces_available=true;

	trace_data.push_back(&data);
	data.set_type_info<sc_dt::sc_logic>();


    data.type="-";
    data.unit="-";

    data.event_driven=true;
    data.dont_interpolate=true;

    return true;

}


void sca_in<sc_dt::sc_logic>::trace(long id, sca_util::sca_implementation::sca_trace_buffer& buffer)
{
	buffer.store_time_stamp(id, sc_core::sc_time_stamp(),(*this)->read());
}



//sc trace of sc_core::sc_object to prevent clang warning due overloaded
//sca_traceable_object function
inline void sca_in<sc_dt::sc_logic>::trace( sc_core::sc_trace_file* tf ) const
{
	sc_core::sc_port_b<sc_core::sc_signal_in_if<sc_dt::sc_logic> >::trace(tf);
}



} //end namespace sca_de
} //end namespace sca_tdf




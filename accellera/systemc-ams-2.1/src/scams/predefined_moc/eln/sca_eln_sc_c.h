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

 sca_eln_sc_c.h - electrical linear net capacitor controlled by a sc_signal<double>

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden / COSEDA Technologies

 Created on: 08.03.2009

 SVN Version       :  $Revision: 1892 $
 SVN last checkin  :  $Date: 2016-01-10 12:59:12 +0100 (Sun, 10 Jan 2016) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_eln_sc_c.h 1892 2016-01-10 11:59:12Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.3.1.27.
 * The class sca_eln::sc_core::sca_c shall implement a primitive module for the
 * ELN MoC that represents a capacitor, which capacitance is controlled by a
 * discrete-event input signal. The primitive shall contribute the following
 * equation to the equation system:
 *
 *        i(p,n)(t) = scale * inp * d(v(p,n)(t)+q0/inp)/dt
 *
 * where scale is the constant scale coefficient, inp is the discrete-event
 * input signal, q0 is the initial charge in coulomb,v(p,n)(t) is the voltage
 * across terminals p and n, and i(p,n)(t) is the current flowing through the
 * primitive from terminal p to terminal n. The product of scale and inp  shall
 * be interpreted as the capacitance in farad.
 */

/*****************************************************************************/

#ifndef SCA_ELN_SC_C_H_
#define SCA_ELN_SC_C_H_

namespace sca_eln
{

namespace sca_de
{

//    class sca_c : public implementation-derived-from sca_core::sca_module,
//                  protected sca_util::sca_traceable_object
class sca_c: public sca_eln::sca_module,
		     public sca_util::sca_traceable_object
{
public:
	sca_eln::sca_terminal p;
	sca_eln::sca_terminal n;

	sc_core::sc_in<double> inp;

	sca_core::sca_parameter<double> scale;
	sca_core::sca_parameter<double> q0;

	virtual const char* kind() const;

	explicit sca_c(sc_core::sc_module_name, double scale_ = 1.0, double q0_ =
			0.0);


	/** method of interactive tracing interface, which returns the value
	 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
	 */
	const double& get_typed_trace_value() const;

	/** method of interactive tracing interface, which returns the value as string
	 * at the current SystemC time (the time returned by sc_core::sc_time_stamp())
	 */
	const std::string& get_trace_value() const;

	/** method of interactive tracing interface, which registers
	 * a trace callback function, which is called at each SystemC timepoint
	 * is a new eln result is available
	 */
	bool register_trace_callback(sca_util::sca_traceable_object::sca_trace_callback,void*);



	//begin implementation specific

private:
	virtual void matrix_stamps();

	void read_cval();

	long nadd, nadd2;

	double curr_value;

	 bool dc_init;
	 void post_solve();

	::sca_tdf::sca_de::sca_in<double>* conv_port;

	 bool trace_init(sca_util::sca_implementation::sca_trace_object_data& data);
	 void trace(long id,sca_util::sca_implementation::sca_trace_buffer& buffer);
	 sca_util::sca_complex calculate_ac_result(sca_util::sca_complex* res_vec);


	 //sc trace of sc_core::sc_object to prevent clang warning due overloaded
	 //sca_traceable_object function
	 void trace( sc_core::sc_trace_file* tf ) const;

	//end implementation specific
};

} // namespace sca_de

typedef sca_eln::sca_de::sca_c sca_de_c;

} // namespace sca_eln

#endif /* SCA_ELN_SC_C_H_ */

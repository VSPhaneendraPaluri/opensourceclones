/*****************************************************************************

    Copyright 2010
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.


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

 sca_lsf_sc_source.h - linear signal flow source controlled by a sc_signal<double>

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: 06.03.2009

 SVN Version       :  $Revision: 1265 $
 SVN last checkin  :  $Date: 2011-11-19 21:43:31 +0100 (Sat, 19 Nov 2011) $
 SVN checkin by    :  $Author: karsten $
 SVN Id            :  $Id: sca_lsf_de_source.h 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/
/*
 * LRM clause 4.2.1.21.
 * The class sca_lsf::sc_core::sca_source shall implement a primitive module for
 * the LSF MoC that realizes the scaled conversion of a discrete-event input
 * signal to an LSF signal. The primitive shall contribute the following
 * equation to the equation system:
 *
 *        y(t) = scale * inp
 *
 * where scale is the constant scale coefficient, inp is the discrete-event
 * input signal that shall be interpreted as a discrete-time signal, and y(t) is
 * the LSF output signal.
 */

/*****************************************************************************/

#ifndef SCA_LSF_SC_SOURCE_H_
#define SCA_LSF_SC_SOURCE_H_

namespace sca_lsf
{

namespace sca_de
{

//class sca_source : public implementation-derived-from sca_core::sca_module
class sca_source: public sca_lsf::sca_module
{
public:
	::sc_core::sc_in<double> inp; // discrete-event input

	sca_lsf::sca_out y; // LSF output

	sca_core::sca_parameter<double> scale; // scale coefficient

	virtual const char* kind() const;

	explicit sca_source(sc_core::sc_module_name, double scale_ = 1.0);

private:
		 virtual void matrix_stamps();
		 double value_t();

		::sca_tdf::sca_de::sca_in<double>* conv_port;

};

} // namespace sca_de

typedef sca_lsf::sca_de::sca_source sca_de_source;

} // namespace sca_lsf


#endif /* SCA_LSF_SC_SOURCE_H_ */

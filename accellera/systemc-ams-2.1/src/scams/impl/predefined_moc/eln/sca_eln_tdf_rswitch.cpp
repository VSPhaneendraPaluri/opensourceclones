/*****************************************************************************

    Copyright 2010-2014
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

  sca_eln_tdf_rswitch.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden / COSEDA Technologies

  Created on: 10.11.2009

   SVN Version       :  $Revision: 1936 $
   SVN last checkin  :  $Date: 2016-03-02 17:59:19 +0100 (Wed, 02 Mar 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_eln_tdf_rswitch.cpp 1936 2016-03-02 16:59:19Z karsten $

 *****************************************************************************/

/*****************************************************************************/


#include "systemc-ams"
#include "scams/predefined_moc/eln/sca_eln_tdf_rswitch.h"
#include "scams/impl/util/data_types/sca_method_object.h"
#include "scams/impl/util/data_types/sca_method_list.h"
#include "scams/impl/core/sca_solver_base.h"
#include "scams/impl/predefined_moc/conservative/sca_con_interactive_trace_data.h"

namespace sca_eln
{

namespace sca_tdf
{

sca_rswitch::	sca_rswitch(sc_core::sc_module_name, double ron_,
		double roff_, bool off_state_) :
	p("p"), n("n"), ctrl("ctrl"), ron("ron", ron_), roff("roff", roff_),
			off_state("off_state", off_state_)
{
  through_value_available = true;
  through_value_type = "I";
  through_value_unit = "A";

  r_val = r_old = 1e5;
  ron_eff = 1.0;
  roff_eff = 1.0e6;

  first_call = true;
  short_cut = false;

  nadd=-1;
}


const char* sca_rswitch::kind() const
{
	return "sca_eln::sca_tdf::sca_rswitch";
}


void sca_rswitch::trace( sc_core::sc_trace_file* tf ) const
{
	std::ostringstream str;
	str << "sc_trace of sca_ln::sca_tdf::sca_rswitch module not supported for module: " << this->name();
	SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
}

////////////////////


void sca_rswitch::update_rval()
{
	  //we change now the equation system immediately
	  //thus the switching will finish at the current time step
	  //on this way we assume a continuous switching from the last
	  //to the current time step - the advantage is a lesser delay
	  //however the switching process starts now before the current
	  //time step

	  read_rval();

	if (r_val != r_old)
	{
		enable_b_change = true;
		continous       = false;

		if (short_cut)
		{
			B_wr(nadd, nadd) = -r_val;
		}
		else
		{
			B_wr(p, p) = B(p, p) - 1.0 / r_old + 1.0 / r_val;
			B_wr(p, n) = B(p, n) + 1.0 / r_old - 1.0 / r_val;
			B_wr(n, p) = B(n, p) + 1.0 / r_old - 1.0 / r_val;
			B_wr(n, n) = B(n, n) - 1.0 / r_old + 1.0 / r_val;
		}

		r_old = r_val;
	}
}


//////////////////

void sca_rswitch::read_rval()
{
  bool sw = ctrl.read();
  if (sw ^ off_state)
    r_val = ron_eff;
  else
    r_val = roff_eff;
}


/////////////////////////

static const double R_MIN_VAL = 1e-16;
//static const double R_MAX_VAL = 1e16;
static const double R_MAX_VAL = 1e14;

static const double R_MIN_SW_VAL = 1e-2;


//////////////////

void sca_rswitch::matrix_stamps()
{
  if (first_call)
  {
      first_call = false;

      ron_eff = ron.get();
      roff_eff = roff.get();

      if(
 		  (fabs(ron_eff)  < R_MIN_SW_VAL) ||
 		  (fabs(roff_eff) < R_MIN_SW_VAL)
		)
      {
          if(roff_eff > R_MAX_VAL)       roff_eff=  R_MAX_VAL;
          else if(roff_eff < -R_MAX_VAL) roff_eff= -R_MAX_VAL;

          if(ron_eff > R_MAX_VAL)       ron_eff=  R_MAX_VAL;
          else if(ron_eff < -R_MAX_VAL) ron_eff= -R_MAX_VAL;

          short_cut = true;
        }
      else
        {
          short_cut = false;
          if (fabs(ron_eff) < R_MIN_VAL)
          {
              if (ron_eff < 0.0)  ron_eff = -R_MIN_VAL;
              else                ron_eff =  R_MIN_VAL;
           }

          if (fabs(roff_eff) < R_MIN_VAL)
          {
              if (roff_eff < 0.0) roff_eff = -R_MIN_VAL;
              else                roff_eff = R_MIN_VAL;
          }
        }

      r_old = r_val = roff_eff;
    }


  if (short_cut)
    {
      nadd = add_equation();

      B_wr(nadd, p) = 1.0;
      B_wr(nadd, n) = -1.0;
      B_wr(p, nadd) = 1.0;
      B_wr(n, nadd) = -1.0;

      B_wr(nadd, nadd) = -r_val;
    }
  else
    {
      B_wr(p, p) += 1.0 / r_val;
      B_wr(p, n) -= 1.0 / r_val;
      B_wr(n, p) -= 1.0 / r_val;
      B_wr(n, n) += 1.0 / r_val;
    }

  add_method(PRE_SOLVE, SCA_VMPTR(sca_rswitch::update_rval));

  // see comment in update_rval
  //add_method(POST_SOLVE, SCA_VMPTR(sca_rswitch::read_rval));
}


bool sca_rswitch::trace_init(sca_util::sca_implementation::sca_trace_object_data& data)
{
    data.type=through_value_type;
    data.unit=through_value_unit;

    //trace will be activated after every complete cluster calculation
    //by teh synchronization layer
    get_sync_domain()->add_solver_trace(data);
    return true;
}

void sca_rswitch::trace(long id,sca_util::sca_implementation::sca_trace_buffer& buffer)
{
    sca_core::sca_time ctime = sca_eln::sca_module::get_time();

    double through_value;

    if(short_cut)
    {
    	through_value= x(nadd);
    }
    else
    {
    	through_value= (x(p)-x(n))/r_val;
    }

	if((id<0)&&(this->trd!=NULL)) //interactive trace
	{
	    this->trd->store_value(through_value);
		return;
	}

    buffer.store_time_stamp(id,ctime,through_value);
}

sca_util::sca_complex sca_rswitch::calculate_ac_result(sca_util::sca_complex* res_vec)
{
	if(short_cut)
	{
		return res_vec[nadd];
	}
	else
	{
	    //if reference node return 0.0
	    sca_util::sca_complex rp = long(p) < 0 ? 0.0 : res_vec[p];
	    sca_util::sca_complex rn = long(n) < 0 ? 0.0 : res_vec[n];

	    return (rp-rn)/r_val;
	}
}




const double& sca_rswitch::get_typed_trace_value() const
{
	if(this->trd==NULL)
	{
		this->trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);

	}

	return this->trd->get_value();
}


const std::string& sca_rswitch::get_trace_value() const
{
	std::ostringstream str;
	str << this->get_typed_trace_value();

	static std::string trace_value_string;
	trace_value_string=str.str().c_str();
	return trace_value_string;
}





bool sca_rswitch::register_trace_callback(sca_trace_callback cb,void* cb_arg)
{
	if(this->trd==NULL)
	{
		this->trd=new sca_core::sca_implementation::sca_con_interactive_trace_data(this);
	}


	this->trd->register_trace_callback(cb,cb_arg);

	return true;

}


} //namespace sca_tdf
} //namespace sca_eln


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

  sca_ac_domain_globals.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 02.01.2010

   SVN Version       :  $Revision: 1265 $
   SVN last checkin  :  $Date: 2011-11-19 21:43:31 +0100 (Sat, 19 Nov 2011) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_ac_domain_globals.cpp 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/impl/analysis/ac/sca_ac_domain_globals.h"
#include "scams/impl/analysis/ac/sca_ac_domain_db.h"
#include "scams/impl/analysis/ac/sca_ac_domain_entity.h"
#include "scams/impl/analysis/ac/sca_ac_domain_solver.h"
#include "scams/impl/core/sca_simcontext.h"

namespace sca_ac_analysis
{
namespace sca_implementation
{

//global ac database
sca_ac_domain_db& get_ac_database()
{
	sca_ac_domain_db* db=sca_core::sca_implementation::sca_get_curr_simcontext()->ac_db;
	if(db==NULL)
	{
		db=new sca_ac_domain_db;
		sca_core::sca_implementation::sca_get_curr_simcontext()->ac_db=db;
	}
	return *db;
}

//////////////////////////////////////////////////////////////////////////////

//methods must be member functions of the registrated sca_module/sca_solver_base
void sca_ac_domain_register_entity(sc_core::sc_module*           mod,
                                   sca_ac_domain_method          ac_fct,
                                   sca_add_ac_domain_eq_method   add_eq,
                                   sca_calc_add_eq_cons_method   outp_cons,
                                   bool                          noise_src
                                  )
{

    sca_ac_domain_entity* entity = new sca_ac_domain_entity(mod);

    entity->ac_domain_method        = ac_fct;
    entity->add_eq_method           = add_eq;
    entity->calc_add_eq_cons_method = outp_cons;
    entity->noise_src               = noise_src;
    entity->ac_db                   = &(get_ac_database());

    get_ac_database().register_entity(entity);
}

//////////////////////////////////////////////////////////////////////////////

void sca_ac_domain_register_entity(sca_core::sca_implementation::sca_solver_base*      solv,
                                   sca_ac_domain_method          ac_fct,
                                   sca_add_ac_domain_eq_method   add_eq,
                                   sca_calc_add_eq_cons_method   outp_cons,
                                   bool                          noise_src
                                  )
{
    sca_ac_domain_entity* entity = new sca_ac_domain_entity(solv);

    entity->ac_domain_method        = ac_fct;
    entity->add_eq_method           = add_eq;
    entity->calc_add_eq_cons_method = outp_cons;
    entity->noise_src               = noise_src;
    entity->ac_db                   = &(get_ac_database());

    get_ac_database().register_entity(entity);
}

//////////////////////////////////////////////////////////////////////////////

void sca_ac_register_arc(sc_core::sc_interface* sca_if)
{
	get_ac_database().register_arc(sca_if);
}

//////////////////////////////////////////////////////////////////////////////

void sca_ac_register_add_eq_arc(sc_core::sc_interface* sca_if,
                                long* rel_pos,
                                bool* ignore_flag )
{
	get_ac_database().register_add_eq_arc(sca_if, rel_pos, ignore_flag);
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////


sca_util::sca_vector<double>& generate_frequencies(
    sca_util::sca_vector<double>& frequencies,
    double startf,
    double endf,
    unsigned long npoints,
    sca_ac_analysis::sca_ac_scale spacet)
{
    double space=0.0, startfr=0.0;
    frequencies.resize(npoints);

    if(npoints>1)
    {
        switch(spacet)
        {
        case sca_ac_analysis::SCA_LIN:
            space=(endf-startf)/(double)(npoints-1);
            break;
        case sca_ac_analysis::SCA_LOG:
            startfr=startf;
            if(startf<=0.0)
                startfr=endf/npoints/1e12;
            space=pow(10.0,log10(endf/startfr)/(double)(npoints-1));
            break;
        default:
            std::ostringstream str;
            str << "Not defined Space for ac simulation" << std::endl;
            SC_REPORT_ERROR("SystemC-AMS",str.str().c_str());
        }
    }
    else
    {
        space=1.0;
    }

    double f=startf;
    for(unsigned long i=0;i<frequencies.length();i++)
    {
    	frequencies(i)=f;
        switch(spacet)
        {
        case SCA_LIN:
            f+=space;
            break;
        case SCA_LOG:
            if(f<=0.0)
                f=startfr;
            f*=space;
            break;
        }
    }

    return frequencies;
}




} // sca_implementation
} // sca_ac_analysis


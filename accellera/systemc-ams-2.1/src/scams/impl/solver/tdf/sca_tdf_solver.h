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

  sca_tdf_solver.h - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 25.08.2009

   SVN Version       :  $Revision: 1318 $
   SVN last checkin  :  $Date: 2012-04-19 18:31:53 +0200 (Thu, 19 Apr 2012) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_tdf_solver.h 1318 2012-04-19 16:31:53Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#ifndef SCA_TDF_SOLVER_H_
#define SCA_TDF_SOLVER_H_

#include "scams/core/sca_module.h"
#include "scams/impl/core/sca_solver_base.h"
#include "scams/predefined_moc/tdf/sca_tdf_module.h"

namespace sca_tdf
{

namespace sca_implementation
{

/**
  Solver class for static dataflow simulation.
  Due the synchronization layer supports static dataflow
  scheduling, the solver registers the dataflow graph and
  module's processing methods to the synchronization layer.
*/
class sca_tdf_solver : public sca_core::sca_implementation::sca_solver_base
{

public:

	sca_tdf_solver(std::vector<sca_core::sca_module*>& mods,
			std::vector<sca_core::sca_interface*>& chans);
	virtual ~sca_tdf_solver();

  /** Implements the pure virtual initialize method of @ref #sca_solver_base. */
  void initialize ();

  /**
    Implements the register_methods method of the
    @ref #class sca_sdf_solver_interface .
  */
  void register_methods(
		  sca_core::sca_module* mod,
          sca_tdf::sca_module::sca_module_method init_fct,
          sca_tdf::sca_module::sca_module_method reinit_fct,
          sca_tdf::sca_module::sca_module_method change_attributes_fct,
          sca_tdf::sca_module::sca_module_method proc_fct,
          sca_tdf::sca_module::sca_module_method post_fct);

  void register_decluster_port(sca_core::sca_implementation::sca_port_base* port);


private:

  void decluster_processing();
  sca_core::sca_implementation::sca_port_base* dec_port;

  sca_core::sca_module* module;

};

}
}

#endif

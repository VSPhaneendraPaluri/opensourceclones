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

  sca_eln_node_ref.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 10.11.2009

   SVN Version       :  $Revision: 1265 $
   SVN last checkin  :  $Date: 2011-11-19 21:43:31 +0100 (Sat, 19 Nov 2011) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_eln_node_ref.cpp 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc-ams>
#include "scams/predefined_moc/eln/sca_eln_node_ref.h"


namespace sca_eln
{

sca_node_ref::sca_node_ref():
	sca_node(sc_core::sc_gen_unique_name("sca_eln_node_ref"))
{
	  reference_node = true;
	  node_number    = -1;
}

sca_node_ref::sca_node_ref(const char* name_): sca_node(name_)
{
	  reference_node = true;
	  node_number    = -1;
}

const char* sca_node_ref::kind() const
{
	return "sca_eln::sca_node_ref";
}

//Disabled not used
sca_node_ref::sca_node_ref(const sca_eln::sca_node_ref&)
{
}

}

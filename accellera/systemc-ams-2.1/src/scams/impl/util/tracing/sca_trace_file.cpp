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

  sca_trace_file.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

  Created on: 16.11.2009

   SVN Version       :  $Revision: 1265 $
   SVN last checkin  :  $Date: 2011-11-19 21:43:31 +0100 (Sat, 19 Nov 2011) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_trace_file.cpp 1265 2011-11-19 20:43:31Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include "systemc-ams"
#include "scams/utility/tracing/sca_trace_file.h"

namespace sca_util
{

void sca_trace_file::enable()
{
	sca_util::sca_implementation::sca_trace_file_base::enable();
}

void sca_trace_file::disable()
{
	sca_util::sca_implementation::sca_trace_file_base::disable();
}

void sca_trace_file::set_mode_impl(const sca_util::sca_trace_mode_base& mode)
{
	sca_util::sca_implementation::sca_trace_file_base::set_mode(mode);
}


void sca_trace_file::set_mode(const sca_util::sca_trace_mode_base& mode)
{
	set_mode_impl(mode);
}


void sca_trace_file::reopen_impl(const std::string& name, std::ios_base::openmode mode)
{
	sca_util::sca_implementation::sca_trace_file_base::reopen(name,mode);
}


void sca_trace_file::reopen(const std::string& name, std::ios_base::openmode mode)
{
	reopen_impl(name,mode);
}


void sca_trace_file::reopen(std::ostream& outstream)
{
	sca_util::sca_implementation::sca_trace_file_base::reopen(outstream);
}


} // namespace sca_util

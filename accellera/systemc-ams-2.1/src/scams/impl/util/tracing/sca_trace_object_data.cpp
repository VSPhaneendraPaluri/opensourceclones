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

  sca_trace_object_data.cpp - description

  Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden / COSEDA Technologies

  Created on: 13.11.2009

   SVN Version       :  $Revision: 1892 $
   SVN last checkin  :  $Date: 2016-01-10 12:59:12 +0100 (Sun, 10 Jan 2016) $
   SVN checkin by    :  $Author: karsten $
   SVN Id            :  $Id: sca_trace_object_data.cpp 1892 2016-01-10 11:59:12Z karsten $

 *****************************************************************************/

/*****************************************************************************/

#include <systemc-ams>
#include "scams/impl/util/tracing/sca_trace_file_base.h"
#include "scams/impl/util/tracing/sca_trace_object_data.h"
#include "scams/impl/util/tracing/sca_trace_buffer.h"
#include "scams/impl/core/sca_simcontext.h"


namespace sca_util
{
namespace sca_implementation
{


void sca_trace_object_data::set_written_flag()
{
    //set bit to sign trace as written
    trace_base->written_flags[word]&=bit_mask;
}

void sca_trace_object_data::send_to_file()
{

	if(trace_base->closed) return;

    //fast trace activation check
    bool ready_flag=true;
    for(int i=0;i<nwords;i++)
    {
        if(trace_base->written_flags[i]!=0)
        {
            ready_flag=false;
            break;
        }
    }

    if(ready_flag)
    {
        trace_base->write_to_file();
    }
}


void sca_trace_object_data::trace()
{

	if(trace_base==NULL) //interactive tracing
	{
		if(trace_object!=NULL)
		{
			trace_object->trace(id,*trace_buffer);
		}
		return;
	}

    if(trace_disabled || trace_base->trace_disabled())
        return;

    //write value to trace buffer
    trace_object->trace(id,*trace_buffer);

    set_written_flag();

    send_to_file();
}

///////////////////////////////////////////////////////////////////////////////

sca_trace_object_data::sca_trace_object_data()
{
    name="";
    type="";
    unit="-";

    trace_object = NULL;
    trace_buffer = NULL;
    trace_base   = NULL;

    event_driven=false;
    dont_interpolate = false;

    id=-1;
    type_info=new sca_type_explorer<double>(); //default is double

    trace_disabled=false;

    not_used_value=0;

    word     = 0;
    nwords   = 0;
    bit_mask = 0;
}

///////////////////////////////////////////////////////////////////////////////
sca_type_explorer_base& sca_trace_object_data::get_type_info()
{
    return *type_info;
}

void sca_trace_object_data::trace_init(sca_trace_file_base*   trace_str)
{
    trace_base=trace_str;
    trace_buffer=trace_str->buffer;

    long number_of_traces =  (long)(trace_base->traces.size());

    //prepare variables for fast check wether all traces were activated
    word   = id/(8*sizeof(long));
    nwords = number_of_traces/(8*sizeof(long)) + 1;

    unsigned long bit=id - (nwords-1)*8*sizeof(long);
    bit_mask =  ~((unsigned long)1 << bit);

    if(word>=nwords-1)
    {
        unsigned long used_bits=number_of_traces-8*sizeof(unsigned long)*(nwords-1);
        unsigned long empty_bit_mask = ((unsigned long)1 << used_bits) - 1;
        bit_mask&=empty_bit_mask;
    }

    bool init_ok=trace_object->trace_init(*this);

    if(!init_ok) //ignore trace if not used signal
    {
        trace_disabled=true;
        trace_base->disabled_traces[word]&=bit_mask;
        not_used_value=0;
        trace_buffer->store_time_stamp<long>(id,sc_core::sc_time_stamp(),not_used_value);
        dont_interpolate=true;
        event_driven=true;

        std::ostringstream str;
        str << "Initialization for tracing of: " << name << " failed ";
        str << " set wave to 0" << std::endl;
        SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());
    }
    else
    {
    	if(event_driven)
    	{   //value must not be written before writting - only time must be reached
    		trace_base->disabled_traces[word]&=bit_mask;
    	}
        trace_disabled=false;
    }
}



}
}


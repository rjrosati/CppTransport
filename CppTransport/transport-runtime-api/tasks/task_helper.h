//
// Created by David Seery on 05/06/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __task_helper_H_
#define __task_helper_H_

#include "transport-runtime-api/serialization/serializable.h"

#include "transport-runtime-api/concepts/initial_conditions.h"

#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/tasks/integration_tasks.h"
#include "transport-runtime-api/tasks/output_tasks.h"

#include "transport-runtime-api/messages.h"


namespace transport
  {

    namespace integration_task_helper
      {

        template <typename number>
        task<number>* deserialize(const std::string& nm, serialization_reader* reader, const initial_conditions<number>& i)
          {
            assert(reader != nullptr);
            if(reader == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TASK_NULL_SERIALIZATION_READER);

            task<number>* rval = nullptr;

            std::string type;
            reader->read_value(__CPP_TRANSPORT_NODE_TASK_TYPE, type);

            if(type == __CPP_TRANSPORT_NODE_TASK_TYPE_TWOPF) rval = new twopf_task<number>(nm, reader, i);
            else if(type == __CPP_TRANSPORT_NODE_TASK_TYPE_THREEPF_CUBIC) rval = new threepf_cubic_task<number>(nm, reader, i);
            else if(type == __CPP_TRANSPORT_NODE_TASK_TYPE_THREEPF_FLS) rval = new threepf_fls_task<number>(nm, reader, i);
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_TASK_TYPE_UNKNOWN << " '" << type << "'";
                throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
              }

            return (rval);
          }

      }   // namespace integration_task_helper

    namespace output_task_helper
      {

        template <typename number>
        task<number>* deserialize(const std::string& nm, serialization_reader* reader, typename output_task<number>::derived_product_finder& pfinder)
          {
            assert(reader != nullptr);
            if(reader == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TASK_NULL_SERIALIZATION_READER);

            task<number>* rval = nullptr;

            std::string type;
            reader->read_value(__CPP_TRANSPORT_NODE_TASK_TYPE, type);

            if(type == __CPP_TRANSPORT_NODE_TASK_TYPE_OUTPUT) rval = new output_task<number>(nm, reader, pfinder);
            else
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_TASK_TYPE_UNKNOWN << " '" << type << "'";
                throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
              }

            return (rval);
          }

      }   // namespace output_task_helper

  }   // namespace transport


#endif //__task_helper_H_
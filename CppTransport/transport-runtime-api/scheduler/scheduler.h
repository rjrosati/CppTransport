//
// Created by David Seery on 20/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __scheduler_H_
#define __scheduler_H_


#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <set>
#include <sstream>
#include <stdexcept>

#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/scheduler/context.h"
#include "transport-runtime-api/scheduler/work_queue.h"


namespace transport
  {

    //! Abstract filter function, used to filter work items when adding to a schedule
    class abstract_filter
      {
      public:
        virtual bool filter(const twopf_kconfig& config) const = 0;
        virtual bool filter(const threepf_kconfig& config) const = 0;
      };

    //! Empty filter
    class trivial_filter: public abstract_filter
      {
      public:
        bool filter(const twopf_kconfig& config)   const { return(true); }
        bool filter(const threepf_kconfig& config) const { return(true); }
      };

    //! Filter function for work items -- used by slave nodes to filter out
    //! pieces of work intended for them
    class work_item_filter: public abstract_filter
      {
      public:

        //! Construct an empty filter
        work_item_filter()
          {
          }

        //! Construct a filter from a predefined set of items
        work_item_filter(const std::set<unsigned int>& filter_set)
          : items(filter_set)
          {
          }

        //! Add an item to the list of work-items included in this filter
        void add_work_item(unsigned int serial) { this->items.insert(serial); }

        //! Check whether a work-item is part of the filter
        bool filter(const twopf_kconfig& config)   const { return(this->items.find(config.serial) != this->items.end()); }
        bool filter(const threepf_kconfig& config) const { return(this->items.find(config.serial) != this->items.end()); }

        //! Declare friend function to write a filter to a stream
        friend std::ostream& operator<<(std::ostream& out, const work_item_filter& filter);

      private:
        //! std::set holding work items that we are supposed to process
        std::set<unsigned int> items;
      };

    //! Write a filter to a stream
    std::ostream& operator<<(std::ostream& out, const work_item_filter& filter)
      {
        std::cerr << __CPP_TRANSPORT_FILTER_TAG;
        for(std::set<unsigned int>::iterator t = filter.items.begin(); t != filter.items.end(); t++)
          {
            std::cerr << (t != filter.items.begin() ? ", " : " ") << *t;
          }
        std::cerr << std::endl;
        return(out);
      }


    class scheduler
      {
      public:
        scheduler(context& c)
          : ctx(c)
          {
          }

        //! set up a work queue for a two-point function, using a user-provided filter function
        template <typename number>
        work_queue<twopf_kconfig> make_queue(unsigned int size, const twopf_task<number>& task, const abstract_filter& F);

        //! set up a work queue for a three-point function, using a user-provided filter function
        template <typename number>
        work_queue<threepf_kconfig> make_queue(unsigned int size, const threepf_task<number>& task, const abstract_filter& F);

        //! set up a work queue for a two-point function, using no filter
        template <typename number>
        work_queue<twopf_kconfig> make_queue(unsigned int size, const twopf_task<number>& task);

        //! set up a work queue for a three-point function, using no filter
        template <typename number>
        work_queue<threepf_kconfig> make_queue(unsigned int size, const threepf_task<number>& task);

      protected:
        const context&    ctx;
      };


    // QUEUE-MAKING FUNCTIONS -- with a filter

    template <typename number>
    work_queue<twopf_kconfig> scheduler::make_queue(unsigned int size, const twopf_task<number>& task, const abstract_filter& F)
      {
        // set up an empty queue
        work_queue<twopf_kconfig> work(this->ctx, size);

        const std::vector<twopf_kconfig>& config_list = task.get_sample();
        for(std::vector<twopf_kconfig>::const_iterator t = config_list.begin(); t != config_list.end(); t++)
          {
            if(F.filter(*t)) work.enqueue_work_item(*t);
          }

        return(work);
      }


    template <typename number>
    work_queue<threepf_kconfig> scheduler::make_queue(unsigned int size, const threepf_task<number>& task, const abstract_filter& F)
      {
        work_queue<threepf_kconfig> work(this->ctx, size);

        const std::vector<threepf_kconfig>& config_list = task.get_sample();
        for(std::vector<threepf_kconfig>::const_iterator t = config_list.begin(); t != config_list.end(); t++)
          {
            if(F.filter(*t)) work.enqueue_work_item(*t);
          }

        return(work);
      }


    // QUEUE-MAKING FUNCTIONS -- without a filter


    template <typename number>
    work_queue<twopf_kconfig> scheduler::make_queue(unsigned int size, const twopf_task<number>& task)
      {
        return(this->make_queue(size, task, trivial_filter()));
      }


    template <typename number>
    work_queue<threepf_kconfig> scheduler::make_queue(unsigned int size, const threepf_task<number>& task)
      {
        return(this->make_queue(size, task, trivial_filter()));
      }


  } // / namespace transport



#endif //__scheduler_H_
//
// Created by David Seery on 19/12/2015.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_CANONICAL_SR_VELOCITY_H
#define CPPTRANSPORT_CANONICAL_SR_VELOCITY_H


#include <memory>

#include "concepts/tensors/SR_velocity.h"
#include "shared/shared_resources.h"
#include "canonical/resources.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"
#include "expression_cache.h"


namespace canonical
  {
    
    class SR_velocity : public ::SR_velocity
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        SR_velocity(language_printer& p, cse& cw, expression_cache& c, resources& r, shared_resources& s,
                    boost::timer::cpu_timer& tm, index_flatten& f);

        //! destructor is default
        virtual ~SR_velocity() = default;


        // INTERFACE -- IMPLEMENTS A 'SR_velocity' TENSOR CONCEPT

      public:

        //! evaluate full tensor, returning a flattened list
        std::unique_ptr<flattened_tensor>
        compute(const index_literal_list& indices) override;

        //! evaluate a component of the tensor
        GiNaC::ex
        compute_component(field_index i) override;

        //! evaluate lambda for tensor
        std::unique_ptr<atomic_lambda>
        compute_lambda(const index_literal& i) override;


        // INTERFACE -- IMPLEMENTS A 'transport_tensor' CONCEPT

      public:

        //! determine whether this tensor can be unrolled with the current resources
        unroll_state get_unroll(const index_literal_list& idx_list) override;


        // INTERFACE -- JANITORIAL API

        //! cache resources required for evaluation
        void pre_explicit(const index_literal_list& indices) override;

        //! cache resources required for evaluation on a lambda
        void pre_lambda();

        //! release resources
        void post() override;


        // INTERNAL API

      private:

        //! underlying symbolic expression
        GiNaC::ex expr(const GiNaC::ex& Vi);



        // INTERNAL DATA

      private:


        // CACHES

        //! reference to supplied language printer
        language_printer& printer;

        //! reference to supplied CSE worker
        cse& cse_worker;

        //! reference to expression cache
        expression_cache& cache;

        //! reference to resource object
        resources& res;

        //! reference to shared resource object
        shared_resources& shared;


        // AGENTS

        //! index flattener
        index_flatten& fl;


        // TIMER

        //! compute timer
        boost::timer::cpu_timer& compute_timer;


        // WORKSPACE AND CACHE

        //! potential
        GiNaC::ex V;

        //! flattened dV tensor
        std::unique_ptr<flattened_tensor> dV;

        //! Planck mass
        symbol_wrapper Mp;

        //! cache status
        bool cached;

      };

  }   // namespace canonical



#endif //CPPTRANSPORT_CANONICAL_SR_VELOCITY_H

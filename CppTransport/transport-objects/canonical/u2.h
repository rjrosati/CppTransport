//
// Created by David Seery on 20/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CANONICAL_U2_H
#define CPPTRANSPORT_CANONICAL_U2_H


#include <memory>

#include "concepts/u2.h"
#include "utilities/shared_resources.h"
#include "canonical/resources.h"

#include "indices.h"

#include "index_flatten.h"
#include "index_traits.h"

#include "language_printer.h"
#include "expression_cache.h"



namespace canonical
  {

    class canonical_u2: public u2
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        canonical_u2(language_printer& p, cse& cw, expression_cache& c, resources& r, shared_resources& s,
                     boost::timer::cpu_timer& tm, index_flatten& f, index_traits& t)
          : u2(),
            printer(p),
            cse_worker(cw),
            cache(c),
            res(r),
            shared(s),
            fl(f),
            traits(t),
            compute_timer(tm)
          {
          }

        //! destructor is default
        virtual ~canonical_u2() = default;


        // INTERFACE -- IMPLEMENTS A 'u2' TENSOR CONCEPT

      public:

        //! evaluate full tensor, returning a flattened list
        virtual std::unique_ptr<flattened_tensor> compute(GiNaC::symbol& k, GiNaC::symbol& a) override;

        //! evaluate component of tensor
        virtual GiNaC::ex compute_component(phase_index i, phase_index j, GiNaC::symbol& k, GiNaC::symbol& a) override;

        //! evaluate lambda for tensor
        virtual std::unique_ptr<map_lambda> compute_lambda(const abstract_index& i, const abstract_index& j,
                                                           GiNaC::symbol& k, GiNaC::symbol& a) override;

        //! invalidate cache
        virtual void reset_cache() override { this->cached = false; }


        // INTERFACE -- IMPLEMENTS A 'transport_tensor' CONCEPT

      public:

        //! determine whether this tensor can be unrolled with the current resources
        virtual enum unroll_behaviour get_unroll() override;


        // INTERNAL API

      private:

        //! cache symbols
        void cache_symbols();

        //! populate workspcae
        void populate_workspace();

        //! underlying symbolic expression
        GiNaC::ex expr_field_momentum(GiNaC::idx& i, GiNaC::idx& j, GiNaC::ex& Vij, GiNaC::ex& Vi, GiNaC::ex& Vj,
                                      GiNaC::symbol& deriv_i, GiNaC::symbol& deriv_j, GiNaC::symbol& k, GiNaC::symbol& a);


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

        //! index introspection
        index_traits& traits;


        // TIMER

        //! compute timer
        boost::timer::cpu_timer& compute_timer;


        // WORKSPACE AND CACHE

        //! list of momentum symbols
        std::unique_ptr<symbol_list> derivs;

        //! flattened dV tensor
        std::unique_ptr<flattened_tensor> dV;

        //! flattened ddV tensor
        std::unique_ptr<flattened_tensor> ddV;

        //! Hubble parameter
        GiNaC::ex Hsq;

        //! epsilon
        GiNaC::ex eps;

        //! Planck mass
        GiNaC::symbol Mp;

        //! cache status
        bool cached;

      };

  }   // namespace canonical


#endif //CPPTRANSPORT_CANONICAL_U2_H
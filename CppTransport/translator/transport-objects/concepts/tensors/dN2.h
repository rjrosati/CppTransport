//
// Created by David Seery on 20/12/2015.
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

#ifndef CPPTRANSPORT_DN2_H
#define CPPTRANSPORT_DN2_H


#include "concepts/transport_tensor.h"
#include "concepts/flattened_tensor.h"

#include "indices.h"
#include "index_literal.h"


constexpr auto DN2_TENSOR_INDICES = 2;


class dN2: public transport_tensor
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor is default
    dN2() = default;

    //! destructor is default
    virtual ~dN2() = default;


    // INTERFACE

  public:

    //! evaluate full tensor, returning a flattened list
    virtual std::unique_ptr<flattened_tensor>
    compute(const index_literal_list& indices) = 0;

    //! evaluate component of tensor
    virtual GiNaC::ex
    compute_component(phase_index i, phase_index j) = 0;

  };



#endif //CPPTRANSPORT_DN2_H

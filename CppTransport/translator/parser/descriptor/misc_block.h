//
// Created by David Seery on 24/05/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_MISC_BLOCK_H
#define CPPTRANSPORT_MISC_BLOCK_H


#include "contexted_value.h"
#include "model_settings.h"
#include "y_common.h"


class misc_block
  {

    // CONSTRUCTOR, DESTRUCTOR

  public:

    //! constructor
    misc_block(unsigned int& ec)
      : err_count(ec),
        order(index_order::right)
      {
      }

    //! destructor
    ~misc_block() = default;


    // MISCELLANEOUS SETTINGS

  public:

    //! set index order flag (left-most first or right-most first)
    void set_indexorder(index_order o);

    //! get index order flag
    index_order get_indexorder() const;


    // INTERNAL DATA

  private:

    //! error count
    unsigned int& err_count;


    // MISCELLANEOUS SETTINGS

    //! index ordering setting (left-most first or right-most first)
    index_order order;

  };


#endif //CPPTRANSPORT_MISC_BLOCK_H

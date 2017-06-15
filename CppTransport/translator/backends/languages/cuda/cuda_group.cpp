//
// Created by David Seery on 11/12/2013.
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


#include "cuda_group.h"


#include "directives.h"
#include "fundamental.h"
#include "flow_tensors.h"
#include "lagrangian_tensors.h"
#include "utensors.h"
#include "gauge_xfm.h"
#include "temporary_pool.h"
#include "kernel_argument_macros.h"
#include "resources.h"

#include "cpp_cse.h"
#include "cuda_printer.h"


cuda_group::cuda_group(translator_data& p, tensor_factory& fctry)
  : package_group(p, fctry, PackageGroupComponentAgent<cuda::cuda_printer, cpp::cpp_cse, lambda_manager>())
  {
    // construct replacement rule packages
    this->add_directive_package<macro_packages::directives>(p);
    this->add_directive_package<macro_packages::resources>(p, fctry.get_resource_manager());

    this->add_rule_package<macro_packages::fundamental>(p, *l_printer);
    this->add_rule_package<macro_packages::flow_tensors>(p, *l_printer);
    this->add_rule_package<macro_packages::lagrangian_tensors>(p, *l_printer);
    this->add_rule_package<macro_packages::utensors>(p, *l_printer);
    this->add_rule_package<macro_packages::gauge_xfm>(p, *l_printer);
    this->add_rule_package<macro_packages::temporary_pool>(p, *l_printer);
    this->add_rule_package<shared::kernel_argument_macros>(p, *l_printer);
  }

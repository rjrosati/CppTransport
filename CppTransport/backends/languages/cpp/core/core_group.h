//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __core_group_H_
#define __core_group_H_


#include "package_group.h"
#include "ginac_cache.h"

#include "fundamental.h"
#include "flow_tensors.h"
#include "lagrangian_tensors.h"
#include "utensors.h"
#include "gauge_xfm.h"
#include "temporary_pool.h"
#include "summation.h"
#include "core_macros.h"
#include "cpp_printer.h"


class core_group: public package_group
  {

  public:

    core_group(translation_unit* u, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache);
    ~core_group();

  protected:

    cpp::cpp_printer                    printer;

    macro_packages::fundamental*        f;
    macro_packages::flow_tensors*       ft;
    macro_packages::lagrangian_tensors* lt;
    macro_packages::utensors*           ut;
    macro_packages::gauge_xfm*          xf;
    macro_packages::temporary_pool*     tp;
    macro_packages::summation*          su;
    cpp::core_macros*                   cm;
  };


#endif //__core_group_H_

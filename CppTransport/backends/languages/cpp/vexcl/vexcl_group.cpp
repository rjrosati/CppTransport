//
// Created by David Seery on 06/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "vexcl_group.h"

#include "cpp_printer.h"
#include "cpp_cse.h"


vexcl_group::vexcl_group(translation_unit* u, ginac_cache<expression_item_types, DEFAULT_GINAC_CACHE_SIZE>& cache)
  : package_group(u, CPP_COMMENT_SEPARATOR, cache), printer()
  {
    // set up cse worker instance
    // this has to happen before setting up the individual macro packages,
    // because it gets pushed to them when they join the package group
    cse_worker = new cpp::cpp_cse(0, this->printer, this->unit->get_do_cse());

    f  = new macro_packages::fundamental       (u, this->printer);
    ft = new macro_packages::flow_tensors      (u, this->printer);
    lt = new macro_packages::lagrangian_tensors(u, this->printer);
    ut = new macro_packages::utensors          (u, this->printer);
    xf = new macro_packages::gauge_xfm         (u, this->printer);
    tp = new macro_packages::temporary_pool    (u, this->printer);
    su = new macro_packages::summation         (u, this->printer);
    vs = new cpp::vexcl_steppers               (u, this->printer);
    vk = new cpp::vexcl_kernels                (u, this->printer);

    this->push_back(vk);
    this->push_back(vs);
    this->push_back(su);
    this->push_back(tp);
    this->push_back(ut);
    this->push_back(xf);
    this->push_back(lt);
    this->push_back(ft);
    this->push_back(f);
  }


vexcl_group::~vexcl_group()
  {
    delete f;
    delete ft;
    delete lt;
    delete ut;
    delete xf;
    delete tp;
    delete su;
    delete vs;
    delete vk;

    delete cse_worker;
  }

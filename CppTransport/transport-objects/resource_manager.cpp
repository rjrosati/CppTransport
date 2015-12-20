//
// Created by David Seery on 18/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#include "resource_manager.h"


void resource_manager::release()
  {
    this->parameters_cache.reset();
    this->coordinates_cache.reset();
    this->phase_flatten_cache.reset();
    this->field_flatten_cache.reset();
    this->dV_cache.reset();
    this->ddV_cache.reset();
    this->dddV_cache.reset();
    this->connexion_cache.reset();
    this->Riemann_cache.reset();
  }

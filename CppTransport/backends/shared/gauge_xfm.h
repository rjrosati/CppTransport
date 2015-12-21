//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __macropackagCPPTRANSPORT_MACROS_GAUGE_XFM_H
#define __macropackagCPPTRANSPORT_MACROS_GAUGE_XFM_H


#include "replacement_rule_package.h"


namespace macro_packages
  {

    class gauge_xfm: public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        gauge_xfm(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~gauge_xfm() = default;


        // INTERFACE

      public:

        //! return index- macros package
        const std::vector<index_rule>  get_index_rules() override;


        // INTERNAL API

      protected:

        std::unique_ptr<cse_map> pre_zeta_xfm_1(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_zeta_xfm_2(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_deltaN_xfm_1(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_deltaN_xfm_2(const macro_argument_list& args);


        // INTERNAL DATA

      private:

        //! zeta1 tensor
        std::unique_ptr<zeta1> zeta1_tensor;

        //! zeta2 tensor
        std::unique_ptr<zeta2> zeta2_tensor;

        //! dN1 tensor
        std::unique_ptr<dN1> dN1_tensor;

        //! dN2 tensor
        std::unique_ptr<dN2> dN2_tensor;

      };

  } // namespace macro_packages


#endif //CPPTRANSPORT_MACROS_GAUGE_XFM_H

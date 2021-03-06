//
// Created by David Seery on 05/08/2013.
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

#ifndef CPPTRANSPORT_DEFAULT_SYMBOLS_H_
#define CPPTRANSPORT_DEFAULT_SYMBOLS_H_


#define ZETA_SYMBOL                "\\zeta"
#define ZETA_NAME                  "zeta"

#define PRIME_SYMBOL               "\\prime"
#define PRIME_NAME                 "'"

#define K_SYMBOL                   "k"
#define K_NAME                     "k"

#define K1_SYMBOL                  "k_1"
#define K1_NAME                    "k1"
#define K2_SYMBOL                  "k_2"
#define K2_NAME                    "k2"
#define K3_SYMBOL                  "k_3"
#define K3_NAME                    "k3"

#define KT_SYMBOL                  "k_t"
#define KT_NAME                    "kt"
#define ALPHABETA_ALPHA_SYMBOL     "\\alpha"
#define ALPHABETA_ALPHA_NAME       "alpha"
#define ALPHABETA_BETA_SYMBOL      "\\beta"
#define ALPHABETA_BETA_NAME        "beta"

#define TWOPF_SYMBOL               "\\Sigma"
#define TWOPF_NAME                 "Sigma"

#define DIMENSIONLESS_TWOPF_SYMBOL "\\mathcal{P}"
#define DIMENSIONLESS_TWOPF_NAME   "P"

#define THREEPF_SYMBOL             "\\alpha"
#define THREEPF_NAME               "3pf"

#define TWOPF_LABEL                "two-point function"
#define THREEPF_LABEL              "three-point function"
#define SHAPE_LABEL                "shape function"

#define U2_SYMBOL                  "u"
#define U2_NAME                    "u2"
#define U3_SYMBOL                  "u"
#define U3_NAME                    "u3"

#define U2_LABEL_LATEX             "$u_2$ tensor"
#define U3_LABEL_LATEX             "$u_3$ tensor"
#define U2_LABEL                   "u2 tensor"
#define U3_LABEL                   "u3 tensor"
#define FIELDS_LABEL               "fields"

#define REDUCED_BISPECTRUM_SYMBOL  "f_{\\mathrm{NL}}(k_1, k_2, k_3)"
#define REDUCED_BISPECTRUM_NAME    "fNL(k1,k2,k3)"

#define SHAPE_BISPECTRUM_SYMBOL    "S(k_1, k_2, k_3)"
#define SHAPE_BISPECTRUM_NAME      "S(k1, k2, k3)"

#define N_LABEL_LATEX              "$N$"
#define N_LABEL                    "N"

#define PICK_N_LABEL               (gadget->latex_labels() ? N_LABEL_LATEX : N_LABEL)

#define PICK_U2_LABEL              (gadget->latex_labels() ? U2_LABEL_LATEX : U2_LABEL)
#define PICK_U3_LABEL              (gadget->latex_labels() ? U3_LABEL_LATEX : U3_LABEL)

#define DEFAULT_OUTPUT_DOTPHI      (true)


#endif // CPPTRANSPORT_DEFAULT_SYMBOLS

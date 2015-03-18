//
// Created by David Seery on 26/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_DERIVED_LINE_H
#define __CPP_TRANSPORT_MESSAGES_EN_DERIVED_LINE_H


#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_CONTENT_TYPE_UNKNOWN "Internal error: Cannot deserialize unknown 'derived_line' content type"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_TWOPF_TYPE_UNKNOWN   "Internal error: Unknown twopf type in 'derived_line'"


#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_LABEL                    "Derived content:"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_TASK_NAME                "Derived from task"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_MODEL_NAME               "model"

#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_XTYPE                    "axis data represent"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_TIME_SERIES_LABEL        "time series"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_K_SERIES_LABEL           "k wavenumber series"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_ANGLE_SERIES_LABEL       "angle series"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_SQUEEZING_FRACTION_LABEL "squeezing fraction"

#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_YTYPE                    "values represent"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_CF_LABEL                 "correlation functions"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_BGFIELD_LABEL            "background fields"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_FNL_LABEL                "fNL"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_R_LABEL                  "tensor-to-scale ratio"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_SPECTRAL_INDEX_LABEL     "spectral index"

#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_MEANING              "correlation functions involve"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_DERIVATIVE           "field derivatives"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_MOMENTA              "momenta"

#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_MEANING           "wavenumber normalization is"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_COMOVING          "comoving"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_CONVENTIONAL      "conventional"

#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_XTYPE            "derived_line error: unknown axis type"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_UNKNOWN_YTYPE            "derived_line error: unknown value type"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_DOT_TYPE_UNKNOWN         "derived_line error: unknown dot meaning"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN      "derived_line error: unknown klabel meaning"

#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_NOT_INTEGRATION_TASK     "derived_line error: cannot derive data from a non-integration task"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_NULL_READER              "derived_line error: null serialization reader"
#define __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_NULL_MODEL               "derived_line error: null model"


#endif // __CPP_TRANSPORT_MESSAGES_EN_DERIVED_LINE_H

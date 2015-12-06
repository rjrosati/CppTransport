//
// Created by David Seery on 15/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __threepf_task_H_
#define __threepf_task_H_


#include <memory>

#include "transport-runtime-api/tasks/integration_detail/common.h"
#include "transport-runtime-api/tasks/integration_detail/abstract.h"
#include "transport-runtime-api/tasks/integration_detail/twopf_list_task.h"
#include "transport-runtime-api/tasks/configuration-database/threepf_config_database.h"

#include "transport-runtime-api/utilities/spline1d.h"

#include "transport-runtime-api/defaults.h"

#include "boost/math/tools/roots.hpp"

#include "sqlite3.h"


#define CPPTRANSPORT_NODE_THREEPF_INTEGRABLE            "integrable"

#define CPPTRANSPORT_NODE_THREEPF_CUBIC_SPACING         "k-spacing"
#define CPPTRANSPORT_NODE_THREEPF_FLS_KT_SPACING        "kt-spacing"
#define CPPTRANSPORT_NODE_THREEPF_FLS_ALPHA_SPACING     "alpha-spacing"
#define CPPTRANSPORT_NODE_THREEPF_FLS_BETA_SPACING      "beta-spacing"


namespace transport
	{

		enum class threepf_ics_exit_type { smallest_wavenumber_exit, kt_wavenumber_exit};

    // three-point function task
    template <typename number>
    class threepf_task: public twopf_list_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a threepf-task
        threepf_task(const std::string& nm, const initial_conditions<number>& i, range<double>& t, bool ff);

        //! deserialization constructor
        threepf_task(const std::string& n, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i);

        //! Destroy a three-point function task
        virtual ~threepf_task() = default;


        // INTERFACE - THREEPF K-CONFIGURATIONS

      public:

        //! Provide access to threepf k-configuration database
        const threepf_kconfig_database& get_threepf_database() const { return(*(this->threepf_db)); }

        //! Compute horizon-exit times for each mode in the database

        //! Should be called once the database has been populated.
        //! Horizon exit times are stored when the database is serialized, so does not need to be called again.
        virtual void compute_horizon_exit_times() override;

        //! Determine whether this task is integrable
        bool is_integrable() const { return(this->integrable); }

        //! get size of a voxel on the integration grid
        virtual double voxel_size() const = 0;

        //! get measure at a particular k-configuration
        virtual number measure(const threepf_kconfig& config) const = 0;

      protected:

        //! Compute horizon-exit times for each mode in the database -- using a supplied spline
        template <typename SplineObject, typename TolerancePolicy>
        void threepf_compute_horizon_exit_times(SplineObject& sp, TolerancePolicy tol);



        // INTERFACE - INITIAL CONDITIONS AND INTEGRATION DETAILS

      public:

        //! Get std::vector of initial conditions, offset using fast forwarding if enabled
        std::vector<number> get_ics_vector(const threepf_kconfig& kconfig) const;

        //! Get time of horizon exit for a k-configuration
        double get_ics_exit_time(const threepf_kconfig& kconfig, threepf_ics_exit_type type=threepf_ics_exit_type::smallest_wavenumber_exit) const;

		    //! Get std::vector of initial conditions at horizon exit time for a k-configuration
		    std::vector<number> get_ics_exit_vector(const threepf_kconfig& kconfig, threepf_ics_exit_type type=threepf_ics_exit_type::smallest_wavenumber_exit) const;

        //! Build time-sample database
        const time_config_database get_time_config_database(const threepf_kconfig& config) const;


        // INTERFACE - FAST FORWARD MANAGEMENT

      public:

        //! Get start time for a threepf configuration
        double get_fast_forward_start(const threepf_kconfig& config) const;


        // SERIALIZATION -- implements a 'serialiazble' interface

      public:

        //! Serialize this task to the repository
        virtual void serialize(Json::Value& writer) const override;


        // WRITE K-CONFIGURATION DATABASE

      public:

        //! Write k-configuration database to disk
        virtual void write_kconfig_database(sqlite3* handle) override;

		    //! Check whether k-configuration databases have been modified
		    virtual bool is_kconfig_database_modified() const override { return(this->threepf_db->is_modified() || this->twopf_list_task<number>::is_kconfig_database_modified()); }


        // INTERNAL DATA

      protected:

        //! database of threepf k-configurations

		    //! Use std::shared_ptr<> to manage the lifetime of the database.
		    //! This avoids costly copy operations when the database is large.
		    //! When a task is cloned, it simply inherits a reference to the
		    //! shared database
        std::shared_ptr<threepf_kconfig_database> threepf_db;

        //! Is this threepf task integrable? ie., have we dropped any configurations, and is the spacing linear?
        bool integrable;

	    };


    template <typename number>
    threepf_task<number>::threepf_task(const std::string& nm, const initial_conditions<number>& i, range<double>& t, bool ff)
	    : twopf_list_task<number>(nm, i, t, ff),
	      integrable(true)
	    {
        threepf_db = std::make_shared<threepf_kconfig_database>(this->twopf_list_task<number>::kstar);
	    }


    template <typename number>
    threepf_task<number>::threepf_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i)
	    : twopf_list_task<number>(nm, reader, handle, i)
	    {
		    threepf_db = std::make_shared<threepf_kconfig_database>(this->twopf_list_task<number>::kstar, handle, *this->twopf_list_task<number>::twopf_db);

        //! deserialize integrable status
        integrable = reader[CPPTRANSPORT_NODE_THREEPF_INTEGRABLE].asBool();

        // rebuild database of stored times; this isn't serialized but recomputed on-the-fly
        this->cache_stored_time_config_database();
	    }


    template <typename number>
    void threepf_task<number>::serialize(Json::Value& writer) const
	    {
        // serialize integrable status
        writer[CPPTRANSPORT_NODE_THREEPF_INTEGRABLE] = this->integrable;

		    // threepf database is serialized separately to a SQLite database
        // this serialization is handled by the repository layer via write_kconfig_database() below

        this->twopf_list_task<number>::serialize(writer);
	    }


		template <typename number>
		void threepf_task<number>::write_kconfig_database(sqlite3* handle)
			{
		    this->twopf_list_task<number>::write_kconfig_database(handle);
				this->threepf_db->write(handle);
			}


    template <typename number>
    const time_config_database threepf_task<number>::get_time_config_database(const threepf_kconfig& config) const
      {
        return(this->build_time_config_database(this->get_fast_forward_start(config)));
      }


    template <typename number>
    double threepf_task<number>::get_fast_forward_start(const threepf_kconfig& config) const
      {
        double kmin = std::min(std::min(config.k1_conventional, config.k2_conventional), config.k3_conventional);

        twopf_kconfig_database::record_iterator rec;
		    bool found = this->twopf_db->find(kmin, rec);
		    assert(found);

        return((*rec)->t_exit - this->ff_efolds);
      }


    template <typename number>
    std::vector<number> threepf_task<number>::get_ics_vector(const threepf_kconfig& config) const
	    {
        if(this->fast_forward)
          {
            return this->integration_task<number>::get_ics_vector(this->get_fast_forward_start(config));
          }
        else
          {
            return this->ics.get_vector();
          }
	    }


		template <typename number>
		double threepf_task<number>::get_ics_exit_time(const threepf_kconfig& config, threepf_ics_exit_type type) const
			{
		    double time = config.t_exit;

		    switch(type)
			    {
		        case threepf_ics_exit_type::smallest_wavenumber_exit:
			        {
		            double kmin = std::min(std::min(config.k1_conventional, config.k2_conventional), config.k3_conventional);

		            twopf_kconfig_database::record_iterator rec;
		            bool found = this->twopf_db->find(kmin, rec);
		            assert(found);

		            time = (*rec)->t_exit;
		            break;
			        }

		        case threepf_ics_exit_type::kt_wavenumber_exit:
			        {
		            time = config.t_exit;
		            break;
			        };
			    }

				return(time);
			}


		template <typename number>
		std::vector<number> threepf_task<number>::get_ics_exit_vector(const threepf_kconfig& config, threepf_ics_exit_type type) const
			{

				return this->integration_task<number>::get_ics_vector(this->get_ics_exit_time(config, type));
			}


    template <typename number>
    void threepf_task<number>::compute_horizon_exit_times()
	    {
		    double largest_kt = this->threepf_db->get_kmax_comoving()/3.0;
        double largest_k = this->twopf_db->get_kmax_comoving();

        std::vector<double> N;
        std::vector<number> log_aH;

        try
          {
            this->get_model()->compute_aH(this, N, log_aH, std::max(largest_k, largest_kt));
            assert(N.size() == log_aH.size());

            spline1d<number> sp(N, log_aH);

            this->threepf_compute_horizon_exit_times(sp, task_impl::TolerancePredicate(CPPTRANSPORT_ROOT_FIND_TOLERANCE));

            // forward to underlying twopf_list_task to also update its database
            this->twopf_list_task<number>::twopf_compute_horizon_exit_times(sp, task_impl::TolerancePredicate(CPPTRANSPORT_ROOT_FIND_TOLERANCE));
          }
        catch(failed_to_compute_horizon_exit& xe)
          {
            this->compute_horizon_exit_times_fail(xe);
            exit(EXIT_FAILURE);
          }
	    };


		template <typename number>
		template <typename SplineObject, typename TolerancePolicy>
		void threepf_task<number>::threepf_compute_horizon_exit_times(SplineObject& sp, TolerancePolicy tol)
			{
		    for(threepf_kconfig_database::config_iterator t = this->threepf_db->config_begin(); t != this->threepf_db->config_end(); ++t)
			    {
		        // set spline to evaluate aH-k and then solve for N
		        sp.set_offset(log(t->kt_comoving/3.0));

            t->t_exit = task_impl::find_zero_of_spline(sp, tol);
			    }
			}


    template <typename number>
    class threepf_cubic_task: public threepf_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a named three-point function task based on sampling from a cubic lattice of ks,
        //! with specified policies
        template <typename StoragePolicy>
        threepf_cubic_task(const std::string& nm, const initial_conditions<number>& i,
                           range<double>& t, range<double>& ks, StoragePolicy policy,
                           bool ff=true, double tol=CPPTRANSPORT_DEFAULT_KCONFIG_TOLERANCE);

        //! Deserialization constructor
        threepf_cubic_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i);


        // INTERFACE

      public:

        //! get size of a voxel on the integration grid
        virtual double voxel_size() const override { return(this->spacing*this->spacing*this->spacing); }

        //! get measure; here, just dk1 dk2 dk3 so there is nothing to do
        virtual number measure(const threepf_kconfig& config) const override { return(1.0); }


        // SERIALIZATION (implements a 'serialiazble' interface)

      public:

        //! Serialize this task to the repository
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! Virtual copy
        virtual threepf_cubic_task<number>* clone() const override { return new threepf_cubic_task<number>(static_cast<const threepf_cubic_task<number>&>(*this)); }


        // INTERNAL DATA

      protected:

        double spacing;

	    };


    // build a 3pf task from a cubic lattice of k-modes
    template <typename number>
    template <typename StoragePolicy>
    threepf_cubic_task<number>::threepf_cubic_task(const std::string& nm, const initial_conditions<number>& i,
                                                   range<double>& t, range<double>& ks, StoragePolicy policy,
                                                   bool ff, double tol)
	    : threepf_task<number>(nm, i, t, ff)
	    {
        // step through the lattice of k-modes, recording which are viable triangular configurations
        // we insist on ordering, so i <= j <= k
        for(unsigned int j = 0; j < ks.size(); ++j)
	        {
            for(unsigned int k = 0; k <= j; ++k)
	            {
                for(unsigned int l = 0; l <= k; ++l)
	                {
                    auto maxij  = (ks[j] > ks[k] ? ks[j] : ks[k]);
                    auto maxijk = (maxij > ks[l] ? maxij : ks[l]);

                    if(ks[j] + ks[k] + ks[l] - 2.0*maxijk >= -std::abs(tol))   // impose the triangle conditions
	                    {
                        if(this->threepf_task<number>::threepf_db->add_k1k2k3_record(*this->twopf_list_task<number>::twopf_db, ks[j], ks[k], ks[l], policy) < 0)
                          {
                            this->threepf_task<number>::integrable = false;    // can't integrate any task which has dropped configurations, because the points may be scattered over the integration region
                          }
	                    }
	                }
	            }
	        }

        // need linear spacing to be integrable
        if(!ks.is_simple_linear()) this->threepf_task<number>::integrable = false;
        spacing = (ks.get_max() - ks.get_min())/ks.get_steps();

        std::cout << "'" << this->get_name() << "': " << CPPTRANSPORT_TASK_THREEPF_ELEMENTS_A << " " << this->threepf_db->size() << " "
          << CPPTRANSPORT_TASK_THREEPF_ELEMENTS_B << " " << this->twopf_db->size() << " " <<CPPTRANSPORT_TASK_THREEPF_ELEMENTS_C << '\n';

        this->compute_horizon_exit_times();

		    // write_time_details() should come *after* compute_horizon_exit_times();
        this->write_time_details();
        this->cache_stored_time_config_database();
	    }


    template <typename number>
    threepf_cubic_task<number>::threepf_cubic_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i)
	    : threepf_task<number>(nm, reader, handle, i)
	    {
        spacing = reader[CPPTRANSPORT_NODE_THREEPF_CUBIC_SPACING].asDouble();
	    }


    template <typename number>
    void threepf_cubic_task<number>::serialize(Json::Value& writer) const
	    {
        writer[CPPTRANSPORT_NODE_TASK_TYPE]             = std::string(CPPTRANSPORT_NODE_TASK_TYPE_THREEPF_CUBIC);
        writer[CPPTRANSPORT_NODE_THREEPF_CUBIC_SPACING] = this->spacing;

        this->threepf_task<number>::serialize(writer);
	    }


    template <typename number>
    class threepf_fls_task: public threepf_task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a named three-point function task based on sampling at specified values of
        //! the Fergusson-Shellard-Liguori parameters k_t, alpha and beta,
        //! with specified storage policies
        template <typename StoragePolicy>
        threepf_fls_task(const std::string& nm, const initial_conditions<number>& i, range<double>& t,
                         range<double>& kts, range<double>& alphas, range<double>& betas,
                         StoragePolicy kp, bool ff=true,
                         double smallest_squeezing=CPPTRANSPORT_DEFAULT_SMALLEST_SQUEEZING,
                         double tol=CPPTRANSPORT_DEFAULT_KCONFIG_TOLERANCE);

        //! Deserialization constructor
        threepf_fls_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i);


        // INTERFACE

      public:

        //! get size of a voxel on the integration grid
        virtual double voxel_size() const override { return(this->kt_spacing*this->alpha_spacing*this->beta_spacing); }

        //! get measure; here k_t^2 dk_t dalpha dbeta
        virtual number measure(const threepf_kconfig& config) const override { return(static_cast<number>(config.kt_conventional *config.kt_conventional)); }


        // SERIALIZATION (implements a 'serialiazble' interface)

      public:

        //! Serialize this task to the repository
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! Virtual copy
        virtual threepf_fls_task<number>* clone() const override { return new threepf_fls_task<number>(static_cast<const threepf_fls_task<number>&>(*this)); }


        // INTERNAL DATA

      protected:

        //! k_t spacing
        double kt_spacing;

        //! alpha spacing
        double alpha_spacing;

        //! beta spacing
        double beta_spacing;

	    };


    // build a threepf task from sampling at specific values of the Fergusson-Shellard-Liguori parameters k_t, alpha, beta
    template <typename number>
    template <typename StoragePolicy>
    threepf_fls_task<number>::threepf_fls_task(const std::string& nm, const initial_conditions<number>& i, range<double>& t,
                                               range<double>& kts, range<double>& alphas, range<double>& betas,
                                               StoragePolicy policy, bool ff, double smallest_squeezing, double tol)
	    : threepf_task<number>(nm, i, t, ff)
	    {
        for(unsigned int j = 0; j < kts.size(); ++j)
	        {
            for(unsigned int k = 0; k < alphas.size(); ++k)
	            {
                for(unsigned int l = 0; l < betas.size(); ++l)
	                {
                    if(betas[l] >= 0.0
	                     && betas[l] <= 1.0
	                     && betas[l]-1.0 - alphas[k] <= std::abs(tol)
	                     && alphas[k] - (1.0-betas[l]) < std::abs(tol)                   // impose triangle conditions,
	                     && alphas[k] >= 0.0
	                     && betas[l] - (1.0+alphas[k])/3.0 >= -std::abs(tol)             // impose k1 >= k2 >= k3
	                     && std::abs(1.0 - betas[l]) > smallest_squeezing                // impose maximum squeezing on k3
	                     && std::abs(1.0 + alphas[k] + betas[l]) > smallest_squeezing
	                     && std::abs(1.0 - alphas[k] + betas[l]) > smallest_squeezing)   // impose maximum squeezing on k1, k2
	                    {
                        if(this->threepf_task<number>::threepf_db->add_FLS_record(*this->threepf_task<number>::twopf_db, kts[j], alphas[k], betas[l], policy) < 0)
                          {
                            this->threepf_task<number>::integrable = false;    // can't integrate any task which has dropped configurations, because the points may be scattered over the integration region
                          }
	                    }
	                }
	            }
	        }

        // need linear spacing to be integrable
        if(!kts.is_simple_linear() || !alphas.is_simple_linear() || !betas.is_simple_linear()) this->threepf_task<number>::integrable = false;
        kt_spacing    = (kts.get_max() - kts.get_min()) / kts.get_steps();
        alpha_spacing = (alphas.get_max() - alphas.get_min()) / alphas.get_steps();
        beta_spacing  = (betas.get_max() - betas.get_min()) / betas.get_steps();

        std::cout << "'" << this->get_name() << "': " << CPPTRANSPORT_TASK_THREEPF_ELEMENTS_A << " " << this->threepf_db->size() << " "
          << CPPTRANSPORT_TASK_THREEPF_ELEMENTS_B << " " << this->twopf_db->size() << " " <<CPPTRANSPORT_TASK_THREEPF_ELEMENTS_C << '\n';

        this->compute_horizon_exit_times();

		    // write_time_details() should come *after* compute_horizon_exit_times();
        this->write_time_details();
        this->cache_stored_time_config_database();
	    }


    template <typename number>
    threepf_fls_task<number>::threepf_fls_task(const std::string& nm, Json::Value& reader, sqlite3* handle, const initial_conditions<number>& i)
	    : threepf_task<number>(nm, reader, handle, i)
	    {
        kt_spacing    = reader[CPPTRANSPORT_NODE_THREEPF_FLS_KT_SPACING].asDouble();
        alpha_spacing = reader[CPPTRANSPORT_NODE_THREEPF_FLS_ALPHA_SPACING].asDouble();
        beta_spacing  = reader[CPPTRANSPORT_NODE_THREEPF_FLS_BETA_SPACING].asDouble();
	    }


    template <typename number>
    void threepf_fls_task<number>::serialize(Json::Value& writer) const
	    {
        writer[CPPTRANSPORT_NODE_TASK_TYPE] = std::string(CPPTRANSPORT_NODE_TASK_TYPE_THREEPF_FLS);

        writer[CPPTRANSPORT_NODE_THREEPF_FLS_KT_SPACING]    = this->kt_spacing;
        writer[CPPTRANSPORT_NODE_THREEPF_FLS_ALPHA_SPACING] = this->alpha_spacing;
        writer[CPPTRANSPORT_NODE_THREEPF_FLS_BETA_SPACING]  = this->beta_spacing;

        this->threepf_task<number>::serialize(writer);
	    }

	}   // namespace transport


#endif //__threepf_task_H_

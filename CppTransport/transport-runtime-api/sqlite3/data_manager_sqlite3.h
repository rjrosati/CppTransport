//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __data_manager_sqlite3_H_
#define __data_manager_sqlite3_H_


#include <sstream>
#include <list>
#include <string>
#include <vector>

#include "transport-runtime-api/data/data_manager.h"

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"

#include "boost/filesystem/operations.hpp"

#include "sqlite3.h"
#include "transport-runtime-api/sqlite3/operations/data_manager.h"


#define CPPTRANSPORT_TEMPORARY_CONTAINER_STEM "worker"
#define CPPTRANSPORT_TEMPORARY_CONTAINER_XTN  ".sqlite"


namespace transport
  {

    namespace
      {

        const std::string default_excpt_a = CPPTRANSPORT_DATACTR_OPEN_A;
        const std::string default_excpt_b = CPPTRANSPORT_DATACTR_OPEN_B;

      }   // unnamed namespace


    // implements the data_manager interface using sqlite3
    template <typename number>
    class data_manager_sqlite3: public data_manager<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Create a data_manager_sqlite3 instance
        data_manager_sqlite3(unsigned int bcap, unsigned int dcap, unsigned int ckp)
          : data_manager<number>(bcap, dcap, ckp),
            temporary_container_serial(0)
          {
          }

        //! Destroy a data_manager_sqlite3 instance
        ~data_manager_sqlite3();


        // WRITER HANDLONG -- implements a 'data_manager' interface

      public:

        //! Initialize a new integration_writer object, including the data container.
        //! Never overwrites existing data; if the container already exists, an exception is thrown
        virtual void initialize_writer(std::shared_ptr< integration_writer<number> >& writer, bool recover_mode = false) override;

        //! Close an open integration_writer object.

        //! Any open sqlite3 handles are closed, meaning that any integration_writer objects will be invalidated.
        //! After closing, attempting to use an integration_writer will lead to unsubtle errors.
        virtual void close_writer(std::shared_ptr< integration_writer<number> >& writer) override;

		    //! Initialize a new derived_content_writer object.
		    virtual void initialize_writer(std::shared_ptr< derived_content_writer<number> >& writer, bool recover_mode = false) override;

		    //! Close an open derived_content_writer object.

		    //! Any open sqlite3 handles are closed. Attempting to use the writer after closing
		    //! will lead to unsubtle errors.
		    virtual void close_writer(std::shared_ptr< derived_content_writer<number> >& writer) override;

        //! Initialize a new postintegration_writer object.
        virtual void initialize_writer(std::shared_ptr< postintegration_writer<number> >& writer, bool recover_mode = false) override;

        //! Close an open postintegration_writer object
        virtual void close_writer(std::shared_ptr< postintegration_writer<number> >& writer) override;


        // WRITE TABLES -- implements a 'data_manager' interface

      public:

        //! Create tables needed for a twopf container
        virtual void create_tables(std::shared_ptr< integration_writer<number> >& writer, twopf_task<number>* tk) override;

        //! Create tables needed for a threepf container
        virtual void create_tables(std::shared_ptr< integration_writer<number> >& writer, threepf_task<number>* tk) override;

        //! Create tables needed for a zeta twopf container
        virtual void create_tables(std::shared_ptr< postintegration_writer<number> >& writer, zeta_twopf_task<number>* tk) override;

        //! Create tables needed for a zeta threepf container
        virtual void create_tables(std::shared_ptr< postintegration_writer<number> >& writer, zeta_threepf_task<number>* tk) override;

        //! Create tables needed for an fNL container
        virtual void create_tables(std::shared_ptr< postintegration_writer<number> >& writer, fNL_task<number>* tk) override;


        // SEEDING -- implements a 'data manager' interface

      public:

        //! Seed a writer for a twopf task
        virtual void seed_writer(std::shared_ptr< integration_writer<number> >& writer, twopf_task<number>* tk,
                                 const std::shared_ptr< output_group_record<integration_payload> >& seed) override;

        //! Seed a writer for a threepf task
        virtual void seed_writer(std::shared_ptr< integration_writer<number> >& writer, threepf_task<number>* tk,
                                 const std::shared_ptr< output_group_record<integration_payload> >& seed) override;

        //! Seed a writer for a zeta twopf task
        virtual void seed_writer(std::shared_ptr< postintegration_writer<number> >& writer, zeta_twopf_task<number>* tk,
                                 const std::shared_ptr< output_group_record<postintegration_payload> >& seed) override;

        //! Seed a writer for a zeta threepf task
        virtual void seed_writer(std::shared_ptr< postintegration_writer<number> >& writer, zeta_threepf_task<number>* tk,
                                 const std::shared_ptr< output_group_record<postintegration_payload> >& seed) override;

        //! Seed a writer for an fNL task
        virtual void seed_writer(std::shared_ptr< postintegration_writer<number> >& writer, fNL_task<number>* tk,
                                 const std::shared_ptr< output_group_record<postintegration_payload> >& seed) override;


        // CONSTRUCT BATCHERS  -- implements a 'data_manager' interface

      public:

        //! Create a temporary container for twopf data. Returns a batcher which can be used for writing to the container.
        virtual twopf_batcher<number> create_temp_twopf_container(twopf_task<number>* tk, const boost::filesystem::path& tempdir,
                                                                  const boost::filesystem::path& logdir,
                                                                  unsigned int worker, unsigned int group, model<number>* m,
                                                                  generic_batcher::container_dispatch_function dispatcher) override;

        //! Create a temporary container for threepf data. Returns a batcher which can be used for writing to the container.
        virtual threepf_batcher<number> create_temp_threepf_container(threepf_task<number>* tk, const boost::filesystem::path& tempdir,
                                                                      const boost::filesystem::path& logdir,
                                                                      unsigned int worker, unsigned int group, model<number>* m,
                                                                      generic_batcher::container_dispatch_function dispatcher) override;

        //! Create a temporary container for zeta twopf data. Returns a batcher which can be used for writing to the container.
        virtual zeta_twopf_batcher<number> create_temp_zeta_twopf_container(const boost::filesystem::path& tempdir,
                                                                            const boost::filesystem::path& logdir,
                                                                            unsigned int worker,
                                                                            generic_batcher::container_dispatch_function dispatcher) override;

        //! Create a temporary container for zeta threepf data. Returns a batcher which can be used for writing to the container.
        virtual zeta_threepf_batcher<number> create_temp_zeta_threepf_container(const boost::filesystem::path& tempdir,
                                                                                const boost::filesystem::path& logdir,
                                                                                unsigned int worker,
                                                                                generic_batcher::container_dispatch_function dispatcher) override;

        //! Create a temporary container for fNL data. Returns a batcher which can be used for writing to the container.
        virtual fNL_batcher<number> create_temp_fNL_container(const boost::filesystem::path& tempdir,
                                                              const boost::filesystem::path& logdir,
                                                              unsigned int worker,
                                                              generic_batcher::container_dispatch_function dispatcher,
                                                              derived_data::template_type type) override;


        // AGGREGATION HANDLERS

      protected:

        //! Aggregate a temporary twopf container into a principal container
        bool aggregate_twopf_batch(integration_writer<number>& writer, const std::string& temp_ctr);

        //! Aggregate a temporary threepf container into a principal container
        bool aggregate_threepf_batch(integration_writer<number>& writer, const std::string& temp_ctr);

        //! Aggregate a derived product
        bool aggregate_derived_product(derived_content_writer<number>& writer, const std::string& temp_name, const std::list<std::string>& used_groups);

        //! Aggregate a temporary zeta_twopf container
        bool aggregate_zeta_twopf_batch(postintegration_writer<number>& writer, const std::string& temp_ctr);

        //! Aggregate a temporary zeta_threepf container
        bool aggregate_zeta_threepf_batch(postintegration_writer<number>& writer, const std::string& temp_ctr);

        //! Aggregate a temporary fNL container
        bool aggregate_fNL_batch(postintegration_writer<number>& writer, const std::string& temp_ctr, derived_data::template_type type);


        // INTEGRITY CHECK HANDLERS

      protected:

        //! Check integrity for a twopf container
        void check_twopf_integrity_handler(integration_writer<number>& writer, integration_task<number>* tk);

        //! Check integrity for a threepf container
        void check_threepf_integrity_handler(integration_writer<number>& writer, integration_task<number>* tk);

        //! Check integrity for a zeta twopf container
        void check_zeta_twopf_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* tk);

        //! Check integrity for a zeta threepf container
        void check_zeta_threepf_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* tk);

        //! Check integrity for an fNL container
        void check_fNL_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* tk);


        // DATA PIPES -- implements a 'data_manager' interface

      public:

        //! Create a new datapipe
        virtual datapipe<number> create_datapipe(const boost::filesystem::path& logdir,
                                                 const boost::filesystem::path& tempdir,
                                                 typename datapipe<number>::integration_content_finder integration_finder,
                                                 typename datapipe<number>::postintegration_content_finder postintegration_finder,
                                                 typename datapipe<number>::dispatch_function dispatcher,
                                                 unsigned int worker, bool no_log = false) override;

        //! Pull a set of time sample-points from a datapipe
        virtual void pull_time_config(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_time_config_query>& tquery, std::vector<time_config>& sample) override;

        //! Pull a set of 2pf k-configuration serial numbers from a datapipe
        void pull_kconfig_twopf(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_twopf_kconfig_query>& kquery, std::vector<twopf_kconfig>& sample) override;

        //! Pull a set of 3pd k-configuration serial numbesr from a datapipe
        //! Simultaneously, populates three lists (k1, k2, k3) with serial numbers for the 2pf k-configurations
        //! corresponding to k1, k2, k3
        void pull_kconfig_threepf(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_threepf_kconfig_query>& query, std::vector<threepf_kconfig>& sample) override;

        //! Pull a time sample of a background field from a datapipe
        virtual void pull_background_time_sample(datapipe<number>* pipe, unsigned int id, const std::shared_ptr<derived_data::SQL_query>& query, std::vector<number>& sample) override;

        //! Pull a time sample of a twopf component at fixed k-configuration from a datapipe
        virtual void pull_twopf_time_sample(datapipe<number>* pipe, unsigned int id, const std::shared_ptr<derived_data::SQL_query>& query,
                                            unsigned int k_serial, std::vector<number>& sample, typename datapipe<number>::twopf_type type) override;

        //! Pull a sample of a threepf at fixed k-configuration from a datapipe
        virtual void pull_threepf_time_sample(datapipe<number>* pipe, unsigned int id, const std::shared_ptr<derived_data::SQL_query>& query,
                                              unsigned int k_serial, std::vector<number>& sample) override;

        //! Pull a sample of a tensor twopf component at fixed k-configuration from a datapipe
        virtual void pull_tensor_twopf_time_sample(datapipe<number>* pipe, unsigned int id, const std::shared_ptr<derived_data::SQL_query>& query,
                                                   unsigned int k_serial, std::vector<number>& sample) override;

        //! Pull a sample of the zeta twopf at fixed k-configuration from a datapipe
        virtual void pull_zeta_twopf_time_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                 unsigned int k_serial, std::vector<number>& sample) override;

        //! Pull a sample of the zeta threepf at fixed k-configuration from a datapipe
        virtual void pull_zeta_threepf_time_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                   unsigned int k_serial, std::vector<number>& sample) override;

        //! Pull a sample of the zeta reduced bispectrum at fixed k-configuration from a datapipe
        virtual void pull_zeta_redbsp_time_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                  unsigned int k_serial, std::vector<number>& sample) override;

        //! Pull a sample of fNL from a datapipe
        virtual void pull_fNL_time_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                          std::vector<number>& sample, derived_data::template_type type) override;

        //! Pull a sample of bispectrum.template from a datapipe
        virtual void pull_BT_time_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                          std::vector<number>& sample, derived_data::template_type type) override;

        //! Pull a sample of template.template from a datapipe
        virtual void pull_TT_time_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                          std::vector<number>& sample, derived_data::template_type type) override;

        //! Pull a kconfig sample of a twopf component at fixed time from a datapipe
        virtual void pull_twopf_kconfig_sample(datapipe<number>* pipe, unsigned int id, const std::shared_ptr<derived_data::SQL_query>& query,
                                               unsigned int t_serial, std::vector<number>& sample, typename datapipe<number>::twopf_type type) override;

        //! Pull a kconfig sample of a threepf at fixed time from a datapipe
        virtual void pull_threepf_kconfig_sample(datapipe<number>* pipe, unsigned int id,
                                                 const std::shared_ptr<derived_data::SQL_query>& query,
                                                 unsigned int t_serial, std::vector<number>& sample) override;

        //! Pull a kconfig sample of a tensor twopf component at fixed time from a datapipe
        virtual void pull_tensor_twopf_kconfig_sample(datapipe<number>* pipe, unsigned int id, const std::shared_ptr<derived_data::SQL_query>& query,
                                                      unsigned int t_serial, std::vector<number>& sample) override;

        //! Pull a kconfig sample of the zeta twopf at fixed time from a datapipe
        virtual void pull_zeta_twopf_kconfig_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                    unsigned int t_serial, std::vector<number>& sample) override;

        //! Pull a kconfig sample of the zeta threepf at fixed time from a datapipe
        virtual void pull_zeta_threepf_kconfig_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                      unsigned int t_serial, std::vector<number>& sample) override;

        //! Pull a kconfig sample of the zeta reduced bispectrum at fixed time from a datapipe
        virtual void pull_zeta_redbsp_kconfig_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                     unsigned int t_serial, std::vector<number>& sample) override;

        //! Pull a sample of k-configuration statistics from a datapipe
        virtual void pull_k_statistics_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                              std::vector<kconfiguration_statistics>& data) override;

      protected:

        //! Attach a SQLite database to a datapipe
        void datapipe_attach_container(datapipe<number>* pipe, const boost::filesystem::path& ctr_path);

        //! Attach an integration content group to a datapipe
        std::shared_ptr <output_group_record<integration_payload>>
	        datapipe_attach_integration_content(datapipe<number>* pipe, typename datapipe<number>::integration_content_finder& finder,
	                                            const std::string& name, const std::list<std::string>& tags);

        //! Attach an postintegration content group to a datapipe
        std::shared_ptr <output_group_record<postintegration_payload>>
	        datapipe_attach_postintegration_content(datapipe<number>* pipe, typename datapipe<number>::postintegration_content_finder& finder,
	                                                const std::string& name, const std::list<std::string>& tags);

        //! Detach an output_group_record from a pipe
        void datapipe_detach(datapipe<number>* pipe);


		    // INTERNAL UTILITY FUNCTIONS

      protected:

        //! Replace a temporary twopf container with a new one
        void replace_temp_twopf_container(const boost::filesystem::path& tempdir, unsigned int worker, model<number>* m,
                                          bool ics, generic_batcher* batcher, generic_batcher::replacement_action action);

        //! Replace a temporary threepf container with a new one
        void replace_temp_threepf_container(const boost::filesystem::path& tempdir, unsigned int worker, model<number>* m,
                                            bool ics, generic_batcher* batcher, generic_batcher::replacement_action action);

        //! Replace a temporary zeta twopf container with a new one
        void replace_temp_zeta_twopf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                               generic_batcher* batcher, generic_batcher::replacement_action action);

        //! Replace a temporary zeta threepf container with a new one
        void replace_temp_zeta_threepf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                                 generic_batcher* batcher, generic_batcher::replacement_action action);

        //! Replace a temporary fNL container with a new one
        void replace_temp_fNL_container(const boost::filesystem::path& tempdir, unsigned int worker, derived_data::template_type type,
                                        generic_batcher* batcher, generic_batcher::replacement_action action);

        //! Generate the name for a temporary container
        boost::filesystem::path generate_temporary_container_path(const boost::filesystem::path& tempdir, unsigned int worker);


        // INTERNAL DATA

      private:

        //! List of open sqlite3 connexions
        std::list< sqlite3* > open_containers;

        //! Serial number used to distinguish
        //! the set of temporary containers associated with this worker.
		    //! Begins at zero and is incremented as temporary containers are generated.
        unsigned int          temporary_container_serial;

      };


    // CONSTRUCTOR, DESTRUCTOR

    template <typename number>
    data_manager_sqlite3<number>::~data_manager_sqlite3()
      {
        for(std::list<sqlite3*>::iterator t = this->open_containers.begin(); t != this->open_containers.end(); ++t)
          {
            int status = sqlite3_close(*t);

            if(status != SQLITE_OK)
              {
                if(status == SQLITE_BUSY) throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, CPPTRANSPORT_DATACTR_NOT_CLOSED);
                else
                  {
                    std::ostringstream msg;
                    msg << CPPTRANSPORT_DATACTR_CLOSE << status << ")";
                    throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
                  }
              }
          }
      }


    // Create data files for a new integration_writer object
    template <typename number>
    void data_manager_sqlite3<number>::initialize_writer(std::shared_ptr< integration_writer<number> >& writer, bool recovery_mode)
      {
        sqlite3* db = nullptr;

        // get paths of the data container
        boost::filesystem::path ctr_path = writer->get_abs_container_path();

        // open the main container
        int mode = recovery_mode ? SQLITE_OPEN_READWRITE : SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        int status = sqlite3_open_v2(ctr_path.string().c_str(), &db, mode, nullptr);

        if(status != SQLITE_OK)
	        {
            std::ostringstream msg;
            if(db != nullptr)
	            {
                msg << CPPTRANSPORT_DATACTR_CREATE_A << " '" << ctr_path.string() << "' " << CPPTRANSPORT_DATACTR_CREATE_B << status << ": " << sqlite3_errmsg(db) << ")";
                sqlite3_close(db);
	            }
            else
	            {
                msg << CPPTRANSPORT_DATACTR_CREATE_A << " '" << ctr_path.string() << "' " << CPPTRANSPORT_DATACTR_CREATE_B << status << ")";
	            }
            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
	        }

        sqlite3_extended_result_codes(db, 1);

        // enable foreign key constraints
        char* errmsg;
        sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &errmsg);

        // force temporary databases to be stored in memory, for speed
        sqlite3_exec(db, "PRAGMA main.temp_store = 2;", nullptr, nullptr, &errmsg);

        // try to speed up SQLite accesses
//        sqlite3_exec(db, "PRAGMA main.page_size = 4096;", nullptr, nullptr, &errmsg)
        sqlite3_exec(db, "PRAGMA main.synchronous = 1;", nullptr, nullptr, &errmsg);
        sqlite3_exec(db, "PRAGMA main.cache_size = 10000;", nullptr, nullptr, &errmsg);

        // remember this connexion
        this->open_containers.push_back(db);
        writer->set_data_manager_handle(db);

        // set up aggregation handlers
        integration_task_record<number>* rec = writer->get_record();
        assert(rec != nullptr);

        integration_task<number>* tk = rec->get_task();

        twopf_task<number>* tk2 = nullptr;
        threepf_task<number>* tk3 = nullptr;
        if((tk2 = dynamic_cast<twopf_task<number>*>(tk)) != nullptr)
          {
            writer->set_aggregation_handler(std::bind(&data_manager_sqlite3<number>::aggregate_twopf_batch, this, std::placeholders::_1, std::placeholders::_2));
            writer->set_integrity_check_handler(std::bind(&data_manager_sqlite3<number>::check_twopf_integrity_handler, this, std::placeholders::_1, std::placeholders::_2));
          }
        else if((tk3 = dynamic_cast<threepf_task<number>*>(tk)) != nullptr)
          {
            writer->set_aggregation_handler(std::bind(&data_manager_sqlite3<number>::aggregate_threepf_batch, this, std::placeholders::_1, std::placeholders::_2));
            writer->set_integrity_check_handler(std::bind(&data_manager_sqlite3<number>::check_threepf_integrity_handler, this, std::placeholders::_1, std::placeholders::_2));
          }
        else
          {
            assert(false);
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATACTR_AGGREGATION_HANDLER_NOT_SET);
          }
      }


    // Close data files associated with an integration_writer object
    template <typename number>
    void data_manager_sqlite3<number>::close_writer(std::shared_ptr< integration_writer<number> >& writer)
      {
        // close sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer->get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // vacuum the database
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << std::endl << "** Performing routine maintenance on SQLite3 container '" << writer->get_abs_container_path().string() << "'";
        boost::timer::cpu_timer timer;
        char* errmsg;
        sqlite3_exec(db, "VACUUM;", nullptr, nullptr, &errmsg);
        timer.stop();
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "** Database vacuum complete in wallclock time " << format_time(timer.elapsed().wall);

        this->open_containers.remove(db);
        sqlite3_close(db);

        // physically remove the tempfiles directory
        boost::filesystem::remove_all(writer->get_abs_tempdir_path());
      }


		// Create data files for a new derived_content_writer object
		template <typename number>
		void data_manager_sqlite3<number>::initialize_writer(std::shared_ptr< derived_content_writer<number> >& writer, bool recovery_mode)
			{
        // set up aggregation handler
        writer->set_aggregation_handler(std::bind(&data_manager_sqlite3<number>::aggregate_derived_product, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			}


		// Close data files for a derived_content_writer object
		template <typename number>
		void data_manager_sqlite3<number>::close_writer(std::shared_ptr< derived_content_writer<number> >& writer)
			{
				// physically remove the tempfiles directory
		    boost::filesystem::remove_all(writer->get_abs_tempdir_path());
			}


    // Initialize a new postintegration_writer object
    template <typename number>
    void data_manager_sqlite3<number>::initialize_writer(std::shared_ptr< postintegration_writer<number> >& writer, bool recovery_mode)
      {
        sqlite3* db = nullptr;

        // get paths to the data container
        boost::filesystem::path ctr_path = writer->get_abs_container_path();

        // open the main container
        int mode = recovery_mode ? SQLITE_OPEN_READWRITE : SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        int status = sqlite3_open_v2(ctr_path.string().c_str(), &db, mode, nullptr);

        if(status != SQLITE_OK)
          {
            std::ostringstream msg;
            if(db != nullptr)
              {
                msg << CPPTRANSPORT_DATACTR_CREATE_A << " '" << ctr_path.string() << "' " << CPPTRANSPORT_DATACTR_CREATE_B << status << ": " << sqlite3_errmsg(db) << ")";
                sqlite3_close(db);
              }
            else
              {
                msg << CPPTRANSPORT_DATACTR_CREATE_A << " '" << ctr_path.string() << "' " << CPPTRANSPORT_DATACTR_CREATE_B << status << ")";
              }
            throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
          }

        sqlite3_extended_result_codes(db, 1);
        // leave foreign keys disabled

        // force temporary databases to be stored in memory, for speed
        char* errmsg;
        sqlite3_exec(db, "PRAGMA main.temp_store = 2;", nullptr, nullptr, &errmsg);

        // try to speed up SQLite accesses
//        sqlite3_exec(db, "PRAGMA main.page_size = 4096;", nullptr, nullptr, &errmsg)
        sqlite3_exec(db, "PRAGMA main.synchronous = 1;", nullptr, nullptr, &errmsg);
        sqlite3_exec(db, "PRAGMA main.cache_size = 10000;", nullptr, nullptr, &errmsg);

        // remember this connexion
        this->open_containers.push_back(db);
        writer->set_data_manager_handle(db);

        // set aggregation handler
        postintegration_task_record<number> * rec = writer->get_record();
        assert(rec != nullptr);

        postintegration_task<number>* tk = rec->get_task();

        zeta_twopf_task<number>* z2pf = nullptr;
        zeta_threepf_task<number>* z3pf = nullptr;
        fNL_task<number>* zfNL = nullptr;
        if((z2pf = dynamic_cast<zeta_twopf_task<number>*>(tk)) != nullptr)
          {
            writer->set_aggregation_handler(std::bind(&data_manager_sqlite3<number>::aggregate_zeta_twopf_batch, this, std::placeholders::_1, std::placeholders::_2));
            writer->set_integrity_check_handler(std::bind(&data_manager_sqlite3<number>::check_zeta_twopf_integrity_handler, this, std::placeholders::_1, std::placeholders::_2));
		        writer->get_products().add_zeta_twopf();
          }
        else if((z3pf = dynamic_cast<zeta_threepf_task<number>*>(tk)) != nullptr)
          {
            writer->set_aggregation_handler(std::bind(&data_manager_sqlite3<number>::aggregate_zeta_threepf_batch, this, std::placeholders::_1, std::placeholders::_2));
            writer->set_integrity_check_handler(std::bind(&data_manager_sqlite3<number>::check_zeta_threepf_integrity_handler, this, std::placeholders::_1, std::placeholders::_2));
		        writer->get_products().add_zeta_twopf();
		        writer->get_products().add_zeta_threepf();
		        writer->get_products().add_zeta_redbsp();
          }
        else if((zfNL = dynamic_cast<fNL_task<number>*>(tk)) != nullptr)
          {
            writer->set_aggregation_handler(std::bind(&data_manager_sqlite3<number>::aggregate_fNL_batch, this, std::placeholders::_1, std::placeholders::_2, zfNL->get_template()));
            writer->set_integrity_check_handler(std::bind(&data_manager_sqlite3<number>::check_fNL_integrity_handler, this, std::placeholders::_1, std::placeholders::_2));
		        switch(zfNL->get_template())
			        {
		            case derived_data::fNL_local_template:
			            writer->get_products().add_fNL_local();
				          break;

		            case derived_data::fNL_equi_template:
			            writer->get_products().add_fNL_equi();
				          break;

		            case derived_data::fNL_ortho_template:
			            writer->get_products().add_fNL_ortho();
				          break;

		            case derived_data::fNL_DBI_template:
			            writer->get_products().add_fNL_DBI();
				          break;
			        }
          }
        else
          {
            assert(false);
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATACTR_AGGREGATION_HANDLER_NOT_SET);
          }
      }


    // Close a postintegration_writer object
    template <typename number>
    void data_manager_sqlite3<number>::close_writer(std::shared_ptr< postintegration_writer<number> >& writer)
      {
        // close sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer->get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        // vacuum the database
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << std::endl << "** Performing routine maintenance on SQLite3 container '" << writer->get_abs_container_path().string() << "'";
        boost::timer::cpu_timer timer;
        char* errmsg;
        sqlite3_exec(db, "VACUUM;", nullptr, nullptr, &errmsg);
        timer.stop();
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "** Database vacuum complete in wallclock time " << format_time(timer.elapsed().wall);

        this->open_containers.remove(db);
        sqlite3_close(db);

        // physically remove the tempfiles directory
        boost::filesystem::remove_all(writer->get_abs_tempdir_path());
      }


    // INDEX TABLE MANAGEMENT

    template <typename number>
    void data_manager_sqlite3<number>::create_tables(std::shared_ptr< integration_writer<number> >& writer, twopf_task<number>* tk)
      {
        sqlite3* db = nullptr;
        writer->get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        unsigned int Nfields = tk->get_model()->get_N_fields();

        sqlite3_operations::create_time_sample_table(db, tk);
        sqlite3_operations::create_twopf_sample_table(db, tk);
        sqlite3_operations::create_backg_table(db, Nfields, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_twopf_table<number, typename integration_items<number>::twopf_re_item>(db, Nfields, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_tensor_twopf_table(db, sqlite3_operations::foreign_keys);

        sqlite3_operations::create_worker_info_table(db, sqlite3_operations::foreign_keys);
        if(writer->is_collecting_statistics()) sqlite3_operations::create_stats_table(db, sqlite3_operations::foreign_keys, sqlite3_operations::twopf_configs);

		    if(writer->is_collecting_initial_conditions()) sqlite3_operations::create_ics_table<number, typename integration_items<number>::ics_item>(db, Nfields, sqlite3_operations::foreign_keys, sqlite3_operations::twopf_configs);
      }


    template <typename number>
    void data_manager_sqlite3<number>::create_tables(std::shared_ptr< integration_writer<number> >& writer, threepf_task<number>* tk)
      {
        sqlite3* db = nullptr;
        writer->get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        unsigned int Nfields = tk->get_model()->get_N_fields();

        sqlite3_operations::create_time_sample_table(db, tk);
        sqlite3_operations::create_twopf_sample_table(db, tk);
        sqlite3_operations::create_threepf_sample_table(db, tk);
        sqlite3_operations::create_backg_table(db, Nfields, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_twopf_table<number, typename integration_items<number>::twopf_re_item>(db, Nfields, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_twopf_table<number, typename integration_items<number>::twopf_im_item>(db, Nfields, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_tensor_twopf_table(db, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_threepf_table(db, Nfields, sqlite3_operations::foreign_keys);

        sqlite3_operations::create_worker_info_table(db, sqlite3_operations::foreign_keys);
        if(writer->is_collecting_statistics()) sqlite3_operations::create_stats_table(db, sqlite3_operations::foreign_keys, sqlite3_operations::threepf_configs);

        if(writer->is_collecting_initial_conditions())
	        {
            sqlite3_operations::create_ics_table<number, typename integration_items<number>::ics_item>(db, Nfields, sqlite3_operations::foreign_keys, sqlite3_operations::threepf_configs);
            sqlite3_operations::create_ics_table<number, typename integration_items<number>::ics_kt_item>(db, Nfields, sqlite3_operations::foreign_keys, sqlite3_operations::threepf_configs);
	        }
      }


    template <typename number>
    void data_manager_sqlite3<number>::create_tables(std::shared_ptr< postintegration_writer<number> >& writer, zeta_twopf_task<number>* tk)
      {
        sqlite3* db = nullptr;
        writer->get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::create_time_sample_table(db, tk);
        sqlite3_operations::create_twopf_sample_table(db, tk);
        sqlite3_operations::create_zeta_twopf_table(db, sqlite3_operations::foreign_keys);
      }


    template <typename number>
    void data_manager_sqlite3<number>::create_tables(std::shared_ptr< postintegration_writer<number> >& writer, zeta_threepf_task<number>* tk)
      {
        sqlite3* db = nullptr;
        writer->get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::create_time_sample_table(db, tk);
        sqlite3_operations::create_twopf_sample_table(db, tk);
        sqlite3_operations::create_threepf_sample_table(db, tk);
        sqlite3_operations::create_zeta_twopf_table(db, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_zeta_threepf_table(db, sqlite3_operations::foreign_keys);
        sqlite3_operations::create_zeta_reduced_bispectrum_table(db, sqlite3_operations::foreign_keys);
      }


    template <typename number>
    void data_manager_sqlite3<number>::create_tables(std::shared_ptr< postintegration_writer<number> >& writer, fNL_task<number>* tk)
      {
        sqlite3* db = nullptr;
        writer->get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::create_time_sample_table(db, tk);
        sqlite3_operations::create_fNL_table(db, tk->get_template(), sqlite3_operations::foreign_keys);
      }


    // SEEDING


    template <typename number>
    void data_manager_sqlite3<number>::seed_writer(std::shared_ptr< integration_writer<number> >& writer, twopf_task<number>* tk,
                                                   const std::shared_ptr< output_group_record<integration_payload> >& seed)
      {
        sqlite3* db = nullptr;
        writer->get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "** Seeding SQLite3 container '" << writer->get_abs_container_path().string() << "' "
          << "from previous content group '" << seed->get_name() << "', container '" << seed->get_abs_output_path().string() << "'";

        boost::timer::cpu_timer timer;
        boost::filesystem::path seed_container_path = seed->get_abs_repo_path() / seed->get_payload().get_container_path();

        sqlite3_operations::aggregate_backg<number>(db, *writer, seed_container_path.string());
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_re_item>(db, *writer, seed_container_path.string());
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::tensor_twopf_item>(db, *writer, seed_container_path.string());

        sqlite3_operations::aggregate_workers<number>(db, *writer, seed_container_path.string());
        if(writer->is_collecting_statistics() && seed->get_payload().has_statistics())
	        sqlite3_operations::aggregate_statistics<number>(db, *writer, seed_container_path.string());

        if(writer->is_collecting_initial_conditions() && seed->get_payload().has_initial_conditions())
	        sqlite3_operations::aggregate_ics<number, typename integration_items<number>::ics_item>(db, *writer, seed_container_path.string());

        timer.stop();
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "** Seeding complete in time " << format_time(timer.elapsed().wall);
      }


    template <typename number>
    void data_manager_sqlite3<number>::seed_writer(std::shared_ptr< integration_writer<number> >& writer, threepf_task<number>* tk,
                                                   const std::shared_ptr< output_group_record<integration_payload> >& seed)
      {
        sqlite3* db = nullptr;
        writer->get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "** Seeding SQLite3 container '" << writer->get_abs_container_path().string() << "' "
            << "from previous content group '" << seed->get_name() << "', container '" << seed->get_abs_output_path().string() << "'";

        boost::timer::cpu_timer timer;
        boost::filesystem::path seed_container_path = seed->get_abs_repo_path() / seed->get_payload().get_container_path();

        sqlite3_operations::aggregate_backg<number>(db, *writer, seed_container_path.string());
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_re_item>(db, *writer, seed_container_path.string());
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_im_item>(db, *writer, seed_container_path.string());
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::tensor_twopf_item>(db, *writer, seed_container_path.string());
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::threepf_item>(db, *writer, seed_container_path.string());

        sqlite3_operations::aggregate_workers<number>(db, *writer, seed_container_path.string());
        if(writer->is_collecting_statistics() && seed->get_payload().has_statistics()) sqlite3_operations::aggregate_statistics<number>(db, *writer, seed_container_path.string());

        if(writer->is_collecting_initial_conditions() && seed->get_payload().has_initial_conditions())
	        {
            sqlite3_operations::aggregate_ics<number, typename integration_items<number>::ics_item>(db, *writer, seed_container_path.string());
            sqlite3_operations::aggregate_ics<number, typename integration_items<number>::ics_kt_item>(db, *writer, seed_container_path.string());
	        }

        timer.stop();
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "** Seeding complete in time " << format_time(timer.elapsed().wall);
      }


    template <typename number>
    void data_manager_sqlite3<number>::seed_writer(std::shared_ptr< postintegration_writer<number> >& writer, zeta_twopf_task<number>* tk,
                                                   const std::shared_ptr< output_group_record<postintegration_payload> >& seed)
      {
        sqlite3* db = nullptr;
        writer->get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "** Seeding SQLite3 container '" << writer->get_abs_container_path().string() << "' "
            << "from previous content group '" << seed->get_name() << "', container '" << seed->get_abs_output_path().string() << "'";

        boost::timer::cpu_timer timer;
        boost::filesystem::path seed_container_path = seed->get_abs_repo_path() / seed->get_payload().get_container_path();

        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::zeta_twopf_item>(db, *writer, seed_container_path.string());

        timer.stop();
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "** Seeding complete in time " << format_time(timer.elapsed().wall);
      }


    template <typename number>
    void data_manager_sqlite3<number>::seed_writer(std::shared_ptr< postintegration_writer<number> >& writer, zeta_threepf_task<number>* tk,
                                                   const std::shared_ptr< output_group_record<postintegration_payload> >& seed)
      {
        sqlite3* db = nullptr;
        writer->get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "** Seeding SQLite3 container '" << writer->get_abs_container_path().string() << "' "
            << "from previous content group '" << seed->get_name() << "', container '" << seed->get_abs_output_path().string() << "'";

        boost::timer::cpu_timer timer;
        boost::filesystem::path seed_container_path = seed->get_abs_repo_path() / seed->get_payload().get_container_path();

        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::zeta_twopf_item>(db, *writer, seed_container_path.string());
        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::zeta_threepf_item>(db, *writer, seed_container_path.string());
        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::zeta_redbsp_item>(db, *writer, seed_container_path.string());

        timer.stop();
        BOOST_LOG_SEV(writer->get_log(), base_writer::normal) << "** Seeding complete in time " << format_time(timer.elapsed().wall);
      }


    template <typename number>
    void data_manager_sqlite3<number>::seed_writer(std::shared_ptr< postintegration_writer<number> >& writer, fNL_task<number>* tk,
                                                   const std::shared_ptr< output_group_record<postintegration_payload> >& seed)
      {
        assert(false);
      }


    // BATCHERS


    template <typename number>
    twopf_batcher<number>
    data_manager_sqlite3<number>::create_temp_twopf_container(twopf_task<number>* tk, const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                              unsigned int worker, unsigned int group,
                                                              model<number>* m, generic_batcher::container_dispatch_function dispatcher)
      {
        boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

        sqlite3* db = sqlite3_operations::create_temp_twopf_container<number>(container, m->get_N_fields(), m->supports_per_configuration_statistics(), tk->get_collect_initial_conditions());

        // set up writers
        typename twopf_batcher<number>::writer_group writers;
        writers.host_info    = std::bind(&sqlite3_operations::write_host_info<number>, std::placeholders::_1);
        writers.stats        = std::bind(&sqlite3_operations::write_stats<number>, std::placeholders::_1, std::placeholders::_2);
		    writers.ics          = std::bind(&sqlite3_operations::write_coordinate_output<number, typename integration_items<number>::ics_item>, std::placeholders::_1, std::placeholders::_2);
        writers.backg        = std::bind(&sqlite3_operations::write_coordinate_output<number, typename integration_items<number>::backg_item>, std::placeholders::_1, std::placeholders::_2);
        writers.twopf        = std::bind(&sqlite3_operations::write_paged_output<number, integration_batcher<number>, typename integration_items<number>::twopf_re_item>, std::placeholders::_1, std::placeholders::_2);
        writers.tensor_twopf = std::bind(&sqlite3_operations::write_paged_output<number, integration_batcher<number>, typename integration_items<number>::tensor_twopf_item>, std::placeholders::_1, std::placeholders::_2);

        // set up a replacement function
        generic_batcher::container_replacement_function replacer =
	                                                        std::bind(&data_manager_sqlite3<number>::replace_temp_twopf_container,
	                                                                  this, tempdir, worker, m, tk->get_collect_initial_conditions(), std::placeholders::_1, std::placeholders::_2);

        // set up batcher
        twopf_batcher<number> batcher(this->batcher_capacity, this->checkpoint_interval, m, tk, container, logdir, writers, dispatcher, replacer, db, worker, group);

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal) << "** Created new temporary twopf container " << container;

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }


    template <typename number>
    threepf_batcher<number>
    data_manager_sqlite3<number>::create_temp_threepf_container(threepf_task<number>* tk, const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                                unsigned int worker, unsigned int group,
                                                                model<number>* m, generic_batcher::container_dispatch_function dispatcher)
      {
        boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

        sqlite3* db = sqlite3_operations::create_temp_threepf_container<number>(container, m->get_N_fields(), m->supports_per_configuration_statistics(), tk->get_collect_initial_conditions());

        // set up writers
        typename threepf_batcher<number>::writer_group writers;
		    writers.host_info    = std::bind(&sqlite3_operations::write_host_info<number>, std::placeholders::_1);
        writers.stats        = std::bind(&sqlite3_operations::write_stats<number>, std::placeholders::_1, std::placeholders::_2);
        writers.ics          = std::bind(&sqlite3_operations::write_coordinate_output<number, typename integration_items<number>::ics_item>, std::placeholders::_1, std::placeholders::_2);
        writers.kt_ics       = std::bind(&sqlite3_operations::write_coordinate_output<number, typename integration_items<number>::ics_kt_item>, std::placeholders::_1, std::placeholders::_2);
        writers.backg        = std::bind(&sqlite3_operations::write_coordinate_output<number, typename integration_items<number>::backg_item>, std::placeholders::_1, std::placeholders::_2);
        writers.twopf_re     = std::bind(&sqlite3_operations::write_paged_output<number, integration_batcher<number>, typename integration_items<number>::twopf_re_item>, std::placeholders::_1, std::placeholders::_2);
        writers.twopf_im     = std::bind(&sqlite3_operations::write_paged_output<number, integration_batcher<number>, typename integration_items<number>::twopf_im_item>, std::placeholders::_1, std::placeholders::_2);
        writers.tensor_twopf = std::bind(&sqlite3_operations::write_paged_output<number, integration_batcher<number>, typename integration_items<number>::tensor_twopf_item>, std::placeholders::_1, std::placeholders::_2);
        writers.threepf      = std::bind(&sqlite3_operations::write_paged_output<number, integration_batcher<number>, typename integration_items<number>::threepf_item>, std::placeholders::_1, std::placeholders::_2);

        // set up a replacement function
        generic_batcher::container_replacement_function replacer =
	                                                        std::bind(&data_manager_sqlite3<number>::replace_temp_threepf_container,
	                                                                  this, tempdir, worker, m, tk->get_collect_initial_conditions(), std::placeholders::_1, std::placeholders::_2);

        // set up batcher
        threepf_batcher<number> batcher(this->batcher_capacity, this->checkpoint_interval, m, tk, container, logdir, writers, dispatcher, replacer, db, worker, group);
        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal) << "** Created new temporary threepf container " << container;

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }


    template <typename number>
    zeta_twopf_batcher<number>
    data_manager_sqlite3<number>::create_temp_zeta_twopf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir, unsigned int worker,
                                                                   generic_batcher::container_dispatch_function dispatcher)
      {
        boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

        sqlite3* db = sqlite3_operations::create_temp_zeta_twopf_container(container);

        // set up writers
        typename zeta_twopf_batcher<number>::writer_group writers;
        writers.twopf = std::bind(&sqlite3_operations::write_unpaged<number, postintegration_batcher, typename postintegration_items<number>::zeta_twopf_item>, std::placeholders::_1, std::placeholders::_2);

        // set up replacement function
        generic_batcher::container_replacement_function replacer =
	                                                        std::bind(&data_manager_sqlite3<number>::replace_temp_zeta_twopf_container,
	                                                                  this, tempdir, worker, std::placeholders::_1, std::placeholders::_2);

        // set up batcher
        zeta_twopf_batcher<number> batcher(this->batcher_capacity, this->checkpoint_interval, container, logdir, writers, dispatcher, replacer, db, worker);

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal) << "** Created new temporary zeta twopf container " << container;

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }


    template <typename number>
    zeta_threepf_batcher<number>
    data_manager_sqlite3<number>::create_temp_zeta_threepf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir, unsigned int worker,
                                                                     generic_batcher::container_dispatch_function dispatcher)
      {
        boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

        sqlite3* db = sqlite3_operations::create_temp_zeta_threepf_container(container);

        // set up writers
        typename zeta_threepf_batcher<number>::writer_group writers;
        writers.twopf   = std::bind(&sqlite3_operations::write_unpaged<number, postintegration_batcher, typename postintegration_items<number>::zeta_twopf_item>, std::placeholders::_1, std::placeholders::_2);
        writers.threepf = std::bind(&sqlite3_operations::write_unpaged<number, postintegration_batcher, typename postintegration_items<number>::zeta_threepf_item>, std::placeholders::_1, std::placeholders::_2);
        writers.redbsp  = std::bind(&sqlite3_operations::write_unpaged<number, postintegration_batcher, typename postintegration_items<number>::zeta_redbsp_item>, std::placeholders::_1, std::placeholders::_2);

        // set up replacement function
        generic_batcher::container_replacement_function replacer =
	                                                        std::bind(&data_manager_sqlite3<number>::replace_temp_zeta_threepf_container,
	                                                                  this, tempdir, worker, std::placeholders::_1, std::placeholders::_2);

        // set up batcher
        zeta_threepf_batcher<number> batcher(this->batcher_capacity, this->checkpoint_interval, container, logdir, writers, dispatcher, replacer, db, worker);

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal) << "** Created new temporary zeta threepf container " << container;

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }


    template <typename number>
    fNL_batcher<number>
    data_manager_sqlite3<number>::create_temp_fNL_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir, unsigned int worker,
                                                            generic_batcher::container_dispatch_function dispatcher, derived_data::template_type type)
      {
        boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

        sqlite3* db = sqlite3_operations::create_temp_fNL_container(container, type);

        // set up writers
        typename fNL_batcher<number>::writer_group writers;
        writers.fNL = std::bind(&sqlite3_operations::write_fNL<number>, std::placeholders::_1, std::placeholders::_2, type);

        // set up replacement function
        generic_batcher::container_replacement_function replacer =
	                                                        std::bind(&data_manager_sqlite3<number>::replace_temp_fNL_container,
	                                                                  this, tempdir, worker, type, std::placeholders::_1, std::placeholders::_2);

        // set up batcher
        fNL_batcher<number> batcher(this->batcher_capacity, this->checkpoint_interval, container, logdir, writers, dispatcher, replacer, db, worker, type);

        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal) << "** Created new temporary " << derived_data::template_name(type) << " container " << container;

        // add this database to our list of open connections
        this->open_containers.push_back(db);

        return(batcher);
      }


    template <typename number>
    void data_manager_sqlite3<number>::replace_temp_twopf_container(const boost::filesystem::path& tempdir, unsigned int worker, model<number>* m,
                                                                    bool ics, generic_batcher* batcher, generic_batcher::replacement_action action)
      {
        sqlite3* db = nullptr;

        BOOST_LOG_SEV(batcher->get_log(), generic_batcher::normal)
            << "** " << (action == generic_batcher::action_replace ? "Replacing" : "Closing")
            << " temporary twopf container " << batcher->get_container_path();

        batcher->get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        if(action == generic_batcher::action_replace)
          {
            boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

            sqlite3* new_db = sqlite3_operations::create_temp_twopf_container<number>(container, m->get_N_fields(), m->supports_per_configuration_statistics(), ics);

            batcher->set_container_path(container);
            batcher->set_manager_handle(new_db);

            this->open_containers.push_back(new_db);
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::replace_temp_threepf_container(const boost::filesystem::path& tempdir, unsigned int worker, model<number>* m,
                                                                      bool ics, generic_batcher* batcher, generic_batcher::replacement_action action)
      {
        sqlite3* db = nullptr;

        BOOST_LOG_SEV(batcher->get_log(), generic_batcher::normal)
            << "** " << (action == generic_batcher::action_replace ? "Replacing" : "Closing")
            << " temporary threepf container " << batcher->get_container_path();

        batcher->get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        BOOST_LOG_SEV(batcher->get_log(), generic_batcher::normal) << "** Closed SQLite3 handle for " << batcher->get_container_path();

        if(action == generic_batcher::action_replace)
          {
            boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

            BOOST_LOG_SEV(batcher->get_log(), generic_batcher::normal) << "** Opening new threepf container " << container;

            sqlite3* new_db = sqlite3_operations::create_temp_threepf_container<number>(container, m->get_N_fields(), m->supports_per_configuration_statistics(), ics);

            batcher->set_container_path(container);
            batcher->set_manager_handle(new_db);

            this->open_containers.push_back(new_db);
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::replace_temp_zeta_twopf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                                                         generic_batcher* batcher, generic_batcher::replacement_action action)
      {
        sqlite3* db = nullptr;

        BOOST_LOG_SEV(batcher->get_log(), generic_batcher::normal)
            << "** " << (action == generic_batcher::action_replace ? "Replacing" : "Closing")
            << " temporary zeta twopf container " << batcher->get_container_path();

        batcher->get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        if(action == generic_batcher::action_replace)
          {
            boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

            sqlite3* new_db = sqlite3_operations::create_temp_zeta_twopf_container(container);

            batcher->set_container_path(container);
            batcher->set_manager_handle(new_db);

            this->open_containers.push_back(new_db);
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::replace_temp_zeta_threepf_container(const boost::filesystem::path& tempdir, unsigned int worker,
                                                                           generic_batcher* batcher, generic_batcher::replacement_action action)
      {
        sqlite3* db = nullptr;

        BOOST_LOG_SEV(batcher->get_log(), generic_batcher::normal)
            << "** " << (action == generic_batcher::action_replace ? "Replacing" : "Closing")
            << " temporary zeta threepf container " << batcher->get_container_path();

        batcher->get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        if(action == generic_batcher::action_replace)
          {
            boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

            sqlite3* new_db = sqlite3_operations::create_temp_zeta_threepf_container(container);

            batcher->set_container_path(container);
            batcher->set_manager_handle(new_db);

            this->open_containers.push_back(new_db);
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::replace_temp_fNL_container(const boost::filesystem::path& tempdir, unsigned int worker, derived_data::template_type type,
                                                                  generic_batcher* batcher, generic_batcher::replacement_action action)
      {
        sqlite3* db = nullptr;

        BOOST_LOG_SEV(batcher->get_log(), generic_batcher::normal)
            << "** " << (action == generic_batcher::action_replace ? "Replacing" : "Closing")
            << " temporary " << derived_data::template_name(type) << " container " << batcher->get_container_path();

        batcher->get_manager_handle(&db);
        this->open_containers.remove(db);
        sqlite3_close(db);

        if(action == generic_batcher::action_replace)
          {
            boost::filesystem::path container = this->generate_temporary_container_path(tempdir, worker);

            sqlite3* new_db = sqlite3_operations::create_temp_fNL_container(container, type);

            batcher->set_container_path(container);
            batcher->set_manager_handle(new_db);

            this->open_containers.push_back(new_db);
          }
      }


    template <typename number>
    boost::filesystem::path data_manager_sqlite3<number>::generate_temporary_container_path(const boost::filesystem::path& tempdir, unsigned int worker)
      {
        std::ostringstream container_name;
        container_name << CPPTRANSPORT_TEMPORARY_CONTAINER_STEM << worker << "_" << this->temporary_container_serial++ << CPPTRANSPORT_TEMPORARY_CONTAINER_XTN;

        boost::filesystem::path container = tempdir / container_name.str();

        return(container);
      }


    template <typename number>
    bool data_manager_sqlite3<number>::aggregate_twopf_batch(integration_writer<number>& writer, const std::string& temp_ctr)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::aggregate_backg<number>(db, writer, temp_ctr);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_re_item>(db, writer, temp_ctr);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::tensor_twopf_item>(db, writer, temp_ctr);

        sqlite3_operations::aggregate_workers<number>(db, writer, temp_ctr);
        if(writer.is_collecting_statistics()) sqlite3_operations::aggregate_statistics<number>(db, writer, temp_ctr);

		    if(writer.is_collecting_initial_conditions())
			    sqlite3_operations::aggregate_ics<number, typename integration_items<number>::ics_item>(db, writer, temp_ctr);

        return(true);
      }


    template <typename number>
    bool data_manager_sqlite3<number>::aggregate_threepf_batch(integration_writer<number>& writer, const std::string& temp_ctr)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::aggregate_backg<number>(db, writer, temp_ctr);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_re_item>(db, writer, temp_ctr);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::twopf_im_item>(db, writer, temp_ctr);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::tensor_twopf_item>(db, writer, temp_ctr);
        sqlite3_operations::aggregate_table<number, integration_writer<number>, typename integration_items<number>::threepf_item>(db, writer, temp_ctr);

        sqlite3_operations::aggregate_workers<number>(db, writer, temp_ctr);
        if(writer.is_collecting_statistics()) sqlite3_operations::aggregate_statistics<number>(db, writer, temp_ctr);

        if(writer.is_collecting_initial_conditions())
	        {
            sqlite3_operations::aggregate_ics<number, typename integration_items<number>::ics_item>(db, writer, temp_ctr);
            sqlite3_operations::aggregate_ics<number, typename integration_items<number>::ics_kt_item>(db, writer, temp_ctr);
	        }

        return(true);
      }


    template <typename number>
    bool data_manager_sqlite3<number>::aggregate_zeta_twopf_batch(postintegration_writer<number>& writer, const std::string& temp_ctr)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::zeta_twopf_item>(db, writer, temp_ctr);

        return(true);
      }


    template <typename number>
    bool data_manager_sqlite3<number>::aggregate_zeta_threepf_batch(postintegration_writer<number>& writer, const std::string& temp_ctr)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::zeta_twopf_item>(db, writer, temp_ctr);
        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::zeta_threepf_item>(db, writer, temp_ctr);
        sqlite3_operations::aggregate_table<number, postintegration_writer<number>, typename postintegration_items<number>::zeta_redbsp_item>(db, writer, temp_ctr);

        return(true);
      }


    template <typename number>
    bool data_manager_sqlite3<number>::aggregate_fNL_batch(postintegration_writer<number>& writer, const std::string& temp_ctr, derived_data::template_type type)
      {
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        sqlite3_operations::aggregate_fNL<number>(db, writer, temp_ctr, type);

        return(true);
      }


    template <typename number>
    bool data_manager_sqlite3<number>::aggregate_derived_product(derived_content_writer<number>& writer,
                                                                 const std::string& temp_name, const std::list<std::string>& used_groups)
      {
        bool success = true;

        // lookup derived product from output task
        output_task_record<number>* rec = writer.get_record();
        assert(rec != nullptr);

        output_task<number>* tk = rec->get_task();
        assert(tk != nullptr);

        derived_data::derived_product<number>* product = tk->lookup_derived_product(temp_name);

        if(product == nullptr)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::error) << "!! Failed to lookup derived product '" << temp_name << "'; skipping this product";
            return(false);
          }

        // find output product in the temporary folder
        boost::filesystem::path temp_location = writer.get_abs_tempdir_path() / product->get_filename();
        boost::filesystem::path dest_location = writer.get_abs_output_path() / product->get_filename();

        if(!boost::filesystem::exists(temp_location))
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::error) << "!! Derived product " << temp_location << " missing; skipping this product";
            return(false);
          }

        if(boost::filesystem::exists(dest_location))
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::error) << "!! Destination " << dest_location << " for derived product " << temp_location << " already exists; skipping this product";
            return(false);
          }

        boost::filesystem::rename(temp_location, dest_location);

        BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "** Emplaced derived product " << dest_location;

        // commit this product to the current output group
        writer.push_content(*product, used_groups);

        return(success);
      }


    // INTEGRITY CHECK


    template <typename number>
    void data_manager_sqlite3<number>::check_twopf_integrity_handler(integration_writer<number>& writer, integration_task<number>* itk)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        twopf_task<number>* tk = dynamic_cast< twopf_task<number>* >(itk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << std::endl << "** Performing integrity check for container '" << writer.get_abs_container_path().string() << "'";

        std::list<unsigned int> serials = sqlite3_operations::get_missing_serials<number, typename integration_items<number>::twopf_re_item>(db);

        if(serials.size() > 0)
          {
            std::list<unsigned int> remainder = this->advise_missing_content(writer, serials, tk->get_twopf_database());

            if(remainder.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "** Dropping extra configurations not missing from container, but advised by backend:";
                sqlite3_operations::drop_k_configurations(db, writer, remainder, tk->get_twopf_database(),
                                                          sqlite3_operations::data_traits<number, typename integration_items<number>::twopf_re_item>::sqlite_table());
                serials.merge(remainder);
              }

		        // push list of missing serial numbers to writer and mark as a fail
		        writer.set_missing_serials(serials);
            writer.set_fail(true);

            if(writer.is_collecting_statistics()) sqlite3_operations::drop_statistics(db, serials, tk->get_twopf_database());
		        if(writer.is_collecting_initial_conditions()) sqlite3_operations::drop_ics<number, typename integration_items<number>::ics_item, twopf_kconfig_database>(db, serials, tk->get_twopf_database());
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::check_threepf_integrity_handler(integration_writer<number>& writer, integration_task<number>* itk)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        threepf_task<number>* tk = dynamic_cast< threepf_task<number>* >(itk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << std::endl << "** Performing integrity check for container '" << writer.get_abs_container_path().string() << "'";

        // get lists of missing serial numbers for threepf, real twopf and imaginary twopf
        std::list<unsigned int> twopf_re_serials = sqlite3_operations::get_missing_serials<number, typename integration_items<number>::twopf_re_item>(db);
        std::list<unsigned int> twopf_im_serials = sqlite3_operations::get_missing_serials<number, typename integration_items<number>::twopf_im_item>(db);
        std::list<unsigned int> threepf_serials  = sqlite3_operations::get_missing_serials<number, typename integration_items<number>::threepf_item>(db);

        // merge missing twopf lists into a single one
        std::list<unsigned int> twopf_total_serials = twopf_re_serials;
        std::list<unsigned int> temp = twopf_im_serials;
        twopf_total_serials.merge(temp);
        twopf_total_serials.unique();

        // map missing twopf serials into threepf serials
        std::list<unsigned int> twopf_to_threepf_map = this->map_twopf_to_threepf_serials(twopf_total_serials, tk->get_threepf_database());

        // advise missing threepf serials
        if(threepf_serials.size() > 0)
          {
            std::list<unsigned int> remainder = this->advise_missing_content(writer, threepf_serials, tk->get_threepf_database());

            if(remainder.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << std::endl << "** Dropping extra threepf configurations not missing from container, but advised by backend:";
                sqlite3_operations::drop_k_configurations(db, writer, remainder, tk->get_threepf_database(),
                                                          sqlite3_operations::data_traits<number, typename integration_items<number>::threepf_item>::sqlite_table());
                threepf_serials.merge(remainder);   // not necessary to remove duplicates, since there should not be any; result is sorted
              }
          }

        // check if any twopf configurations require dropping even more threepfs
        std::list<unsigned int> undropped;
        std::set_difference(twopf_to_threepf_map.begin(), twopf_to_threepf_map.end(),
                            threepf_serials.begin(), threepf_serials.end(), std::back_inserter(undropped));

        if(undropped.size() > 0)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "** Dropping extra threepf configurations not missing from container, but implied by missing twopf configurations:";
            sqlite3_operations::drop_k_configurations(db, writer, undropped, tk->get_threepf_database(),
                                                      sqlite3_operations::data_traits<number, typename integration_items<number>::threepf_item>::sqlite_table());
            threepf_serials.merge(undropped);   // not necessary to remove duplicates, since there should be any; result is sorted
          }

        if(threepf_serials.size() > 0)
          {
            writer.set_missing_serials(threepf_serials);
            writer.set_fail(true);

            if(writer.is_collecting_statistics()) sqlite3_operations::drop_statistics(db, threepf_serials, tk->get_threepf_database());
		        if(writer.is_collecting_initial_conditions())
			        {
		            sqlite3_operations::drop_ics<number, typename integration_items<number>::ics_item, threepf_kconfig_database>(db, threepf_serials, tk->get_threepf_database());
		            sqlite3_operations::drop_ics<number, typename integration_items<number>::ics_kt_item, threepf_kconfig_database>(db, threepf_serials, tk->get_threepf_database());
			        }

            // build list of twopf configurations which should be dropped for this entire set of threepf configurations
            std::list<unsigned int> twopf_drop = this->compute_twopf_drop_list(threepf_serials, tk->get_threepf_database());

            // compute real twopf configurations which should be dropped.
            undropped.clear();
            std::set_difference(twopf_drop.begin(), twopf_drop.end(),
                                twopf_re_serials.begin(), twopf_re_serials.end(), std::back_inserter(undropped));

            if(undropped.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << std::endl << "** Dropping real twopf configurations entailed by these threepf configurations, but present in the container";
                sqlite3_operations::drop_k_configurations(db, writer, undropped, tk->get_twopf_database(),
                                                          sqlite3_operations::data_traits<number, typename integration_items<number>::twopf_re_item>::sqlite_table());
              }

            // compute imaginary twopf configurations which should be dropped
            undropped.clear();
            std::set_difference(twopf_drop.begin(), twopf_drop.end(),
                                twopf_im_serials.begin(), twopf_im_serials.end(), std::back_inserter(undropped));

            if(undropped.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << std::endl << "** Dropping real twopf configurations entailed by these threepf configurations, but present in the container";
                sqlite3_operations::drop_k_configurations(db, writer, undropped, tk->get_twopf_database(),
                                                          sqlite3_operations::data_traits<number, typename integration_items<number>::twopf_im_item>::sqlite_table());
              }
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::check_zeta_twopf_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* ptk)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        zeta_twopf_task<number>* tk = dynamic_cast< zeta_twopf_task<number>* >(ptk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << std::endl << "** Performing integrity check for container '" << writer.get_abs_container_path().string() << "'";

        std::list<unsigned int> serials = sqlite3_operations::get_missing_serials<number, typename postintegration_items<number>::zeta_twopf_item>(db);

        if(serials.size() > 0)
          {
            std::list<unsigned int> remainder = this->advise_missing_content(writer, serials, tk->get_twopf_database());

            if(remainder.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << std::endl << "** Dropping extra configurations not missing from container, but advised by backend:";
                sqlite3_operations::drop_k_configurations(db, writer, remainder, tk->get_twopf_database(),
                                                          sqlite3_operations::data_traits<number, typename postintegration_items<number>::zeta_twopf_item>::sqlite_table());
              }

            // push list of missing serial numbers to writer
            std::list<unsigned int> merged_missing = serials;
            merged_missing.merge(remainder);

            writer.set_missing_serials(merged_missing);
            writer.set_fail(true);
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::check_zeta_threepf_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* ptk)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        zeta_threepf_task<number>* tk = dynamic_cast< zeta_threepf_task<number>* >(ptk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << std::endl << "** Performing integrity check for container '" << writer.get_abs_container_path().string() << "'";

        // get lists of missing serial numbers for threepf, redbsp and twopf
        std::list<unsigned int> threepf_serials = sqlite3_operations::get_missing_serials<number, typename postintegration_items<number>::zeta_threepf_item>(db);
        std::list<unsigned int> redbsp_serials  = sqlite3_operations::get_missing_serials<number, typename postintegration_items<number>::zeta_redbsp_item>(db);
        std::list<unsigned int> twopf_serials   = sqlite3_operations::get_missing_serials<number, typename postintegration_items<number>::zeta_twopf_item>(db);

        // map missing twopf serials into threepf serials
        std::list<unsigned int> twopf_to_threepf_map = this->map_twopf_to_threepf_serials(twopf_serials, tk->get_threepf_database());

        // merge threepf and redbsp serials into a single list
        std::list<unsigned int> threepf_total_serials = threepf_serials;
        std::list<unsigned int> temp = redbsp_serials;
        threepf_serials.merge(temp);

        // drop any threepf configurations which are present but have no matching redbsp configuraitons, and vice versa
        std::list<unsigned int> undropped;
        std::set_difference(threepf_total_serials.begin(), threepf_total_serials.end(),
                            threepf_serials.begin(), threepf_serials.end(), std::back_inserter(undropped));
        if(undropped.size() > 0)
          {
            sqlite3_operations::drop_k_configurations(db, writer, undropped, tk->get_threepf_database(),
                                                      sqlite3_operations::data_traits<number, typename postintegration_items<number>::zeta_threepf_item>::sqlite_table(), true);
          }

        undropped.clear();
        std::set_difference(threepf_total_serials.begin(), threepf_total_serials.end(),
                            redbsp_serials.begin(), redbsp_serials.end(), std::back_inserter(undropped));
        if(undropped.size() > 0)
          {
            sqlite3_operations::drop_k_configurations(db, writer, undropped, tk->get_threepf_database(),
                                                      sqlite3_operations::data_traits<number, typename postintegration_items<number>::zeta_redbsp_item>::sqlite_table(), true);
          }

        // threepf and redbsp are now missing the same configurations, so log these:

        if(threepf_total_serials.size() > 0)
          {
            std::list<unsigned int> remainder = this->advise_missing_content(writer, threepf_total_serials, tk->get_threepf_database());

            if(remainder.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << std::endl << "** Dropping extra configurations not missing from container, but advised by backend:";
                sqlite3_operations::drop_k_configurations(db, writer, remainder, tk->get_threepf_database(),
                                                          sqlite3_operations::data_traits<number, typename postintegration_items<number>::zeta_threepf_item>::sqlite_table());
                sqlite3_operations::drop_k_configurations(db, writer, remainder, tk->get_threepf_database(),
                                                          sqlite3_operations::data_traits<number, typename postintegration_items<number>::zeta_redbsp_item>::sqlite_table(), true);
                threepf_total_serials.merge(remainder);
              }
          }

        // check if any twopf configurations require dropping even more threepfs
        undropped.clear();
        std::set_difference(twopf_to_threepf_map.begin(), twopf_to_threepf_map.end(),
                            threepf_total_serials.begin(), threepf_total_serials.end(), std::back_inserter(undropped));

        if(undropped.size() > 0)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << "** Dropping extra threepf configurations not missing from container, but implied by missing twopf configurations:";
            sqlite3_operations::drop_k_configurations(db, writer, undropped, tk->get_threepf_database(),
                                                      sqlite3_operations::data_traits<number, typename postintegration_items<number>::zeta_threepf_item>::sqlite_table());
            sqlite3_operations::drop_k_configurations(db, writer, undropped, tk->get_threepf_database(),
                                                      sqlite3_operations::data_traits<number, typename postintegration_items<number>::zeta_redbsp_item>::sqlite_table(), true);
            threepf_total_serials.merge(undropped);   // not necessary to remove duplicates, since there should be any; result is sorted
          }

        if(threepf_total_serials.size() > 0)
          {
            writer.set_missing_serials(threepf_total_serials);
            writer.set_fail(true);

            // build list of twopf configurations which should be dropped for this entire set of threepf configurations
            std::list<unsigned int> twopf_drop = this->compute_twopf_drop_list(threepf_total_serials, tk->get_threepf_database());

            // compute twopf configurations which should be dropped
            undropped.clear();
            std::set_difference(twopf_drop.begin(), twopf_drop.end(),
                                twopf_serials.begin(), twopf_serials.end(), std::back_inserter(undropped));

            if(undropped.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << std::endl << "** Dropping twopf configurations entailed by these threepf configurations, but present in the container";
                sqlite3_operations::drop_k_configurations(db, writer, undropped, tk->get_twopf_database(),
                                                          sqlite3_operations::data_traits<number, typename postintegration_items<number>::zeta_twopf_item>::sqlite_table());
              }
          }
      }


    template <typename number>
    void data_manager_sqlite3<number>::check_fNL_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* tk)
      {
        // get sqlite3 handle to principal database
        sqlite3* db = nullptr;
        writer.get_data_manager_handle(&db); // throws an exception if handle is unset, so the return value is guaranteed not to be nullptr

        BOOST_LOG_SEV(writer.get_log(), base_writer::normal) << std::endl << "** Performing integrity check for container '" << writer.get_abs_container_path().string() << "'";
      }


		// DATAPIPES


    template <typename number>
    datapipe<number> data_manager_sqlite3<number>::create_datapipe(const boost::filesystem::path& logdir, const boost::filesystem::path& tempdir,
                                                                   typename datapipe<number>::integration_content_finder integration_finder,
                                                                   typename datapipe<number>::postintegration_content_finder postintegration_finder,
                                                                   typename datapipe<number>::dispatch_function dispatcher,
                                                                   unsigned int worker, bool no_log)
			{
        // set up callback API
        typename datapipe<number>::utility_callbacks utilities;

        utilities.integration_attach = std::bind(&data_manager_sqlite3<number>::datapipe_attach_integration_content, this,
                                                 std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        utilities.postintegration_attach = std::bind(&data_manager_sqlite3<number>::datapipe_attach_postintegration_content, this,
                                                     std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        utilities.detach = std::bind(&data_manager_sqlite3<number>::datapipe_detach, this, std::placeholders::_1);

        utilities.integration_finder     = integration_finder;
        utilities.postintegration_finder = postintegration_finder;
        utilities.dispatch               = dispatcher;

        typename datapipe<number>::config_cache config;

        config.time = std::bind(&data_manager_sqlite3<number>::pull_time_config, this,
                                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

        config.twopf = std::bind(&data_manager_sqlite3<number>::pull_kconfig_twopf, this,
                                 std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

        config.threepf = std::bind(&data_manager_sqlite3<number>::pull_kconfig_threepf, this,
                                   std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

        typename datapipe<number>::timeslice_cache timeslice;

        timeslice.background = std::bind(&data_manager_sqlite3<number>::pull_background_time_sample, this,
                                         std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        timeslice.twopf = std::bind(&data_manager_sqlite3<number>::pull_twopf_time_sample, this,
                                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                    std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);

        timeslice.threepf = std::bind(&data_manager_sqlite3<number>::pull_threepf_time_sample, this,
                                      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                      std::placeholders::_4, std::placeholders::_5);

        timeslice.tensor_twopf = std::bind(&data_manager_sqlite3<number>::pull_tensor_twopf_time_sample, this,
                                           std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                           std::placeholders::_4, std::placeholders::_5);

        timeslice.zeta_twopf = std::bind(&data_manager_sqlite3<number>::pull_zeta_twopf_time_sample, this,
                                         std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        timeslice.zeta_threepf = std::bind(&data_manager_sqlite3<number>::pull_zeta_threepf_time_sample, this,
                                           std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        timeslice.zeta_redbsp = std::bind(&data_manager_sqlite3<number>::pull_zeta_redbsp_time_sample, this,
                                          std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        timeslice.fNL = std::bind(&data_manager_sqlite3<number>::pull_fNL_time_sample, this,
                                  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        timeslice.BT = std::bind(&data_manager_sqlite3<number>::pull_BT_time_sample, this,
                                 std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        timeslice.TT = std::bind(&data_manager_sqlite3<number>::pull_TT_time_sample, this,
                                 std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        typename datapipe<number>::kslice_cache kslice;

        kslice.twopf = std::bind(&data_manager_sqlite3<number>::pull_twopf_kconfig_sample, this,
                                 std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                 std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);

        kslice.threepf = std::bind(&data_manager_sqlite3<number>::pull_threepf_kconfig_sample, this,
                                   std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                   std::placeholders::_4, std::placeholders::_5);

        kslice.tensor_twopf = std::bind(&data_manager_sqlite3<number>::pull_tensor_twopf_kconfig_sample, this,
                                        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                        std::placeholders::_4, std::placeholders::_5);

        kslice.zeta_twopf = std::bind(&data_manager_sqlite3<number>::pull_zeta_twopf_kconfig_sample, this,
                                      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        kslice.zeta_threepf = std::bind(&data_manager_sqlite3<number>::pull_zeta_threepf_kconfig_sample, this,
                                        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        kslice.zeta_redbsp = std::bind(&data_manager_sqlite3<number>::pull_zeta_redbsp_kconfig_sample, this,
                                       std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        typename datapipe<number>::stats_cache stats;

        stats.k_statistics = std::bind(&data_manager_sqlite3<number>::pull_k_statistics_sample, this,
                                       std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

        // set up datapipe
        datapipe<number> pipe(this->pipe_capacity, logdir, tempdir, worker, utilities, config, timeslice, kslice, stats, no_log);

        return(pipe);
			}


    template <typename number>
    void data_manager_sqlite3<number>::pull_time_config(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_time_config_query>& query,
                                                        std::vector<time_config>& sample)
	    {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
		    pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_time_config_sample(db, query, sample, pipe->get_worker_number());
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_kconfig_twopf(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_twopf_kconfig_query>& query,
                                                          std::vector<twopf_kconfig>& sample)
			{
		    assert(pipe != nullptr);
		    if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

		    sqlite3* db = nullptr;
		    pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

		    sqlite3_operations::pull_twopf_kconfig_sample<number>(db, query, sample, pipe->get_worker_number());
			}


    template <typename number>
    void data_manager_sqlite3<number>::pull_kconfig_threepf(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_threepf_kconfig_query>& query,
                                                            std::vector<threepf_kconfig>& sample)
	    {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_threepf_kconfig_sample<number>(db, query, sample, pipe->get_worker_number());
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_background_time_sample(datapipe<number>* pipe, unsigned int id,
                                                                   const std::shared_ptr<derived_data::SQL_query>& query,
                                                                   std::vector<number>& sample)
	    {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_background_time_sample(db, id, query, sample, pipe->get_worker_number(), pipe->get_N_fields());
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_twopf_time_sample(datapipe<number>* pipe, unsigned int id,
                                                              const std::shared_ptr<derived_data::SQL_query>& query,
                                                              unsigned int k_serial, std::vector<number>& sample,
                                                              typename datapipe<number>::twopf_type type)
	    {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

		    if(type == datapipe<number>::twopf_real)
			    {
		        sqlite3_operations::pull_paged_time_sample<number, typename integration_items<number>::twopf_re_item>(db, id, query, k_serial, sample,
		                                                                                                              pipe->get_worker_number(), pipe->get_N_fields());
			    }
		    else if(type == datapipe<number>::twopf_imag)
			    {
		        sqlite3_operations::pull_paged_time_sample<number, typename integration_items<number>::twopf_im_item>(db, id, query, k_serial, sample,
		                                                                                                              pipe->get_worker_number(), pipe->get_N_fields());
			    }
		    else assert(false);
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_threepf_time_sample(datapipe<number>* pipe, unsigned int id,
                                                                const std::shared_ptr<derived_data::SQL_query>& query,
                                                                unsigned int k_serial, std::vector<number>& sample)
	    {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_paged_time_sample<number, typename integration_items<number>::threepf_item>(db, id, query, k_serial, sample,
                                                                                                             pipe->get_worker_number(), pipe->get_N_fields());
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_tensor_twopf_time_sample(datapipe<number>* pipe, unsigned int id,
                                                                     const std::shared_ptr<derived_data::SQL_query>& query,
                                                                     unsigned int k_serial, std::vector<number>& sample)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_paged_time_sample<number, typename integration_items<number>::tensor_twopf_item>(db, id, query, k_serial, sample,
                                                                                                                  pipe->get_worker_number(), pipe->get_N_fields());
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_zeta_twopf_time_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                                   unsigned int k_serial, std::vector<number>& sample)
	    {
		    assert(pipe != nullptr);
		    if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

		    sqlite3* db = nullptr;
		    pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_unpaged_time_sample<number, typename postintegration_items<number>::zeta_twopf_item>(db, query, k_serial, sample,
                                                                                                                      pipe->get_worker_number());
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_zeta_threepf_time_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                                     unsigned int k_serial, std::vector<number>& sample)
	    {
		    assert(pipe != nullptr);
		    if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

		    sqlite3* db = nullptr;
		    pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_unpaged_time_sample<number, typename postintegration_items<number>::zeta_threepf_item>(db, query, k_serial, sample,
                                                                                                                        pipe->get_worker_number());
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_zeta_redbsp_time_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                                    unsigned int k_serial, std::vector<number>& sample)
	    {
		    assert(pipe != nullptr);
		    if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

		    sqlite3* db = nullptr;
		    pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_unpaged_time_sample<number, typename postintegration_items<number>::zeta_redbsp_item>(db, query, k_serial, sample,
                                                                                                                        pipe->get_worker_number());
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_fNL_time_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                            std::vector<number>& sample, derived_data::template_type type)
	    {
		    assert(pipe != nullptr);
		    if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

		    sqlite3* db = nullptr;
		    pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_fNL_time_sample(db, query, sample, pipe->get_worker_number(), type);
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_BT_time_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                           std::vector<number>& sample, derived_data::template_type type)
	    {
		    assert(pipe != nullptr);
		    if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

		    sqlite3* db = nullptr;
		    pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_BT_time_sample(db, query, sample, pipe->get_worker_number(), type);
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_TT_time_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                           std::vector<number>& sample, derived_data::template_type type)
	    {
		    assert(pipe != nullptr);
		    if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

		    sqlite3* db = nullptr;
		    pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_TT_time_sample(db, query, sample, pipe->get_worker_number(), type);
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_twopf_kconfig_sample(datapipe<number>* pipe, unsigned int id,
                                                                 const std::shared_ptr<derived_data::SQL_query>& query,
                                                                 unsigned int t_serial, std::vector<number>& sample,
                                                                 typename datapipe<number>::twopf_type type)
	    {
				assert(pipe != nullptr);
		    if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

		    sqlite3* db = nullptr;
		    pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

		    if(type == datapipe<number>::twopf_real)
			    {
		        sqlite3_operations::pull_paged_kconfig_sample<number, typename integration_items<number>::twopf_re_item>(db, id, query, t_serial, sample,
		                                                                                                                 pipe->get_worker_number(), pipe->get_N_fields());
			    }
		    else if(type == datapipe<number>::twopf_imag)
			    {
		        sqlite3_operations::pull_paged_kconfig_sample<number, typename integration_items<number>::twopf_im_item>(db, id, query, t_serial, sample,
		                                                                                                                 pipe->get_worker_number(), pipe->get_N_fields());
			    }
		    else assert(false);
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_threepf_kconfig_sample(datapipe<number>* pipe, unsigned int id,
                                                                   const std::shared_ptr<derived_data::SQL_query>& query,
                                                                   unsigned int t_serial, std::vector<number>& sample)
	    {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_paged_kconfig_sample<number, typename integration_items<number>::threepf_item>(db, id, query, t_serial, sample,
                                                                                                                pipe->get_worker_number(), pipe->get_N_fields());
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_tensor_twopf_kconfig_sample(datapipe<number>* pipe, unsigned int id,
                                                                        const std::shared_ptr<derived_data::SQL_query>& query,
                                                                        unsigned int t_serial, std::vector<number>& sample)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_paged_kconfig_sample<number, typename integration_items<number>::tensor_twopf_item>(db, id, query, t_serial, sample,
                                                                                                                     pipe->get_worker_number(), pipe->get_N_fields());
      }


    template <typename number>
    void data_manager_sqlite3<number>::pull_zeta_twopf_kconfig_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                                      unsigned int t_serial, std::vector<number>& sample)
	    {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_unpaged_kconfig_sample<number, typename postintegration_items<number>::zeta_twopf_item>(db, query, t_serial, sample,
                                                                                                                         pipe->get_worker_number());
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_zeta_threepf_kconfig_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                                        unsigned int t_serial, std::vector<number>& sample)
	    {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_unpaged_kconfig_sample<number, typename postintegration_items<number>::zeta_threepf_item>(db, query, t_serial, sample,
                                                                                                                           pipe->get_worker_number());
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_zeta_redbsp_kconfig_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                                       unsigned int t_serial, std::vector<number>& sample)
	    {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_unpaged_kconfig_sample<number, typename postintegration_items<number>::zeta_redbsp_item>(db, query, t_serial, sample,
                                                                                                                          pipe->get_worker_number());
	    }


    template <typename number>
    void data_manager_sqlite3<number>::pull_k_statistics_sample(datapipe<number>* pipe, const std::shared_ptr<derived_data::SQL_query>& query,
                                                                std::vector<kconfiguration_statistics>& data)
      {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        sqlite3* db = nullptr;
        pipe->get_manager_handle(&db);    // throws an exception if the handle is unset, so safe to proceed; we can't get nullptr back

        sqlite3_operations::pull_k_statistics_sample(db, query, data, pipe->get_worker_number());
      }


		template <typename number>
		void data_manager_sqlite3<number>::datapipe_attach_container(datapipe<number>* pipe, const boost::filesystem::path& ctr_path)
			{
		    sqlite3* db = nullptr;

		    int status = sqlite3_open_v2(ctr_path.string().c_str(), &db, SQLITE_OPEN_READONLY, nullptr);

		    if(status != SQLITE_OK)
			    {
		        std::ostringstream msg;
		        if(db != nullptr)
			        {
		            msg << CPPTRANSPORT_DATACTR_OPEN_A << " '" << ctr_path.string() << "' " << CPPTRANSPORT_DATACTR_OPEN_B << status << ": " << sqlite3_errmsg(db) << ")";
		            sqlite3_close(db);
			        }
		        else
			        {
		            msg << CPPTRANSPORT_DATACTR_OPEN_A << " '" << ctr_path.string() << "' " << CPPTRANSPORT_DATACTR_OPEN_B << status << ")";
			        }
		        throw runtime_exception(runtime_exception::DATA_CONTAINER_ERROR, msg.str());
			    }
		    sqlite3_extended_result_codes(db, 1);

		    // enable foreign key constraints
		    char* errmsg;
		    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &errmsg);

		    // force temporary databases to be stored in memory, for speed
		    sqlite3_exec(db, "PRAGMA main.temp_store = 2;", nullptr, nullptr, &errmsg);

        // try to speed up SQLite accesses
        sqlite3_exec(db, "PRAGMA main.synchronous = 1;", nullptr, nullptr, &errmsg);
        sqlite3_exec(db, "PRAGMA main.cache_size = 10000;", nullptr, nullptr, &errmsg);

		    // remember this connexion
		    this->open_containers.push_back(db);
		    pipe->set_manager_handle(db);

		    BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::normal) << "** Attached SQLite3 container '" << ctr_path.string() << "' to datapipe";
			}


    template <typename number>
    std::shared_ptr< output_group_record<integration_payload> >
    data_manager_sqlite3<number>::datapipe_attach_integration_content(datapipe<number>* pipe, typename datapipe<number>::integration_content_finder& finder,
                                                                      const std::string& name, const std::list<std::string>& tags)
			{
				assert(pipe != nullptr);
				if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        // find a suitable output group for this task
        std::shared_ptr< output_group_record<integration_payload> > group = finder(name, tags);

        integration_payload& payload = group->get_payload();

				// get path to the output group data container
		    boost::filesystem::path ctr_path = group->get_abs_repo_path() / payload.get_container_path();

        this->datapipe_attach_container(pipe, ctr_path);

        return(group);
			}


    template <typename number>
    std::shared_ptr <output_group_record<postintegration_payload>>
    data_manager_sqlite3<number>::datapipe_attach_postintegration_content(datapipe<number>* pipe, typename datapipe<number>::postintegration_content_finder& finder,
                                                                          const std::string& name, const std::list<std::string>& tags)
	    {
        assert(pipe != nullptr);
        if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

        // find a suitable output group for this task
        std::shared_ptr< output_group_record<postintegration_payload> > group = finder(name, tags);

        postintegration_payload& payload = group->get_payload();

        // get path to the output group data container
        boost::filesystem::path ctr_path = group->get_abs_repo_path() / payload.get_container_path();

		    this->datapipe_attach_container(pipe, ctr_path);

        return(group);
	    }


		template <typename number>
		void data_manager_sqlite3<number>::datapipe_detach(datapipe<number>* pipe)
			{
		    assert(pipe != nullptr);
		    if(pipe == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_DATAMGR_NULL_DATAPIPE);

				sqlite3* db = nullptr;
				pipe->get_manager_handle(&db);
				this->open_containers.remove(db);
				sqlite3_close(db);

				BOOST_LOG_SEV(pipe->get_log(), datapipe<number>::normal) << "** Detached SQLite3 container from datapipe";
			}


    // FACTORY FUNCTIONS TO BUILD A DATA_MANAGER


    template <typename number>
    std::shared_ptr< data_manager<number> > data_manager_factory(unsigned int bcap, unsigned int dcap, unsigned int ckp)
      {
        return std::make_shared< data_manager_sqlite3<number> >(bcap, dcap, ckp);
      }


  };   // namespace transport



#endif //__data_manager_sqlite3_H_

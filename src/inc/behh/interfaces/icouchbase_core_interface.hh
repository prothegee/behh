#ifndef BEHH_ICOUCHBASE_CORE_INTERFACE_HH
#define BEHH_ICOUCHBASE_CORE_INTERFACE_HH
#include "../pch.hh"

#if BEHH_USING_COUCHBASE_CXX_CLIENT
#include <couchbase/cluster.hxx>
#include <couchbase/codec/tao_json_serializer.hxx>
#include <couchbase/logger.hxx>

#include <spdlog/fmt/bundled/core.h>

#include <couchbase/fmt/error.hxx>

namespace behh {

// reserved

////////////////////////////////////////////////

/**
 * @brief couchbase cluster constant profiles option/s
 *
 * @note 1. wan_development
 */
struct couchbase_cluster_profiles {
    // currently only available "wan_development"?
    INLNSTTCCNST std::string wan_development = "wan_development";
};

/**
 * @brief couchbase cluster type structure
 *
 */
struct couchbase_cluster_t {
    std::string bucket_name;
};

/**
 * @brief couchbase connection type structure
 */
struct couchbase_connection_t {
    std::string host;
    std::string username;
    std::string password;
    std::string scope_name;
    std::string collection_name;

    std::string cluster_bucket_name;
};

/**
 * @brief behh couchbase core interface structure
 *
 * @note this interface handle connection data while
 * @note child interface of ICouchbase is responsible to handle the cluster contact
 * @note if initialize_constructor return true or 1, you can get cluster data, collection data after that
 *
 * @note this interface structure still useless
 */
struct ICouchbaseCoreInterface {
    virtual ~ICouchbaseCoreInterface();

    // skipped: initialize_table; table/collection is dynamic (schemaless)

    /**
     * @brief initialize table index implementation
     *
     * @note prior 1
     */
    virtual void initialize_table_index() = 0;

    // skipped: initialize_table_materialized_view

    // skipped: initialize_table_and_alterize_table

    // couchbase interface structure
    struct _ICouchbase {
        virtual ~_ICouchbase();

        /**
         * @brief immediately initialize connection/s for session & cluster
         *
         * @note should be call in constructor before any usage
         * @note after cluster hit contact point, you can do other thing/s
         *
         * @param conn
         * @param couchbase_log_level 0:trace 1:debug 2:info 3:warn 4:error 5:critical 6:off // ( default 6 ) // will not appear in release build
         * @param extra_info
         * @return int32_t
         */
        int32_t initialize_constructor(couchbase_connection_t& conn, const int32_t& couchbase_log_level = 6, const char* extra_info = "");

        /**
         * @brief get this interface connection data
         *
         * @return couchbase_connection_t
         */
        couchbase_connection_t get_connection_data() {
            return m_connection;
        }

        /**
         * @brief get the scope name from active interface connection data
         *
         * @return std::string
         */
        std::string get_scope_name() { return m_connection.scope_name; }
        /**
         * @brief get the bucket name from active interface connection data
         *
         * @return std::string
         */
        std::string get_bucket_name() { return m_connection.cluster_bucket_name; }
        /**
         * @brief get the collection name from active interface connection data
         *
         * @return std::string
         */
        std::string get_collection_name() { return m_connection.collection_name; }

        /**
         * @brief get this interface cluster object
         *
         * @return couchbase::cluster
         */
        couchbase::cluster get_cluster() { return m_cluster; }
        /**
         * @brief get this interface cluster error object
         *
         * @return couchbase::error
         */
        couchbase::error get_cluster_error() { return m_cluster_error; }

        /**
         * @brief get this interface connected cluster bucket
         *
         * @return couchbase::bucket
         */
        couchbase::bucket get_cluster_bucket() {
            return m_cluster.bucket(get_bucket_name());
        }
        /**
         * @brief get this interface connected cluster bucket scope
         *
         * @return couchbase::scope
         */
        couchbase::scope get_cluster_bucket_scope() {
            return m_cluster.bucket(get_bucket_name())
                .scope(get_scope_name());
        }
        /**
         * @brief get this interface connected cluster bucket scope collection
         *
         * @return couchbase::collection
         */
        couchbase::collection get_cluster_bucket_scope_collection() {
            return m_cluster.bucket(get_bucket_name())
                .scope(get_scope_name())
                .collection(get_collection_name());
        }

        /**
         * @brief get this interface bucket manager object
         *
         * @return couchbase::bucket_manager
         */
        couchbase::bucket_manager get_bucket_manager() {
            return m_cluster.buckets();
        }
        /**
         * @brief get this interface bucket collection manager object
         *
         * @return couchbase::collection_manager
         */
        couchbase::collection_manager get_bucket_collection_manager() {
            return m_cluster.bucket(m_connection.cluster_bucket_name)
                .collections();
        }

        /**
         * @brief get this interface current bucket is exists
         *
         * @return int32_t 1 mean ok
         */
        int32_t get_current_bucket_is_exists() { return m_current_bucket_exists; };
        /**
         * @brief get this interface current bucket scope is exists
         *
         * @return int32_t 1 mean ok
         */
        int32_t get_current_bucket_scope_is_exists() { return m_current_bucket_scope_exists; };
        /**
         * @brief get this interface current bucket scope collection is exists
         *
         * @return int32_t 1 mean ok
         */
        int32_t get_current_bucket_scope_collection_is_exists() { return m_current_bucket_scope_collection_exists; };

        /**
         * @brief immediately execute sql++ (N1QL)
         *
         * @param query
         * @param data_to_pass
         * @param consistent default false, performance first | true, trade of with data scan consistency
         * @return int32_t 1 mean ok
         */
        int32_t execute_sqlpp(const std::string& query, std::pair<couchbase::error, couchbase::query_result>& data_to_pass, const bool_t& consistent = false);
        /**
         * @brief execute sql++ (N1QL) with future object to pass
         *
         * @note mostly true, unless exception happen
         * @note you handle the future object at your implementation
         *
         * @param query
         * @param data_to_pass future object
         * @param consistent default false, performance first | true, trade of with data scan consistency
         * @return int32_t 1 mean ok
         */
        int32_t execute_sqlpp_future(const std::string& query, std::future<std::pair<couchbase::error, couchbase::query_result>>& data_to_pass, const bool_t& consistent);

    private:
        couchbase_connection_t m_connection; // shouldn't be modify at the runtime

        couchbase::cluster m_cluster;
        couchbase::error m_cluster_error;

        int32_t m_current_bucket_exists;
        int32_t m_current_bucket_scope_exists;
        int32_t m_current_bucket_scope_collection_exists;
    };
    /**
     * @brief couchbase interface access
     *
     * @note if you want raw query, use get_cluster first then do .query
     */
    _ICouchbase ICouchbase = _ICouchbase();
};

} // namespace behh
#endif // BEHH_USING_COUCHBASE_CXX_CLIENT

#endif // BEHH_ICOUCHBASE_CORE_INTERFACE_HH

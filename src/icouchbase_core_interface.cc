#include <behh/interfaces/icouchbase_core_interface.hh>

#if BEHH_USING_COUCHBASE_CXX_CLIENT
#include <couchbase/management/bucket_settings.hxx>

#include <iostream>
#include <utility>

namespace behh {
ICouchbaseCoreInterface::~ICouchbaseCoreInterface() {
}

ICouchbaseCoreInterface::_ICouchbase::~_ICouchbase() {
    m_cluster.close().get();
}

int32_t ICouchbaseCoreInterface::_ICouchbase::initialize_constructor(couchbase_connection_t& conn, const int32_t& couchbase_log_level, const char* extra_info) {
#if BEHH_IS_DEBUG
    if (couchbase_log_level > -1 || couchbase_log_level < 7) {
        couchbase::logger::initialize_console_logger();
        couchbase::logger::set_level((couchbase::logger::log_level)couchbase_log_level);
    }
#endif // BEHH_IS_DEBUG

#if BEHH_IS_DEBUG
    if (conn.host.empty()) {
        std::cout << "DEBUG: warning, conn.host is empty\n";
    }
    if (conn.username.empty()) {
        std::cout << "DEBUG: warning, conn.username is empty\n";
    }
    if (conn.password.empty()) {
        std::cout << "DEBUG: warning, conn.password is empty\n";
    }

    if (conn.scope_name.empty()) {
        std::cout << "DEBUG: warning, conn.scope_name is empty\n";
    }
    if (conn.cluster_bucket_name.empty()) {
        std::cout << "DEBUG: warning, conn.cluster_bucket_name is empty\n";
    }
    if (conn.collection_name.empty()) {
        std::cout << "DEBUG: warning, conn.collection_name is empty\n";
    }
#else
    // this is release
    // make it all error
    if (conn.host.empty()) {
        std::cerr << "ERROR: conn.host can't empty\n";
        return -1;
    }
    if (conn.username.empty()) {
        std::cerr << "ERROR: conn.username can't empty\n";
        return -2;
    }
    if (conn.password.empty()) {
        std::cerr << "ERROR: conn.password can't empty\n";
        return -3;
    }

    if (conn.scope_name.empty()) {
        std::cerr << "ERROR: conn.scope_name can't empty\n";
        return -4;
    }
    if (conn.cluster_bucket_name.empty()) {
        std::cerr << "ERROR: conn.cluster_bucket_name can't empty\n";
        return -5;
    }
    if (conn.collection_name.empty()) {
        std::cerr << "ERROR: conn.collection_name can't empty\n";
        return -7;
    }
#endif // BEHH_IS_DEBUG

    // move connection paramater to member data
    { // std::forward<couchbase::cluster>(connection_result.second);
        m_connection.host = std::forward<std::string>(conn.host);
        m_connection.username = std::forward<std::string>(conn.username);
        m_connection.password = std::forward<std::string>(conn.password);
        m_connection.scope_name = std::forward<std::string>(conn.scope_name);
        m_connection.cluster_bucket_name = std::forward<std::string>(conn.cluster_bucket_name);
        m_connection.collection_name = std::forward<std::string>(conn.collection_name);
    }

    couchbase::cluster_options option = couchbase::cluster_options(m_connection.username, m_connection.password);

    option.apply_profile(couchbase_cluster_profiles::wan_development);

    auto&& connection_result = couchbase::cluster::connect(m_connection.host, option).get();

    if (connection_result.first) {
#if BEHH_IS_DEBUG
    (std::strlen(extra_info) > 0)
        ? std::cerr << "DEBUG: \"ICouchbaseCoreInterface::_ICouchbase::initialize_constructor\" fail to connect: \"" << extra_info << "\" | error code: " << connection_result.first.ec().message() << "\n"
        : std::cerr << "DEBUG: \"ICouchbaseCoreInterface::_ICouchbase::initialize_constructor\" fail to connect: ( extra_info is not provided )\n";
#endif // BEHH_IS_DEBUG
        return -9;
    }

    m_current_bucket_exists = 0; // false
    m_current_bucket_scope_exists = 0; // false
    m_current_bucket_scope_collection_exists = 0; // false

    // move connection_result where connection_result can't be used again
    m_cluster = std::forward<couchbase::cluster>(connection_result.second);
    m_cluster_error = std::forward<couchbase::error>(connection_result.first);

    auto&& bucket_manager = m_cluster.buckets();

    auto buckets = bucket_manager.get_all_buckets().get();
    auto all_buckets = buckets.second;

    const int8_t MAX_BUCKET_SIZE_WARN = 3;
    // check all buckets size
    if (all_buckets.size() >= MAX_BUCKET_SIZE_WARN) {
        std::cout << "NOTE: the bucket size/list is " << all_buckets.size() << ", since it's grater or equal than " << MAX_BUCKET_SIZE_WARN << "\n";
    }

    for (auto& bucket : all_buckets) {
        if (bucket.name == m_connection.cluster_bucket_name) {
            m_current_bucket_exists = 1; // true
            break;
        }
    }

    if (!m_current_bucket_exists) {
        try {
            couchbase::management::cluster::bucket_settings settings;

            // in debug it's empty, but in release it shouldn't reach here
            if (m_connection.cluster_bucket_name.empty()) {
        #if BEHH_IS_DEBUG
                std::cout << "WARNING: this seems a debug and bucket name is empty, set to \"bucket_default\"\n";

                settings.name = "bucket_default";
        #else
                std::cerr << "ERROR: it's release build, but bucket name is still empty somehow\n";
                exit(555666);
        #endif // BEHH_IS_DEBUG
            } else {
                settings.name = m_connection.cluster_bucket_name;
            }

            bucket_manager.create_bucket(settings).get();
        } catch (const std::exception& e) {
            std::cerr << "FATAL: can't create \"" << m_connection.cluster_bucket_name << "\" bucket: " << e.what() << "\n";
            return -69;
        }
    }

    PRAGMA_MESSAGE("TODO?: restart if exists and con config apply reinit?")

    auto bucket = m_cluster.bucket(m_connection.cluster_bucket_name);

    auto collection_manager = bucket.collections();
    auto all_scopes = collection_manager.get_all_scopes().get();

    // check current scope & collection
    for (auto& scope : all_scopes.second) {
        if (scope.name == m_connection.scope_name) {
            m_current_bucket_scope_exists = 1; // true

            for (auto& collection : scope.collections) {
                if (collection.name == m_connection.collection_name) {
                    m_current_bucket_scope_collection_exists = 1; // true
                    break;
                }
            }
            break;
        }
    }

    if (!m_current_bucket_scope_exists) {
        std::cout << "NOTE: be warn, scope of \"" << m_connection.scope_name << "\" not exists, creating the new one by default\n";
        collection_manager.create_scope(m_connection.scope_name).get();
    }

    if (!m_current_bucket_scope_collection_exists) {
        std::cout << "NOTE: be warn, colletion of \"" << m_connection.collection_name << "\" not exists, creating the new one by default\n";
        collection_manager.create_collection(m_connection.scope_name, m_connection.collection_name).get();
    }

#if BEHH_IS_DEBUG
    (std::strlen(extra_info) > 0)
        ? std::cout << "DEBUG: \"ICouchbaseCoreInterface::_ICouchbase::initialize_constructor\" connected: " << extra_info << "\"\n"
        : std::cout << "DEBUG: \"ICouchbaseCoreInterface::_ICouchbase::initialize_constructor\" connected: ( extra_info is not provided )\n";
#endif // BEHH_IS_DEBUG

    return 1;
}


int32_t ICouchbaseCoreInterface::_ICouchbase::execute_sqlpp(const std::string& query, std::pair<couchbase::error, couchbase::query_result>& data_to_pass, const bool_t& consistent) {
    try {
        (!consistent)
            ? data_to_pass = m_cluster.query(query, {}).get()
            : data_to_pass = m_cluster.query(query, couchbase::query_options().scan_consistency(couchbase::query_scan_consistency::request_plus)).get();

        if (data_to_pass.first) {
            std::cerr << "ERROR: query fail ec " << data_to_pass.first.ec().message() << "\n";
            return -1;
        }

    } catch(const std::exception& e) {
        std::cerr << "ERROR execute_sqlpp: " << e.what() << '\n';
        return -69;
    }

    return 1;
}

int32_t ICouchbaseCoreInterface::_ICouchbase::execute_sqlpp_future(const std::string& query, std::future<std::pair<couchbase::error, couchbase::query_result>>& data_to_pass, const bool_t& consistent) {
    try {
        (consistent)
            ? data_to_pass = m_cluster.query(query, {})
            : data_to_pass = m_cluster.query(query, couchbase::query_options().scan_consistency(couchbase::query_scan_consistency::request_plus));
    } catch(const std::exception& e) {
        std::cerr << "ERROR execute_sqlpp_future: " << e.what() << '\n';
        return -69;
    }

    return 1;
}
} // namespace behh
#endif // BEHH_USING_COUCHBASE_CXX_CLIENT

#ifndef BEHH_ISCYLLADB_CORE_INTERFACE_HH
#define BEHH_ISCYLLADB_CORE_INTERFACE_HH
#include "../pch.hh"

#if BEHH_USING_SCYLLADB
#include <behh/types/auth_type.hh>
#include <cassandra.h>

namespace behh {

/**
 * @brief scylladb auth mode enum
 */
enum scylladb_auth_mode_e : int32_t {
    SCYLLADB_AUTH_MODE_UNDEFINED,
    SCYLLADB_AUTH_MODE_ALLOW_ALL_AUTHENTICATOR,
    SCYLLADB_AUTH_MODE_PASSWORD_AUTHENTICATOR,
    SCYLLADB_AUTH_MODE_COM_SCYLLADB_AUTH_TRANSITIONAL_AUTHENTICATOR,
};

INLNSTTCCNST std::string SCYLLADB_AUTH_MODE_UNDEFINED_HINT = "Undefined";                                                                      // Undefined
INLNSTTCCNST std::string SCYLLADB_AUTH_MODE_ALLOW_ALL_AUTHENTICATOR_HINT = "AllowAllAuthenticator";                                            // AllowAllAuthenticator
INLNSTTCCNST std::string SCYLLADB_AUTH_MODE_PASSWORD_AUTHENTICATOR_HINT = "PasswordAuthenticator";                                             // PasswordAuthenticator
INLNSTTCCNST std::string SCYLLADB_AUTH_MODE_COM_SCYLLADB_AUTH_TRANSITIONAL_AUTHENTICATOR_HINT = "com.scylladb.auth.TransitionalAuthenticator"; // com.scylladb.auth.TransitionalAuthenticator

/**
 * @brief convert scylladb_auth_mode_e to string value
 *
 * @param auth_mode_enum
 * @return std::string
 */
INLNSTTC std::string scylladb_auth_mode_to_string(const scylladb_auth_mode_e& auth_mode_enum) {
    std::string result = "";

    switch (auth_mode_enum) {
    case SCYLLADB_AUTH_MODE_ALLOW_ALL_AUTHENTICATOR: {
        result = SCYLLADB_AUTH_MODE_ALLOW_ALL_AUTHENTICATOR_HINT;
    } break;
    case SCYLLADB_AUTH_MODE_PASSWORD_AUTHENTICATOR: {
        result = SCYLLADB_AUTH_MODE_PASSWORD_AUTHENTICATOR_HINT;
    } break;
    case SCYLLADB_AUTH_MODE_COM_SCYLLADB_AUTH_TRANSITIONAL_AUTHENTICATOR: {
        result = SCYLLADB_AUTH_MODE_COM_SCYLLADB_AUTH_TRANSITIONAL_AUTHENTICATOR_HINT;
    } break;
    default: {
        result = SCYLLADB_AUTH_MODE_UNDEFINED_HINT;
    } break;
    }

    return result;
}

/**
 * @brief convert scylladb_auth_mode string to enum
 *
 * @param auth_mode_string
 * @return scylladb_auth_mode_e
 */
INLNSTTC scylladb_auth_mode_e scylladb_auth_mode_to_enum(const std::string& auth_mode_string) {
    scylladb_auth_mode_e result = scylladb_auth_mode_e::SCYLLADB_AUTH_MODE_UNDEFINED;

    if (auth_mode_string == SCYLLADB_AUTH_MODE_ALLOW_ALL_AUTHENTICATOR_HINT) {
        return SCYLLADB_AUTH_MODE_ALLOW_ALL_AUTHENTICATOR;
    }
    if (auth_mode_string == SCYLLADB_AUTH_MODE_PASSWORD_AUTHENTICATOR_HINT) {
        return SCYLLADB_AUTH_MODE_PASSWORD_AUTHENTICATOR;
    }
    if (auth_mode_string == SCYLLADB_AUTH_MODE_COM_SCYLLADB_AUTH_TRANSITIONAL_AUTHENTICATOR_HINT) {
        return SCYLLADB_AUTH_MODE_COM_SCYLLADB_AUTH_TRANSITIONAL_AUTHENTICATOR;
    }

    return SCYLLADB_AUTH_MODE_UNDEFINED;
}

////////////////////////////////////////////////

/**
 * @brief scylladb topology strategy enum
 */
enum scylladb_topology_strat_e : int32_t {
    SCYLLADB_TOPOLOGY_STRAT_UNDEFINED,
    SCYLLADB_TOPOLOGY_STRAT_SIMPLE_STRATEGY,
    SCYLLADB_TOPOLOGY_STRAT_NETWORK_TOPOLOGY_STRATEGY,
    SCYLLADB_TOPOLOGY_STRAT_LOCAL_STRATEGY,
    SCYLLADB_TOPOLOGY_STRAT_EVERYWHERE_STRATEGY,
};

INLNSTTCCNST std::string SCYLLADB_TOPOLOGY_STRAT_UNDEFINED_HINT = "Undefined";                               // Undefined
INLNSTTCCNST std::string SCYLLADB_TOPOLOGY_STRAT_SIMPLE_STRATEGY_HINT = "SimpleStrategy";                    // SimpleStrategy
INLNSTTCCNST std::string SCYLLADB_TOPOLOGY_STRAT_NETWORK_TOPOLOGY_STRATEGY_HINT = "NetworkTopologyStrategy"; // NetworkTopologyStrategy
INLNSTTCCNST std::string SCYLLADB_TOPOLOGY_STRAT_LOCAL_STRATEGY_HINT = "LocalStrategy";                      // LocalStrategy
INLNSTTCCNST std::string SCYLLADB_TOPOLOGY_STRAT_EVERYWHERE_STRATEGY_HINT = "EverywhereStrategy";            // EverywhereStrategy

/**
 * @brief convert scylladb_topology_strat enum to string
 *
 * @param topology_strat_enum
 * @return std::string
 */
INLNSTTC std::string scylladb_topology_strat_to_string(const scylladb_topology_strat_e& topology_strat_enum) {
    std::string result = "";

    switch (topology_strat_enum) {
    case SCYLLADB_TOPOLOGY_STRAT_SIMPLE_STRATEGY: {
        result = SCYLLADB_TOPOLOGY_STRAT_SIMPLE_STRATEGY_HINT;
    } break;
    case SCYLLADB_TOPOLOGY_STRAT_NETWORK_TOPOLOGY_STRATEGY: {
        result = SCYLLADB_TOPOLOGY_STRAT_NETWORK_TOPOLOGY_STRATEGY_HINT;
    } break;
    case SCYLLADB_TOPOLOGY_STRAT_LOCAL_STRATEGY: {
        result = SCYLLADB_TOPOLOGY_STRAT_LOCAL_STRATEGY_HINT;
    } break;
    case SCYLLADB_TOPOLOGY_STRAT_EVERYWHERE_STRATEGY: {
        result = SCYLLADB_TOPOLOGY_STRAT_EVERYWHERE_STRATEGY_HINT;
    } break;
    default: {
        result = SCYLLADB_TOPOLOGY_STRAT_UNDEFINED_HINT;
    } break;
    }

    return result;
}

// reserved: scylladb_topology_strat_to_enum

////////////////////////////////////////////////

/**
 * @brief scylladb connection type structure
 */
struct scylladb_connection_t {
    std::string host;
    std::string username;
    std::string password;
    std::string keyspace;
    scylladb_auth_mode_e auth_mode;
    scylladb_topology_strat_e strategy;
    bool multiple_datacenters;
    std::string factors_configs;
    std::string factors_configs_extra;
};

/**
 * @brief scylladb core interface structure
 *
 */
struct IScyllaDbCoreInterface {
    virtual ~IScyllaDbCoreInterface() = default;

    /**
     * @brief initialize table implementation
     *
     * @note prior 0
     */
    virtual void initialize_table() = 0;

    /**
     * @brief initialize table index implementation
     *
     * @note prior 1
     */
    virtual void initialize_table_index() = 0;

    /**
     * @brief initialize table materialized view implementation
     *
     * @note prior 2
     */
    virtual void initialize_table_materialized_view() = 0;

    /**
     * @brief initialize table and alterize current table implementation
     *
     * @note prior 3
     */
    virtual void initialize_table_and_alterize_table() = 0;

    struct OnAuth {
        static void initialize(CassAuthenticator* pCassAuth, void* pVoidData);

        static void challenge(CassAuthenticator* pCassAuth, void* pVoidData, const char* pToken, size_t tokenSize);

        static void success(CassAuthenticator* pCassAuth, void* pVoidData, const char* pToken, size_t tokenSize);

        static void cleanup(CassAuthenticator* pCassAuth, void* pVoidData);
    };

    /**
     * @brief scylladb interface structure
     *
     */
    struct _IScyllaDb {
        virtual ~_IScyllaDb();

        /**
         * @brief scylladb print error
         *
         * @param pCassFuture
         * @param info
         */
        void print_error(CassFuture* pCassFuture, const char* info = "");

        /**
         * @brief immediately initialize pointer/s for session & cluster
         *
         * @note should be call in constructor before any usage
         * @note after cluster hit contact point, you can do other thing/s
         *
         * @param connection
         * @param extra_info
         * @return int32_t 1 mean ok
         */
        int32_t initialize_constructor(const scylladb_connection_t& connection, const char* extra_info = "");

        /**
         * @brief get this interface keyspace
         *
         * @return std::string
         */
        std::string get_keyspace() { return IScyllaDbCoreInterface::m_connection.keyspace; }

        /**
         * @brief get this interface auth mode
         *
         * @return scylladb_auth_mode_e
         */
        scylladb_auth_mode_e get_auth_mode() { return IScyllaDbCoreInterface::m_connection.auth_mode; }

        /**
         * @brief get this interface future cass ptr
         *
         * @return CassFuture*
         */
        CassFuture* get_cass_future() { return m_pCassFuture; }

        /**
         * @brief get this interface cluster cass ptr
         *
         * @return CassCluster*
         */
        CassCluster* get_cass_cluster() { return m_pCassCluster; }

        /**
         * @brief get this interface session cass ptr
         *
         * @return CassSession*
         */
        CassSession* get_cass_session() { return m_pCassSession; }

        /**
         * @brief get this interface topology strategy
         *
         * @return scylladb_topology_strat_e
         */
        scylladb_topology_strat_e get_topology_strat() { return IScyllaDbCoreInterface::m_connection.strategy; }

        /**
         * @brief generate uuid v1 as string from scylladb
         *
         * @return std::string
         */
        std::string generate_uuid_v1_as_string();
        /**
         * @brief generate uuid v1 as string from scylladb
         *
         * @return CassUuid
         */
        CassUuid generate_uuid_v1_as_uuid();

        /**
         * @brief generate uuid v4 as string from scylladb
         *
         * @return std::string
         */
        std::string generate_uuid_v4_as_string();
        /**
         * @brief generate uuid v4 as string from scylladb
         *
         * @return CassUuid
         */
        CassUuid generate_uuid_v4_as_uuid();

        /**
         * @brief convert uuid cass to string
         *
         * @param uuid_cass
         * @return std::string
         */
        std::string convert_uuid_to_string(const CassUuid& uuid_cass);
        /**
         * @brief convert uuid string to uuid cass
         *
         * @param uuid_string
         * @return CassUuid
         */
        CassUuid convert_string_to_uuid(const std::string& uuid_string);

        /**
         * @brief execute cqlsh for query
         *
         * @param pCassSession
         * @param query
         * @param note
         * @return CassError
         */
        CassError execute_cqlsh(CassSession* pCassSession, const char* query, const char* note = "");

    private:
        CassFuture* m_pCassFuture = nullptr;

        CassCluster* m_pCassCluster = nullptr;

        CassSession* m_pCassSession = nullptr;
    };
    // scylladb interface access
    _IScyllaDb IScyllaDb = _IScyllaDb();

private:
    INLNSTTC scylladb_connection_t m_connection;
};

} // namespace behh
#endif // BEHH_USING_SCYLLADB

#endif // BEHH_ISCYLLADB_CORE_INTERFACE_HH

#ifndef BEHH_IPOSTGRESQL_CORE_INTERFACE_HH
#define BEHH_IPOSTGRESQL_CORE_INTERFACE_HH
#include "../pch.hh"

#if BEHH_USING_POSTGRESQL
#if BEHH_USING_VCPKG
#include <libpq-fe.h>
#else
#include <libpq-fe.h> // hasn't found need to be include postgresql dir first
#endif // BEHH_USING_VCPKG

namespace behh {

/**
 * @brief postgresql connection type structure
 */
struct postgresql_connection_t {
    std::string dbname;
    std::string host;
    std::string port;
    std::string username;
    std::string password;
};

/**
 * @brief behh postgresql core interface structure
 *
 * @note meant to use on drogon-framework project
 * @note still don't have test & not much functionalities
 */
struct IPostgreSqlCoreInterface {
    virtual ~IPostgreSqlCoreInterface() = default;

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

    // postgresql interface structure
    struct _IPostgreSQL {
        virtual ~_IPostgreSQL();
        /**
         * @brief initialize database
         *
         * @param connection_data
         * @param create_db
         * @return int32_t 1 mean ok
         */
        int32_t initialize_database(const postgresql_connection_t& connection_data, const bool& create_db = false);

        /**
         * @brief this function use postgresql internal uuid function
         *
         * @return std::string
         */
        std::string generate_uuid_v1_as_string();

        /**
         * @brief this function use postgresql internal uuid function
         *
         * @return std::string
         */
        std::string generate_uuid_v4_as_string();

    private:
        PGconn* m_pConn = nullptr;
    };
    // postgresql interface access
    _IPostgreSQL IPostgreSQL = _IPostgreSQL();

private:
    INLNSTTC postgresql_connection_t m_connection;
};

} // namespace behh
#endif // BEHH_USING_POSTGRESQL

#endif // BEHH_IPOSTGRESQL_CORE_INTERFACE_HH

#include <behh/interfaces/iscylladb_core_interface.hh>

#include <cstring>
#include <iostream>

#if BEHH_COMPILER_GNU
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif // BEHH_COMPILER_GNU
#if BEHH_COMPILER_MSVC
#endif // BEHH_COMPILER_MSVC
#if BEHH_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif // BEHH_COMPILER_CLANG

namespace behh {
#if BEHH_USING_SCYLLADB
void IScyllaDbCoreInterface::OnAuth::initialize(CassAuthenticator* pCassAuth, void* pVoidData) {
    const behh::auth_basic_t* pCredential = (const behh::auth_basic_t*)new behh::auth_basic_t({IScyllaDbCoreInterface::m_connection.username.c_str(),
                                                                                               IScyllaDbCoreInterface::m_connection.password.c_str()});

    size_t username_size = std::strlen(pCredential->username);
    size_t password_size = std::strlen(pCredential->password);
    size_t size = username_size + password_size + 2;

    char* response = cass_authenticator_response(pCassAuth, size);

    response[0] = '\0';
    std::memcpy(response + 1, pCredential->username, username_size);

    response[username_size + 1] = '\0';
    std::memcpy(response + username_size + 2, pCredential->password, password_size);

    delete pCredential;
}

void IScyllaDbCoreInterface::OnAuth::challenge(CassAuthenticator* pCassAuth, void* pVoidData, const char* pToken, size_t tokenSize) {
    /**
     * not used for plain text authentication, but this is to be used for handling an authentication challenge initiated by the server
     */
}

void IScyllaDbCoreInterface::OnAuth::success(CassAuthenticator* pCassAuth, void* pVoidData, const char* pToken, size_t tokenSize) {
    /**
     * not used for plain text authentication, but this is to be used for handling the success phase of an exchange
     */
}

void IScyllaDbCoreInterface::OnAuth::cleanup(CassAuthenticator* pCassAuth, void* pVoidData) {
    /**
     * no resources cleanup is necessary for plain text authentication, but this is used to cleanup resources acquired during the authentication exchange
     */
}

IScyllaDbCoreInterface::_IScyllaDb::~_IScyllaDb() {
    cass_future_free(m_pCassFuture);
    cass_cluster_free(m_pCassCluster);
    cass_session_free(m_pCassSession);
}

void IScyllaDbCoreInterface::_IScyllaDb::print_error(CassFuture* pCassFuture, const char* info) {
    const char* message;
    size_t messageSize;

    cass_future_error_message(pCassFuture, &message, &messageSize);

    std::fprintf(stderr, "\nERROR: \"%s\":\n%.*s\n", info, (int)messageSize, message);
}

int32_t IScyllaDbCoreInterface::_IScyllaDb::initialize_constructor(const scylladb_connection_t& connection, const char* extra_info) {
    IScyllaDbCoreInterface::m_connection.host = connection.host;
    IScyllaDbCoreInterface::m_connection.username = connection.username;
    IScyllaDbCoreInterface::m_connection.password = connection.password;

    IScyllaDbCoreInterface::m_connection.keyspace = connection.keyspace;
    IScyllaDbCoreInterface::m_connection.strategy = connection.strategy;

    IScyllaDbCoreInterface::m_connection.multiple_datacenters = connection.multiple_datacenters;

    IScyllaDbCoreInterface::m_connection.factors_configs = connection.factors_configs;
    IScyllaDbCoreInterface::m_connection.factors_configs_extra = connection.factors_configs_extra;

    if (m_pCassCluster == nullptr) {
        m_pCassCluster = cass_cluster_new();
    }
    if (m_pCassSession == nullptr) {
        m_pCassSession = cass_session_new();
    }

    behh::auth_basic_t* pAuth = new behh::auth_basic_t({IScyllaDbCoreInterface::m_connection.username.c_str(),
                                                        IScyllaDbCoreInterface::m_connection.password.c_str()});

    CassAuthenticatorCallbacks authCallbacks = {
        IScyllaDbCoreInterface::OnAuth::initialize,
        IScyllaDbCoreInterface::OnAuth::challenge,
        IScyllaDbCoreInterface::OnAuth::success,
        IScyllaDbCoreInterface::OnAuth::cleanup};

    IScyllaDbCoreInterface::m_connection.auth_mode = connection.auth_mode;

    bool error = false;

    switch (IScyllaDbCoreInterface::m_connection.auth_mode) {
    case SCYLLADB_AUTH_MODE_ALLOW_ALL_AUTHENTICATOR: {
        cass_cluster_set_contact_points(m_pCassCluster, IScyllaDbCoreInterface::m_connection.host.c_str());

        cass_cluster_set_authenticator_callbacks(m_pCassCluster, &authCallbacks, nullptr, pAuth);
    } break;

    case SCYLLADB_AUTH_MODE_PASSWORD_AUTHENTICATOR: {
        cass_cluster_set_contact_points(m_pCassCluster, IScyllaDbCoreInterface::m_connection.host.c_str());

        cass_cluster_set_authenticator_callbacks(m_pCassCluster, &authCallbacks, nullptr, pAuth);

        m_pCassFuture = cass_session_connect(m_pCassSession, m_pCassCluster);
    } break;

    default: {
        std::cerr << "ERROR: core interface for scylladb auth mode is default, it's not implemented\n";
        error = true;
    } break;
    }

    if (error) {
        return -1;
    }

    if (cass_future_error_code(m_pCassFuture) != CASS_OK) {
        std::cerr << "ERROR: \"IScyllaDbCoreInterface::_IScyllaDb::initialize_constructor\" fail to make connection\n";
        print_error(m_pCassFuture, extra_info);
        return -2;
    }

#if BEHH_IS_DEBUG
    (std::strlen(extra_info) > 0)
        ? std::cout << "DEBUG: \
        \"IScyllaDbCoreInterface::_IScyllaDb::initialize_constructor\" connected: "
                    << extra_info << "\n"
        : std::cout << "DEBUG: \
        \"IScyllaDbCoreInterface::_IScyllaDb::initialize_constructor\" connected ( extra_info is not provided )\n";
#endif // BEHH_IS_DEBUG

    return 1;
}

std::string IScyllaDbCoreInterface::_IScyllaDb::generate_uuid_v1_as_string() {
    CassUuid uuid;
    CassUuidGen* pUuid = cass_uuid_gen_new();
    char uuid_str[CASS_UUID_STRING_LENGTH];

    cass_uuid_gen_time(pUuid, &uuid);
    cass_uuid_string(uuid, uuid_str);

    cass_uuid_gen_free(pUuid);

    return std::string(uuid_str);
}

CassUuid IScyllaDbCoreInterface::_IScyllaDb::generate_uuid_v1_as_uuid() {
    CassUuid uuid;
    CassUuidGen* pUuid = cass_uuid_gen_new();

    cass_uuid_gen_time(pUuid, &uuid);

    cass_uuid_gen_free(pUuid);

    return uuid;
}

std::string IScyllaDbCoreInterface::_IScyllaDb::generate_uuid_v4_as_string() {
    CassUuid uuid;
    CassUuidGen* pUuid = cass_uuid_gen_new();
    char uuid_str[CASS_UUID_STRING_LENGTH];

    cass_uuid_gen_random(pUuid, &uuid);
    cass_uuid_string(uuid, uuid_str);

    cass_uuid_gen_free(pUuid);

    return std::string(uuid_str);
}

CassUuid IScyllaDbCoreInterface::_IScyllaDb::generate_uuid_v4_as_uuid() {
    CassUuid uuid;
    CassUuidGen* pUuid = cass_uuid_gen_new();

    cass_uuid_gen_random(pUuid, &uuid);

    cass_uuid_gen_free(pUuid);

    return uuid;
}

std::string IScyllaDbCoreInterface::_IScyllaDb::convert_uuid_to_string(const CassUuid& uuid_cass) {
    std::string result;
    CassUuid _uuid_cass = uuid_cass;
    char uuid[CASS_UUID_STRING_LENGTH];

    cass_uuid_string(_uuid_cass, uuid);

    result = std::string(uuid);

    cass_uuid_from_string(std::string(uuid).c_str(), &_uuid_cass);

    return result;
}

CassUuid IScyllaDbCoreInterface::_IScyllaDb::convert_string_to_uuid(const std::string& uuid_string) {
    CassUuid result;

    char uuid[CASS_UUID_STRING_LENGTH];

    std::string _uuid_string = uuid_string;

    cass_uuid_from_string(_uuid_string.c_str(), &result);

    cass_uuid_string(result, uuid);

    return result;
}

CassError IScyllaDbCoreInterface::_IScyllaDb::execute_cqlsh(CassSession* pCassSession, const char* query, const char* note) {
    auto pStatement = cass_statement_new(query, 0);

    auto pFuture = cass_session_execute(pCassSession, pStatement);
    cass_future_wait(pFuture);

    auto status = cass_future_error_code(pFuture);

    if (status != CASS_OK) {
#if BEHH_IS_DEBUG
        // std::cout << "DEBUG: execute_cqlsh: fail, note \"" << note << "\"\n";
        (std::strlen(note) > 0)
            ? std::cout << "DEBUG: execute_cqlsh: fail, note: " << note << "\n"
            : std::cout << "DEBUG: execute_cqlsh: fail ( note is not provided )\n";
#endif // BEHH_IS_DEBUG
        print_error(pFuture, note);
        return status;
    } else {
#if BEHH_IS_DEBUG
        (std::strlen(note) > 0)
            ? std::cout << "DEBUG: execute_cqlsh: ok, note: " << note << "\n"
            : std::cout << "DEBUG: execute_cqlsh: ok ( note is not provided )\n";
#endif // BEHH_IS_DEBUG
    }

    cass_future_free(pFuture);
    cass_statement_free(pStatement);

    return status;
}
#endif // BEHH_USING_SCYLLADB
} // namespace behh

#if BEHH_COMPILER_GNU
#pragma GCC diagnostic pop
#endif // BEHH_COMPILER_GNU
#if BEHH_COMPILER_MSVC
#endif // BEHH_COMPILER_MSVC
#if BEHH_COMPILER_CLANG
#pragma clang diagnostic pop
#endif // BEHH_COMPILER_CLANG

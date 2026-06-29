#include <behh/pch.hh>

#if BEHH_USING_COUCHBASE_CXX_CLIENT
#include <behh/functions/date_and_time.hh>
#include <behh/functions/utility.hh>
#include <behh/interfaces/icouchbase_core_interface.hh>

#include <spdlog/fmt/bundled/core.h>

#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <system_error>
#include <thread>

// main config file for this test
// if doesn't exists, copy the "test_database_couchbase_rw.json.debug" and rename to "test_database_couchbase_rw.json"
INLNSTTCCNST std::string CONFIG_FILE = BEHH_TESTS_DIR "/test_database_couchbase_rw.json";

std::mutex print_mutex;
class BasicDbTable : public behh::ICouchbaseCoreInterface {
private:
    behh::couchbase_connection_t m_conn;

public:
    INLNSTTCCNST std::string TABLE_NAME = "test_rw"; // this is equivalent to collection

    /////////////////////////////////////////////////////////////////

    // this is couchbase related table document, similar to cols in postgresql & scylladb columns key
    struct doc_key {
        struct pk {
            INLNSTTCCNST std::string id = "id";
        };
        INLNSTTCCNST std::string random_text = "random_text";
        INLNSTTCCNST std::string random_integer = "random_integer";
        INLNSTTCCNST std::string random_big_integer = "random_big_integer";
        INLNSTTCCNST std::string random_float = "random_float";
        INLNSTTCCNST std::string random_double = "random_double";
        INLNSTTCCNST std::string created_timestamp = "created_timestamp";
        INLNSTTCCNST std::string created_timestring = "created_timestring";
    };

    struct table_data {
        std::string id;
        std::string random_text;
        int32_t random_integer;
        int64_t random_big_integer;
        float32_t random_float;
        float64_t random_double;
        int64_t created_timestamp;
        std::string created_timestring;
    }; // struct table_data

    /////////////////////////////////////////////////////////////////

    BasicDbTable() {
        const auto CONFIG = behh::utility_functions::jsoncpp::from_json_file(CONFIG_FILE);

        behh::couchbase_connection_t _conn;

        _conn.host = CONFIG["host"].asString();
        _conn.username = CONFIG["username"].asString();
        _conn.password = CONFIG["password"].asString();
        _conn.scope_name = CONFIG["scope_name"].asString();
        _conn.collection_name = TABLE_NAME;

        _conn.cluster_bucket_name = CONFIG["cluster"]["bucket_name"].asString();

        // copy it first
        m_conn = _conn;

        // ICouchbase.initialize_constructor(_conn, 1); // trace
        ICouchbase.initialize_constructor(_conn, 6); // no log
    }
    ~BasicDbTable() {
        //
    }

    /////////////////////////////////////////////////////////////////

    // equivalent initialize_bucket
    void initialize_table_index() {
        /* not implemented */
    }

    /////////////////////////////////////////////////////////////////

    void generate_random_data(std::atomic<uint64_t>& count_error_ref, std::atomic<uint64_t>& count_iter_ref, std::atomic<uint64_t>& count_collide_ref, const uint64_t& max_iter) {
        auto& count_error = reinterpret_cast<std::atomic<uint64_t>&>(count_error_ref);
        auto& count_iter = reinterpret_cast<std::atomic<uint64_t>&>(count_iter_ref);
        auto& count_collide = reinterpret_cast<std::atomic<uint64_t>&>(count_collide_ref);

        std::lock_guard<std::mutex> lock1(print_mutex);

        table_data data;
        data.id = behh::utility_functions::generate::uuid::v4();
        data.random_text = behh::utility_functions::generate::random_alphanumeric_with_special_character(32);
        data.random_integer = behh::utility_functions::generate::random_number(10000000, 99999999);
        data.random_big_integer = behh::utility_functions::generate::random_number(10000000000000, 99999999999999);
        data.random_float = behh::utility_functions::generate::random_number(100.00000f, 999.99999f);
        data.random_double = behh::utility_functions::generate::random_number(100000.00000000, 999999.99999999);
        data.created_timestamp = behh::date_and_time_functions::utc::YYYYMMDDhhmmss::to_millis_now();
        data.created_timestring = behh::date_and_time_functions::utc::YYYYMMDDhhmmss::to_millis_string(data.created_timestamp);

        // CHECK: cluster
        auto cluster_error = ICouchbase.get_cluster_error();

        uint64_t error_iter = 0;
        if (cluster_error) {
            error_iter += 1;
            error_iter = count_error.fetch_add(1, std::memory_order_relaxed) + 1;
        }

        // TMP: hold
        auto collection = ICouchbase.get_cluster_bucket_scope_collection();

        auto [err, resp] = collection.upsert(
                                         // primary key
                                         data.id,
                                         tao::json::value{
                                             {doc_key::random_text, data.random_text},
                                             {doc_key::random_integer, data.random_integer},
                                             {doc_key::random_big_integer, data.random_big_integer},
                                             {doc_key::random_float, data.random_float},
                                             {doc_key::random_double, data.random_double},
                                             {doc_key::created_timestamp, data.created_timestamp},
                                             {doc_key::created_timestring, data.created_timestring},
                                         },
                                         {/* n/a */})
                               .get();

        if (err.ec()) {
            error_iter += 1;
            error_iter = count_error.fetch_add(1, std::memory_order_relaxed) + 1;
        }

        uint64_t current_iter = count_iter.fetch_add(1, std::memory_order_relaxed) + 1;
        if (count_iter <= max_iter) {
            std::cout << "\r         \r";
            std::cout << "-- note: current iter is " << current_iter
                      << " with " << count_error.load(std::memory_order_relaxed) << " errors "
                      << count_collide.load(std::memory_order_relaxed) << " collides" << std::flush;
        }

        if (count_iter == max_iter) {
            std::cout << "\n";
        }
        std::cout << std::flush;
    }

    // this function will:
    // - drop collection
    void cleanup() {
        auto collection = ICouchbase.get_bucket_collection_manager();

        auto delres = collection.drop_collection(m_conn.scope_name, m_conn.collection_name).get();

        std::cout << "cleanup: " << delres.ec().message() << "\n";
    }
};
#endif // BEHH_USING_COUCHBASE_CXX_CLIENT

int main() {
#if BEHH_USING_COUCHBASE_CXX_CLIENT
    // base data table
    BasicDbTable table;
    table.initialize_table_index();

    std::atomic<uint64_t> count_iter(0), count_error(0), count_collide(0);

    const uint64_t MAX_ITER_WRITE = 100;
    // const uint64_t MAX_ITER_READ = 100;
    // const uint64_t NUM_THREADS = std::thread::hardware_concurrency();

    std::cout << "start write\n";
    {
        std::vector<std::future<void>> futures;

        auto start = std::chrono::high_resolution_clock::now();

        futures.reserve(MAX_ITER_WRITE);

        for (uint64_t i = 0; i < MAX_ITER_WRITE; ++i) {
            futures.push_back(std::async(
                std::launch::async,
                [&table, &count_error, &count_iter, &count_collide, &MAX_ITER_WRITE]() {
                    table.generate_random_data(count_error, count_iter, count_collide, MAX_ITER_WRITE);
                }));
        }

        // for (auto& future : futures) {
        //     future.get();
        // }

        //
        /*
        @prothegee (cpu 12 ram 32 gb) at 100 iter:
            - for 100 with get   : 0.16340700 to 0.17328500 seconds
            - for 100 without get: 0.00860249 to 0.00932439 seconds
        */
        //

        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float64_t> duration = end - start;

        uint64_t errors = count_error.load();
        uint64_t collides = count_collide.load();

        if (errors > 0 || count_collide > 0) {
            std::cout << "\n-- note: found " << errors << " error & " << collides << " collide, while executing in " << duration.count() << "seconds\n";
        } else {
            std::cout << "\n-- note: no error & collide found in " << duration.count() << " seconds\n";
        }
    }

    std::cout << "skipped: start read\n";
    {
        // tba
    }

    // cleanup?
    table.cleanup();
#endif // BEHH_USING_COUCHBASE_CXX_CLIENT
    return 0;
}
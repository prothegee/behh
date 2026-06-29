#include <behh/config.hh>

#if BEHH_USING_COUCHBASE_CXX_CLIENT
#include <couchbase/cluster.hxx>
#include <couchbase/codec/tao_json_serializer.hxx>
#include <couchbase/logger.hxx>

#include <spdlog/fmt/bundled/core.h>

#include <couchbase/fmt/error.hxx>
#endif // BEHH_USING_COUCHBASE_CXX_CLIENT

#include <iostream>
#include <system_error>

namespace behh {
} // namespace behh

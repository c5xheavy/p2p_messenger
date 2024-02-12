#include <catch2/catch.hpp>

#include "../src/login_hasher.h"
#include "../src/static_file_ip_resolver.h"

TEST_CASE("Static File Ip Resolving") {

    StaticFileIpResolver static_file_ip_resolver{"../tests/static/login_to_ip.txt"};

    SECTION("successful resolve") {
        REQUIRE(static_file_ip_resolver.Resolve(LoginHasher::Hash("amir")).value_or("not found") == "127.0.0.1");
    }
    
    SECTION("unsuccessful resolve") {
        REQUIRE(static_file_ip_resolver.Resolve(LoginHasher::Hash("not a login")).value_or("not found") == "not found");
    }
}

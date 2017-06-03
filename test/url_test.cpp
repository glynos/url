// Copyright 2010 Jeroen Habraken.
// Copyright 2009-2017 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <network/uri/whatwg/url.hpp>
#include <algorithm>
#include <memory>
#include <map>
#include <set>
#include <unordered_set>
#include "string_utility.hpp"

TEST(uri_test, construct_invalid_uri) {
  EXPECT_THROW(network::whatwg::url("I am not a valid URI."), network::uri_syntax_error);
}

TEST(uri_test, make_invalid_uri) {
  std::error_code ec;
  network::whatwg::url uri = network::whatwg::make_url("I am not a valid URI.", ec);
  EXPECT_TRUE(static_cast<bool>(ec));
}

TEST(uri_test, construct_uri_from_char_array) {
  EXPECT_NO_THROW(network::whatwg::url("http://www.example.com/"));
}

TEST(uri_test, construct_uri_starting_with_ipv4_like) {
  EXPECT_NO_THROW(network::whatwg::url("http://198.51.100.0.example.com/"));
}

TEST(uri_test, construct_uri_starting_with_ipv4_like_glued) {
  ASSERT_NO_THROW(network::whatwg::url("http://198.51.100.0example.com/"));
}

TEST(uri_test, construct_uri_like_short_ipv4) {
  EXPECT_NO_THROW(network::whatwg::url("http://198.51.100/"));
}

TEST(uri_test, construct_uri_like_long_ipv4) {
  EXPECT_NO_THROW(network::whatwg::url("http://198.51.100.0.255/"));
}

TEST(uri_test, make_uri_from_char_array) {
  std::error_code ec;
  network::whatwg::url uri = network::whatwg::make_url("http://www.example.com/", ec);
  EXPECT_FALSE(ec);
}

TEST(uri_test, construct_uri_from_wchar_t_array) {
  EXPECT_NO_THROW(network::whatwg::url(L"http://www.example.com/"));
}

TEST(uri_test, make_uri_from_wchar_t_array) {
  std::error_code ec;
  network::whatwg::url uri = network::whatwg::make_url(L"http://www.example.com/", ec);
  EXPECT_FALSE(ec);
}

TEST(uri_test, construct_uri_from_string) {
  EXPECT_NO_THROW(network::whatwg::url(std::string("http://www.example.com/")));
}

TEST(uri_test, make_uri_from_string) {
  std::error_code ec;
  network::whatwg::url uri = network::whatwg::make_url(std::string("http://www.example.com/"), ec);
  EXPECT_FALSE(ec);
}

TEST(uri_test, construct_uri_from_wstring) {
  EXPECT_NO_THROW(network::whatwg::url(std::wstring(L"http://www.example.com/")));
}

TEST(uri_test, make_uri_from_wstring) {
  std::error_code ec;
  network::whatwg::url uri = network::whatwg::make_url(std::wstring(L"http://www.example.com/"), ec);
  EXPECT_FALSE(ec);
}

TEST(uri_test, basic_uri_scheme_test) {
  network::whatwg::url instance("http://www.example.com/");
  ASSERT_TRUE(instance.has_scheme());
  EXPECT_EQ("http", instance.scheme());
}

TEST(uri_test, basic_uri_user_info_test) {
  network::whatwg::url instance("http://www.example.com/");
  EXPECT_FALSE(instance.has_user_info());
}

TEST(uri_test, basic_uri_host_test) {
  network::whatwg::url instance("http://www.example.com/");
  ASSERT_TRUE(instance.has_host());
  EXPECT_EQ("www.example.com", instance.host());
}

TEST(uri_test, basic_uri_port_test) {
  network::whatwg::url instance("http://www.example.com/");
  EXPECT_FALSE(instance.has_port());
}

TEST(uri_test, basic_uri_path_test) {
  network::whatwg::url instance("http://www.example.com/");
  ASSERT_TRUE(instance.has_path());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, basic_uri_query_test) {
  network::whatwg::url instance("http://www.example.com/");
  EXPECT_FALSE(instance.has_query());
}

TEST(uri_test, basic_uri_fragment_test) {
  network::whatwg::url instance("http://www.example.com/");
  EXPECT_FALSE(instance.has_fragment());
}

TEST(uri_test, basic_uri_value_semantics_test) {
  network::whatwg::url original;
  network::whatwg::url assigned;
  assigned = original;
  EXPECT_EQ(original, assigned);
  assigned = network::whatwg::url("http://www.example.com/");
  EXPECT_NE(original, assigned);
  network::whatwg::url copy(assigned);
  EXPECT_EQ(copy, assigned);
}

TEST(uri_test, full_uri_scheme_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("http", instance.scheme());
}

TEST(uri_test, full_uri_user_info_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("user", instance.user_info());
}

TEST(uri_test, full_uri_host_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("www.example.com", instance.host());
}

TEST(uri_test, full_uri_port_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("80", instance.port());
}

TEST(uri_test, full_uri_port_as_int_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ(80, instance.port<int>());
}

TEST(uri_test, full_uri_path_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("/path", instance.path());
}

TEST(uri_test, full_uri_query_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("query", instance.query());
}

TEST(uri_test, full_uri_fragment_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("fragment", instance.fragment());
}

TEST(uri_test, full_uri_range_scheme_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.has_scheme());
  EXPECT_EQ("http", instance.scheme());
}

TEST(uri_test, full_uri_range_user_info_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.has_user_info());
  EXPECT_EQ("user", instance.user_info());
}

TEST(uri_test, full_uri_range_host_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.has_host());
  EXPECT_EQ("www.example.com", instance.host());
}

TEST(uri_test, full_uri_range_port_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.has_port());
  EXPECT_EQ("80", instance.port());
}

TEST(uri_test, full_uri_range_path_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.has_path());
  EXPECT_EQ("/path", instance.path());
}

TEST(uri_test, full_uri_range_query_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.has_query());
  EXPECT_EQ("query", instance.query());
}

TEST(uri_test, full_uri_range_fragment_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.has_fragment());
  EXPECT_EQ("fragment", instance.fragment());
}

TEST(uri_test, uri_with_empty_query) {
  network::whatwg::url instance("http://example.com/?");
  ASSERT_TRUE(instance.has_query());
  EXPECT_EQ("", instance.query());
}

TEST(uri_test, mailto_test) {
  network::whatwg::url instance("mailto:john.doe@example.com");
  EXPECT_EQ("mailto", instance.scheme());
  EXPECT_EQ("john.doe@example.com", instance.path());
}

TEST(uri_test, file_test) {
  network::whatwg::url instance("file:///bin/bash");
  EXPECT_EQ("file", instance.scheme());
  EXPECT_EQ("/bin/bash", instance.path());
}

TEST(uri_test, file_path_has_host_bug_98) {
  network::whatwg::url instance("file:///bin/bash");
  EXPECT_TRUE(instance.has_scheme());
  EXPECT_FALSE(instance.has_user_info());
  EXPECT_TRUE(instance.has_host());
  EXPECT_FALSE(instance.has_port());
  EXPECT_TRUE(instance.has_path());
  EXPECT_FALSE(instance.has_query());
  EXPECT_FALSE(instance.has_fragment());
}

TEST(uri_test, xmpp_test) {
  network::whatwg::url instance("xmpp:example-node@example.com?message;subject=Hello%20World");
  EXPECT_EQ("xmpp", instance.scheme());
  EXPECT_EQ("example-node@example.com", instance.path());
  EXPECT_EQ("message;subject=Hello%20World", instance.query());
}

TEST(uri_test, ipv4_address_test) {
  network::whatwg::url instance("http://129.79.245.252/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("129.79.245.252", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv4_loopback_test) {
  network::whatwg::url instance("http://127.0.0.1/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("127.0.0.1", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_address_test_1) {
  network::whatwg::url instance("http://[1080:0:0:0:8:800:200C:417A]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[1080:0:0:0:8:800:200C:417A]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_address_test_2) {
  network::whatwg::url instance("http://[2001:db8:85a3:8d3:1319:8a2e:370:7348]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:db8:85a3:8d3:1319:8a2e:370:7348]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_address_test_3) {
  network::whatwg::url instance("http://[2001:db8:85a3:0:0:8a2e:370:7334]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:db8:85a3:0:0:8a2e:370:7334]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_address_test_4) {
  network::whatwg::url instance("http://[2001:db8:85a3::8a2e:370:7334]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:db8:85a3::8a2e:370:7334]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_address_test_5) {
  network::whatwg::url instance("http://[2001:0db8:0000:0000:0000:0000:1428:57ab]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:0db8:0000:0000:0000:0000:1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_address_test_6) {
  network::whatwg::url instance("http://[2001:0db8:0000:0000:0000::1428:57ab]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:0db8:0000:0000:0000::1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_address_test_7) {
  network::whatwg::url instance("http://[2001:0db8:0:0:0:0:1428:57ab]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:0db8:0:0:0:0:1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_address_test_8) {
  network::whatwg::url instance("http://[2001:0db8:0:0::1428:57ab]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:0db8:0:0::1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_address_test_9) {
  network::whatwg::url instance("http://[2001:0db8::1428:57ab]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:0db8::1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_address_test_10) {
  network::whatwg::url instance("http://[2001:db8::1428:57ab]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:db8::1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_address_test_11) {
  network::whatwg::url instance("http://[::ffff:0c22:384e]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[::ffff:0c22:384e]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_address_test_12) {
  network::whatwg::url instance("http://[fe80::]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[fe80::]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_address_test_13) {
  network::whatwg::url instance("http://[::ffff:c000:280]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[::ffff:c000:280]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_loopback_test) {
  network::whatwg::url instance("http://[::1]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[::1]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_loopback_test_1) {
  network::whatwg::url instance("http://[0000:0000:0000:0000:0000:0000:0000:0001]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[0000:0000:0000:0000:0000:0000:0000:0001]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_v4inv6_test_1) {
  network::whatwg::url instance("http://[::ffff:12.34.56.78]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[::ffff:12.34.56.78]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ipv6_v4inv6_test_2) {
  network::whatwg::url instance("http://[::ffff:192.0.2.128]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[::ffff:192.0.2.128]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, ftp_test) {
  network::whatwg::url instance("ftp://john.doe@ftp.example.com/");
  EXPECT_EQ("ftp", instance.scheme());
  EXPECT_EQ("john.doe", instance.user_info());
  EXPECT_EQ("ftp.example.com", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, news_test) {
  network::whatwg::url instance("news:comp.infosystems.www.servers.unix");
  EXPECT_EQ("news", instance.scheme());
  EXPECT_EQ("comp.infosystems.www.servers.unix", instance.path());
}

TEST(uri_test, tel_test) {
  network::whatwg::url instance("tel:+1-816-555-1212");
  EXPECT_EQ("tel", instance.scheme());
  EXPECT_EQ("+1-816-555-1212", instance.path());
}

TEST(uri_test, ldap_test) {
  network::whatwg::url instance("ldap://[2001:db8::7]/c=GB?objectClass?one");
  EXPECT_EQ("ldap", instance.scheme());
  EXPECT_EQ("[2001:db8::7]", instance.host());
  EXPECT_EQ("/c=GB", instance.path());
  EXPECT_EQ("objectClass?one", instance.query());
}

TEST(uri_test, urn_test) {
  network::whatwg::url instance("urn:oasis:names:specification:docbook:dtd:xml:4.1.2");
  EXPECT_EQ("urn", instance.scheme());
  EXPECT_EQ("oasis:names:specification:docbook:dtd:xml:4.1.2", instance.path());
}

TEST(uri_test, svn_ssh_test) {
  network::whatwg::url instance("svn+ssh://example.com/");
  EXPECT_EQ("svn+ssh", instance.scheme());
  EXPECT_EQ("example.com", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(uri_test, copy_constructor_test) {
  network::whatwg::url instance("http://www.example.com/");
  network::whatwg::url copy = instance;
  EXPECT_EQ(instance, copy);
}

TEST(uri_test, assignment_test) {
  network::whatwg::url instance("http://www.example.com/");
  network::whatwg::url copy;
  copy = instance;
  EXPECT_EQ(instance, copy);
}

TEST(uri_test, swap_test) {
  network::whatwg::url original("http://example.com/path/to/file.txt");
  network::whatwg::url instance("file:///something/different/");
  original.swap(instance);

  ASSERT_TRUE(original.has_scheme());
  ASSERT_TRUE(original.has_host());
  ASSERT_TRUE(original.has_path());
  EXPECT_EQ("file", original.scheme());
  EXPECT_EQ("", original.host());
  EXPECT_EQ("/something/different/", original.path());

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_host());
  ASSERT_TRUE(instance.has_path());
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("example.com", instance.host());
  EXPECT_EQ("/path/to/file.txt", instance.path());
}

TEST(uri_test, authority_test) {
  network::whatwg::url instance("http://user@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.has_authority());
  EXPECT_EQ("user@www.example.com:80", instance.authority());
}

TEST(uri_test, partial_authority_test) {
  network::whatwg::url instance("http://www.example.com/path?query#fragment");
  ASSERT_TRUE(instance.has_authority());
  EXPECT_EQ("www.example.com", instance.authority());
}

TEST(uri_test, range_test) {
  const std::string url("http://www.example.com/");
  network::whatwg::url instance(url);
  EXPECT_TRUE(std::equal(std::begin(instance), std::end(instance),
			 std::begin(url)));
}

TEST(uri_test, issue_104_test) {
  // https://github.com/cpp-netlib/cpp-netlib/issues/104
  std::unique_ptr<network::whatwg::url> instance(new network::whatwg::url("http://www.example.com/"));
  network::whatwg::url copy = *instance;
  instance.reset();
  EXPECT_EQ("http", copy.scheme());
}

TEST(uri_test, uri_set_test) {
  std::set<network::whatwg::url> uri_set;
  uri_set.insert(network::whatwg::url("http://www.example.com/"));
  EXPECT_FALSE(uri_set.empty());
  EXPECT_EQ(network::whatwg::url("http://www.example.com/"), (*std::begin(uri_set)));
}

TEST(uri_test, uri_unordered_set_test) {
  std::unordered_set<network::whatwg::url> uri_set;
  uri_set.insert(network::whatwg::url("http://www.example.com/"));
  EXPECT_FALSE(uri_set.empty());
  EXPECT_EQ(network::whatwg::url("http://www.example.com/"), (*std::begin(uri_set)));
}

TEST(uri_test, empty_uri) {
  network::whatwg::url instance;
  EXPECT_TRUE(instance.empty());
}

TEST(uri_test, empty_uri_has_no_scheme) {
  network::whatwg::url instance;
  EXPECT_FALSE(instance.has_scheme());
}

TEST(uri_test, empty_uri_has_no_user_info) {
  network::whatwg::url instance;
  EXPECT_FALSE(instance.has_user_info());
}

TEST(uri_test, empty_uri_has_no_host) {
  network::whatwg::url instance;
  EXPECT_FALSE(instance.has_host());
}

TEST(uri_test, empty_uri_has_no_port) {
  network::whatwg::url instance;
  EXPECT_FALSE(instance.has_port());
}

TEST(uri_test, empty_uri_has_no_path) {
  network::whatwg::url instance;
  EXPECT_FALSE(instance.has_path());
}

TEST(uri_test, empty_uri_has_no_query) {
  network::whatwg::url instance;
  EXPECT_FALSE(instance.has_query());
}

TEST(uri_test, empty_uri_has_no_fragment) {
  network::whatwg::url instance;
  EXPECT_FALSE(instance.has_fragment());
}

TEST(uri_test, http_is_absolute) {
  network::whatwg::url instance("http://www.example.com/");
  EXPECT_TRUE(instance.is_absolute());
}

TEST(uri_test, mailto_has_no_user_info) {
  network::whatwg::url instance("mailto:john.doe@example.com");
  EXPECT_FALSE(instance.has_user_info());
}

TEST(uri_test, mailto_has_no_host) {
  network::whatwg::url instance("mailto:john.doe@example.com");
  EXPECT_FALSE(instance.has_host());
}

TEST(uri_test, mailto_has_no_port) {
  network::whatwg::url instance("mailto:john.doe@example.com");
  EXPECT_FALSE(instance.has_port());
}

TEST(uri_test, mailto_has_no_authority) {
  network::whatwg::url instance("mailto:john.doe@example.com");
  EXPECT_FALSE(instance.has_authority());
}

TEST(uri_test, http_is_not_opaque) {
  network::whatwg::url instance("http://www.example.com/");
  EXPECT_FALSE(instance.is_opaque());
}

TEST(uri_test, file_is_not_opaque) {
  network::whatwg::url instance("file:///bin/bash");
  EXPECT_FALSE(instance.is_opaque());
}

TEST(uri_test, mailto_is_absolute) {
  network::whatwg::url instance("mailto:john.doe@example.com");
  EXPECT_TRUE(instance.is_absolute());
}

TEST(uri_test, mailto_is_opaque) {
  network::whatwg::url instance("mailto:john.doe@example.com");
  EXPECT_TRUE(instance.is_opaque());
}

TEST(uri_test, whitespace_no_throw) {
  EXPECT_NO_THROW(network::whatwg::url(" http://www.example.com/ "));
}

TEST(uri_test, whitespace_is_trimmed) {
  network::whatwg::url instance(" http://www.example.com/ ");
  EXPECT_EQ("http://www.example.com/", instance);
}

TEST(uri_test, unnormalized_invalid_path_doesnt_throw) {
  EXPECT_NO_THROW(network::whatwg::url("http://www.example.com/.."));
}

TEST(uri_test, unnormalized_invalid_path_is_valid) {
  network::whatwg::url instance("http://www.example.com/..");
  EXPECT_TRUE(instance.has_path());
}

TEST(uri_test, unnormalized_invalid_path_value) {
  network::whatwg::url instance("http://www.example.com/..");
  EXPECT_EQ("/..", instance.path());
}

TEST(uri_test, git) {
  network::whatwg::url instance("git://github.com/cpp-netlib/cpp-netlib.git");
  EXPECT_EQ("git", instance.scheme());
  EXPECT_EQ("github.com", instance.host());
  EXPECT_EQ("/cpp-netlib/cpp-netlib.git", instance.path());
}

TEST(uri_test, invalid_port_test) {
  EXPECT_THROW(network::whatwg::url("http://123.34.23.56:6662626/"), network::uri_syntax_error);
}

TEST(uri_test, valid_empty_port_test) {
  EXPECT_NO_THROW(network::whatwg::url("http://123.34.23.56:/"));
}

TEST(uri_test, empty_port_test) {
  network::whatwg::url instance("http://123.34.23.56:/");
  ASSERT_TRUE(instance.has_port());
  EXPECT_EQ("", instance.port());
}

TEST(uri_test, full_copy_uri_scheme_test) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = origin;
  EXPECT_EQ("http", instance.scheme());
}

TEST(uri_test, full_copy_uri_user_info_test) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = origin;
  EXPECT_EQ("user", instance.user_info());
}

TEST(uri_test, full_copy_uri_host_test) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = origin;
  EXPECT_EQ("www.example.com", instance.host());
}

TEST(uri_test, full_copy_uri_port_test) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = origin;
  EXPECT_EQ("80", instance.port());
}

TEST(uri_test, full_copy_uri_path_test) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = origin;
  EXPECT_EQ("/path", instance.path());
}

TEST(uri_test, full_copy_uri_query_test) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = origin;
  EXPECT_EQ("query", instance.query());
}

TEST(uri_test, full_copy_uri_fragment_test) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = origin;
  EXPECT_EQ("fragment", instance.fragment());
}

TEST(uri_test, full_move_uri_scheme_test) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = std::move(origin);
  EXPECT_EQ("http", instance.scheme());
}

TEST(uri_test, full_move_uri_user_info_test) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = std::move(origin);
  EXPECT_EQ("user", instance.user_info());
}

TEST(uri_test, full_move_uri_host_test) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = std::move(origin);
  EXPECT_EQ("www.example.com", instance.host());
}

TEST(uri_test, full_move_uri_port_test) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = std::move(origin);
  EXPECT_EQ("80", instance.port());
}

TEST(uri_test, full_move_uri_path_test) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = std::move(origin);
  EXPECT_EQ("/path", instance.path());
}

TEST(uri_test, full_move_uri_query_test) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = std::move(origin);
  EXPECT_EQ("query", instance.query());
}

TEST(uri_test, full_move_uri_fragment_test) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = std::move(origin);
  EXPECT_EQ("fragment", instance.fragment());
}

TEST(uri_test, mailto_uri_path) {
  network::whatwg::url origin("mailto:john.doe@example.com?query#fragment");
  network::whatwg::url instance = origin;
  EXPECT_EQ("john.doe@example.com", instance.path());
}

TEST(uri_test, mailto_uri_query) {
  network::whatwg::url origin("mailto:john.doe@example.com?query#fragment");
  network::whatwg::url instance = origin;
  EXPECT_EQ("query", instance.query());
}

TEST(uri_test, mailto_uri_fragment) {
  network::whatwg::url origin("mailto:john.doe@example.com?query#fragment");
  network::whatwg::url instance = origin;
  EXPECT_EQ("fragment", instance.fragment());
}

TEST(uri_test, opaque_uri_with_one_slash) {
  network::whatwg::url instance("scheme:/path/");
  EXPECT_TRUE(instance.is_opaque());
}

TEST(uri_test, opaque_uri_with_one_slash_scheme) {
  network::whatwg::url instance("scheme:/path/");
  EXPECT_EQ("scheme", instance.scheme());
}

TEST(uri_test, opaque_uri_with_one_slash_path) {
  network::whatwg::url instance("scheme:/path/");
  EXPECT_EQ("/path/", instance.path());
}

TEST(uri_test, opaque_uri_with_one_slash_query) {
  network::whatwg::url instance("scheme:/path/?query#fragment");
  EXPECT_EQ("query", instance.query());
}

TEST(uri_test, opaque_uri_with_one_slash_fragment) {
  network::whatwg::url instance("scheme:/path/?query#fragment");
  EXPECT_EQ("fragment", instance.fragment());
}

TEST(uri_test, opaque_uri_with_one_slash_copy) {
  network::whatwg::url origin("scheme:/path/");
  network::whatwg::url instance = origin;
  EXPECT_TRUE(instance.is_opaque());
}

TEST(uri_test, opaque_uri_with_one_slash_copy_query) {
  network::whatwg::url origin("scheme:/path/?query#fragment");
  network::whatwg::url instance = origin;
  EXPECT_EQ("query", instance.query());
}

TEST(uri_test, opaque_uri_with_one_slash_copy_fragment) {
  network::whatwg::url origin("scheme:/path/?query#fragment");
  network::whatwg::url instance = origin;
  EXPECT_EQ("fragment", instance.fragment());
}

TEST(uri_test, move_empty_uri_check_scheme) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = std::move(origin);
  EXPECT_FALSE(origin.has_scheme());
}

TEST(uri_test, move_empty_uri_check_user_info) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = std::move(origin);
  EXPECT_FALSE(origin.has_user_info());
}

TEST(uri_test, move_empty_uri_check_host) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = std::move(origin);
  EXPECT_FALSE(origin.has_host());
}

TEST(uri_test, move_empty_uri_check_port) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = std::move(origin);
  EXPECT_FALSE(origin.has_port());
}

TEST(uri_test, move_empty_uri_check_path) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = std::move(origin);
  EXPECT_FALSE(origin.has_path());
}

TEST(uri_test, move_empty_uri_check_query) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = std::move(origin);
  EXPECT_FALSE(origin.has_query());
}

TEST(uri_test, move_empty_uri_check_fragment) {
  network::whatwg::url origin("http://user@www.example.com:80/path?query#fragment");
  network::whatwg::url instance = std::move(origin);
  EXPECT_FALSE(origin.has_fragment());
}

TEST(uri_test, empty_username_in_user_info) {
  EXPECT_THROW(network::whatwg::url("ftp://:@localhost"), network::uri_syntax_error);
}

TEST(uri_test, uri_begins_with_a_colon) {
  EXPECT_THROW(network::whatwg::url("://example.com"), network::uri_syntax_error);
}

TEST(uri_test, uri_begins_with_a_number) {
  EXPECT_THROW(network::whatwg::url("3http://example.com"), network::uri_syntax_error);
}

TEST(uri_test, uri_scheme_contains_an_invalid_character) {
  EXPECT_THROW(network::whatwg::url("ht%tp://example.com"), network::uri_syntax_error);
}

TEST(uri_test, default_constructed_assignment_test) {
  network::whatwg::url instance("http://www.example.com/");
  instance = network::whatwg::url(); // <-- CRASHES HERE
  EXPECT_TRUE(instance.empty());
}

TEST(uri_test, opaque_path_no_double_slash) {
  network::whatwg::url instance("file:/path/to/something/");
  ASSERT_TRUE(instance.has_path());
  EXPECT_EQ("/path/to/something/", instance.path());
  EXPECT_TRUE(instance.is_opaque());
}

TEST(uri_test, non_opaque_path_has_double_slash) {
  network::whatwg::url instance("file:///path/to/something/");
  ASSERT_TRUE(instance.has_path());
  EXPECT_EQ("/path/to/something/", instance.path());
  EXPECT_FALSE(instance.is_opaque());
}

TEST(uri_test, query_iterator_with_no_query) {
  network::whatwg::url instance("http://example.com/");
  ASSERT_FALSE(instance.has_query());
  ASSERT_EQ(instance.query_begin(), instance.query_end());
}

TEST(uri_test, query_iterator_with_empty_query) {
  network::whatwg::url instance("http://example.com/?");
  ASSERT_TRUE(instance.has_query());
  EXPECT_EQ("", instance.query());
  EXPECT_EQ(instance.query_begin(), instance.query_end());
}

TEST(uri_test, query_iterator_with_single_kvp) {
  network::whatwg::url instance("http://example.com/?a=b");
  ASSERT_TRUE(instance.has_query());
  auto query_it = instance.query_begin();
  ASSERT_NE(query_it, instance.query_end());
  EXPECT_EQ("a", query_it->first);
  EXPECT_EQ("b", query_it->second);
  ++query_it;
  EXPECT_EQ(query_it, instance.query_end());
}

TEST(uri_test, query_iterator_with_two_kvps) {
  network::whatwg::url instance("http://example.com/?a=b&c=d");

  ASSERT_TRUE(instance.has_query());
  auto query_it = instance.query_begin();
  ASSERT_NE(query_it, instance.query_end());
  EXPECT_EQ("a", query_it->first);
  EXPECT_EQ("b", query_it->second);
  ++query_it;
  ASSERT_NE(query_it, instance.query_end());
  EXPECT_EQ("c", query_it->first);
  EXPECT_EQ("d", query_it->second);
  ++query_it;
  EXPECT_EQ(query_it, instance.query_end());
}

TEST(uri_test, query_iterator_with_two_kvps_using_semicolon_separator) {
  network::whatwg::url instance("http://example.com/?a=b;c=d");

  ASSERT_TRUE(instance.has_query());
  auto query_it = instance.query_begin();
  ASSERT_NE(query_it, instance.query_end());
  EXPECT_EQ("a", query_it->first);
  EXPECT_EQ("b", query_it->second);
  ++query_it;
  ASSERT_NE(query_it, instance.query_end());
  EXPECT_EQ("c", query_it->first);
  EXPECT_EQ("d", query_it->second);
  ++query_it;
  EXPECT_EQ(query_it, instance.query_end());
}

TEST(uri_test, query_iterator_with_key_and_no_value) {
  network::whatwg::url instance("http://example.com/?query");
  ASSERT_TRUE(instance.has_query());
  auto query_it = instance.query_begin();
  EXPECT_EQ("query", query_it->first);
  EXPECT_EQ("", query_it->second);
  ++query_it;
  EXPECT_EQ(query_it, instance.query_end());
}

TEST(uri_test, query_iterator_with_fragment) {
  network::whatwg::url instance("http://example.com/?a=b;c=d#fragment");
  ASSERT_TRUE(instance.has_query());
  ASSERT_NE(instance.query_begin(), instance.query_end());
  auto query_it = instance.query_begin();
  EXPECT_EQ("a", query_it->first);
  EXPECT_EQ("b", query_it->second);
  ++query_it;
  EXPECT_EQ("c", query_it->first);
  EXPECT_EQ("d", query_it->second);
  ++query_it;
  EXPECT_EQ(query_it, instance.query_end());
}

TEST(uri_test, copy_assignment_bug_98) {
  network::whatwg::url original("file:///path/to/file.txt");

  ASSERT_TRUE(original.has_scheme());
  ASSERT_FALSE(original.is_opaque());
  ASSERT_TRUE(original.has_host());
  ASSERT_TRUE(original.has_path());

  network::whatwg::url instance;
  instance = original;

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_host());
  ASSERT_TRUE(instance.has_path());
  EXPECT_EQ("file", instance.scheme());
  EXPECT_EQ("", instance.host());
  EXPECT_EQ("/path/to/file.txt", instance.path());
}

TEST(uri_test, copy_assignment_bug_98_2) {
  network::whatwg::url original("file:///path/to/file.txt?query=value#foo");

  network::whatwg::url instance;
  instance = original;

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_path());
  ASSERT_TRUE(instance.has_query());
  ASSERT_TRUE(instance.has_fragment());
  EXPECT_EQ("file", instance.scheme());
  EXPECT_EQ("/path/to/file.txt", instance.path());
  EXPECT_EQ("query=value", instance.query());
  EXPECT_EQ("foo", instance.fragment());
}

TEST(uri_test, copy_constructor_bug_98) {
  network::whatwg::url original("file:///path/to/file.txt?query=value#foo");

  network::whatwg::url instance(original);

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_path());
  ASSERT_TRUE(instance.has_query());
  ASSERT_TRUE(instance.has_fragment());
  EXPECT_EQ("file", instance.scheme());
  EXPECT_EQ("/path/to/file.txt", instance.path());
  EXPECT_EQ("query=value", instance.query());
  EXPECT_EQ("foo", instance.fragment());
}

TEST(uri_test, move_assignment_bug_98) {
  network::whatwg::url original("file:///path/to/file.txt?query=value#foo");

  network::whatwg::url instance;
  instance = std::move(original);

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_path());
  ASSERT_TRUE(instance.has_query());
  ASSERT_TRUE(instance.has_fragment());
  EXPECT_EQ("file", instance.scheme());
  EXPECT_EQ("/path/to/file.txt", instance.path());
  EXPECT_EQ("query=value", instance.query());
  EXPECT_EQ("foo", instance.fragment());
}

TEST(uri_test, move_constructor_bug_98) {
  network::whatwg::url original("file:///path/to/file.txt?query=value#foo");

  network::whatwg::url instance(std::move(original));

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_path());
  ASSERT_TRUE(instance.has_query());
  ASSERT_TRUE(instance.has_fragment());
  EXPECT_EQ("file", instance.scheme());
  EXPECT_EQ("/path/to/file.txt", instance.path());
  EXPECT_EQ("query=value", instance.query());
  EXPECT_EQ("foo", instance.fragment());
}

TEST(uri_test, http_copy_assignment_bug_98) {
  network::whatwg::url original("http://example.com/path/to/file.txt");

  network::whatwg::url instance;
  instance = original;

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_path());
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("/path/to/file.txt", instance.path());
}

TEST(uri_test, uri_has_host_bug_87) {
  EXPECT_THROW(network::whatwg::url("http://"), network::uri_syntax_error);
}

TEST(uri_test, uri_has_host_bug_87_2) {
  EXPECT_THROW(network::whatwg::url("http://user@"), network::uri_syntax_error);
}

TEST(uri_test, uri_has_host_bug_88) {
  network::whatwg::url instance("http://user@host");

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_user_info());
  ASSERT_TRUE(instance.has_host());
  ASSERT_FALSE(instance.has_port());
  ASSERT_TRUE(instance.has_path());
  ASSERT_FALSE(instance.has_query());
  ASSERT_FALSE(instance.has_fragment());

  EXPECT_EQ("host", instance.host().to_string());
}

TEST(uri_test, uri_has_host_bug_88_2) {
  network::whatwg::url instance("http://user@example.com");

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_user_info());
  ASSERT_TRUE(instance.has_host());
  ASSERT_FALSE(instance.has_port());
  ASSERT_TRUE(instance.has_path());
  ASSERT_FALSE(instance.has_query());
  ASSERT_FALSE(instance.has_fragment());

  EXPECT_EQ("example.com", instance.host().to_string());
}

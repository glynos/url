// Copyright 2010 Jeroen Habraken.
// Copyright 2009-2018 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <network/url.hpp>
#include <algorithm>
#include <memory>
#include <map>
#include <set>
#include <unordered_set>
#include "string_utility.hpp"

TEST(whatwg_url_test, construct_invalid_url) {
  EXPECT_THROW(skyr::url("I am not a valid url."), skyr::url_syntax_error);
}

TEST(whatwg_url_test, make_invalid_url) {
  std::error_code ec;
  skyr::url url = skyr::make_url("I am not a valid url.", ec);
  EXPECT_TRUE(static_cast<bool>(ec));
}

TEST(whatwg_url_test, construct_url_from_char_array) {
  EXPECT_NO_THROW(skyr::url("http://www.example.com/"));
}

TEST(whatwg_url_test, construct_url_starting_with_ipv4_like) {
  EXPECT_NO_THROW(skyr::url("http://198.51.100.0.example.com/"));
}

TEST(whatwg_url_test, construct_url_starting_with_ipv4_like_glued) {
  ASSERT_NO_THROW(skyr::url("http://198.51.100.0example.com/"));
}

TEST(whatwg_url_test, construct_url_like_short_ipv4) {
  EXPECT_NO_THROW(skyr::url("http://198.51.100/"));
}

TEST(whatwg_url_test, construct_url_like_long_ipv4) {
  EXPECT_NO_THROW(skyr::url("http://198.51.100.0.255/"));
}

TEST(whatwg_url_test, make_url_from_char_array) {
  std::error_code ec;
  skyr::url url = skyr::make_url("http://www.example.com/", ec);
  EXPECT_FALSE(ec);
}

TEST(whatwg_url_test, construct_url_from_wchar_t_array) {
  EXPECT_NO_THROW(skyr::url(L"http://www.example.com/"));
}

TEST(whatwg_url_test, make_url_from_wchar_t_array) {
  std::error_code ec;
  skyr::url url = skyr::make_url(L"http://www.example.com/", ec);
  EXPECT_FALSE(ec);
}

TEST(whatwg_url_test, construct_url_from_string) {
  EXPECT_NO_THROW(skyr::url(std::string("http://www.example.com/")));
}

TEST(whatwg_url_test, make_url_from_string) {
  std::error_code ec;
  skyr::url url = skyr::make_url(std::string("http://www.example.com/"), ec);
  EXPECT_FALSE(ec);
}

TEST(whatwg_url_test, construct_url_from_wstring) {
  EXPECT_NO_THROW(skyr::url(std::wstring(L"http://www.example.com/")));
}

TEST(whatwg_url_test, make_url_from_wstring) {
  std::error_code ec;
  skyr::url url = skyr::make_url(std::wstring(L"http://www.example.com/"), ec);
  EXPECT_FALSE(ec);
}

TEST(whatwg_url_test, basic_url_scheme_test) {
  skyr::url instance("http://www.example.com/");
  ASSERT_TRUE(instance.has_scheme());
  EXPECT_EQ("http", instance.scheme());
}

TEST(whatwg_url_test, basic_url_user_info_test) {
  skyr::url instance("http://www.example.com/");
  EXPECT_FALSE(instance.has_user_info());
}

TEST(whatwg_url_test, basic_url_host_test) {
  skyr::url instance("http://www.example.com/");
  ASSERT_TRUE(instance.has_host());
  EXPECT_EQ("www.example.com", instance.host());
}

TEST(whatwg_url_test, basic_url_port_test) {
  skyr::url instance("http://www.example.com/");
  EXPECT_FALSE(instance.has_port());
}

TEST(whatwg_url_test, basic_url_path_test) {
  skyr::url instance("http://www.example.com/");
  ASSERT_TRUE(instance.has_path());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, basic_url_query_test) {
  skyr::url instance("http://www.example.com/");
  EXPECT_FALSE(instance.has_query());
}

TEST(whatwg_url_test, basic_url_fragment_test) {
  skyr::url instance("http://www.example.com/");
  EXPECT_FALSE(instance.has_fragment());
}

TEST(whatwg_url_test, DISABLED_basic_url_value_semantics_test) {
  skyr::url original;
  skyr::url assigned;
  assigned = original;
  EXPECT_EQ(original, assigned);
  assigned = skyr::url("http://www.example.com/");
  EXPECT_NE(original, assigned);
  skyr::url copy(assigned);
  EXPECT_EQ(copy, assigned);
}

TEST(whatwg_url_test, full_url_scheme_test) {
  skyr::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("http", instance.scheme());
}

TEST(whatwg_url_test, full_url_user_info_test) {
  skyr::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("user", instance.user_info());
}

TEST(whatwg_url_test, full_url_host_test) {
  skyr::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("www.example.com", instance.host());
}

TEST(whatwg_url_test, full_url_port_test) {
  skyr::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("80", instance.port());
}

TEST(whatwg_url_test, full_url_port_as_int_test) {
  skyr::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ(80, instance.port<int>());
}

TEST(whatwg_url_test, full_url_path_test) {
  skyr::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("/path", instance.path());
}

TEST(whatwg_url_test, full_url_query_test) {
  skyr::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("?query", instance.query());
}

TEST(whatwg_url_test, full_url_fragment_test) {
  skyr::url instance("http://user@www.example.com:80/path?query#fragment");
  EXPECT_EQ("#fragment", instance.fragment());
}

TEST(whatwg_url_test, full_url_range_scheme_test) {
  skyr::url instance("http://user@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.has_scheme());
  EXPECT_EQ("http", instance.scheme());
}

TEST(whatwg_url_test, full_url_range_user_info_test) {
  skyr::url instance("http://user@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.has_user_info());
  EXPECT_EQ("user", instance.user_info());
}

TEST(whatwg_url_test, full_url_range_host_test) {
  skyr::url instance("http://user@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.has_host());
  EXPECT_EQ("www.example.com", instance.host());
}

TEST(whatwg_url_test, full_url_range_port_test) {
  skyr::url instance("http://user@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.has_port());
  EXPECT_EQ("80", instance.port());
}

TEST(whatwg_url_test, full_url_range_path_test) {
  skyr::url instance("http://user@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.has_path());
  EXPECT_EQ("/path", instance.path());
}

TEST(whatwg_url_test, full_url_range_query_test) {
  skyr::url instance("http://user@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.has_query());
  EXPECT_EQ("?query", instance.query());
}

TEST(whatwg_url_test, full_url_range_fragment_test) {
  skyr::url instance("http://user@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.has_fragment());
  EXPECT_EQ("#fragment", instance.fragment());
}

TEST(whatwg_url_test, url_with_empty_query) {
  skyr::url instance("http://example.com/?");
  ASSERT_TRUE(instance.has_query());
  EXPECT_EQ("?", instance.query());
}

TEST(whatwg_url_test, mailto_test) {
  skyr::url instance("mailto:john.doe@example.com");
  EXPECT_EQ("mailto:", instance.scheme());
  EXPECT_EQ("john.doe@example.com", instance.path());
}

TEST(whatwg_url_test, file_test) {
  skyr::url instance("file:///bin/bash");
  EXPECT_EQ("file:", instance.scheme());
  EXPECT_EQ("/bin/bash", instance.path());
}

TEST(whatwg_url_test, file_path_has_host_bug_98) {
  skyr::url instance("file:///bin/bash");
  EXPECT_TRUE(instance.has_scheme());
  EXPECT_FALSE(instance.has_user_info());
  EXPECT_TRUE(instance.has_host());
  EXPECT_FALSE(instance.has_port());
  EXPECT_TRUE(instance.has_path());
  EXPECT_FALSE(instance.has_query());
  EXPECT_FALSE(instance.has_fragment());
}

TEST(whatwg_url_test, xmpp_test) {
  skyr::url instance("xmpp:example-node@example.com?message;subject=Hello%20World");
  EXPECT_EQ("xmpp:", instance.scheme());
  EXPECT_EQ("example-node@example.com", instance.path());
  EXPECT_EQ("?message;subject=Hello%20World", instance.query());
}

TEST(whatwg_url_test, ipv4_address_test) {
  skyr::url instance("http://129.79.245.252/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("129.79.245.252", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv4_loopback_test) {
  skyr::url instance("http://127.0.0.1/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("127.0.0.1", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_address_test_1) {
  skyr::url instance("http://[1080:0:0:0:8:800:200C:417A]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[1080:0:0:0:8:800:200C:417A]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_address_test_2) {
  skyr::url instance("http://[2001:db8:85a3:8d3:1319:8a2e:370:7348]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:db8:85a3:8d3:1319:8a2e:370:7348]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_address_test_3) {
  skyr::url instance("http://[2001:db8:85a3:0:0:8a2e:370:7334]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:db8:85a3:0:0:8a2e:370:7334]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_address_test_4) {
  skyr::url instance("http://[2001:db8:85a3::8a2e:370:7334]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:db8:85a3::8a2e:370:7334]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_address_test_5) {
  skyr::url instance("http://[2001:0db8:0000:0000:0000:0000:1428:57ab]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:0db8:0000:0000:0000:0000:1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_address_test_6) {
  skyr::url instance("http://[2001:0db8:0000:0000:0000::1428:57ab]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:0db8:0000:0000:0000::1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_address_test_7) {
  skyr::url instance("http://[2001:0db8:0:0:0:0:1428:57ab]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:0db8:0:0:0:0:1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_address_test_8) {
  skyr::url instance("http://[2001:0db8:0:0::1428:57ab]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:0db8:0:0::1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_address_test_9) {
  skyr::url instance("http://[2001:0db8::1428:57ab]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:0db8::1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_address_test_10) {
  skyr::url instance("http://[2001:db8::1428:57ab]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[2001:db8::1428:57ab]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_address_test_11) {
  skyr::url instance("http://[::ffff:0c22:384e]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[::ffff:0c22:384e]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_address_test_12) {
  skyr::url instance("http://[fe80::]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[fe80::]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_address_test_13) {
  skyr::url instance("http://[::ffff:c000:280]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[::ffff:c000:280]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_loopback_test) {
  skyr::url instance("http://[::1]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[::1]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_loopback_test_1) {
  skyr::url instance("http://[0000:0000:0000:0000:0000:0000:0000:0001]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[0000:0000:0000:0000:0000:0000:0000:0001]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_v4inv6_test_1) {
  skyr::url instance("http://[::ffff:12.34.56.78]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[::ffff:12.34.56.78]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ipv6_v4inv6_test_2) {
  skyr::url instance("http://[::ffff:192.0.2.128]/");
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("[::ffff:192.0.2.128]", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, ftp_test) {
  skyr::url instance("ftp://john.doe@ftp.example.com/");
  EXPECT_EQ("ftp:", instance.scheme());
  EXPECT_EQ("john.doe", instance.user_info());
  EXPECT_EQ("ftp.example.com", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, news_test) {
  skyr::url instance("news:comp.infosystems.www.servers.unix");
  EXPECT_EQ("news:", instance.scheme());
  EXPECT_EQ("comp.infosystems.www.servers.unix", instance.path());
}

TEST(whatwg_url_test, tel_test) {
  skyr::url instance("tel:+1-816-555-1212");
  EXPECT_EQ("tel:", instance.scheme());
  EXPECT_EQ("+1-816-555-1212", instance.path());
}

TEST(whatwg_url_test, ldap_test) {
  skyr::url instance("ldap://[2001:db8::7]/c=GB?objectClass?one");
  EXPECT_EQ("ldap:", instance.scheme());
  EXPECT_EQ("[2001:db8::7]", instance.host());
  EXPECT_EQ("/c=GB", instance.path());
  EXPECT_EQ("?objectClass?one", instance.query());
}

TEST(whatwg_url_test, urn_test) {
  skyr::url instance("urn:oasis:names:specification:docbook:dtd:xml:4.1.2");
  EXPECT_EQ("urn:", instance.scheme());
  EXPECT_EQ("oasis:names:specification:docbook:dtd:xml:4.1.2", instance.path());
}

TEST(whatwg_url_test, svn_ssh_test) {
  skyr::url instance("svn+ssh://example.com/");
  EXPECT_EQ("svn+ssh:", instance.scheme());
  EXPECT_EQ("example.com", instance.host());
  EXPECT_EQ("/", instance.path());
}

TEST(whatwg_url_test, copy_constructor_test) {
  skyr::url instance("http://www.example.com/");
  skyr::url copy = instance;
  EXPECT_EQ(instance, copy);
}

TEST(whatwg_url_test, assignment_test) {
  skyr::url instance("http://www.example.com/");
  skyr::url copy;
  copy = instance;
  EXPECT_EQ(instance, copy);
}

TEST(whatwg_url_test, swap_test) {
  skyr::url original("http://example.com/path/to/file.txt");
  skyr::url instance("file:///something/different/");
  original.swap(instance);

  ASSERT_TRUE(original.has_scheme());
  ASSERT_TRUE(original.has_host());
  ASSERT_TRUE(original.has_path());
  EXPECT_EQ("file:", original.scheme());
  EXPECT_EQ("", original.host());
  EXPECT_EQ("/something/different/", original.path());

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_host());
  ASSERT_TRUE(instance.has_path());
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("example.com", instance.host());
  EXPECT_EQ("/path/to/file.txt", instance.path());
}

TEST(whatwg_url_test, range_test) {
  const std::string url("http://www.example.com/");
  skyr::url instance(url);
  EXPECT_TRUE(std::equal(std::begin(instance), std::end(instance),
			 std::begin(url)));
}

TEST(whatwg_url_test, issue_104_test) {
  // https://github.com/cpp-netlib/cpp-netlib/issues/104
  std::unique_ptr<skyr::url> instance(new skyr::url("http://www.example.com/"));
  skyr::url copy = *instance;
  instance.reset();
  EXPECT_EQ("http", copy.scheme());
}

TEST(whatwg_url_test, DISABLED_url_set_test) {
  std::set<skyr::url> url_set;
  url_set.insert(skyr::url("http://www.example.com/"));
  EXPECT_FALSE(url_set.empty());
  EXPECT_EQ(skyr::url("http://www.example.com/"), (*std::begin(url_set)));
}

TEST(whatwg_url_test, DISABLED_url_unordered_set_test) {
  std::unordered_set<skyr::url> url_set;
  url_set.insert(skyr::url("http://www.example.com/"));
  EXPECT_FALSE(url_set.empty());
  EXPECT_EQ(skyr::url("http://www.example.com/"), (*std::begin(url_set)));
}

TEST(whatwg_url_test, empty_url) {
  skyr::url instance;
  EXPECT_TRUE(instance.empty());
}

TEST(whatwg_url_test, empty_url_has_no_scheme) {
  skyr::url instance;
  EXPECT_FALSE(instance.has_scheme());
}

TEST(whatwg_url_test, empty_url_has_no_user_info) {
  skyr::url instance;
  EXPECT_FALSE(instance.has_user_info());
}

TEST(whatwg_url_test, empty_url_has_no_host) {
  skyr::url instance;
  EXPECT_FALSE(instance.has_host());
}

TEST(whatwg_url_test, empty_url_has_no_port) {
  skyr::url instance;
  EXPECT_FALSE(instance.has_port());
}

TEST(whatwg_url_test, empty_url_has_no_path) {
  skyr::url instance;
  EXPECT_FALSE(instance.has_path());
}

TEST(whatwg_url_test, empty_url_has_no_query) {
  skyr::url instance;
  EXPECT_FALSE(instance.has_query());
}

TEST(whatwg_url_test, empty_url_has_no_fragment) {
  skyr::url instance;
  EXPECT_FALSE(instance.has_fragment());
}

TEST(whatwg_url_test, http_is_absolute) {
  skyr::url instance("http://www.example.com/");
  EXPECT_TRUE(instance.is_absolute());
}

TEST(whatwg_url_test, mailto_has_no_user_info) {
  skyr::url instance("mailto:john.doe@example.com");
  EXPECT_FALSE(instance.has_user_info());
}

TEST(whatwg_url_test, mailto_has_no_host) {
  skyr::url instance("mailto:john.doe@example.com");
  EXPECT_FALSE(instance.has_host());
}

TEST(whatwg_url_test, mailto_has_no_port) {
  skyr::url instance("mailto:john.doe@example.com");
  EXPECT_FALSE(instance.has_port());
}

TEST(whatwg_url_test, http_is_not_opaque) {
  skyr::url instance("http://www.example.com/");
  EXPECT_FALSE(instance.is_opaque());
}

TEST(whatwg_url_test, file_is_not_opaque) {
  skyr::url instance("file:///bin/bash");
  EXPECT_FALSE(instance.is_opaque());
}

TEST(whatwg_url_test, mailto_is_absolute) {
  skyr::url instance("mailto:john.doe@example.com");
  EXPECT_TRUE(instance.is_absolute());
}

TEST(whatwg_url_test, mailto_is_opaque) {
  skyr::url instance("mailto:john.doe@example.com");
  EXPECT_TRUE(instance.is_opaque());
}

TEST(whatwg_url_test, whitespace_no_throw) {
  EXPECT_NO_THROW(skyr::url(" http://www.example.com/ "));
}

TEST(whatwg_url_test, whitespace_is_trimmed) {
  skyr::url instance(" http://www.example.com/ ");
  EXPECT_EQ("http://www.example.com/", instance);
}

TEST(whatwg_url_test, unnormalized_invalid_path_doesnt_throw) {
  EXPECT_NO_THROW(skyr::url("http://www.example.com/.."));
}

TEST(whatwg_url_test, unnormalized_invalid_path_is_valid) {
  skyr::url instance("http://www.example.com/..");
  EXPECT_TRUE(instance.has_path());
}

TEST(whatwg_url_test, unnormalized_invalid_path_value) {
  skyr::url instance("http://www.example.com/..");
  EXPECT_EQ("/..", instance.path());
}

TEST(whatwg_url_test, git) {
  skyr::url instance("git://github.com/cpp-netlib/cpp-netlib.git");
  EXPECT_EQ("git:", instance.scheme());
  EXPECT_EQ("github.com", instance.host());
  EXPECT_EQ("/cpp-netlib/cpp-netlib.git", instance.path());
}

TEST(whatwg_url_test, invalid_port_test) {
  EXPECT_THROW(skyr::url("http://123.34.23.56:6662626/"), skyr::url_syntax_error);
}

TEST(whatwg_url_test, valid_empty_port_test) {
  EXPECT_NO_THROW(skyr::url("http://123.34.23.56:/"));
}

TEST(whatwg_url_test, empty_port_test) {
  skyr::url instance("http://123.34.23.56:/");
  ASSERT_TRUE(instance.has_port());
  EXPECT_EQ("", instance.port());
}

TEST(whatwg_url_test, full_copy_url_scheme_test) {
  std::cout << "WOWZERS" << std::endl;
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  std::cout << "GADZOOKS" << std::endl;
  skyr::url instance = origin;
  EXPECT_EQ("http", instance.scheme());
}

TEST(whatwg_url_test, full_copy_url_user_info_test) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = origin;
  EXPECT_EQ("user", instance.user_info());
}

TEST(whatwg_url_test, full_copy_url_host_test) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = origin;
  EXPECT_EQ("www.example.com", instance.host());
}

TEST(whatwg_url_test, full_copy_url_port_test) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = origin;
  EXPECT_EQ("80", instance.port());
}

TEST(whatwg_url_test, full_copy_url_path_test) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = origin;
  EXPECT_EQ("/path", instance.path());
}

TEST(whatwg_url_test, full_copy_url_query_test) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = origin;
  EXPECT_EQ("?query", instance.query());
}

TEST(whatwg_url_test, full_copy_url_fragment_test) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = origin;
  EXPECT_EQ("#fragment", instance.fragment());
}

TEST(whatwg_url_test, full_move_url_scheme_test) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = std::move(origin);
  EXPECT_EQ("http", instance.scheme());
}

TEST(whatwg_url_test, full_move_url_user_info_test) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = std::move(origin);
  EXPECT_EQ("user", instance.user_info());
}

TEST(whatwg_url_test, full_move_url_host_test) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = std::move(origin);
  EXPECT_EQ("www.example.com", instance.host());
}

TEST(whatwg_url_test, full_move_url_port_test) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = std::move(origin);
  EXPECT_EQ("80", instance.port());
}

TEST(whatwg_url_test, full_move_url_path_test) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = std::move(origin);
  EXPECT_EQ("/path", instance.path());
}

TEST(whatwg_url_test, full_move_url_query_test) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = std::move(origin);
  EXPECT_EQ("?query", instance.query());
}

TEST(whatwg_url_test, full_move_url_fragment_test) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = std::move(origin);
  EXPECT_EQ("#fragment", instance.fragment());
}

TEST(whatwg_url_test, mailto_url_path) {
  skyr::url origin("mailto:john.doe@example.com?query#fragment");
  skyr::url instance = origin;
  EXPECT_EQ("john.doe@example.com", instance.path());
}

TEST(whatwg_url_test, mailto_url_query) {
  skyr::url origin("mailto:john.doe@example.com?query#fragment");
  skyr::url instance = origin;
  EXPECT_EQ("?query", instance.query());
}

TEST(whatwg_url_test, mailto_url_fragment) {
  skyr::url origin("mailto:john.doe@example.com?query#fragment");
  skyr::url instance = origin;
  EXPECT_EQ("#fragment", instance.fragment());
}

TEST(whatwg_url_test, DISABLED_opaque_url_with_one_slash) {
  skyr::url instance("scheme:/path/");
  EXPECT_TRUE(instance.is_opaque());
}

TEST(whatwg_url_test, DISABLED_opaque_url_with_one_slash_scheme) {
  skyr::url instance("scheme:/path/");
  EXPECT_EQ("scheme:", instance.scheme());
}

TEST(whatwg_url_test, DISABLED_opaque_url_with_one_slash_path) {
  skyr::url instance("scheme:/path/");
  EXPECT_EQ("/path/", instance.path());
}

TEST(whatwg_url_test, DISABLED_opaque_url_with_one_slash_query) {
  skyr::url instance("scheme:/path/?query#fragment");
  EXPECT_EQ("?query", instance.query());
}

TEST(whatwg_url_test, DISABLED_opaque_url_with_one_slash_fragment) {
  skyr::url instance("scheme:/path/?query#fragment");
  EXPECT_EQ("#fragment", instance.fragment());
}

TEST(whatwg_url_test, DISABLED_opaque_url_with_one_slash_copy) {
  skyr::url origin("scheme:/path/");
  skyr::url instance = origin;
  EXPECT_TRUE(instance.is_opaque());
}

TEST(whatwg_url_test, DISABLED_opaque_url_with_one_slash_copy_query) {
  skyr::url origin("scheme:/path/?query#fragment");
  skyr::url instance = origin;
  EXPECT_EQ("?query", instance.query());
}

TEST(whatwg_url_test, DISABLED_opaque_url_with_one_slash_copy_fragment) {
  skyr::url origin("scheme:/path/?query#fragment");
  skyr::url instance = origin;
  EXPECT_EQ("#fragment", instance.fragment());
}

TEST(whatwg_url_test, move_empty_url_check_scheme) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = std::move(origin);
  EXPECT_FALSE(origin.has_scheme());
}

TEST(whatwg_url_test, move_empty_url_check_user_info) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = std::move(origin);
  EXPECT_FALSE(origin.has_user_info());
}

TEST(whatwg_url_test, move_empty_url_check_host) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = std::move(origin);
  EXPECT_FALSE(origin.has_host());
}

TEST(whatwg_url_test, move_empty_url_check_port) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = std::move(origin);
  EXPECT_FALSE(origin.has_port());
}

TEST(whatwg_url_test, move_empty_url_check_path) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = std::move(origin);
  EXPECT_FALSE(origin.has_path());
}

TEST(whatwg_url_test, move_empty_url_check_query) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = std::move(origin);
  EXPECT_FALSE(origin.has_query());
}

TEST(whatwg_url_test, move_empty_url_check_fragment) {
  skyr::url origin("http://user@www.example.com:80/path?query#fragment");
  skyr::url instance = std::move(origin);
  EXPECT_FALSE(origin.has_fragment());
}

TEST(whatwg_url_test, DISABLED_empty_username_in_user_info) {
  skyr::url instance("ftp://:@localhost");
  ASSERT_TRUE(instance.has_user_info());
  EXPECT_EQ(":", instance.user_info());
  ASSERT_TRUE(instance.has_host());
  EXPECT_EQ("localhost", instance.host());
}

TEST(whatwg_url_test, url_begins_with_a_colon) {
  EXPECT_THROW(skyr::url("://example.com"), skyr::url_syntax_error);
}

TEST(whatwg_url_test, url_begins_with_a_number) {
  EXPECT_THROW(skyr::url("3http://example.com"), skyr::url_syntax_error);
}

TEST(whatwg_url_test, url_scheme_contains_an_invalid_character) {
  EXPECT_THROW(skyr::url("ht%tp://example.com"), skyr::url_syntax_error);
}

TEST(whatwg_url_test, default_constructed_assignment_test) {
  skyr::url instance("http://www.example.com/");
  instance = skyr::url(); // <-- CRASHES HERE
  EXPECT_TRUE(instance.empty());
}

TEST(whatwg_url_test, DISABLED_opaque_path_no_double_slash) {
  skyr::url instance("file:/path/to/something/");
  ASSERT_TRUE(instance.has_path());
  EXPECT_EQ("/path/to/something/", instance.path());
  EXPECT_TRUE(instance.is_opaque());
}

TEST(whatwg_url_test, non_opaque_path_has_double_slash) {
  skyr::url instance("file:///path/to/something/");
  ASSERT_TRUE(instance.has_path());
  EXPECT_EQ("/path/to/something/", instance.path());
  EXPECT_FALSE(instance.is_opaque());
}

TEST(whatwg_url_test, path_iterator_with_empty_path) {
  skyr::url instance("http://example.com/");
  ASSERT_TRUE(instance.has_path());
  EXPECT_NE(instance.path_begin(), instance.path_end());
}

TEST(whatwg_url_test, path_iterator_with_single_element) {
  skyr::url instance("http://example.com/a");
  ASSERT_TRUE(instance.has_path());
  auto path_it = instance.path_begin();
  ASSERT_NE(path_it, instance.path_end());
  EXPECT_EQ("a", *path_it);
  ++path_it;
  EXPECT_EQ(path_it, instance.path_end());
}

TEST(whatwg_url_test, path_iterator_with_two_elements) {
  skyr::url instance("http://example.com/a/b");

  ASSERT_TRUE(instance.has_path());
  auto path_it = instance.path_begin();
  ASSERT_NE(path_it, instance.path_end());
  EXPECT_EQ("a", *path_it);
  ++path_it;
  ASSERT_NE(path_it, instance.path_end());
  EXPECT_EQ("b", *path_it);
  ++path_it;
  EXPECT_EQ(path_it, instance.path_end());
}

TEST(whatwg_url_test, path_iterator_with_query) {
  skyr::url instance("http://example.com/a/b?query");
  ASSERT_TRUE(instance.has_path());
  ASSERT_NE(instance.path_begin(), instance.path_end());
  auto path_it = instance.path_begin();
  EXPECT_EQ("a", *path_it);
  ++path_it;
  EXPECT_EQ("b", *path_it);
  ++path_it;
  EXPECT_EQ(path_it, instance.path_end());
}

TEST(whatwg_url_test, path_iterator_with_fragment) {
  skyr::url instance("http://example.com/a/b#fragment");
  ASSERT_TRUE(instance.has_path());
  ASSERT_NE(instance.path_begin(), instance.path_end());
  auto path_it = instance.path_begin();
  EXPECT_EQ("a", *path_it);
  ++path_it;
  EXPECT_EQ("b", *path_it);
  ++path_it;
  EXPECT_EQ(path_it, instance.path_end());
}

TEST(whatwg_url_test, query_iterator_with_no_query) {
  skyr::url instance("http://example.com/");
  ASSERT_FALSE(instance.has_query());
  ASSERT_EQ(instance.query_begin(), instance.query_end());
}

TEST(whatwg_url_test, query_iterator_with_empty_query) {
  skyr::url instance("http://example.com/?");
  ASSERT_TRUE(instance.has_query());
  EXPECT_EQ("?", instance.query());
  EXPECT_EQ(instance.query_begin(), instance.query_end());
}

TEST(whatwg_url_test, query_iterator_with_single_kvp) {
  skyr::url instance("http://example.com/?a=b");
  ASSERT_TRUE(instance.has_query());
  auto query_it = instance.query_begin();
  ASSERT_NE(query_it, instance.query_end());
  EXPECT_EQ("a", query_it->first);
  EXPECT_EQ("b", query_it->second);
  ++query_it;
  EXPECT_EQ(query_it, instance.query_end());
}

TEST(whatwg_url_test, query_iterator_with_two_kvps) {
  skyr::url instance("http://example.com/?a=b&c=d");

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

TEST(whatwg_url_test, query_iterator_with_two_kvps_using_semicolon_separator) {
  skyr::url instance("http://example.com/?a=b;c=d");

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

TEST(whatwg_url_test, query_iterator_with_key_and_no_value) {
  skyr::url instance("http://example.com/?query");
  ASSERT_TRUE(instance.has_query());
  auto query_it = instance.query_begin();
  EXPECT_EQ("query", query_it->first);
  EXPECT_EQ("", query_it->second);
  ++query_it;
  EXPECT_EQ(query_it, instance.query_end());
}

TEST(whatwg_url_test, query_iterator_with_fragment) {
  skyr::url instance("http://example.com/?a=b;c=d#fragment");
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

TEST(whatwg_url_test, copy_assignment_bug_98) {
  skyr::url original("file:///path/to/file.txt");

  ASSERT_TRUE(original.has_scheme());
  ASSERT_FALSE(original.is_opaque());
  ASSERT_TRUE(original.has_host());
  ASSERT_TRUE(original.has_path());

  skyr::url instance;
  instance = original;

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_host());
  ASSERT_TRUE(instance.has_path());
  EXPECT_EQ("file:", instance.scheme());
  EXPECT_EQ("", instance.host());
  EXPECT_EQ("/path/to/file.txt", instance.path());
}

TEST(whatwg_url_test, copy_assignment_bug_98_2) {
  skyr::url original("file:///path/to/file.txt?query=value#foo");

  skyr::url instance;
  instance = original;

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_path());
  ASSERT_TRUE(instance.has_query());
  ASSERT_TRUE(instance.has_fragment());
  EXPECT_EQ("file:", instance.scheme());
  EXPECT_EQ("/path/to/file.txt", instance.path());
  EXPECT_EQ("?query=value", instance.query());
  EXPECT_EQ("#foo", instance.fragment());
}

TEST(whatwg_url_test, copy_constructor_bug_98) {
  skyr::url original("file:///path/to/file.txt?query=value#foo");

  skyr::url instance(original);

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_path());
  ASSERT_TRUE(instance.has_query());
  ASSERT_TRUE(instance.has_fragment());
  EXPECT_EQ("file:", instance.scheme());
  EXPECT_EQ("/path/to/file.txt", instance.path());
  EXPECT_EQ("?query=value", instance.query());
  EXPECT_EQ("#foo", instance.fragment());
}

TEST(whatwg_url_test, move_assignment_bug_98) {
  skyr::url original("file:///path/to/file.txt?query=value#foo");

  skyr::url instance;
  instance = std::move(original);

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_path());
  ASSERT_TRUE(instance.has_query());
  ASSERT_TRUE(instance.has_fragment());
  EXPECT_EQ("file:", instance.scheme());
  EXPECT_EQ("/path/to/file.txt", instance.path());
  EXPECT_EQ("?query=value", instance.query());
  EXPECT_EQ("#foo", instance.fragment());
}

TEST(whatwg_url_test, move_constructor_bug_98) {
  skyr::url original("file:///path/to/file.txt?query=value#foo");

  skyr::url instance(std::move(original));

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_path());
  ASSERT_TRUE(instance.has_query());
  ASSERT_TRUE(instance.has_fragment());
  EXPECT_EQ("file:", instance.scheme());
  EXPECT_EQ("/path/to/file.txt", instance.path());
  EXPECT_EQ("?query=value", instance.query());
  EXPECT_EQ("#foo", instance.fragment());
}

TEST(whatwg_url_test, http_copy_assignment_bug_98) {
  skyr::url original("http://example.com/path/to/file.txt");

  skyr::url instance;
  instance = original;

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_path());
  EXPECT_EQ("http", instance.scheme());
  EXPECT_EQ("/path/to/file.txt", instance.path());
}

TEST(whatwg_url_test, url_has_host_bug_87) {
  EXPECT_THROW(skyr::url("http://"), skyr::url_syntax_error);
}

TEST(whatwg_url_test, url_has_host_bug_87_2) {
  EXPECT_THROW(skyr::url("http://user@"), skyr::url_syntax_error);
}

TEST(whatwg_url_test, url_has_host_bug_88) {
  skyr::url instance("http://user@host");

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_user_info());
  ASSERT_TRUE(instance.has_host());
  ASSERT_FALSE(instance.has_port());
  ASSERT_TRUE(instance.has_path());
  ASSERT_FALSE(instance.has_query());
  ASSERT_FALSE(instance.has_fragment());

  EXPECT_EQ("host", instance.host().to_string());
}

TEST(whatwg_url_test, url_has_host_bug_88_2) {
  skyr::url instance("http://user@example.com");

  ASSERT_TRUE(instance.has_scheme());
  ASSERT_TRUE(instance.has_user_info());
  ASSERT_TRUE(instance.has_host());
  ASSERT_FALSE(instance.has_port());
  ASSERT_TRUE(instance.has_path());
  ASSERT_FALSE(instance.has_query());
  ASSERT_FALSE(instance.has_fragment());

  EXPECT_EQ("example.com", instance.host().to_string());
}

TEST(whatwg_url_test, http_scheme_is_special) {
  skyr::url instance("http://example.com");
  ASSERT_TRUE(instance.is_special());
}

TEST(whatwg_url_test, https_scheme_is_special) {
  skyr::url instance("http://example.com");
  ASSERT_TRUE(instance.is_special());
}

TEST(whatwg_url_test, file_scheme_is_special) {
  skyr::url instance("http://example.com");
  ASSERT_TRUE(instance.is_special());
}

TEST(whatwg_url_test, git_scheme_is_not_special) {
  skyr::url instance("git://example.com");
  ASSERT_FALSE(instance.is_special());
}

TEST(whatwg_url_test, http_default_port_is_80) {
  auto port = skyr::url::default_port("http");
  ASSERT_TRUE(port);
  ASSERT_EQ(80, *port);
}

TEST(whatwg_url_test, https_default_port_is_443) {
  auto port = skyr::url::default_port("https");
  ASSERT_TRUE(port);
  ASSERT_EQ(443, *port);
}

TEST(whatwg_url_test, file_default_port_doesnt_exist) {
  auto port = skyr::url::default_port("file");
  ASSERT_FALSE(port);
}

TEST(whatwg_url_test, git_default_port_is_not_given) {
  auto port = skyr::url::default_port("git");
  ASSERT_FALSE(port);
}

TEST(whatwg_url_test, DISABLED_file_scheme_cant_have_user_info) {
  EXPECT_THROW(skyr::url("file://user@www.example.com/path"), skyr::url_syntax_error);
}

TEST(whatwg_url_test, DISABLED_file_scheme_cant_have_port) {
  EXPECT_THROW(skyr::url("file://www.example.com:80/path"), skyr::url_syntax_error);
}

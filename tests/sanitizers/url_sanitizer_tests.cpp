// Copyright 2025 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <format>
#include <iostream>
#include <string_view>
#include <vector>

#include <skyr/url.hpp>

// Comprehensive URL sanitizer tests covering edge cases and diverse inputs
// This test is designed to stress-test the URL parser with AddressSanitizer
// to detect any memory safety issues (buffer overflows, use-after-free, etc.)

struct test_case {
  std::string input;
  std::string description;
  bool should_parse;
};

int main() {
  // Test cases covering a wide range of URL patterns to stress-test with ASan
  const std::vector<test_case> test_cases = {
      // Basic URLs
      {"http://example.com", "Simple HTTP URL", true},
      {"https://example.com", "Simple HTTPS URL", true},
      {"ftp://ftp.example.com", "FTP URL", true},
      {"file:///path/to/file", "File URL", true},

      // URLs with ports
      {"http://example.com:8080", "HTTP with port", true},
      {"https://example.com:443", "HTTPS with default port", true},
      {"http://example.com:0", "Port 0 (edge case)", true},
      {"http://example.com:65535", "Maximum valid port", true},
      {"http://example.com:65536", "Port overflow", true},
      {"http://example.com:99999", "Port out of range", true},

      // URLs with authentication
      {"http://user:pass@example.com", "URL with credentials", true},
      {"http://user@example.com", "URL with username only", true},
      {"http://:pass@example.com", "URL with password only", true},
      {"http://user%20name:pass%20word@example.com", "Encoded credentials", true},

      // IPv4 addresses
      {"http://192.168.1.1", "IPv4 address", true},
      {"http://127.0.0.1:8080", "Localhost with port", true},
      {"http://255.255.255.255", "Max IPv4 address", true},
      {"http://256.1.1.1", "Invalid IPv4 (overflow)", true},
      {"http://0.0.0.0", "Zero IPv4 address", true},

      // IPv6 addresses
      {"http://[::1]", "IPv6 loopback", true},
      {"http://[2001:db8::1]", "IPv6 address", true},
      {"http://[::ffff:192.0.2.1]", "IPv4-mapped IPv6", true},
      {"http://[2001:db8::1]:8080", "IPv6 with port", true},
      {"http://[::1]:65536", "IPv6 with invalid port", true},

      // Path components
      {"http://example.com/path/to/resource", "URL with path", true},
      {"http://example.com/path/../other", "URL with dot segments", true},
      {"http://example.com/./path", "URL with single dot", true},
      {"http://example.com/../path", "URL starting with ..", true},
      {"http://example.com//double//slash", "Double slashes in path", true},

      // Query strings
      {"http://example.com?key=value", "URL with query", true},
      {"http://example.com?key1=value1&key2=value2", "Multiple query params", true},
      {"http://example.com?key=", "Empty query value", true},
      {"http://example.com?=value", "Empty query key", true},
      {"http://example.com?", "Empty query string", true},
      {"http://example.com?key=value%20with%20spaces", "Encoded query", true},

      // Fragments
      {"http://example.com#fragment", "URL with fragment", true},
      {"http://example.com#", "Empty fragment", true},
      {"http://example.com#fragment%20with%20spaces", "Encoded fragment", true},
      {"http://example.com?query=1#fragment", "Query and fragment", true},

      // Percent encoding edge cases (potential for buffer issues)
      {"http://example.com/%20", "Encoded space", true},
      {"http://example.com/%00", "Null byte encoded", true},
      {"http://example.com/%", "Incomplete encoding", true},
      {"http://example.com/%2", "Incomplete encoding 2", true},
      {"http://example.com/%GG", "Invalid hex encoding", true},
      {"http://example.com/%C3%A9", "UTF-8 encoded character", true},

      // Unicode and internationalized domains (potential encoding issues)
      {"http://\xE2\x98\x83.example.com", "Snowman in domain", true},
      {"http://\xF0\x9F\x92\xA9.example.com", "Emoji in domain", true},
      {"http://münchen.de", "German umlaut domain", true},
      {"http://\xE4\xB8\xAD\xE5\x9B\xBD.cn", "Chinese domain", true},
      {"http://example.com/\xF0\x9F\x92\xA9", "Emoji in path", true},

      // Special schemes
      {"data:text/plain,Hello", "Data URL", true},
      {"mailto:user@example.com", "Mailto URL", true},
      {"tel:+1-234-567-8900", "Tel URL", true},
      {"javascript:alert('xss')", "JavaScript URL", true},
      {"about:blank", "About URL", true},

      // Edge cases and malformed URLs (boundary conditions)
      {"http://", "No host", true},
      {"http:///path", "Empty host", true},
      {"//example.com", "Protocol-relative URL", true},
      {"/path/to/resource", "Path-only URL", true},
      {"http://example.com:abc", "Non-numeric port", true},
      {"http://exam ple.com", "Space in host", true},
      {"http://example..com", "Double dot in domain", true},
      {"http://.example.com", "Leading dot in domain", true},
      {"http://example.com.", "Trailing dot in domain", true},

      // Very long URLs (stress test buffers)
      {"http://example.com/" + std::string(1000, 'a'), "Very long path (1KB)", true},
      {"http://example.com/" + std::string(10000, 'x'), "Very long path (10KB)", true},
      {"http://" + std::string(253, 'a') + ".com", "Very long domain (253 chars)", true},
      {"http://example.com?" + std::string(1000, 'q'), "Very long query (1KB)", true},
      {"http://example.com#" + std::string(1000, 'f'), "Very long fragment (1KB)", true},

      // Special characters (potential for injection or buffer issues)
      {"http://example.com/path?key=<script>", "HTML in query", true},
      {"http://example.com/path?key=\xF0\x9F\x92\xA9", "Emoji in query", true},
      {"http://example.com/\x00\x01\x02", "Control characters", true},
      {"http://example.com/\xFF\xFE", "Invalid UTF-8", true},

      // Backslash handling (Windows path edge cases)
      {"http://example.com\\path", "Backslash in path", true},
      {"http:\\\\example.com", "Backslashes instead of slashes", true},

      // Empty and whitespace (boundary conditions)
      {"", "Empty string", false},
      {" ", "Single space", true},
      {"\t", "Tab character", true},
      {"\n", "Newline character", true},
      {"  http://example.com  ", "URL with surrounding whitespace", true},

      // Case sensitivity
      {"HTTP://EXAMPLE.COM", "Uppercase scheme and host", true},
      {"hTtP://eXaMpLe.CoM", "Mixed case", true},

      // Punycode (IDNA encoding)
      {"http://xn--nxasmq6b.example.com", "Punycode domain", true},
      {"http://xn--ls8h.example.com", "Punycode emoji", true},

      // Multiple encoding/decoding rounds (potential for bugs)
      {"http://example.com/%252F", "Double-encoded slash", true},
      {"http://example.com/%25%32%46", "Triple-encoded slash", true},

      // Null and boundary values
      {"http://example.com/" + std::string(1, '\0') + "path", "Embedded null byte", true},
      {"http://example.com/\x7F", "DEL character", true},

      // Repeated characters (stress test for buffer operations)
      {"http://example.com/" + std::string(100, '/'), "Many slashes", true},
      {"http://example.com?" + std::string(100, '&'), "Many ampersands", true},
      {"http://example.com#" + std::string(100, '#'), "Many hashes", true},
  };

  std::cout << std::format("Running {} URL test cases with AddressSanitizer + UndefinedBehaviorSanitizer\n\n",
                           test_cases.size());

  size_t total_count = 0;
  size_t pass_count = 0;
  size_t fail_count = 0;

  for (const auto& tc : test_cases) {
    total_count++;
    bool test_passed = false;

    try {
      auto url_result = skyr::url(tc.input);

      if (tc.should_parse) {
        // Expected to parse successfully
        test_passed = true;
        std::cout << std::format("[PASS] Test {}: {}\n", total_count, tc.description);
      } else {
        // Expected to fail but parsed successfully
        test_passed = false;
        std::cout << std::format("[FAIL] Test {}: {} - Expected parse failure\n", total_count, tc.description);
        std::cout << std::format("       Input: {}\n", tc.input);
        std::cout << std::format("       Got: {}\n", url_result.href());
      }
    } catch (const std::exception& e) {
      if (!tc.should_parse) {
        // Expected to fail and did fail
        test_passed = true;
        std::cout << std::format("[PASS] Test {}: {} (correctly rejected)\n", total_count, tc.description);
      } else {
        // Expected to parse but failed
        test_passed = false;
        std::cout << std::format("[FAIL] Test {}: {} - Parse error\n", total_count, tc.description);
        std::cout << std::format("       Input: {}\n", tc.input);
        std::cout << std::format("       Error: {}\n", e.what());
      }
    }

    if (test_passed) {
      pass_count++;
    } else {
      fail_count++;
    }
  }

  std::cout << std::format("\n{}\n", std::string(80, '='));
  std::cout << std::format("AddressSanitizer Test Summary:\n");
  std::cout << std::format("  Total:  {}\n", total_count);
  std::cout << std::format("  Passed: {} ({:.1f}%)\n", pass_count, 100.0 * pass_count / total_count);
  std::cout << std::format("  Failed: {} ({:.1f}%)\n", fail_count, 100.0 * fail_count / total_count);
  std::cout << std::format("{}\n", std::string(80, '='));

  if (fail_count == 0) {
    std::cout << "\nNo memory safety issues detected by sanitizers!\n";
  }

  return fail_count == 0 ? 0 : 1;
}
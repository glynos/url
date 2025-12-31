// Copyright 2025 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.

#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

#include <skyr/url.hpp>

namespace {
// Realistic URL test cases covering various scenarios
const std::vector<std::string> test_urls = {
    // Simple ASCII URLs
    "http://example.com/path",
    "https://www.github.com/cpp-netlib/url",
    "ftp://ftp.example.org/file.txt",

    // URLs with query parameters
    "https://search.example.com/search?q=test&lang=en&page=1",
    "http://api.example.com/v1/users?id=123&format=json&pretty=true",

    // URLs with fragments
    "https://docs.example.com/guide#section-1",
    "http://example.org/page.html#top",

    // URLs with authentication
    "https://user:pass@secure.example.com/resource",
    "ftp://admin@ftp.example.net/uploads/",

    // URLs with ports
    "http://localhost:8080/api/endpoint",
    "https://example.com:3000/app",

    // Internationalized domain names (IDN)
    "http://example.إختبار/path",
    "https://sub.example.测试/file",
    "http://münchen.de/page",

    // URLs with Unicode in path
    "http://example.com/π",
    "https://example.org/文档/page",
    "http://test.com/ελληνικά",

    // URLs with percent encoding
    "http://example.com/path%20with%20spaces",
    "https://example.org/query?name=John%20Doe&age=30",
    "http://test.com/%E2%9C%93/check",

    // Complex real-world URLs
    "https://www.google.com/search?q=c%2B%2B+url+parsing&oq=c%2B%2B&aqs=chrome",
    "https://github.com/cpp-netlib/url/blob/main/include/skyr/url.hpp#L123",
    "https://en.wikipedia.org/wiki/Uniform_Resource_Identifier#Syntax",

    // IPv4 addresses
    "http://192.168.1.1/admin",
    "https://127.0.0.1:8443/secure",
    "http://255.255.255.255/",

    // IPv6 addresses
    "http://[::1]/localhost",
    "https://[2001:db8::1]/resource",
    "http://[fe80::1%eth0]/",

    // Edge cases
    "file:///C:/Users/test/document.txt",
    "data:text/plain;base64,SGVsbG8gV29ybGQ=",
    "mailto:user@example.com?subject=Test",

    // URLs with multiple subdomains
    "https://api.v2.staging.example.com/endpoint",
    "http://www.blog.en.example.org/post/123",
};

struct benchmark_result {
  std::size_t iterations;
  std::size_t urls_processed;
  long long total_ms;
  double avg_us_per_url;
  std::size_t successful_parses;
  std::size_t failed_parses;
};

auto run_benchmark(std::size_t iterations) -> benchmark_result {
  std::size_t successful = 0;
  std::size_t failed = 0;

  auto start = std::chrono::high_resolution_clock::now();

  for (std::size_t i = 0; i < iterations; ++i) {
    for (const auto& url_str : test_urls) {
      auto result = skyr::make_url(url_str);
      if (result) {
        ++successful;
        // Force use of result to prevent dead code elimination
        [[maybe_unused]] auto hostname = result->hostname();
      } else {
        ++failed;
      }
    }
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  auto total_urls = iterations * test_urls.size();
  auto avg_us = (static_cast<double>(duration_ms) * 1000.0) / static_cast<double>(total_urls);

  return {iterations, total_urls, duration_ms, avg_us, successful, failed};
}

void print_results(const benchmark_result& result) {
  std::cout << "\n=================================================\n";
  std::cout << "URL Parsing Benchmark Results\n";
  std::cout << "=================================================\n\n";

  std::cout << "Configuration:\n";
  std::cout << "  Test URLs:     " << test_urls.size() << " unique patterns\n";
  std::cout << "  Iterations:    " << result.iterations << "\n";
  std::cout << "  Total URLs:    " << result.urls_processed << "\n\n";

  std::cout << "Results:\n";
  std::cout << "  Total time:    " << result.total_ms << " ms\n";
  std::cout << "  Successful:    " << result.successful_parses << " (" << std::fixed << std::setprecision(1)
            << (100.0 * result.successful_parses / result.urls_processed) << "%)\n";
  std::cout << "  Failed:        " << result.failed_parses << " (" << std::fixed << std::setprecision(1)
            << (100.0 * result.failed_parses / result.urls_processed) << "%)\n\n";

  std::cout << "Performance:\n";
  std::cout << "  Average:       " << std::fixed << std::setprecision(3) << result.avg_us_per_url << " µs/URL\n";
  std::cout << "  Throughput:    " << std::fixed << std::setprecision(0) << (1'000'000.0 / result.avg_us_per_url)
            << " URLs/second\n\n";

  std::cout << "=================================================\n";
}

}  // namespace

int main(int argc, char* argv[]) {
  std::size_t iterations = 10'000;

  if (argc > 1) {
    try {
      iterations = std::stoull(argv[1]);
    } catch (...) {
      std::cerr << "Usage: " << argv[0] << " [iterations]\n";
      std::cerr << "  iterations: number of times to parse all test URLs (default: 10000)\n";
      return 1;
    }
  }

  std::cout << "Running URL parsing benchmark...\n";
  std::cout << "This may take a moment...\n";

  auto result = run_benchmark(iterations);
  print_results(result);

  return 0;
}
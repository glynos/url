// Copyright 2017 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef NETWORK_URI_URI_QUERY_PARAMETERS_INC
#define NETWORK_URI_URI_QUERY_PARAMETERS_INC

namespace network {

  class query_parameters {
   public:
    query_parameters() = default;

    explicit query_parameters(const string_type &query)
        : query_(query),
          query_view_(query_),
          query_part_(detail::uri_part{std::begin(query_view_), std::end(query_view_)}) {}

    void append(const string_type &name, const string_type &value) {
      // auto query = split_query_string(query_);
      // query.push_back({name, value});
      // query_ = to_query_string(query);
    }

    void remove(const string_type &name) {
      // auto query = split_query_string(query_);
      // auto it = std::remove_if(std::begin(query), std::end(query),
      //                          [&name] (const auto &param) { return
      //                          param.first == name; });

      // query.erase(it, std::end(query));
      // query_ = to_query_string(query);
    }

    optional<string_view> get(const string_type &name) const { return {}; }

    bool has(const string_type &name) const {
      auto name_view = string_view(name.c_str(), name.size());
      return std::end(*this) != std::find_if(std::begin(*this), std::end(*this),
                                             [&name_view](const auto &param) {
                                               return param.first == name_view;
                                             });
    }

    void set(const string_type &name, const string_type &value) {}

    query_iterator begin() const { return query_iterator{*query_part_}; }

    query_iterator end() const { return query_iterator{}; }

    string_type to_string() const { return query_; }

   private:
    void update();

    void sort() {
      // auto query = split_query_string(query_);
      // std::sort(std::begin(query), std::end(query),
      //           [] (const auto &lhs, const auto &rhs) {
      //             return lhs.first < rhs.first;
      //           });
      // query_ = to_query_string(query);
    }

    string_type query_;
    string_view query_view_;
    optional<detail::uri_part> query_part_;
  };

};

#endif // NETWORK_URI_URI_QUERY_PARAMETERS_INC

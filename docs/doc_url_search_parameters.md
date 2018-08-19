# Header file `url_search_parameters.hpp`

``` cpp
#define SKYR_URL_SEARCH_PARAMETERS_INC

namespace skyr
{
    class url_search_parameters;
}
```

### Class `skyr::url_search_parameters`

``` cpp
class url_search_parameters
{
public:
    using string_type = std::string;

    using string_view = std::string_view;

    using value_type = std::pair<string_type, string_type>;

    using const_iterator = std::vector<value_type>::const_iterator;

    url_search_parameters();

    explicit url_search_parameters(skyr::url_search_parameters::string_view search);

    void append(skyr::url_search_parameters::string_type const& name, skyr::url_search_parameters::string_type const& value);

    void remove(skyr::url_search_parameters::string_type const& name);

    optional<skyr::url_search_parameters::string_type> get(skyr::url_search_parameters::string_type const& name) const noexcept;

    std::vector<string_type> get_all(skyr::url_search_parameters::string_type const& name) const;

    bool contains(skyr::url_search_parameters::string_type const& name) const noexcept;

    void set(skyr::url_search_parameters::string_type const& name, skyr::url_search_parameters::string_type const& value);

    void clear() noexcept;

    void sort();

    const_iterator begin() const noexcept;

    const_iterator end() const noexcept;

    string_type to_string() const;
};
```

Allows iterating through search parameters.

### Constructor `skyr::url_search_parameters::url_search_parameters`

``` cpp
url_search_parameters();
```

Constructor.

-----

### Constructor `skyr::url_search_parameters::url_search_parameters`

``` cpp
explicit url_search_parameters(skyr::url_search_parameters::string_view search);
```

Constructor.

#### Parameters

  - `search` - The search string.

-----

### Function `skyr::url_search_parameters::append`

``` cpp
void append(skyr::url_search_parameters::string_type const& name, skyr::url_search_parameters::string_type const& value);
```

Appends a name-value pair to the search string.

#### Parameters

  - `name` - The parameter name.
  - `value` - The parameter value.

-----

### Function `skyr::url_search_parameters::remove`

``` cpp
void remove(skyr::url_search_parameters::string_type const& name);
```

Removes a parameter from the search string.

#### Parameters

  - `name` - The name of the parameter to remove.

-----

### Function `skyr::url_search_parameters::begin`

``` cpp
const_iterator begin() const noexcept;
```

*Returns:* 

-----

### Function `skyr::url_search_parameters::end`

``` cpp
const_iterator end() const noexcept;
```

*Returns:* 

-----

### Function `skyr::url_search_parameters::to_string`

``` cpp
string_type to_string() const;
```

*Returns:* 

-----

-----

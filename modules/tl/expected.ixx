module;

#include <tl/expected.hpp>

export module tl.expected;

export namespace tl {
	using tl::expected;
	using tl::unexpected;
	using tl::make_unexpected;
}

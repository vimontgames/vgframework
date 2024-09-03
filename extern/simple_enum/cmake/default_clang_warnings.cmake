set( clang_default_warnings
  -Weverything -Wno-unsafe-buffer-usage -Wno-pre-c++17-compat
  -Wno-c++98-compat -Wno-pre-c++14-compat -Wno-pre-c++17-compat -Wno-c++98-compat-pedantic
  -Wno-pre-c++20-compat-pedantic -Wno-padded -Wno-exit-time-destructors -Wno-global-constructors
  -Wno-c++20-compat # 'consteval' specifier is incompatible with C++ standards before C++20
  -Wno-undefined-func-template
  -Wno-ctad-maybe-unsupported
  )

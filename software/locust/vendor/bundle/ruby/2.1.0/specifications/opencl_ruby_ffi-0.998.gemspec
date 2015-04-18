# -*- encoding: utf-8 -*-
# stub: opencl_ruby_ffi 0.998 ruby lib

Gem::Specification.new do |s|
  s.name = "opencl_ruby_ffi"
  s.version = "0.998"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.require_paths = ["lib"]
  s.authors = ["Brice Videau"]
  s.date = "2015-01-21"
  s.description = "Ruby OpenCL FFI bindings. OpenCL 2.0 ready"
  s.email = "brice.videau@imag.fr"
  s.homepage = "https://github.com/Nanosim-LIG/opencl-ruby"
  s.licenses = ["BSD"]
  s.required_ruby_version = Gem::Requirement.new(">= 1.8.7")
  s.rubygems_version = "2.2.2"
  s.summary = "Ruby OpenCL FFI bindings"

  s.installed_by_version = "2.2.2" if s.respond_to? :installed_by_version

  if s.respond_to? :specification_version then
    s.specification_version = 4

    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_runtime_dependency(%q<narray>, [">= 0.6.0.8"])
      s.add_runtime_dependency(%q<ffi>, [">= 1.9.3"])
      s.add_runtime_dependency(%q<narray_ffi>, [">= 0.2"])
    else
      s.add_dependency(%q<narray>, [">= 0.6.0.8"])
      s.add_dependency(%q<ffi>, [">= 1.9.3"])
      s.add_dependency(%q<narray_ffi>, [">= 0.2"])
    end
  else
    s.add_dependency(%q<narray>, [">= 0.6.0.8"])
    s.add_dependency(%q<ffi>, [">= 1.9.3"])
    s.add_dependency(%q<narray_ffi>, [">= 0.2"])
  end
end

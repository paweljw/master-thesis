# -*- encoding: utf-8 -*-
# stub: narray 0.6.1.1 ruby .
# stub: src/extconf.rb

Gem::Specification.new do |s|
  s.name = "narray"
  s.version = "0.6.1.1"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.require_paths = ["."]
  s.authors = ["Masahiro Tanaka"]
  s.date = "2015-01-01"
  s.description = "Numerical N-dimensional Array class"
  s.email = "masa16.tanaka@gmail.com"
  s.extensions = ["src/extconf.rb"]
  s.files = ["src/extconf.rb"]
  s.homepage = "http://masa16.github.io/narray/"
  s.licenses = ["Ruby"]
  s.rdoc_options = ["--title", "NArray", "--main", "NArray", "--exclude", "mk.*", "--exclude", "extconf\\.rb", "--exclude", "src/.*\\.h", "--exclude", "src/lib/", "--exclude", ".*\\.o", "--exclude", "narray\\.so", "--exclude", "libnarray\\.*"]
  s.rubygems_version = "2.4.6"
  s.summary = "N-dimensional Numerical Array class for Ruby"

  s.installed_by_version = "2.4.6" if s.respond_to? :installed_by_version
end

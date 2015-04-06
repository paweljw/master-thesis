require 'yaml'
require 'logger'
require 'active_record'
require 'sqlite3'
require 'rest_client'
require 'fileutils'
require 'uri'
require 'optparse'
require 'json'
require 'zlib'
require 'opencl_ruby_ffi'

# Setup Locust root
LOCUST_ROOT = File.dirname(__FILE__)

# Select DB environment
DATABASE_ENV = ENV['DATABASE_ENV'] || 'development'

# Set up DB config
DATABASE_CONFIG = YAML.load_file(File.join(LOCUST_ROOT, 'config/database.yml'))[DATABASE_ENV]

# Set up Locust config itself
LOCUST_CONFIG = YAML.load_file(File.join(LOCUST_ROOT, 'config/locust.yml'))

# Require lib folder
Dir.glob( File.join(LOCUST_ROOT, "lib", "*")).each do |folder|
  Dir.glob(folder +"/*.rb").each do |file|
    require file
  end
end

# Connect AR to database
ActiveRecord::Base.establish_connection(
  :adapter => DATABASE_CONFIG["adapter"],
  :database => File.join( LOCUST_ROOT, DATABASE_CONFIG["database"] )
)

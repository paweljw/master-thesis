# encoding: UTF-8
# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your
# database schema. If you need to create the application database on another
# system, you should be using db:schema:load, not running all the migrations
# from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended that you check this file into your version control system.

ActiveRecord::Schema.define(version: 20150426104425) do

  create_table "nodes", force: :cascade do |t|
    t.string   "name"
    t.string   "key"
    t.datetime "created_at", null: false
    t.datetime "updated_at", null: false
    t.datetime "active_at"
  end

  create_table "solutions", force: :cascade do |t|
    t.integer  "state"
    t.string   "name"
    t.string   "solution"
    t.datetime "completed"
    t.datetime "created_at", null: false
    t.datetime "updated_at", null: false
    t.string   "mtx"
    t.string   "rhs"
    t.string   "part_size"
    t.string   "metafile"
    t.integer  "dim"
    t.integer  "full_size"
  end

  create_table "tasks", force: :cascade do |t|
    t.integer  "wave_id"
    t.string   "name"
    t.integer  "type"
    t.integer  "state"
    t.integer  "part_num"
    t.string   "metafile"
    t.integer  "node_id"
    t.datetime "created_at",  null: false
    t.datetime "updated_at",  null: false
    t.datetime "completed"
    t.integer  "kind"
    t.decimal  "time"
    t.string   "backend"
    t.integer  "part_size"
    t.integer  "global_size"
  end

  add_index "tasks", ["node_id"], name: "index_tasks_on_node_id"
  add_index "tasks", ["wave_id"], name: "index_tasks_on_wave_id"

  create_table "users", force: :cascade do |t|
    t.string   "email",                  default: "", null: false
    t.string   "encrypted_password",     default: "", null: false
    t.string   "reset_password_token"
    t.datetime "reset_password_sent_at"
    t.datetime "remember_created_at"
    t.integer  "sign_in_count",          default: 0,  null: false
    t.datetime "current_sign_in_at"
    t.datetime "last_sign_in_at"
    t.string   "current_sign_in_ip"
    t.string   "last_sign_in_ip"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.string   "name"
  end

  add_index "users", ["email"], name: "index_users_on_email", unique: true
  add_index "users", ["reset_password_token"], name: "index_users_on_reset_password_token", unique: true

  create_table "waves", force: :cascade do |t|
    t.integer  "solution_id"
    t.integer  "seq"
    t.integer  "state"
    t.datetime "completed"
    t.datetime "created_at",   null: false
    t.datetime "updated_at",   null: false
    t.integer  "tasks_number"
  end

  add_index "waves", ["solution_id"], name: "index_waves_on_solution_id"

end

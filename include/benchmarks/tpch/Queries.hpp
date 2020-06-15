#pragma once

#include <memory>

#include "benchmarks/Config.hpp"
#include "common/runtime/Concurrency.hpp"
#include "common/runtime/Database.hpp"
#include "common/runtime/Types.hpp"
#include "vectorwise/Operators.hpp"
#include "vectorwise/Query.hpp"
#include "vectorwise/QueryBuilder.hpp"

struct Q1Builder : public Query, private vectorwise::QueryBuilder {
   enum {
      sel_date,
      sel_date_grouped,
      selScat,
      result_proj_minus,
      result_proj_plus,
      disc_price,
      charge,
      returnflag,
      linestatus,
      sum_qty,
      sum_base_price,
      sum_disc_price,
      sum_charge,
      count_order
   };
   struct Q1 {
      types::Numeric<12, 2> one = types::Numeric<12, 2>::castString("1.00");
      types::Date c1 = types::Date::castString("1998-09-02");
      std::unique_ptr<vectorwise::Operator> rootOp;
   };
   Q1Builder(runtime::Database& db, vectorwise::SharedStateManager& shared,
             size_t size = 1024)
       : QueryBuilder(db, shared, size) {}
   std::unique_ptr<Q1> getQuery();
};

std::unique_ptr<runtime::Query>
q1_hyper(runtime::Database& db,
         size_t nrThreads = std::thread::hardware_concurrency());


#include <weld.h>

struct WeldConfig {
  weld_conf_t value;

  WeldConfig(size_t threads) {
    value = weld_conf_new();
    weld_conf_set(value, "weld.threads", std::to_string(threads).c_str());
  }

  ~WeldConfig() {
    weld_conf_free(value);
  }
};

struct IWeldRelation {
  weld_value_t value;
  bool has_value = false;


  struct weld_vector {
      void *data;
      size_t length;
  };

  virtual ~IWeldRelation() {
    if (has_value) {
      weld_value_free(value);
    }
  }
};

struct WeldInRelation : IWeldRelation {
  WeldInRelation(size_t card, const std::vector<void*>& columns) {
    for (auto& c : columns) {
      vecs.push_back(weld_vector {c, card});
    }

    value = weld_value_new(&vecs[0]);
  }

private:
  std::vector<weld_vector> vecs;
};

#include <sstream>

struct WeldQuery {
  weld_module_t module;
  WeldConfig config;
  std::unique_ptr<WeldInRelation> input; 
  weld_context_t context;

  WeldQuery(size_t threads, const std::string& q,
      std::unique_ptr<WeldInRelation>&& input) : config(threads), input(std::move(input)) {
    weld_error_t err = weld_error_new();
    module = weld_module_compile(q.c_str(), config.value, err);
    if (weld_error_code(err)) {
      const char *msg = weld_error_message(err);
      printf("Error message: %s\n", msg);
      exit(1);
    }
    weld_error_free(err);

    context = weld_context_new(config.value);
  }

  ~WeldQuery() {
    weld_context_free(context);
    weld_module_free(module);
  }

  weld_value_t run() {
    weld_error_t err = weld_error_new();
    auto r = weld_module_run(module, context, input ? input->value : nullptr, err);
    if (weld_error_code(err)) {
      const char *msg = weld_error_message(err);
      printf("Error message: %s\n", msg);
      exit(1);
    }
    weld_error_free(err);
    return r;
  }
};

inline std::string mkStr(const std::vector<std::string>& strs)
{
  std::ostringstream r;
  for (auto& s : strs) {
    r << s;
  }
  return r.str();
}



WeldQuery* q1_weld_prepare(runtime::Database& db,
  size_t nrThreads);

std::unique_ptr<runtime::Query>
q1_weld(runtime::Database& db,
         size_t nrThreads,
         WeldQuery* q);

std::unique_ptr<runtime::Query>
q1_vectorwise(runtime::Database& db,
              size_t nrThreads = std::thread::hardware_concurrency(),
              size_t vectorSize = 1024);

struct Q3Builder : private vectorwise::QueryBuilder {
   enum {
      sel_order,
      sel_cust,
      cust_ord,
      j1_lineitem,
      j1_lineitem_grouped,
      sel_lineitem,
      result_project,
      l_orderkey,
      o_orderdate,
      o_shippriority,
      result_proj_minus
   };
   struct Q3 {
      std::string building = "BUILDING";
      types::Char<10> c1 =
          types::Char<10>::castString(building.data(), building.size());
      types::Date c2 = types::Date::castString("1995-03-15");
      types::Date c3 = types::Date::castString("1995-03-15");
      types::Numeric<12, 2> one = types::Numeric<12, 2>::castString("1.00");
      int64_t revenue = 0;
      int64_t sum = 0;
      int64_t count = 0;
      size_t n = 0;
      std::unique_ptr<vectorwise::Operator> rootOp;
   };
   Q3Builder(runtime::Database& db, vectorwise::SharedStateManager& shared,
             size_t size = 1024)
       : QueryBuilder(db, shared, size) {}
   std::unique_ptr<Q3> getQuery();
};

std::unique_ptr<runtime::Query>
q3_hyper(runtime::Database& db,
         size_t nrThreads = std::thread::hardware_concurrency());
std::unique_ptr<runtime::Query>
q3_vectorwise(runtime::Database& db,
              size_t nrThreads = std::thread::hardware_concurrency(),
              size_t vectorSize = 1024);

struct Q5Builder : private vectorwise::QueryBuilder {
   enum {
      sel_region,
      sel_ord,
      sel_ord2,
      join_reg_nat,
      join_cust,
      join_ord,
      join_ord_nationkey,
      join_line,
      join_line_nationkey,
      join_supp,
      result_project,
      join_supp_line,
      n_name,
      n_name2,
      selScat,
      result_proj_minus,
      sum
   };
   struct Q5 {
      types::Numeric<12, 2> one = types::Numeric<12, 2>::castString("1.00");
      types::Date c1 = types::Date::castString("1994-01-01");
      types::Date c2 = types::Date::castString("1995-01-01");
      std::string region = "ASIA";
      types::Char<25> c3 =
          types::Char<25>::castString(region.data(), region.size());
      std::unique_ptr<vectorwise::Operator> rootOp;
   };
   Q5Builder(runtime::Database& db, vectorwise::SharedStateManager& shared,
             size_t size = 1024)
       : QueryBuilder(db, shared, size) {}
   std::unique_ptr<Q5> getQuery();
   std::unique_ptr<Q5> getNoSelQuery();
};

std::unique_ptr<runtime::Query>
q5_hyper(runtime::Database& db,
         size_t nrThreads = std::thread::hardware_concurrency());
std::unique_ptr<runtime::Query>
q5_vectorwise(runtime::Database& db,
              size_t nrThreads = std::thread::hardware_concurrency(),
              size_t vectorSize = 1024);

runtime::Relation q5_no_sel_hyper(runtime::Database& db);
std::unique_ptr<runtime::BlockRelation>
q5_no_sel_vectorwise(runtime::Database& db,
                     size_t nrThreads = std::thread::hardware_concurrency());

class Q6Builder : public vectorwise::QueryBuilder {

 public:
   struct Q6 {
      types::Date c1 = types::Date::castString("1994-01-01");
      types::Date c2 = types::Date::castString("1995-01-01");
      types::Numeric<12, 2> c3 = types::Numeric<12, 2>::castString("0.05");
      types::Numeric<12, 2> c4 = types::Numeric<12, 2>::castString("0.07");
      types::Numeric<12, 2> c5 = types::Numeric<12, 2>(types::Integer(24));
      size_t n;
      int64_t aggregator = 0;
      std::unique_ptr<vectorwise::Operator> rootOp;
   };

   std::unique_ptr<Q6> getQuery();
   Q6Builder(runtime::Database& db, vectorwise::SharedStateManager& shared,
             size_t size = 1024)
       : QueryBuilder(db, shared, size) {}
};

runtime::Relation
q6_hyper(runtime::Database& db,
         size_t nrThreads = std::thread::hardware_concurrency());
runtime::Relation
q6_vectorwise(runtime::Database& db,
              size_t nrThreads = std::thread::hardware_concurrency(),
              size_t vectorSize = 1024);

struct Q9Builder : public Query, private vectorwise::QueryBuilder {
   enum {
      nation_supplier,
      part_partsupp,
      pspp,
      xlineitem,
      ordersx,
      n_name,
      ps_supplycost,
      xlineitem_ord,
      disc_price,
      total_cost,
      sel_part,
      l_extendedprice,
      l_discount,
      l_quantity,
      result_proj_minus,
      amount,
      o_year,
      sum_profit
   };
   struct Q9 {
      types::Varchar<55> contains = types::Varchar<55>::castString("green");
      types::Numeric<12, 2> one = types::Numeric<12, 2>::castString("1.00");
      std::unique_ptr<vectorwise::Operator> rootOp;
   };
   Q9Builder(runtime::Database& db, vectorwise::SharedStateManager& shared,
             size_t size = 1024)
       : QueryBuilder(db, shared, size) {}
   std::unique_ptr<Q9> getQuery();
};

std::unique_ptr<runtime::Query>
q9_hyper(runtime::Database& db,
         size_t nrThreads = std::thread::hardware_concurrency());
std::unique_ptr<runtime::Query>
q9_vectorwise(runtime::Database& db,
              size_t nrThreads = std::thread::hardware_concurrency(),
              size_t vectorSize = 1024);

struct Q18Builder : public Query, private vectorwise::QueryBuilder {
   enum {
      l_orderkey,
      l_quantity,
      sel_orderkey,
      orders_matches,
      customer_matches,
      c_name,
      c_name2,
      lineitem_matches,
      o_custkey,
      o_orderdate,
      o_totalprice,
      group_c_name,
      group_o_custkey,
      group_l_orderkey,
      group_o_orderdate,
      group_o_totalprice,
      group_sum,
      lineitem_matches_grouped,
      compact_quantity,
      compact_l_orderkey
   };
   struct Q18 {
      uint64_t zero = 0;
      types::Numeric<12, 2> qty_bound =
          types::Numeric<12, 2>::castString("300");
      std::unique_ptr<vectorwise::Operator> rootOp;
   };
   Q18Builder(runtime::Database& db, vectorwise::SharedStateManager& shared,
              size_t size = 1024)
       : QueryBuilder(db, shared, size) {}
   std::unique_ptr<Q18> getQuery();
   std::unique_ptr<Q18> getGroupQuery();
};

std::unique_ptr<runtime::Query>
q18_hyper(runtime::Database& db,
          size_t nrThreads = std::thread::hardware_concurrency());
std::unique_ptr<runtime::Query>
q18_vectorwise(runtime::Database& db,
               size_t nrThreads = std::thread::hardware_concurrency(),
               size_t vectorSize = 1024);
std::unique_ptr<runtime::Query>
q18group_vectorwise(runtime::Database& db,
                    size_t nrThreads = std::thread::hardware_concurrency(),
                    size_t vectorSize = 1024);

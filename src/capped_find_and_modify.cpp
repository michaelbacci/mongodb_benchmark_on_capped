//
// Created by Michael on 26/03/15.
//

#include "benchmark/benchmark.h"
#include <mongo/client/dbclient.h>
#include <memory>
#include <cassert>

namespace helper {

  mongo::DBClientConnection* GetConnection() {
    mongo::DBClientConnection*connection = new mongo::DBClientConnection();

    std::string err_msg;

    try {
      connection->connect(MONGO_HOST);

      if (!std::string(MONGO_DB_AUTH).empty())
        connection->auth(MONGO_DB_AUTH, MONGO_DB_USER, MONGO_DB_PSWD, err_msg, true);
    }
    catch(const mongo::DBException &e) {

      if (!err_msg.size())
        std::cerr << "Connection Failed: " << e.what() << std::endl;

      assert(0);
    }

    return connection;
  }

}

static void BM_CappedFindAndModify(benchmark::State& state) {
  std::string mongodb_db = MONGO_DB;
  std::string mongodb_collection = "test_v1";

  std::unique_ptr<mongo::DBClientConnection> connection(helper::GetConnection());

  while (state.KeepRunning()) {

    state.PauseTiming();
    std::cout << "A";
    //TODO:destroy capped collection
    //TODO:create capped collection
    mongo::BSONObj info;
    connection->createCollection(mongodb_collection, state.range_x()*64, true, state.range_x(), &info);
    std::cout << info.toString();
    //TODO:fill the data
    state.ResumeTiming();

    mongo::BSONObj result;
    mongo::BSONObjBuilder query;
    query << "findandmodify" << mongodb_collection <<
        "query" << BSON("computed" << false) <<
        "update" << BSON("computed" << true);

    bool has_data = true;
    while(has_data) {
      if(!connection->runCommand(mongodb_db, query.obj(), result))
        assert(0);

      has_data = false;
      /* Extracting Data */
      auto it = result.begin();

      while(it.more() && !has_data) {
        auto data_element = it.next();
        if(!data_element.isABSONObj()) continue;

        auto data_obj = data_element.Obj();
        if(!data_obj.hasField("computed")) continue;

        has_data = true;
        //data_obj contains data we need
      }
    }

  }
}
BENCHMARK(BM_CappedFindAndModify)->Threads(1)->Arg(10);

BENCHMARK_MAIN();
//
// Created by Michael on 26/03/15.
//

#include "benchmark/benchmark.h"
#include <mongo/client/dbclient.h>
#include <memory>
#include <cassert>
#include <vector>

namespace helper {

  mongo::DBClientConnection* GetConnection() {
    mongo::DBClientConnection* connection = new mongo::DBClientConnection();

    std::string err_msg;

    try {
      connection->connect(MONGO_HOST);
      assert(connection->isStillConnected());

      if (!std::string(MONGO_DB_AUTH).empty())
        assert(connection->auth(MONGO_DB_AUTH, MONGO_DB_USER, MONGO_DB_PSWD, err_msg, true));
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
  std::string mongodb_collection = mongodb_db + ".benchmark_v1";

//  std::unique_ptr<mongo::DBClientConnection> connection(helper::GetConnection());
  auto *connection = helper::GetConnection();

  while (state.KeepRunning()) {

    state.PauseTiming();

    /* Create a new the capped collection */
    mongo::BSONObj info;
    connection->dropCollection(mongodb_collection, &info);
    connection->createCollection(mongodb_collection, state.range_x()*1024, true, state.range_x(), &info);
    assert(info.begin().next().toString() == "ok: 1.0");

    /* Filling Up MongoDB */
    mongo::BSONObj data = BSON("data" << "__here_the_data__" << "computed" << 0);
    std::vector<mongo::BSONObj> v(state.range_x(), data);
    connection->insert(mongodb_collection, v);

    state.ResumeTiming();

    auto counter = state.range_x();
    while(--counter) {

      mongo::BSONObjBuilder query;
      query << "findandmodify" << "benchmark_v1" <<
          "query" << BSON("computed" << 0) <<
          "update" << BSON("$inc" << BSON("computed" << 1)) <<
          "upsert" << true <<
          "new" << true;

      mongo::BSONObj result;
      if(!connection->runCommand(mongodb_db, query.obj(), result)) {
        assert(0);
      }
    }

  }

  delete connection;
}
BENCHMARK(BM_CappedFindAndModify)->Threads(1)->Arg(100)->Arg(100000);

BENCHMARK_MAIN();
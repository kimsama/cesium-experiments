#define _CRT_SECURE_NO_WARNINGS
#include <uWebSockets/App.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <uv.h>

struct PerSocketData 
{
  uWS::Loop* mainLoop = nullptr;
  uWS::WebSocket<false, true, PerSocketData>* ws = nullptr;
};

int main()
{
  typename uWS::TemplatedApp</*SSL*/false>::WebSocketBehavior<PerSocketData> wsBehavior;
  wsBehavior.open = [&](auto* ws) {

    auto sd = (PerSocketData*)ws->getUserData();
    sd->mainLoop = uWS::Loop::get();
  };
  wsBehavior.message = [/*this*/](auto* ws, std::string_view message, uWS::OpCode /*opCode*/) {
  };
  wsBehavior.drain = [](auto* ws) {
  };
  wsBehavior.ping = [](auto* /*ws*/, std::string_view /*message*/) {
  };
  wsBehavior.pong = [](auto* /*ws*/, std::string_view /*message*/) {
  };
  wsBehavior.close = [](auto* ws, int /*code*/, std::string_view /*message*/) {
  };

  std::unique_ptr <uWS::TemplatedApp<false>> app;
  app.reset(new uWS::TemplatedApp<false>());

  auto a = (*app).template ws<PerSocketData>("/ws", std::move(wsBehavior));
  a.get("/*", [/*this*/](auto* res, auto* req) {
    res->end("Hello world!");
  }); 

  std::string url = "ws://localhost:4000/"; // "http://localhost:4000/"; // "ws://localhost:4000/socket.io/?EIO=3&transport=websocket";//; //  
  a.connect(url, [](auto* res, auto* req) 
  {
    auto url = std::string(req->getUrl());
    if (url.empty())
    {
      res->writeStatus("400 Bad Request");
      res->end("");
      return;
    }
  });

  struct dummy_us_loop_t
  {
    alignas(LIBUS_EXT_ALIGNMENT) char data[104];

    uv_loop_t* uv_loop;
    int        is_default;
    void* uv_pre;
    void* uv_check;
  };

  auto uv_loop = ((dummy_us_loop_t*)uWS::Loop::get())->uv_loop;

  //Run the uv_loop step by step, instead of app.run()
  while (true)
  {
    // Send gps position with the given stdin
    std::string gps;
    std::getline(std::cin, gps);
    if (gps.empty())
    {
      break;
    }
    else
    {
      // send gps position to the server
      
    }

    uv_run(uv_loop, UV_RUN_NOWAIT);
  }


  return 0;
}

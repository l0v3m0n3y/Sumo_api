# Sumo_api
api for sumo-api.com sumo batabase site
# main
```cpp
#include "Sumo_api.h"
#include <iostream>

int main() {
   Sumo_api api;
    auto rikishis = api.get_rikishis().then([](json::value result) {
        std::cout << result<< std::endl;
    });
    rikishis.wait();
    
    return 0;
}
```

# Launch (your script)
```
g++ -std=c++11 -o main main.cpp -lcpprest -lssl -lcrypto -lpthread -lboost_system -lboost_chrono -lboost_thread
./main
```

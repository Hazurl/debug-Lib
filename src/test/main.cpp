#include "../Logger.h"
#include <iostream>

using namespace haz;

Logger& logger = Logger::get("main");

namespace test {
void thing () {
    logger.debug(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, "in thing");
}
};

int main (int argc, char **argv) {

    logger.addHandler( (new ConsoleHandler())->color(true) );
    //logger.addHandler(new FileHandler("../loggerv3.txt"));

    logger.error(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, "Something happen");
    test::thing();

    return 0;
}


#include "../Logger.h"
#include <iostream>



using namespace haz;

Logger& logger = Logger::get("main");

namespace test {
void thing () {
    logger.entering({});
    logger.debug("inside function thing from namespace test");
    logger.stackTrace();
    logger.exiting();
    logger.exiting("ok");
}
};

int main (int argc, char **argv) {

    logger.addHandler( (new ConsoleHandler())->color(true) );
    //logger.addHandler(new FileHandler("../loggerv3.txt"));

    logger.setColorsLevel(
        {
            { Color::RED, 600 },
            { Color::YELLOW, 500 },
            { Color::MAGENTA, 400 },
            { Color::GREEN, 300 },
            { Color::BLUE, 200 },
            { Color::CYAN, 100 }
        }
    );

    logger.error("with maccro");
    logger.log(Level::CONFIG, "config ok !");
    test::thing();

    return 0;
}


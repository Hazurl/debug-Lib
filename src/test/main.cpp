#include "../Logger.h"
#include <iostream>



using namespace haz;

Logger& logger = Logger::get("main");

namespace test {
void thing (bool thr) {
    if (thr)
        logger.entering({"true"});
    else
        logger.entering({ "false"});
    logger.debug("inside function thing from namespace test");
    logger.stackTrace(0);
    logger.warn("An exception may occur");
    if (thr) logger.throwException(std::runtime_error, "This is a bug !");
    logger.exiting("void");
    return;
}
void other () {
    logger.entering({});
    logger.stackTrace(0);
    logger.info("Something long");
    long ii = 0;
    while (ii++ < 1000) {
        std::string s = "";
        long i = 0;
        while (i++ < 1000000)
            s += ".";
    }

    logger.info("calling \"thing\"");
    thing(true);
    logger.exiting("void");
}
};

int main (int , char **) {
    try {
    logger.addHandler( (new FileHandler("out.txt", true))->color(true) );
    logger.addHandler( (new ConsoleHandler())->color(true) );

    logger.setColorsLevel(
        {
            { Formatting::RED, 600 },
            { Formatting::YELLOW, 500 },
            { Formatting::MAGENTA, 400 },
            { Formatting::GREEN, 300 },
            { Formatting::BLUE, 200 },
            { Formatting::CYAN, 100 }
        }
    );
    logger.warn("Begin now");
    //logger.setLevel(Level::OFF);
    logger.entering({});
    logger.stackTrace(0);

    logger.error("with maccro");
    logger.log(Level::CONFIG, "config ok !");
    logger.trace("calling \"thing\" then \"other\"");
    test::thing(false);
    test::other();

    } catch ( std::runtime_error const& e ) {
        std::cout << "> terminate called after throwing an instance of 'std::runtime_error' " << std::endl << "> what(): " << e.what() << std::endl;
    }

    logger.exiting("0");
    //logger.setLevel(Level::ALL);
    logger.warn("End now");

    return 0;
}


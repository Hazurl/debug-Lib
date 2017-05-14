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
    if (thr)
        logger.throwException(std::runtime_error, "This is a bug !");
    logger.exiting("void");
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
    logger.trace("calling \"thing\" then \"other\"");
    test::thing(false);
    test::other();

    } catch ( std::runtime_error const& e ) {
        std::cout << "terminate called after throwing an instance of 'std::runtime_error' " << std::endl << "what(): " << e.what() << std::endl;
    }

    return 0;
}


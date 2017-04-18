#include "../Logger.h"
#include <iostream>

#define log(l, m) log(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (l), (m));

#define error(m) error(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (m));
#define warn(m) warn(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (m));
#define config(m) config(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (m));
#define trace(m) trace(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (m));
#define debug(m) debug(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (m));
#define info(m) info(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (m));


using namespace haz;

Logger& logger = Logger::get("main");

namespace test {
void thing () {
    logger.entering(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, {} );
    logger.debug("inside function thing from namespace test");
    logger.stackTrace(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__);
    logger.exiting(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, "" );
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


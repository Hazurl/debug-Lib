#include "../logger.h"
#include <iostream>



using namespace haz;

Logger& logger = Logger::get("main");

namespace test {
void thing (bool thr) {
    logger.ENTERING({ stringify(thr) });
    logger.DEBUG("inside function thing from namespace test");
    logger.STRACKTRACE(0);
    logger.WARN("An exception may occur");
    if (thr) logger.THROWEXCEPTION(std::runtime_error, "This is a bug !");
    logger.EXITING("void");
    return;
}
void other () {
    logger.ENTERING({});
    logger.STRACKTRACE(0);
    logger.INFO("Something long");
    long ii = 0;
    while (ii++ < 1000) {
        std::string s = "";
        long i = 0;
        while (i++ < 1000000)
            s += ".";
    }

    logger.INFO("calling \"thing\"");
    thing(true);
    return logger.RET( "void" );
}

class Clazz {
public:
    Clazz () { /*_logger.addHandler( (new ConsoleHandler())->color(true) );*/ }
    int negate(int number, bool do_it) {
        _logger.ENTERING({ stringify(number), stringify(do_it) });
        _logger.STRACKTRACE(0);
        if (do_it)
            return _logger.RET( stringify(-number), -number);
        else
            return _logger.RET_STR(number);
    }

private:
    Logger& _logger = Logger::get("main.clazz");
};

}

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
    logger.WARN("Begin now");
    //logger.setLevel(Level::OFF);
    logger.ENTERING({});
    logger.STRACKTRACE(0);

    logger.ERROR("with maccro");
    logger.LOG(Level::CONFIG, "config ok !");
    logger.TRACE("calling \"thing\" then \"other\"");
    test::thing(false);
    test::other();

    } catch ( std::runtime_error const& e ) {
        std::cout << "> terminate called after throwing an instance of 'std::runtime_error' " << std::endl << "> what(): " << e.what() << std::endl;
    }


    logger.INFO("Creating clazz...");
    test::Clazz tmp;
    tmp.negate(1000, true);
    logger.WARN("Silence logger main");
    tmp.negate(8000, false);
    logger.setLevel(Level::DEBUG);
    logger.INFO("should not be displayed");
    logger.DEBUG("must be displayed");

    logger.EXITING("0");
    //logger.setLevel(Level::ALL);
    logger.WARN("End now");

    return 0;
}


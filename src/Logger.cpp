#include "Logger.h"

using namespace haz;

// ===== LEVEL =====

std::string Level::to_string(unsigned int i) {
    if (i >= OFF)       return "off";
    if (i >= ERROR)     return "error";
    if (i >= WARNING)   return "warning";
    if (i >= CONFIG)    return "config";
    if (i >= TRACE)     return "trace";
    if (i >= DEBUG)     return "debug";
    if (i >= INFO)      return "info";
                        return "detail";
}


// ===== HANDLER =====

Handler::Handler () : _color(true), _indentation(2), 
    _commonFormat("{col}from {func} ({line}) {bld}{lvl}{clr} : {msg}"),
    _exFormat("{red}{bld}{udl}/!\\\\ Exception{clr} >> {msg}"),
    _stackFormat("{endl}{grn}{bld}{udl}Stack Trace{clr} >>{endl}{beg}\t- {func} AT {line} ({file}){endl}{end}{endl}{endl}"),
    _enteringFormat("{grn}{bld}Entering{clr} {func} ({bld}{msg}{clr})"),
    _exitingFormat("{grn}{bld}Exiting{clr} {func} (return {bld}{msg}{clr})") {}
Handler::~Handler() {}

void Handler::write(Message const& msg, std::string const& fmt) {
    bool escape = false;
    bool meta = false;
    bool num = false;

    std::string out = "";
    std::string var = "";

    for (char const& c : fmt) {
        if (num) {
            if (c >= '0' && c <= '9') {
                var += c;
            } else if (c == '}') {
                for (unsigned int i = std::stoi(var); i > out.size(); out += ' ')
                num = false;
                var = "";
            } else {
                var = "";
                num = false;
                out += c;
            }

            escape = false;
        } else if (meta && c != '}') {
            if (c >= '0' && c <= '9' && var == "") {
                num = true;
                meta = false;
            }
            var += c;
            escape = false;
        } else if (meta) {
            meta = false;

            if (var == "lvl")
                out += Level::to_string(msg.level);

            if (var == "line")
                out += std::to_string(msg.line);

            if (var == "func")
                out += msg.func;

            if (var == "file")
                out += msg.file;

            if (var == "col" && _color)
                out += msg.color;

            if (var == "red" && _color)
                out += Color::RED;

            if (var == "grn" && _color)
                out += Color::GREEN;

            if (var == "blu" && _color)
                out += Color::BLUE;

            if (var == "mag" && _color)
                out += Color::MAGENTA;

            if (var == "cya" && _color)
                out += Color::CYAN;

            if (var == "yel" && _color)
                out += Color::YELLOW;

            if (var == "whi" && _color)
                out += Color::WHITE;

            if (var == "bld" && _color)
                out += BLD;

            if (var == "udl" && _color)
                out += UDL;

            time_t rawtime;
            time(&rawtime);
            tm* t = localtime(&rawtime);

            if (var == "hour") {
                if (t->tm_hour < 10)
                    out += '0';
                out += std::to_string(t->tm_hour);
            }

            if (var == "min") {
                if (t->tm_min < 10)
                    out += '0';
                out += std::to_string(t->tm_min);
            }

            if (var == "sec") {
                if (t->tm_sec < 10)
                    out += '0';
                out += std::to_string(t->tm_sec);
            }

            if (var == "mic") {
                struct timeval tv;
                gettimeofday (&tv, nullptr);
                out += std::to_string(tv.tv_usec % 1000);
            }

            if (var == "mil") {
                struct timeval tv;
                gettimeofday (&tv, nullptr);
                out += std::to_string(tv.tv_usec / 1000);
            }

            if (var == "year")
                out += std::to_string(1900 + t->tm_year);

            if (var == "mon") {
                if (t->tm_mon < 10)
                    out += '0';
                out += std::to_string(t->tm_mon);
            }

            if (var == "day") {
                if (t->tm_mday < 10)
                    out += '0';
                out += std::to_string(t->tm_mday);
            }

            if (var == "clr" && _color)
                out += RST;

            if (var == "msg")
                out += msg.msg;

            if (var == "endl")
                out += '\n';

            var = "";
            escape = false;
        } else {
            if (c == '{' && !escape) {
                meta = true;
            } else if (c == '\\' && !escape) {
                escape = true;
            } else {
                out += c;
                escape = false;
            }
        }
    }

    if (out[out.size()-1] != '\n')
        out += '\n';
    append(out + RST);
}

void Handler::writeMulti(std::vector<Message> const& msgs, std::string const& fmt) {
    bool escape = false;
    bool meta = false;
    bool num = false;

    std::string out = "";
    std::string var = "";
    Message msg = msgs[0];
    std::string loopFmt = "";
    bool beginLoop = false;

    for (char const& c : fmt) {
        if (beginLoop)
            loopFmt += c;

        if (num) {
            if (c >= '0' && c <= '9') {
                var += c;
            } else if (c == '}') {
                for (unsigned int i = std::stoi(var); i > out.size(); out += ' ')
                num = false;
                var = "";
            } else {
                var = "";
                num = false;
                out += c;
            }

            escape = false;
        } else if (meta && c != '}') {
            if (c >= '0' && c <= '9' && var == "") {
                num = true;
                meta = false;
            }
            var += c;
            escape = false;
        } else if (meta) {
            meta = false;

            if (beginLoop) {
                if (var == "end") {
                    beginLoop = false;
                    out += writeLoop(msgs, loopFmt);
                }
            } else {
                if (var == "beg")
                    beginLoop = true;

                else if (var == "lvl")
                    out += Level::to_string(msg.level);

                else if (var == "line")
                    out += std::to_string(msg.line);

                else if (var == "func")
                    out += msg.func;

                else if (var == "file")
                    out += msg.file;

                else if (var == "col" && _color)
                    out += msg.color;

                else if (var == "red" && _color)
                    out += Color::RED;

                else if (var == "grn" && _color)
                    out += Color::GREEN;

                else if (var == "blu" && _color)
                    out += Color::BLUE;

                else if (var == "mag" && _color)
                    out += Color::MAGENTA;

                else if (var == "cya" && _color)
                    out += Color::CYAN;

                else if (var == "yel" && _color)
                    out += Color::YELLOW;

                else if (var == "whi" && _color)
                    out += Color::WHITE;

                else if (var == "bld" && _color)
                    out += BLD;

                else if (var == "udl" && _color)
                    out += UDL;
                
                else {
                    time_t rawtime;
                    time(&rawtime);
                    tm* t = localtime(&rawtime);

                    if (var == "hour") {
                        if (t->tm_hour < 10)
                            out += '0';
                        out += std::to_string(t->tm_hour);
                    }

                    else if (var == "min") {
                        if (t->tm_min < 10)
                            out += '0';
                        out += std::to_string(t->tm_min);
                    }

                    else if (var == "sec") {
                        if (t->tm_sec < 10)
                            out += '0';
                        out += std::to_string(t->tm_sec);
                    }

                    else if (var == "mic") {
                        struct timeval tv;
                        gettimeofday (&tv, nullptr);
                        out += std::to_string(tv.tv_usec % 1000);
                    }

                    else if (var == "mil") {
                        struct timeval tv;
                        gettimeofday (&tv, nullptr);
                        out += std::to_string(tv.tv_usec / 1000);
                    }

                    else if (var == "year")
                        out += std::to_string(1900 + t->tm_year);

                    else if (var == "mon") {
                        if (t->tm_mon < 10)
                            out += '0';
                        out += std::to_string(t->tm_mon);
                    }

                    else if (var == "day") {
                        if (t->tm_mday < 10)
                            out += '0';
                        out += std::to_string(t->tm_mday);
                    }
                }

                if (var == "clr" && _color)
                    out += RST;

                else if (var == "msg")
                    out += msg.msg;

                else if (var == "endl")
                    out += '\n';
            }

            var = "";
            escape = false;
        } else {
            if (c == '{' && !escape) {
                meta = true;
            } else if (c == '\\' && !escape) {
                escape = true;
            } else if (!beginLoop) {
                out += c;
                escape = false;
            }
        }
    }

    if (out[out.size()-1] != '\n')
        out += '\n';
    append(out + RST);
}

std::string Handler::writeLoop(std::vector<Message> const& msgs, std::string const& fmt) {
    bool escape = false;
    bool meta = false;
    bool num = false;

    std::string out = "";
    std::string var = "";


    for (auto msg = ++(msgs.begin()); msg != msgs.end(); ++msg) {
        for (char const& c : fmt) {
            if (num) {
                if (c >= '0' && c <= '9') {
                    var += c;
                } else if (c == '}') {
                    for (unsigned int i = std::stoi(var); i > out.size(); out += ' ')
                    num = false;
                    var = "";
                } else {
                    var = "";
                    num = false;
                    out += c;
                }

                escape = false;
            } else if (meta && c != '}') {
                if (c >= '0' && c <= '9' && var == "") {
                    num = true;
                    meta = false;
                }
                var += c;
                escape = false;
            } else if (meta) {
                meta = false;

                if (var == "lvl")
                    out += Level::to_string(msg->level);

                if (var == "line")
                    out += std::to_string(msg->line);

                if (var == "func")
                    out += msg->func;

                if (var == "file")
                    out += msg->file;

                if (var == "col" && _color)
                    out += msg->color;

                if (var == "red" && _color)
                    out += Color::RED;

                if (var == "grn" && _color)
                    out += Color::GREEN;

                if (var == "blu" && _color)
                    out += Color::BLUE;

                if (var == "mag" && _color)
                    out += Color::MAGENTA;

                if (var == "cya" && _color)
                    out += Color::CYAN;

                if (var == "yel" && _color)
                    out += Color::YELLOW;

                if (var == "whi" && _color)
                    out += Color::WHITE;

                if (var == "bld" && _color)
                    out += BLD;

                if (var == "udl" && _color)
                    out += UDL;

                time_t rawtime;
                time(&rawtime);
                tm* t = localtime(&rawtime);

                if (var == "hour") {
                    if (t->tm_hour < 10)
                        out += '0';
                    out += std::to_string(t->tm_hour);
                }

                if (var == "min") {
                    if (t->tm_min < 10)
                        out += '0';
                    out += std::to_string(t->tm_min);
                }

                if (var == "sec") {
                    if (t->tm_sec < 10)
                        out += '0';
                    out += std::to_string(t->tm_sec);
                }

                if (var == "mic") {
                    struct timeval tv;
                    gettimeofday (&tv, nullptr);
                    out += std::to_string(tv.tv_usec % 1000);
                }

                if (var == "mil") {
                    struct timeval tv;
                    gettimeofday (&tv, nullptr);
                    out += std::to_string(tv.tv_usec / 1000);
                }

                if (var == "year")
                    out += std::to_string(1900 + t->tm_year);

                if (var == "mon") {
                    if (t->tm_mon < 10)
                        out += '0';
                    out += std::to_string(t->tm_mon);
                }

                if (var == "day") {
                    if (t->tm_mday < 10)
                        out += '0';
                    out += std::to_string(t->tm_mday);
                }

                if (var == "clr" && _color)
                    out += RST;

                if (var == "msg")
                    out += msg->msg;

                if (var == "endl")
                    out += '\n';

                var = "";
                escape = false;
            } else {
                if (c == '{' && !escape) {
                    meta = true;
                } else if (c == '\\' && !escape) {
                    escape = true;
                } else {
                    out += c;
                    escape = false;
                }
            }
        }
    }
    return out;
}

ConsoleHandler::ConsoleHandler() {}
ConsoleHandler::~ConsoleHandler() {}

void ConsoleHandler::append(std::string const& msg) {
    std::cout << msg;
}

FileHandler::FileHandler(const char* filename, bool append) {
    if (append)
        os.open(filename, std::ios::out | std::ios::app);
    else 
        os.open(filename, std::ios::out);
    if (!os.is_open())
        throw std::runtime_error("pb");
}

FileHandler::~FileHandler() {
    if (os.is_open())
        os.close();
}

void FileHandler::append(std::string const& msg) {
    os << msg;
}

StreamHandler::StreamHandler(std::ostream const& os) : os(os.rdbuf())  {}
StreamHandler::~StreamHandler() {}

void StreamHandler::append(std::string const& msg) {
    os << msg;
}

// ===== LOGGER =====

std::map<const char*, Logger> Logger::loggers = {};

Logger& Logger::get(const char* name) {
    if (loggers.find(name) == loggers.end())
        loggers.insert( { name, Logger(name) } );
    return loggers[name];
}

Logger::Logger(const char* name) : name(name) {}
Logger::~Logger() { clearHandlers(); }

Logger& Logger::clearHandlers() {
    for (auto& hi : handlers)
        if (hi.destroy_by_logger)
            delete hi.h;
    return *this;
}

Logger& Logger::addHandler(Handler* h, bool destroy_by_logger) {
    handlers.push_back({h, destroy_by_logger});
    return *this;
}

Logger& Logger::setLevel(int l) {
    level = l;
    return *this;
}

Logger& Logger::setColorsLevel(std::vector< std::pair<const char*, unsigned int> > vcl) {
    colorsLevel = vcl;
    return *this;
}

bool Logger::isEnabled(unsigned int l) {
    return l >= level;
}

void Logger::_entering(const char* file, std::string const& func, long line, std::vector<std::string> params) {
    stackTr.push_back( {file, func, line, params} );
    std::string ps = "";
    bool first = true;
    for (auto p : params) {
        if (!first)
            ps += ", ";
        else
            first = false;
        ps += p;
    }
    if (ps == "")
        ps = "no_args";
    for (auto& hi : handlers)
        hi.h->enter( {ps, func, file, line, level, Color::GREEN });
}

void Logger::_exiting(const char* , std::string const& , long line, std::string const& obj) {
    for (auto& hi : handlers)
        hi.h->exit({obj, stackTr.back().func, stackTr.back().file, line, level, Color::GREEN });
    
    stackTr.pop_back();
}

void Logger::_stackTrace(const char* file, std::string const& func, long line, int depth) {
    std::string ps = "";
    auto itr = stackTr.rbegin();
    bool first = true;
    std::vector<Message> msgs;
    msgs.push_back({"", func, file, line, level, Color::GREEN });
    unsigned int num = 0;
    while (itr != stackTr.rend() && (--depth) != 0) {
        msgs.push_back({ std::to_string(num++), itr->func, itr->file, itr->line, level, Color::GREEN });

        if (!first)
            ps += ", ";
        else
            first = false;
        ps += itr->func;
        itr++;
    }

    msgs[0].msg = ps;
    for (auto& hi : handlers)
        hi.h->stack( msgs );
}

void Logger::_log(const char* file, std::string const& func, long line, unsigned int level, std::string const& msg) {
    for (auto& hi : handlers)
        hi.h->common( {msg, func, file, line, level, getColor(level) } );
}

void Logger::_error(const char* file, std::string const& func, long line, std::string const& msg) {
    _log(file, func, line, Level::ERROR, msg);
}

void Logger::_warn(const char* file, std::string const& func, long line, std::string const& msg) {
    _log(file, func, line, Level::WARNING, msg);
}

void Logger::_config(const char* file, std::string const& func, long line, std::string const& msg) {
    _log(file, func, line, Level::CONFIG, msg);
}

void Logger::_trace(const char* file, std::string const& func, long line, std::string const& msg) {
    _log(file, func, line, Level::TRACE, msg);
}

void Logger::_debug(const char* file, std::string const& func, long line, std::string const& msg) {
    _log(file, func, line, Level::DEBUG, msg);
}

void Logger::_info(const char* file, std::string const& func, long line, std::string const& msg) {
    _log(file, func, line, Level::INFO, msg);
}

const char* Logger::getColor(unsigned int i) {
    const char* col = Color::WHITE;
    for (auto p : colorsLevel) {
        if (p.second >= i)
            col = p.first;
        else
            return col;
    }

    return Color::WHITE;
}

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

// ===== FORMAT =====

Format::Format(std::string const& fmt) {
    build_abstract_msg(fmt);
}

void Format::build_abstract_msg(std::string const& fmt) {
    abstract_msg = {};

    bool escape = false;
    std::string cur = "";
    unsigned int size = fmt.size();

    for (unsigned int pos = 0; pos < size; pos++) {
        char c = fmt[pos];
        if (escape) {
            cur += std::string(1, c);
            escape = false;
        } else if (c == '{') {
            abstract_msg.push_back( Var(cur) );
            cur = "";
            abstract_msg.push_back( getMetaVar(fmt, pos) );
        } else if (c == '\\') {
            escape = true;
        }
    }

    if(cur != "")
        abstract_msg.push_back( Var(cur) );
}

Var Format::getMetaVar(std::string const& fmt, unsigned int& pos) {
    ++pos;
    unsigned int size = fmt.size();
    std::string meta = "";
    for (; pos < size; pos++) {
        char c = fmt[pos];
        if (escape) {
            escape = false;
            meta += c;
        } else if (c == '\\')
            escape = true;
        else if (c == '}') {
            break;
        } else
            meta += c;
    }
    if (pos++ >= size)
        return Var("{" + meta);

    if (meta == "func" || meta == "function")
                                                                        return Var(Var::Type::FUNC);
    if (meta == "file")
                                                                        return Var(Var::Type::FILE);
    if (meta == "line")
                                                                        return Var(Var::Type::LINE);
    if (meta == "lvl" || meta == "level")
                                                                        return Var(Var::Type::LEVEL);
    if (meta == "day")
                                                                        return Var(Var::Type::DAY);
    if (meta == "mon" || meta == "month")
                                                                        return Var(Var::Type::MONTH);
    if (meta == "year")
                                                                        return Var(Var::Type::YEAR);
    if (meta == "hour")
                                                                        return Var(Var::Type::HOURS);
    if (meta == "min" || meta == "minute")
                                                                        return Var(Var::Type::MIN);
    if (meta == "sec" || meta == "second")
                                                                        return Var(Var::Type::SEC);
    if (meta == "mil" || meta == "milli" || meta == "millisecond")
                                                                        return Var(Var::Type::MIL);
    if (meta == "mic" || meta == "microsec" || meta == "microsecond")
                                                                        return Var(Var::Type::USEC);
    if (meta == "msg" || meta == "message")
                                                                        return Var(Var::Type::MSG);
    if (meta == "beg" || meta == "begin")
                                                                        return Var(Var::Type::BEG);
    if (meta == "end")
                                                                        return Var(Var::Type::END);
    if (meta == "n" || meta == "endl" || meta == "newline")
                                                                        return Var(Var::Type::NEW_LINE);
    if (meta == "color" || meta == "col")
                                                                        return Var(Var::Type::COLOR);
    if (meta == "red")
                                                                        return Var(Var::Type::META, Color::RED);
    if (meta == "grn" || meta == "green")
                                                                        return Var(Var::Type::META, Color::GREEN);
    if (meta == "blu" || meta == "blue")
                                                                        return Var(Var::Type::META, Color::BLUE);
    if (meta == "mag" || meta == "magenta")
                                                                        return Var(Var::Type::META, Color::MAGENTA);
    if (meta == "cya" || meta == "cyan")
                                                                        return Var(Var::Type::META, Color::CYAN);
    if (meta == "yel" || meta == "yellow")
                                                                        return Var(Var::Type::META, Color::YELLOW);
    if (meta == "whi" || meta == "white")
                                                                        return Var(Var::Type::META, Color::WHITE);
    if (meta == "bld" || meta == "bold")
                                                                        return Var(Var::Type::META, BLD);
    if (meta == "udl" || meta == "undl" || meta == "underline")
                                                                        return Var(Var::Type::META, UDL);
    try {
                                                                        return Var(std::stol(meta)); 
    } catch (std::invalid_argument const&) {}
                                                                        return Var("");
}

std::string Format::formate(std::vector<Message> msgs, bool with_color) {
    if (msgs.empty())
        return;

    std::string out = "";
    Message& main = msgs[0];
    long msg_pos = 0;
    Message& cur = msgs[msg_pos];

    bool end_by_endl = true;
    bool go_to_end = false;

    decltype(abstract_msg)::iterator beg;

    for (auto itr = abstract_msg.begin(); itr != abstract_msg.end(); ++itr) {
        end_by_endl = false;
/*
            STRING, FUNC, FILE, LINE, LEVEL,
            DAY, MONTH, YEAR, HOURS, MIN, SEC, MIL, USEC,
            MSG, BEG, END, NEW_LINE, POS,
            COLOR, META
*/
        if (go_to_end) {
            if (itr->type == Var::Type::END) {
                go_to_end = false;
                itr = beg;
            } else 
                continue;
        }

        switch(itr->type) {
            case Var::Type::STRING : out += itr->str; break;
            case Var::Type::FUNC : out += cur.func; break;
            case Var::Type::FILE : out += cur.file; break;
            case Var::Type::LINE : out += cur.line; break;
            case Var::Type::LEVEL : out += cur.func; break;
            case Var::Type::DAY : out += cur.time.tm_mday; break;
            case Var::Type::MONTH : out += cur.time.tm_mon; break;
            case Var::Type::YEAR : out += cur.time.tm_year; break;
            case Var::Type::HOURS : out += cur.time.tm_hour; break;
            case Var::Type::MIN : out += cur.time.tm_min; break;
            case Var::Type::SEC : out += cur.time.tm_sec; break;
            case Var::Type::MIL : out += std::to_string(cur.usec / 1000); break;
            case Var::Type::USEC : out += std::to_string(cur.usec % 1000); break;
            case Var::Type::MSG : out += cur.msg; break;
            case Var::Type::NEW_LINE : out += "\n"; end_by_endl = true; break;
            case Var::Type::POS : for (; itr->pos > out.size(); out += " "); break;
            case Var::Type::COLOR : if (with_color) out += main.color; break;
            case Var::Type::META : if (with_color) out += itr->str; break;

            case Var::Type::BEG:
                beg = itr;
                if (msg_pos < msgs.size())
                    cur = msgs[++msg_pos];
                else
                    go_to_end = true;
            break;

            default: break;
        }
    }

    if (! end_by_endl)
        out += "\n";
    return out;
}


// ===== HANDLER =====

Handler::Handler () : _color(true),
    _commonFormat("[{day}/{mon}/{year} {hour}:{min}:{sec},{mil} {mic}] {col}from {func} ({line}) {bld}{lvl}{clr} : {msg}"),
    _exFormat("════════════════════════════════════════════════════════════════════════════════════════════════════{endl}[{day}/{mon}/{year} {hour}:{min}:{sec},{mil} {mic}] {red}{bld}/!\\\\ {udl}Exception{clr} >> {msg}"),
    _stackFormat("{endl}\t\t═════{clr} {grn}{bld}Stack Trace{clr} ═════{clr}{endl}{beg}({hour}:{min}:{sec},{mil} {mic}) {func} AT {line} ({file}){endl}{end}{endl}"),
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
                for (unsigned int i = std::stoi(var); i > out.size(); out += ' ');
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

            if (var == "hour") {
                if (msg.time.tm_hour < 10)
                    out += '0';
                out += std::to_string(msg.time.tm_hour);
            }

            if (var == "min") {
                if (msg.time.tm_min < 10)
                    out += '0';
                out += std::to_string(msg.time.tm_min);
            }

            if (var == "sec") {
                if (msg.time.tm_sec < 10)
                    out += '0';
                out += std::to_string(msg.time.tm_sec);
            }

            if (var == "mic") {
                int mic = msg.usec % 1000;
                if (mic < 100) {
                    out += '0';
                    if (mic < 10)
                        out += '0';
                }
                out += std::to_string(mic);
            }

            if (var == "mil") {
                int mil = msg.usec / 1000;
                if (mil < 100) {
                    out += '0';
                    if (mil < 10)
                        out += '0';
                }
                out += std::to_string(mil);
            }

            if (var == "year")
                out += std::to_string(1900 + msg.time.tm_year);

            if (var == "mon") {
                if (msg.time.tm_mon < 10)
                    out += '0';
                out += std::to_string(msg.time.tm_mon);
            }

            if (var == "day") {
                if (msg.time.tm_mday < 10)
                    out += '0';
                out += std::to_string(msg.time.tm_mday);
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
                    if (var == "hour") {
                        if (msg.time.tm_hour < 10)
                            out += '0';
                        out += std::to_string(msg.time.tm_hour);
                    }

                    if (var == "min") {
                        if (msg.time.tm_min < 10)
                            out += '0';
                        out += std::to_string(msg.time.tm_min);
                    }

                    if (var == "sec") {
                        if (msg.time.tm_sec < 10)
                            out += '0';
                        out += std::to_string(msg.time.tm_sec);
                    }

                    if (var == "mic") {
                        int mic = msg.usec % 1000;
                        if (mic < 100) {
                            out += '0';
                            if (mic < 10)
                                out += '0';
                        }
                        out += std::to_string(mic);
                    }

                    if (var == "mil") {
                        int mil = msg.usec / 1000;
                        if (mil < 100) {
                            out += '0';
                            if (mil < 10)
                                out += '0';
                        }
                        out += std::to_string(mil);
                    }

                    if (var == "year")
                        out += std::to_string(1900 + msg.time.tm_year);

                    if (var == "mon") {
                        if (msg.time.tm_mon < 10)
                            out += '0';
                        out += std::to_string(msg.time.tm_mon);
                    }

                    if (var == "day") {
                        if (msg.time.tm_mday < 10)
                            out += '0';
                        out += std::to_string(msg.time.tm_mday);
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

                if (var == "hour") {
                    if (msg->time.tm_hour < 10)
                        out += '0';
                    out += std::to_string(msg->time.tm_hour);
                }

                if (var == "min") {
                    if (msg->time.tm_min < 10)
                        out += '0';
                    out += std::to_string(msg->time.tm_min);
                }

                if (var == "sec") {
                    if (msg->time.tm_sec < 10)
                        out += '0';
                    out += std::to_string(msg->time.tm_sec);
                }

                if (var == "mic") {
                    int mic = msg->usec % 1000;
                    if (mic < 100) {
                        out += '0';
                        if (mic < 10)
                            out += '0';
                    }
                    out += std::to_string(mic);
                }

                if (var == "mil") {
                    int mil = msg->usec / 1000;
                    if (mil < 100) {
                        out += '0';
                        if (mil < 10)
                            out += '0';
                    }
                    out += std::to_string(mil);
                }

                if (var == "year")
                    out += std::to_string(1900 + msg->time.tm_year);

                if (var == "mon") {
                    if (msg->time.tm_mon < 10)
                        out += '0';
                    out += std::to_string(msg->time.tm_mon);
                }

                if (var == "day") {
                    if (msg->time.tm_mday < 10)
                        out += '0';
                    out += std::to_string(msg->time.tm_mday);
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
    tm t = getTime();
    long usec = get_usec();
    stackTr.push_back( {file, func, line, params, usec, t } );
    if (!params.empty()) {
        std::string ps = "";
        bool first = true;
        for (auto p : params) {
            if (!first)
                ps += ", ";
            else
                first = false;
            ps += p;
        }
    } else 
        ps = "void";
    for (auto& hi : handlers)
        hi.h->enter( {ps, func, file, line, level, Color::GREEN, usec, t });
}

void Logger::_exiting(const char* /*file*/, std::string const& /*func*/, long line, std::string const& obj) {
    for (auto& hi : handlers)
        hi.h->exit({obj, stackTr.back().func, stackTr.back().file, line, level, Color::GREEN, get_usec(), getTime() });
    
    stackTr.pop_back();
}

void Logger::_stackTrace(const char* file, std::string const& func, long line, int depth) {
    std::string ps = "";
    auto itr = stackTr.rbegin();
    bool first = true;
    std::vector<Message> msgs;
    msgs.push_back({"", func, file, line, level, Color::GREEN, get_usec(), getTime() });
    unsigned int num = 0;
    while (itr != stackTr.rend() && (--depth) != 0) {
        msgs.push_back({ std::to_string(num++), itr->func, itr->file, itr->line, level, Color::GREEN, itr->usec, itr->time });

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
    if (this->level >= level);
    for (auto& hi : handlers)
        hi.h->common( {msg, func, file, line, level, getColor(level), get_usec(), getTime() } );
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

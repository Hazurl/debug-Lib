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
    unsigned int size = fmt.length();

    for (unsigned int pos = 0; pos < size; ++pos) {
        char const& c = fmt.at(pos);
        if (escape) {
            cur += std::string(1, c);
            escape = false;
        } else if (c == '{') {
            if (cur != "") {
                abstract_msg.push_back( Format::Var(cur) );
                cur = "";
            }
            abstract_msg.push_back( getMetaVar(fmt, pos) );
        } else if (c == '\\') {
            escape = true;
        } else  {
            cur += std::string (1, c);
        }
    }

    if(cur != "")
        abstract_msg.push_back( Format::Var(cur) );
}

Format::Var Format::getMetaVar(std::string const& fmt, unsigned int& pos) {
    ++pos;
    unsigned int size = fmt.length();
    std::string meta = "";
    bool escape = false;
    for (; pos < size; pos++) {
        char const& c = fmt.at(pos);
        if (escape) {
            escape = false;
            meta += std::string(1, c);
        } else if (c == '\\')
            escape = true;
        else if (c == '}') {
            break;
        } else
            meta += std::string(1, c);
    }
    if (pos >= size)
        return Format::Var("{" + meta);

    if (meta == "func" || meta == "function")
                                                                        return Format::Var(Format::Var::Type::FUNC);
    if (meta == "file")
                                                                        return Format::Var(Format::Var::Type::FILE);
    if (meta == "line")
                                                                        return Format::Var(Format::Var::Type::LINE);
    if (meta == "lvl" || meta == "level")
                                                                        return Format::Var(Format::Var::Type::LEVEL);
    if (meta == "day")
                                                                        return Format::Var(Format::Var::Type::DAY);
    if (meta == "mon" || meta == "month")
                                                                        return Format::Var(Format::Var::Type::MONTH);
    if (meta == "year")
                                                                        return Format::Var(Format::Var::Type::YEAR);
    if (meta == "hour")
                                                                        return Format::Var(Format::Var::Type::HOURS);
    if (meta == "min" || meta == "minute")
                                                                        return Format::Var(Format::Var::Type::MIN);
    if (meta == "sec" || meta == "second")
                                                                        return Format::Var(Format::Var::Type::SEC);
    if (meta == "mil" || meta == "milli" || meta == "millisecond")
                                                                        return Format::Var(Format::Var::Type::MIL);
    if (meta == "mic" || meta == "microsec" || meta == "microsecond")
                                                                        return Format::Var(Format::Var::Type::USEC);
    if (meta == "msg" || meta == "message")
                                                                        return Format::Var(Format::Var::Type::MSG);
    if (meta == "beg" || meta == "begin")
                                                                        return Format::Var(Format::Var::Type::BEG);
    if (meta == "end")
                                                                        return Format::Var(Format::Var::Type::END);
    if (meta == "n" || meta == "endl" || meta == "newline")
                                                                        return Format::Var(Format::Var::Type::NEW_LINE);
    if (meta == "color" || meta == "col")
                                                                        return Format::Var(Format::Var::Type::COLOR);
    if (meta == "clear" || meta == "clr")
                                                                        return Format::Var(Format::Var::Type::CLEAR);
    if (meta == "red")
                                                                        return Format::Var(Format::Var::Type::META, Color::RED);
    if (meta == "grn" || meta == "green")
                                                                        return Format::Var(Format::Var::Type::META, Color::GREEN);
    if (meta == "blu" || meta == "blue")
                                                                        return Format::Var(Format::Var::Type::META, Color::BLUE);
    if (meta == "mag" || meta == "magenta")
                                                                        return Format::Var(Format::Var::Type::META, Color::MAGENTA);
    if (meta == "cya" || meta == "cyan")
                                                                        return Format::Var(Format::Var::Type::META, Color::CYAN);
    if (meta == "yel" || meta == "yellow")
                                                                        return Format::Var(Format::Var::Type::META, Color::YELLOW);
    if (meta == "whi" || meta == "white")
                                                                        return Format::Var(Format::Var::Type::META, Color::WHITE);
    if (meta == "bld" || meta == "bold")
                                                                        return Format::Var(Format::Var::Type::META, BLD);
    if (meta == "udl" || meta == "undl" || meta == "underline")
                                                                        return Format::Var(Format::Var::Type::META, UDL);
    try {
                                                                        return Format::Var(std::stol(meta)); 
    } catch (std::invalid_argument const&) {}
                                                                        return Format::Var("");
}

std::string Format::formate(std::vector<Message> msgs, bool with_color) const {
    if (msgs.empty())
        return "";

    std::string out = "";
    Message& main = msgs[0];
    unsigned long msg_pos = 0;
    Message& cur = msgs[msg_pos];

    bool end_by_endl = true;
    bool go_to_end = false;

    decltype(abstract_msg)::const_iterator beg;

    for (auto itr = abstract_msg.begin(); itr != abstract_msg.end(); ++itr) {
        end_by_endl = false;
/*
            STRING, FUNC, FILE, LINE, LEVEL,
            DAY, MONTH, YEAR, HOURS, MIN, SEC, MIL, USEC,
            MSG, BEG, END, NEW_LINE, POS,
            COLOR, META
*/
        if (go_to_end) {
            if (itr->type == Format::Var::Type::END) {
                go_to_end = false;
            }
            continue;
        }

        switch(itr->type) {
            case Format::Var::Type::STRING : out += itr->str; break;
            case Format::Var::Type::FUNC : out += cur.func; break;
            case Format::Var::Type::FILE : out += cur.file; break;
            case Format::Var::Type::LINE : out += std::to_string(cur.line); break;
            case Format::Var::Type::LEVEL : out += Level::to_string(cur.level); break;
            case Format::Var::Type::DAY : out += fill(std::to_string(cur.time.tm_mday), '0', 2); break;
            case Format::Var::Type::MONTH : out += fill(std::to_string(cur.time.tm_mon), '0', 2); break;
            case Format::Var::Type::YEAR : out += std::to_string(1900 + cur.time.tm_year); break;
            case Format::Var::Type::HOURS : out += fill(std::to_string(cur.time.tm_hour), '0', 2); break;
            case Format::Var::Type::MIN : out += fill(std::to_string(cur.time.tm_min), '0', 2); break;
            case Format::Var::Type::SEC : out += fill(std::to_string(cur.time.tm_sec), '0', 2); break;
            case Format::Var::Type::MIL : out += fill(std::to_string(cur.usec / 1000), '0', 3); break;
            case Format::Var::Type::USEC : out += fill(std::to_string(cur.usec % 1000), '0', 3); break;
            case Format::Var::Type::MSG : out += cur.msg; break;
            case Format::Var::Type::NEW_LINE : out += "\n"; end_by_endl = true; break;
            case Format::Var::Type::POS : for (; itr->pos > (long)out.size(); out += " "); break;
            case Format::Var::Type::COLOR : if (with_color) out += main.color; break;
            case Format::Var::Type::CLEAR : if (with_color) out += RST; break;
            case Format::Var::Type::META : if (with_color) out += itr->str; break;

            case Format::Var::Type::BEG:
                beg = itr;
                if (++msg_pos >= msgs.size())
                    go_to_end = true;
                else
                    cur = msgs[msg_pos];
            break;

            case Format::Var::Type::END:
                if (msg_pos + 1 < msgs.size())
                    itr = beg - 1;
            break;

            default: break;
        }
    }

    if (! end_by_endl)
        out += "\n";
    return out;
}

std::string Format::fill(std::string str, char filler, unsigned int size) const {
    for (; size > str.size(); str += std::string(1, filler));
    return str;
}

// ===== HANDLER =====

Handler::Handler () : _color(true),
    _commonFormat("[{day}/{mon}/{year} {hour}:{min}:{sec},{mil} {mic}] {col}from {func} ({line}) {bld}{lvl}{clr} : {msg}"),
    _exFormat("═════════════════════════════════════════════════════════════════════════════════════════════════════{endl}[{day}/{mon}/{year} {hour}:{min}:{sec},{mil} {mic}] {red}{bld}/!\\\\ {udl}Exception{clr} >> {msg}"),
    _stackFormat("{endl}\t\t═════{clr} {grn}{bld}Stack Trace{clr} ═════{clr}{endl}{beg}({hour}:{min}:{sec},{mil} {mic}) {udl}{func}{clr}, line {line} ({file}){endl}{end}{endl}"),
    _enteringFormat("{grn}{bld}Entering{clr} {func} ({bld}{msg}{clr})"),
    _exitingFormat("{grn}{bld}Exiting{clr} {func} (return {bld}{msg}{clr})") {}
Handler::~Handler() {}

void Handler::write(Message const& msg, Format const& fmt) {
    writeMulti({msg}, fmt);
}

void Handler::writeMulti(std::vector<Message> const& msgs, Format const& fmt) {
    if (_color)
        append(fmt.formate(msgs, true) + RST);
    else 
        append(fmt.formate(msgs, false));
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
    std::string ps = "";
    if (!params.empty()) {
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
    if (this->level <= level)
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

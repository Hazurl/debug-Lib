#include "Logger.h"

Transport::Transport(std::string const& filePath) : os(std::cout.rdbuf()) {
    fos.open(filePath, std::ios::out | std::ios::app);
    os.rdbuf(fos.rdbuf());
}

Transport::Transport(std::ostream& os) : os(os.rdbuf()) {}

std::ostream& Transport::getStream() {
    return os;
}

void Transport::setFormat(std::string const& format) {
    this->format = format;
}

void Transport::allowColor(bool c) {
    allowCol = c;
}

void Transport::print(std::string const& msg, std::string const& mainColor, std::string const& level, bool endl) {
    bool escape = false;
    bool meta = false;
    bool num = false;

    std::string out = "";
    std::string var = "";
    int pos = 0;

    for (char const& c : format) {
        if (num) {
            if (c >= '0' && c <= '9') {
                var += c;
            } else if (c == '}') {
                int i = std::stoi(var);
                for (; i > pos; pos++)
                    out += ' ';
                pos++;
                num = false;
                var = "";
            } else {
                var = "";
                num = false;
                out += c;
                pos++;
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

            if (var == "lvl") {
                out += level;
                pos += level.size();
            }

            if (var == "col" && allowCol)
                out += mainColor;

            if (var == "bld" && allowCol)
                out += BLD;

            if (var == "udl" && allowCol)
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
                //if (t->tm_sec < 10)
                    //out += '0';
                struct timeval tv;
                gettimeofday (&tv, nullptr);
                out += std::to_string(tv.tv_usec % 1000);
            }

            if (var == "mil") {
                //if (t->tm_sec < 10)
                    //out += '0';
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

            if (var == "clr" && allowCol)
                out += RST;

            if (var == "msg") {
                out += msg;
                pos += level.size();
            }

            var = "";
            escape = false;
        } else {
            if (c == '{' && !escape) {
                meta = true;
            } else if (c == '\\' && !escape) {
                escape = true;
            } else {
                out += c;
                pos++;
                escape = false;
            }
        }
    }

    append(out + RST, endl);
}

void Transport::append(std::string const& msg, bool endl) {
    os << msg;
    if (endl)
        os << std::endl;
}

Transport::~Transport() {
    if (fos)
        fos.close();
}

Logger::Logger() {
    this->transports["cout"] = new Transport(std::cout);

    this->errTrans.insert("cout");
    this->warnTrans.insert("cout");
    this->logTrans.insert("cout");
    this->infoTrans.insert("cout");
    this->verbTrans.insert("cout");
    this->speTrans.insert("cout");
}

Logger::~Logger() {
    for (auto& tuple : transports)
        delete tuple.second;
}

void Logger::createTransport(Transport* t, std::string const& name) {
    Logger& l = Logger::get();
    if (l.transports.find(name) != l.transports.end()) {
        delete l.transports[name];
    }

    l.transports[name] = t;
}

void Logger::setTransportOf(unsigned int levels, std::string const& name) {
    Logger::cleanTransportOf(levels);
    Logger::addTransportOf(levels, name);
}

void Logger::addTransportOf(unsigned int levels, std::string const& name) {
    Logger& l = Logger::get();

    if (l.transports.find(name) == l.transports.end())
        return;

    if (levels & Logger::ERROR)
        l.errTrans.insert(name);
    if (levels & Logger::WARN)
        l.warnTrans.insert(name);
    if (levels & Logger::LOG)
        l.logTrans.insert(name);
    if (levels & Logger::INFO)
        l.infoTrans.insert(name);
    if (levels & Logger::VERB)
        l.verbTrans.insert(name);
    if (levels & Logger::SPE)
        l.speTrans.insert(name);
}

void Logger::removeTransportOf(unsigned int levels, std::string const& name) {
    Logger& l = Logger::get();

    if (l.transports.find(name) == l.transports.end())
        return;

    if (levels & Logger::ERROR)
        l.errTrans.erase(name);
    if (levels & Logger::WARN)
        l.warnTrans.erase(name);
    if (levels & Logger::LOG)
        l.logTrans.erase(name);
    if (levels & Logger::INFO)
        l.infoTrans.erase(name);
    if (levels & Logger::VERB)
        l.verbTrans.erase(name);
    if (levels & Logger::SPE)
        l.speTrans.erase(name);
}

void Logger::cleanTransportOf(unsigned int levels) {
    Logger& l = Logger::get();

    if (levels & Logger::ERROR)
        l.errTrans.clear();
    if (levels & Logger::WARN)
        l.warnTrans.clear();
    if (levels & Logger::LOG)
        l.logTrans.clear();
    if (levels & Logger::INFO)
        l.infoTrans.clear();
    if (levels & Logger::VERB)
        l.verbTrans.clear();
    if (levels & Logger::SPE)
        l.speTrans.clear();
}

void Logger::setFormatOf(std::string const& name, std::string const& format) {
    Logger& l = Logger::get();

    std::map<std::string, Transport*>::iterator it = l.transports.find(name);

    if (it == l.transports.end())
        return;

    it->second->setFormat(format);
}

void Logger::_error (std::string const& msg) {
    if (Logger::ERROR & getLogsLevel())
        for (std::string const& s : errTrans)
            transports[s]->print(msg, KRED, "error");
}

void Logger::_warning(std::string const& msg) {
    if (Logger::WARN & getLogsLevel())
        for (std::string const& s : warnTrans)
            transports[s]->print(msg, KYEL, "warning");
    }

void Logger::_log(std::string const& msg) {
    if (Logger::LOG & getLogsLevel())
        for (std::string const& s : logTrans)
            transports[s]->print(msg, KBLU, "log");

}

void Logger::_info(std::string const& msg) {
    if (Logger::INFO & getLogsLevel())
        for (std::string const& s : infoTrans)
            transports[s]->print(msg, KCYN, "info");

}

void Logger::_verbose(std::string const& msg) {
    if (Logger::VERB & getLogsLevel())
        for (std::string const& s : verbTrans)
            transports[s]->print(msg, KGRN, "verbose");

}

void Logger::_special(std::string const& msg) {
    if (Logger::SPE & getLogsLevel())
        for (std::string const& s : speTrans)
            transports[s]->print(msg, KMAG, "special");

}

void Logger::allowColorOn(std::string const& name, bool allowC) {
    Logger& l = Logger::get();

    if (l.transports.find(name) == l.transports.end())
        return;

    l.transports[name]->allowColor(allowC);
}

void Logger::section(std::string name, unsigned int levels, unsigned int titleLevels) {
    Logger& l = Logger::get();
    l.curSections.insert(name);
    l.sectionLogsLevel[name] = levels;
    Logger::get().logsLevelChanged = true;

    Logger::get()._title(name, titleLevels);
}

void Logger::section_end(std::string name) {
    Logger::get().curSections.erase(name);
    Logger::get().logsLevelChanged = true;
}

void Logger::showSection(std::string name) {
    Logger::get().sectionsHidden.erase(name);
    Logger::get().logsLevelChanged = true;
}

void Logger::hideSection(std::string name) {
    Logger::get().sectionsHidden.insert(name);
    Logger::get().logsLevelChanged = true;
}

void Logger::title(std::string name, unsigned int levels) {
    Logger::get()._title(name, levels);
}

void Logger::_title(std::string name, unsigned int levels) {
    std::string top = "\t╔";
    for (unsigned i = 0; i < name.size() + 2; ++i) top += "═";
    top += "╗";

    std::string center = "\t║ " + name + " ║";

    std::string bottom = "\t╚";
    for (unsigned i = 0; i < name.size() + 2; ++i) bottom += "═";
    bottom += "╝" RST;

    if (levels & Logger::ERROR & getLogsLevel()) {
        for (std::string const& s : errTrans) {
            transports[s]->append(KRED + top, true);
            transports[s]->append(center, true);
            transports[s]->append(bottom, true);
        }
    }
    if (levels & Logger::WARN & getLogsLevel()) {
        for (std::string const& s : warnTrans) {
            transports[s]->append(KYEL + top, true);
            transports[s]->append(center, true);
            transports[s]->append(bottom, true);
        }
    }
    if (levels & Logger::LOG & getLogsLevel()) {
        for (std::string const& s : logTrans) {
            transports[s]->append(KBLU + top, true);
            transports[s]->append(center, true);
            transports[s]->append(bottom, true);
        }
    }
    if (levels & Logger::INFO & getLogsLevel()) {
        for (std::string const& s : infoTrans) {
            transports[s]->append(KCYN + top, true);
            transports[s]->append(center, true);
            transports[s]->append(bottom, true);
        }
    }
    if (levels & Logger::VERB & getLogsLevel()) {
        for (std::string const& s : verbTrans) {
            transports[s]->append(KGRN + top, true);
            transports[s]->append(center, true);
            transports[s]->append(bottom, true);
        }
    }
    if (levels & Logger::SPE & getLogsLevel()) {
        for (std::string const& s : speTrans) {
            transports[s]->append(KMAG + top, true);
            transports[s]->append(center, true);
            transports[s]->append(bottom, true);
        }
    }
}

unsigned int Logger::getLogsLevel() {
    if(logsLevelChanged) {
        curLogsLevel = mainLogsLevel;

        for (auto& curName : curSections) {
            if (sectionsHidden.find(curName) != sectionsHidden.end())
                return 0;
            
            curLogsLevel &= sectionLogsLevel[curName];
        }

        logsLevelChanged = false;
    }

    return curLogsLevel;
}

void Logger::showOnly(unsigned int levels) {
    Logger::get().mainLogsLevel = levels;
}
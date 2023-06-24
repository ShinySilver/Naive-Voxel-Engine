#include "loguru/loguru.hpp"
#include "server.h"

#define _TABLE_SIZE 4

namespace console {
    namespace {

        bool help_command();

        bool bench_command() {
            return false;
        }

        bool status_command() {
            LOG_S(INFO) << "Current TPS: "<<server::metrics::current_tps;
            LOG_S(INFO) << "Average tick time: "<<server::metrics::average_tick_time_ns*1e-6<<" ms";
            return true;
        }

        bool stop_command() {
            server::stop_interactive_session();
            return true;
        }

        struct CommandStruct {
            std::string command;

            bool (*commandHandler)();
        } commandTable[_TABLE_SIZE] = {
                {"help",   help_command},
                {"status", status_command},
                {"stop",   stop_command},
                {"bench",  bench_command}
        };

        bool help_command() {
            LOG_S(INFO) << "Available commands:";
            for (int i = 0; i < _TABLE_SIZE; ++i) {
                LOG_S(INFO) << "  - " << commandTable[i].command;
            }
            return true;
        }
    }

    void run_command(std::string const &command) {
        for (int i = 0; i < _TABLE_SIZE; ++i) {
            if (command == commandTable[i].command) {
                if (!commandTable[i].commandHandler(/*params*/)) {
                    LOG_S(INFO) << "An error occurred running this command. Is it implemented?";
                }
                return;
            }
        }
        LOG_S(INFO)
        << "There is no command called \"" << command << "\". Type \"help\" for a list of all valid commands.";
    }
}
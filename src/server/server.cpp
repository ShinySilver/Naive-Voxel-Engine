//
// Created by silverly on 20/05/2021.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <loguru.hpp>

#include "server.h"
#include "worldgen/world.h"
#include "workers/server_networking.h"
#include "console.h"

#include <atomic>
#include <thread>
#include <iostream>

#define STATS_UPDATE_PERIOD_IN_TICKS 20
#define TARGET_TPS 20

namespace server {
    namespace metrics { std::atomic<double> average_tick_time_ns, current_tps; }
    namespace {
        std::atomic_bool shutting_down;
        std::thread s;
        bool is_interactive = false;

        void tick() {
            loguru::set_thread_name("server_thread");
            LOG_S(INFO) << "Server started!";

            // Initializing clocks
            auto tick_start_time = std::chrono::high_resolution_clock::now();
            auto tick_group_start_time = std::chrono::high_resolution_clock::now();

            // Initializing stats
            int tick_count = 0;
            int total_tick_time = 0;

            // Starting main loop
            while (!shutting_down) {
                // Starting tick time measure
                tick_start_time = std::chrono::high_resolution_clock::now();

                world::tick();

                // Measuring tick duration
                int tick_duration_ns = (int) duration_cast<std::chrono::nanoseconds>(
                        std::chrono::high_resolution_clock::now() - tick_start_time).count();
                total_tick_time += tick_duration_ns;

                // Eventually updating stats
                if (++tick_count % STATS_UPDATE_PERIOD_IN_TICKS == 0) {
                    int time_elapsed_ns = (int) duration_cast<std::chrono::nanoseconds>(
                            std::chrono::high_resolution_clock::now() - tick_group_start_time).count();
                    tick_group_start_time = std::chrono::high_resolution_clock::now();
                    metrics::average_tick_time_ns = total_tick_time / STATS_UPDATE_PERIOD_IN_TICKS;
                    metrics::current_tps = TARGET_TPS / (time_elapsed_ns/1e9);
                    total_tick_time = 0;
                }

                // At last, sleeping if necessary to avoid going over 20 ticks per secondes
                int sleep_duration = 1e9 / TARGET_TPS - tick_duration_ns;
                if (sleep_duration > 0) {
                    std::this_thread::sleep_for(std::chrono::nanoseconds(sleep_duration));
                }
            }
            LOG_S(INFO) << "Server shutting down";
            server_networking::stop();
        }
    }

    void start() {
        world::init();
        server_networking::init();

        shutting_down = false;
        LOG_S(INFO) << "Server main thread starting...";
        s = std::thread(tick);

        #if SERVER_ONLY
        server::start_interactive_session();
        #endif
    }

    void stop() {
        shutting_down = true;
    }

    void join() {
        if (s.joinable()) {
            s.join();
        }
        server_networking::join();
    }

    void start_interactive_session() {
        LOG_S(INFO) << "Interactive session started. Type \"help\" for the list of the available commands.";
        console::run_command("help");
        is_interactive = true;
        std::string command;
        while (is_interactive) {
            std::cin >> command;
            console::run_command(command);
        }
    }

    void stop_interactive_session() {
        is_interactive = false;
    }
}

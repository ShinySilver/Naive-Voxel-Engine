//
// Created by silver on 02/06/23.
//

#ifndef IVY_DEBUG_SCREEN_H
#define IVY_DEBUG_SCREEN_H

#include <ios>
#include <iomanip>
#include "../utils/shader/text_renderer.h"
#include "../camera.h"
#include "../utils/colors.h"


namespace debug_screen{
    inline void render(TextRenderer &debug_font){
        clock_t t0 = clock();
        debug_font.bind();
        debug_font.renderText("iVy dev build ", 0.0125, 0.98, 0.4, colors::PINK);
        debug_font.renderText(__DATE__ ", " __TIME__, 0.0125, 0.95, 0.4, colors::PINK);

        Location l = camera::get_location();
        std::stringstream ss;
        ss << "X=" << std::fixed << std::setprecision(3) << l.position.x;
        ss << " Y=" << std::fixed << std::setprecision(3) << l.position.y;
        ss << " Z=" << std::fixed << std::setprecision(3) << l.position.z;
        debug_font.renderText(ss.str(), 0.0125, 0.91, 0.4, colors::WHITE);

        ss = std::stringstream();
        ss << "RX=" << std::fixed << std::setprecision(3) << l.rotation.x;
        ss << " RY=" << std::fixed << std::setprecision(3) << l.rotation.y;
        ss << " RZ=" << std::fixed << std::setprecision(3) << l.rotation.z;
        debug_font.renderText(ss.str(), 0.0125, 0.88, 0.4, colors::WHITE);

        #if ALLOW_DEBUG_STATS
        debug_font.renderText("chunks_loaded=" + std::to_string(stats::chunk_loaded),
                              0.0125, 0.84, 0.4, colors::WHITE);
        debug_font.renderText("chunks_loaded_this_tick=" + std::to_string(stats::chunk_loaded_this_tick),
                              0.0125, 0.81, 0.4, colors::WHITE);
        debug_font.renderText("chunks_unloaded_this_tick=" + std::to_string(stats::chunk_unloaded_this_tick),
                              0.0125, 0.78, 0.4, colors::WHITE);

        debug_font.renderText("avg_meshing_time=" +
                              std::to_string(int(round(
                                      stats::total_chunk_meshing_time / stats::total_chunk_meshed /
                                      CLOCKS_PER_SEC * 1000)))+" ms/chunk",
                              0.0125, 0.74, 0.4, colors::WHITE);
        debug_font.renderText("avg_generation_time=" +
                              std::to_string(int(round(
                                      stats::total_chunk_generation_time / stats::total_chunk_generated /
                                      CLOCKS_PER_SEC * 1000)))+" ms/chunk",
                              0.0125, 0.71, 0.4, colors::WHITE);

        debug_font.renderText(
                "avg_time_spent_loading_entities=" +
                std::to_string(int(round(stats::avg_time_spent_loading_entities / CLOCKS_PER_SEC * 1000000))) +
                " us",
                0.0125, 0.67, 0.4, colors::WHITE);
        debug_font.renderText(
                "avg_time_spent_unloading_entities=" +
                std::to_string(
                        int(round(stats::avg_time_spent_unloading_entities / CLOCKS_PER_SEC * 1000000))) +
                " us",
                0.0125, 0.64, 0.4, colors::WHITE);
        debug_font.renderText("avg_time_spent_waiting_for_entity_sync=" +
                              std::to_string(int(round(
                                      stats::avg_time_spent_waiting_for_entity_sync / CLOCKS_PER_SEC *
                                      1000000))) +
                              " us",
                              0.0125, 0.61, 0.4, colors::WHITE);
        debug_font.renderText(
                "avg_time_spent_handling_inputs=" +
                std::to_string(int(round(stats::avg_time_spent_handling_inputs / CLOCKS_PER_SEC * 1000000))) +
                " us",
                0.0125, 0.58, 0.4, colors::WHITE);
        debug_font.renderText(
                "avg_time_spent_updating_camera=" +
                std::to_string(int(round(stats::avg_time_spent_updating_camera / CLOCKS_PER_SEC * 1000000))) +
                " us",
                0.0125, 0.55, 0.4, colors::WHITE);

        debug_font.renderText("avg_time_spent_rendering_ui=" +
                              std::to_string(
                                      int(round(stats::avg_time_spent_rendering_ui / CLOCKS_PER_SEC * 1000))) +
                              " ms",
                              0.0125, 0.51, 0.4, colors::WHITE);
        debug_font.renderText(
                "avg_time_spent_rendering_entities=" +
                std::to_string(int(round(stats::avg_time_spent_rendering_entities / CLOCKS_PER_SEC * 1000))) +
                " ms",
                0.0125, 0.48, 0.4, colors::WHITE);
        debug_font.renderText(
                "avg_time_spent_ticking_entities=" +
                std::to_string(int(round(stats::avg_time_spent_ticking_entities / CLOCKS_PER_SEC * 1000))) +
                " ms",
                0.0125, 0.45, 0.4, colors::WHITE);

        debug_font.renderText(
                "avg_time_spent_waiting_for_gpu=" +
                std::to_string(int(round(stats::avg_time_spent_waiting_for_gpu / CLOCKS_PER_SEC * 1000))) +
                " ms",
                0.0125, 0.41, 0.4, colors::WHITE);
        debug_font.renderText(
                "avg_total_frame_duration=" +
                std::to_string(int(round(stats::avg_frame_duration / CLOCKS_PER_SEC * 1000))) + " ms / "
                + std::to_string(int(round(1. / (stats::avg_frame_duration / CLOCKS_PER_SEC)))) + " FPS",
                0.0125, 0.38, 0.4, colors::PINK);

        debug_font.renderText(
                "loading_queue_size=" +
                std::to_string(chunk_loading::loading_queue.size()) + " entities",
                0.0125, 0.34, 0.4, colors::WHITE);
        debug_font.renderText(
                "unloading_queue_size=" +
                std::to_string(chunk_loading::unloading_queue.size()) + " entities",
                0.0125, 0.31, 0.4, colors::WHITE);

        debug_font.renderText(
                "preloading_queue_size=" +
                std::to_string(chunk_loading::preloading_queue.size()) + " cache entries",
                0.0125, 0.27, 0.4, colors::WHITE);
        debug_font.renderText(
                "cascading_loading_queue_size=" +
                std::to_string(chunk_loading::cascading_loading_queue.size()) + " cache entries",
                0.0125, 0.24, 0.4, colors::WHITE);
        debug_font.unbind();
        stats::avg_time_spent_rendering_ui = stats::avg_time_spent_rendering_ui * 0.99 + 0.01 * (clock() - t0);
        #endif
    }
}

#endif //IVY_DEBUG_SCREEN_H

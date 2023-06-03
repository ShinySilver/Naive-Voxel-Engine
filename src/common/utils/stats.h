//
// Created by silver on 01/06/23.
//

#if ALLOW_DEBUG_STATS
#ifndef IVY_STATS_H
#define IVY_STATS_H

namespace stats {
    // Client loading / unloading
    inline int chunk_unloaded_this_tick, chunk_loaded_this_tick;
    inline int chunk_loaded = 0;
    inline double avg_time_spent_loading_entities = 0;
    inline double avg_time_spent_unloading_entities = 0;

    // Client entity ticks
    inline double avg_time_spent_ticking_entities = 0;
    inline double avg_time_spent_waiting_for_entity_sync = 0;

    // Client Misc
    inline double avg_time_spent_handling_inputs = 0;

    // Client rendering
    inline double time_spent_rendering_entities_this_tick, time_spent_ticking_entities_this_tick,
            time_spent_waiting_for_entity_sync_this_tick;
    inline double avg_time_spent_rendering_entities = 0;
    inline double avg_time_spent_updating_camera = 0;
    inline double avg_frame_duration = 0;
    inline double avg_time_spent_waiting_for_gpu = 0;
    inline double avg_time_spent_rendering_ui = 0;

    // Client workers reporting
    inline std::atomic<double> total_chunk_meshing_time = 0;
    inline std::atomic<double> total_chunk_meshed = 0;
    inline std::atomic<double> total_chunk_generation_time = 0;
    inline std::atomic<double> total_chunk_generated = 0;
}
#endif //IVY_STATS_H
#endif

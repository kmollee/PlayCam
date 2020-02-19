#pragma once

#include <gst/gst.h>

#include <string>

enum state { ON_E, OFF_E };

class GstPipe {
    public:
        GstPipe(const std::string &pipe);
        ~GstPipe();

        int play();
        int stop();

    private:
        const std::string m_pipeStr;
        GstElement *m_pipeline = nullptr;
        state m_state = OFF_E;
};

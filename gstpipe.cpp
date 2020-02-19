#include <unistd.h> // sleep

#include <iostream>
#include <string>

#include "gstpipe.h"

GstPipe::GstPipe(const std::string &pipe) : m_pipeStr(pipe)
{
}

GstPipe::~GstPipe()
{
        if (m_pipeline != nullptr) {
                gst_element_set_state(m_pipeline, GST_STATE_NULL);
                gst_object_unref(m_pipeline);
        }
}

int GstPipe::play()
{
        if (m_state == ON_E) {
                printf("already play\n");
                return 0;
        }

        if (m_pipeline == nullptr) {
                GError *error = nullptr;
                m_pipeline = gst_parse_launch(m_pipeStr.c_str(), &error);
                if (error) {
                        std::cerr
                                << "gst_parse_launch() fail:" << error->message
                                << std::endl;
                        g_error_free(error);
                        return 1;
                }
        }
        printf("play\n");
        GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

        for (int i = 0; i < 5; ++i) {
                ret = gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
                if (ret == GST_STATE_CHANGE_SUCCESS) {
                        printf("play ok!\n");
                        break;
                }
                printf("play fail, wait retry!\n");
                sleep(1);
        }
        if (ret != GST_STATE_CHANGE_SUCCESS) {
                printf("after retry, play fail\n");
                return 1;
        }

        m_state = ON_E;
        return 0;
}

int GstPipe::stop()
{
        if (m_state == OFF_E) {
                printf("already off\n");
                return 0;
        }
        printf("stop\n");
        GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

        for (int i = 0; i < 5; ++i) {
                ret = gst_element_set_state(m_pipeline, GST_STATE_NULL);
                if (ret == GST_STATE_CHANGE_SUCCESS) {
                        printf("stop ok!\n");
                        break;
                }
                printf("stop fail, wait retry!\n");
                sleep(1);
        }
        if (ret != GST_STATE_CHANGE_SUCCESS) {
                printf("after retry, stop fail\n");
                return 1;
        }
        gst_object_unref(m_pipeline);
        m_pipeline = nullptr;
        m_state = OFF_E;
        return 0;
}

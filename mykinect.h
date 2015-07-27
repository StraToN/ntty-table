#ifndef MYKINECT_H
#define MYKINECT_H

#include <libfreenect/libfreenect.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <QMutex>
#include <QMutexLocker>

using namespace cv;


class MyKinect : public Freenect::FreenectDevice {
public:
    MyKinect(freenect_context *_ctx, int _index)
        : Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(FREENECT_DEPTH_11BIT),
          m_buffer_rgb(FREENECT_VIDEO_RGB), m_gamma(2048), m_new_rgb_frame(false),
          m_new_depth_frame(false), depthMat(Size(640,480), CV_16UC1),
          rgbMat(Size(640,480), CV_8UC3, Scalar(0)),
          ownMat(Size(640,480), CV_8UC3, Scalar(0)) {

        for( unsigned int i = 0 ; i < 2048 ; i++) {
            float v = i/2048.0;
            v = std::pow(v, 3)* 6;
            m_gamma[i] = v*6*256;
        }
    }

    void setAngle(double angle)
    {
        setTiltDegrees (angle);
    }

    // Do not call directly even in child
    void VideoCallback(void* _rgb, uint32_t timestamp) {
       // std::cout << "RGB callback" << std::endl;
//        m_rgb_mutex.lock();
        QMutexLocker locker(&m_rgb_mutex);
        uint8_t* rgb = static_cast<uint8_t*>(_rgb);
        rgbMat.data = rgb;
        m_new_rgb_frame = true;
//        m_rgb_mutex.unlock();
    };

    // Do not call directly even in child
    void DepthCallback(void* _depth, uint32_t timestamp) {
       // std::cout << "Depth callback" << std::endl;
//        m_depth_mutex.lock();
        QMutexLocker locker(&m_depth_mutex);
        uint16_t* depth = static_cast<uint16_t*>(_depth);
        depthMat.data = (uchar*) depth;
        m_new_depth_frame = true;
//        m_depth_mutex.unlock();
    }

    bool getVideo(Mat& output) {
//        m_rgb_mutex.lock();
        QMutexLocker locker(&m_rgb_mutex);

        if(m_new_rgb_frame) {
            cv::cvtColor(rgbMat, output, cv::COLOR_RGB2BGR);
            m_new_rgb_frame = false;
//            m_rgb_mutex.unlock();
            return true;
        } else {
//            m_rgb_mutex.unlock();
            return false;
        }
    }

    bool getDepth(Mat& output) {
//        m_depth_mutex.lock();
        QMutexLocker locker(&m_depth_mutex);

        if(m_new_depth_frame) {
            depthMat.copyTo(output);
            m_new_depth_frame = false;
//            m_depth_mutex.unlock();
            return true;
        } else {
//            m_depth_mutex.unlock();
            return false;
        }
    }
private:
    std::vector<uint8_t> m_buffer_depth;
    std::vector<uint8_t> m_buffer_rgb;
    std::vector<uint16_t> m_gamma;
    QMutex m_rgb_mutex;
    QMutex m_depth_mutex;
    bool m_new_rgb_frame;
    bool m_new_depth_frame;
    Mat depthMat;
    Mat rgbMat;
    Mat ownMat;
};

#endif // MYKINECT_H

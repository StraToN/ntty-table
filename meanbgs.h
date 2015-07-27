#ifndef MEANBGS_H
#define MEANBGS_H

#include <opencv2/opencv.hpp>
#include <vector>

class MeanBGS
{
public:
    MeanBGS(int rows, int cols, int nbLearningFrames, int thresh, int maskThresh);
    virtual ~MeanBGS();

    int input(cv::Mat& frame, cv::Mat& outSegmentation);
    int operator()(cv::Mat& frame, cv::Mat& outSegmentation) { return input(frame, outSegmentation); }

    void getMeanModel(cv::Mat& out) { m_meanBG.copyTo (out); }
    cv::Mat getMask();

private:
    int m_cols;
    int m_rows;
    cv::Mat m_meanBG;
    cv::Mat m_mask;
    bool modeIsLearning;
    int m_threshold;
    int m_maskThreshold;

    const int nbLearningFrames;
    std::vector<cv::Mat> listLearningFrames;
};

#endif // MEANBGS_H

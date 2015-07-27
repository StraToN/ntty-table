#include "meanbgs.h"

MeanBGS::MeanBGS(int cols, int rows, int nbLearningFrms, int thresh, int maskThreshold) : m_threshold(thresh), modeIsLearning(true),
    m_maskThreshold(maskThreshold), nbLearningFrames(nbLearningFrms), m_cols(cols), m_rows(rows)
{
    m_meanBG.create(cv::Size(640,480), CV_8UC1);
    m_mask.create(cv::Size(640,480), CV_8UC1);
}

MeanBGS::~MeanBGS()
{

}


cv::Mat MeanBGS::getMask() {
    return m_mask;
}

/***
 *
 */
int MeanBGS::input(cv::Mat& frame, cv::Mat& outSegmentation)
{
    if (modeIsLearning)
    {
        if (listLearningFrames.size () < nbLearningFrames)
        {
            cv::Mat tempM;
            threshold(frame, tempM, m_maskThreshold, 1, cv::THRESH_BINARY_INV);
            listLearningFrames.push_back(tempM);
            std::cout << "Learning : added 1 frame. (total " << listLearningFrames.size () << ")" << std::endl;
        }

        if (listLearningFrames.size () == nbLearningFrames)
        {
            modeIsLearning = false;
            for (int i=0 ; i<m_cols ; i++)
            {
                for (int j=0 ; j<m_rows ; j++)
                {
                    int g=0;

                    for (cv::Mat m : listLearningFrames)
                    {
                        //cv::Vec3b* pixel = m.ptr<cv::Vec3b>(i); // point to first pixel in row
                        uchar* pixel = m.ptr<uchar>(i);
                        g += int(pixel[j]);
                    }
                    m_meanBG.at<uchar>(i,j) = g / nbLearningFrames;
                }
            }
            //threshold(m_meanBG, m_mask, m_maskThreshold, 1, cv::THRESH_BINARY_INV);
        }
        return 1;
    }
    else
    {
        cv::Mat tempFrame;
        frame.copyTo(tempFrame, m_mask);
        cv::imshow("depthThresh", tempFrame);

        for (int i=0 ; i<frame.rows ; i++)
        {
            //cv::Vec3b* pixelCurrent = frame.ptr<cv::Vec3b>(i); // point to first pixel in row
            uchar* pixelCurrent = tempFrame.ptr<uchar>(i);
            //cv::Vec3b* pixelMean = m_meanBG.ptr<cv::Vec3b>(i); // point to first pixel in row
            uchar* pixelMean = m_meanBG.ptr<uchar>(i);


            for (int j=0 ; j<frame.cols ; j++)
            {
                if (std::abs(pixelMean[j] - pixelCurrent[j]) > m_threshold)
                {
                    outSegmentation.at<uchar>(i,j) = 255;
                }
                else {
                    outSegmentation.at<uchar>(i,j) = 0;
                }
            }
        }
        return 0;
    }
}

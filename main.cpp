#include <QString>
#include <QApplication>
#include "mykinect.h"
#include "meanbgs.h"
#include "qsocket.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Freenect::Freenect freenect;
    MyKinect& kinect = freenect.createDevice<MyKinect>(0);
    kinect.setDepthFormat(FREENECT_DEPTH_MM);
    kinect.setAngle(5);
    cv::waitKey (0);

    int ll = 20;
    int l = 20;

    cv::namedWindow("rgb", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("depth", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("depthThresh", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("segmentation", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("segmentationRsz", cv::WINDOW_AUTOSIZE);
//    cv::namedWindow("bgMean", cv::WINDOW_AUTOSIZE);


    kinect.startVideo();
    kinect.startDepth();

    cv::Mat rgbMat(480, 640, CV_8UC3);
    cv::Mat depthMat(480, 640, CV_16UC1, cv::Scalar(0));
    cv::Mat depthf (480, 640, CV_8UC1);
    cv::Mat depthMask(480,640, CV_8UC1);
    cv::Mat depthfMasked (480, 640, CV_8UC1);


    cv::Mat segmentation(480, 640, CV_8UC1, cv::Scalar(0));
    cv::Mat segmentationRsz(l, ll, CV_8UC1);

    int erosion_type = MORPH_RECT;
    int erosion_size = 5;
    Mat element = getStructuringElement( erosion_type,
                                         cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                         cv::Point( erosion_size, erosion_size ) );

    MeanBGS bgs(480, 640, 150, 2, 80);

    // Préparation pour l'envoi socket
    QSocket s;
    s.doConnect("192.168.1.115",8080);
    int count = 0;
    bool die = false;
    while(!die)
    {
        kinect.getVideo(rgbMat);
        kinect.getDepth(depthMat);
        depthMat.convertTo(depthf, CV_8UC1, 255.0/4096.0);
        cv::imshow("rgb", rgbMat);


        for (int i=0; i<depthf.rows ; i++) {
            for (int j=0; j<depthf.cols ; j++) {
                if (depthf.at<uchar>(i,j) == 0)
                    depthf.at<uchar>(i,j) = 255;
            }
        }
        //threshold(depthf, depthMask, 80, 1, THRESH_BINARY_INV); // LES PIXELS DE LA TABLE SONT = 1 !!!!


        // calculer minimum disparité
        //double min = 1, max;
        //cv::minMaxIdx(depthf, &min, &max);
        //min = 1;
        //std::cout << "min " << min << " max = " << max << std::endl;


        // theshold sur ce minimum
        // THRESH_BINARY THRESH_BINARY_INV THRESH_TRUNC THRESH_TOZERO THRESH_TOZERO_INV
       // cv::threshold(depthf, depthMask, max-10, 255, THRESH_BINARY);http://www.e-marchespublics.fr/recherche_d_appels_d_offres_marches_publics_1_aapc_________1-informatique.html
       // depthMask = (depthf >= min + 1);




        cv::imshow("depth", depthf);

        int ok = bgs(depthf, segmentation);

        //cv::imshow("depthThresh", depthf);

        // ET logique entre masque et depthf
        //depthfMask = bgs.getMask();
        //depthf.copyTo(depthfMasked, depthMask);

        if (ok == 0 && count % 25 == 0)
        {
            erode( segmentation, segmentation, element );

            cv::resize(segmentation, segmentationRsz, segmentationRsz.size());
            segmentationRsz = ( segmentationRsz >= 250 );


            cv::imshow("segmentation", segmentation);
            cv::imshow("segmentationRsz", segmentationRsz);
            //bgs.getMeanModel(bgModel);
            //cv::imshow("bgMean", bgModel);

            QByteArray msg;
            s.sendString("$");
            s.sendInt(l);
            s.sendInt(ll);

            for (int i=0 ; i<l ; i++)
            {
                for (int j=0 ; j<ll ; j++)
                {
                     msg.append( (segmentationRsz.at<uchar>(i,j) == 255) ? '1' : '0' );
                }
            }
            //std::cout << msg.toStdString() << std::endl;
            //std::cout << ba.toStdString() << std::endl;
            s.sendString(msg);

        }



        if( (char) cv::waitKey(5) == 'q' ){
            die = true;
        }

        count++;
    }

    cv::destroyWindow("rgb");
    cv::destroyWindow("depth");
    cv::destroyWindow("depthThresh");
    cv::destroyWindow("segmentation");
    cv::destroyWindow("segmentationRsz");
    //    cv::destroyWindow("bgMean");

    kinect.stopVideo();
    kinect.stopDepth();

    return a.exec();
}

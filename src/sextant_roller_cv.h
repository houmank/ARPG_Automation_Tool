#ifndef SEXTANT_ROLLER_CV
#define SEXTANT_ROLLER_CV

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "deque"

namespace SextantRoller
{
    namespace CV
    {
        // custom hash function for cv::Point so i can store it in unordered_set
        struct template_match
        {
        public:
            template_match(std::deque<cv::Point> coords, cv::Mat template_img)
                : m_Coords(coords), m_TemplateImg(template_img) {}

            std::deque<cv::Point> m_Coords;
            cv::Mat m_TemplateImg;
        };

        std::string typeToString(int type);
        std::string MatTypeStr(cv::Mat& mat);
        cv::Mat bitmapToMat(HWND winHandle);
        template_match getInvItems(const std::string& template_name, const cv::Mat& screenshot, double threshold, bool useNMS = false);
        template_match getVoidstones(const std::string& template_name, const cv::Mat& screenshot);
        std::vector<cv::Point> getEachVoidstoneCenterPixel(const template_match& voidstones);
        bool CheckAtlas(cv::Mat& screenshot);
        void drawRectOverMatches(cv::Mat& inImg, const template_match& matches, const cv::Scalar& color, int thickness);
        void drawRotatedRectOverMatches(cv::Mat& inImg, cv::Scalar color, cv::Point center, cv::Size RectSize, double rotationDegrees);
        double getSimilarity(const cv::Mat& A, const cv::Mat& B);
    }
}

#endif //SEXTANT_ROLLER_CV
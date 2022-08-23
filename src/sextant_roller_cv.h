#ifndef SEXTANT_ROLLER_CV
#define SEXTANT_ROLLER_CV

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace SextantRoller
{
    namespace CV
    {
        struct template_match
        {
        public:
            template_match(std::vector<cv::Point> coords, cv::Mat template_img)
                : m_Coords(coords), m_TemplateImg(template_img) {}

            std::vector<cv::Point> m_Coords;
            cv::Mat m_TemplateImg;
        };

        std::string typeToString(int type);
        std::string MatTypeStr(cv::Mat& mat);
        cv::Mat bitmapToMat(HWND winHandle);
        template_match getInvItems(const std::string& template_name, cv::Mat& screenshot, double threshold, bool useNMS = false);
        bool CheckAtlas(cv::Mat& screenshot);
        void drawRectOverMatches(cv::Mat& inImg, const template_match& matches, const cv::Scalar& color, int thickness);
        double getSimilarity(const cv::Mat& A, const cv::Mat& B);
    }
}

#endif //SEXTANT_ROLLER_CV
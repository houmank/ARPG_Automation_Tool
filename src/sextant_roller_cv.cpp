#include "sextant_roller_cv.h"
#include "sextant_roller_constants.h"

namespace SextantRoller
{
namespace CV 
{

    std::string typeToString(int type)
    {
        std::string r;
        unsigned char depth = type & CV_MAT_DEPTH_MASK;
        unsigned char chans = 1 + (type >> CV_CN_SHIFT);

        switch (depth)
        {
        case CV_8U:
            r = "8U";
            break;
        case CV_8S:
            r = "8S";
            break;
        case CV_16U:
            r = "16U";
            break;
        case CV_16S:
            r = "16S";
            break;
        case CV_32S:
            r = "32S";
            break;
        case CV_32F:
            r = "32F";
            break;
        case CV_64F:
            r = "64F";
            break;
        default:
            r = "User Defined";
            break;
        }

        r += "C";
        r += chans + '0';
        return r;
    }

    std::string MatTypeStr(cv::Mat &mat)
    {
        std::string type = typeToString(mat.type());
        type += " " + std::to_string(mat.cols) + "x" + std::to_string(mat.rows) + "\n";
        return type;
    }

    cv::Mat bitmapToMat(HWND winHandle)
    {
        cv::Mat matrix;
        RECT rc;
        if (winHandle == NULL || !GetClientRect(winHandle, &rc))
            return matrix;

        // create device context
        HDC hdcScreen = GetDC(NULL);
        HDC hdcWindow = GetDC(winHandle);
        HDC hdcMem = CreateCompatibleDC(hdcWindow);
        if (!hdcMem)
            goto done;

        // This is the best stretch mode.
        SetStretchBltMode(hdcWindow, HALFTONE);

        // The source DC is the entire screen, and the destination DC is the current window (HWND).
        if (!StretchBlt(hdcWindow,
                        0, 0,
                        rc.right, rc.bottom,
                        hdcScreen,
                        0, 0,
                        GetSystemMetrics(SM_CXSCREEN),
                        GetSystemMetrics(SM_CYSCREEN),
                        SRCCOPY))
        {
            goto done;
        }

        HBITMAP hbmp = CreateCompatibleBitmap(hdcWindow, rc.right - rc.left, rc.bottom - rc.top);
        if (!hbmp)
            goto done;
        SelectObject(hdcMem, hbmp);

        // load pixels to mem, PW_RENDERFULLCONTENT | PW_CLIENTONLY
        // PrintWindow(winHandle, hdc, PW_RENDERFULLCONTENT); // Assuming its not minimized rn, https://www.codeproject.com/Articles/20651/Capturing-Minimized-Window-A-Kid-s-Trick

        if (!BitBlt(
                hdcMem, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdcWindow, 0, 0, SRCCOPY))
        {
            goto done;
        }

        // bitmapinfo
        BITMAPINFOHEADER bi;
        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = rc.right;
        bi.biHeight = -rc.bottom;
        bi.biPlanes = 1;
        bi.biBitCount = 32;
        bi.biCompression = BI_RGB;
        bi.biSizeImage = 0;
        bi.biXPelsPerMeter = 0;
        bi.biYPelsPerMeter = 0;
        bi.biClrUsed = 0;
        bi.biClrImportant = 0;

        // Transform bitmap to opencv matrix
        matrix = {rc.bottom, rc.right, CV_8UC4};
        GetDIBits(hdcMem, hbmp, 0, rc.bottom, matrix.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);

        // cleanup
    done:
        DeleteDC(hdcMem);
        DeleteObject(hbmp);
        ReleaseDC(NULL, hdcScreen);
        ReleaseDC(winHandle, hdcWindow);

        return matrix;
    }

    // NMS Flag will determine if non maximum suppression (NMS) or adaptive thresholding is used when cleaning up
    // noise
    template_match getInvItems(const std::string& template_name, const cv::Mat& screenshot, double threshold, bool useNMS)
    {
        std::vector<cv::Point> coords;

        // Search for template in inventory
        cv::Mat template_og = cv::imread(PATH_TO_IMGS + template_name, cv::IMREAD_COLOR);
        cv::Mat template_img, ss_img;

        // crop inventory
        int inv_x = screenshot.cols * INV_TOP_LEFT_X_RATIO;
        int inv_y = screenshot.rows * INV_TOP_LEFT_Y_RATIO;

        cv::Rect inv_crop(inv_x, inv_y, screenshot.cols - inv_x , screenshot.rows - inv_y);
        ss_img = (screenshot)(inv_crop);

        //check if need to resize template
        if(screenshot.cols != DEFAULT_RESOLUTION_X || screenshot.rows != DEFAULT_RESOLUTION_Y)
        {
            double x_ratio = (double)screenshot.cols / DEFAULT_RESOLUTION_X;
            double y_ratio = (double)screenshot.rows / DEFAULT_RESOLUTION_Y;
            cv::resize(template_og, template_og, cv::Size(), x_ratio, y_ratio);
        }
        
        // convert template to 8UC4 since bitmap screenshot is 8UC4
        cv::cvtColor(template_og, template_img, cv::COLOR_BGR2BGRA);

        // if want to convert to grayscale for both and proceed
        // cv::cvtColor(ss_gray, ss_gray, cv::COLOR_BGR2GRAY);
        // cv::cvtColor(template_og, template_img, cv::COLOR_BGR2GRAY);

        cv::Mat res_32f(ss_img.rows - template_img.rows + 1, ss_img.cols - template_img.cols + 1, CV_32FC1); 
        cv::matchTemplate(ss_img, template_img, res_32f, cv::TM_CCOEFF_NORMED);

        // attempt to minimize false positives
        cv::Mat res;
        cv::Mat mask_local_maxima;
        cv::Mat res_dilated;
        int sextant_odd_size;
        res_32f.convertTo(res, CV_8U, 255.0);
    
        // yoinked from https://stackoverflow.com/a/23183388
        if(!useNMS){
            sextant_odd_size = ((template_img.cols + template_img.rows) / 4) * 2 + 1; //force size to be odd
            adaptiveThreshold(res, res, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, sextant_odd_size, -128); 
        }
        else
        {
            cv::dilate(res_32f, res_dilated, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3)));
            cv::compare(res_32f, res_dilated, mask_local_maxima, cv::CMP_GE);
            res_32f.setTo((0), mask_local_maxima);
        }

        while(true)
        {
            // Get coords for matches
            cv::Mat& finalRes = (useNMS) ? res_32f : res;
            double minVal, maxVal;
            cv::Point minLoc, maxLoc;
            cv::minMaxLoc(finalRes, &minVal, &maxVal, &minLoc, &maxLoc);

            if(maxVal >= threshold)
            {
                cv::Point newLoc(maxLoc.x + inv_x, maxLoc.y + inv_y);
                cv::floodFill(finalRes, maxLoc, 0); //mark drawn blob
                coords.push_back(std::move(newLoc));
            }
            else 
                break;
        }

        return template_match(std::move(coords), std::move(template_og));
    }

    template_match getVoidstones(const std::string& template_name, const cv::Mat& screenshot)
    {
        std::vector<cv::Point> coords;

        // Search for template in inventory
        cv::Mat template_og = cv::imread(PATH_TO_IMGS + template_name, cv::IMREAD_COLOR);
        cv::Mat template_img, ss_img;
        cv::imshow("0", template_og);
        // crop voidstones
        int voidstone_x = screenshot.cols * VOIDSTONE_LEFT_X_RATIO;
        int voidstone_y = screenshot.rows * VOIDSTONE_TOP_Y_RATIO;
        int voidstone_width = (screenshot.cols * VOIDSTONE_RIGHT_X_RATIO) - voidstone_x;
        int voidstone_height = (screenshot.rows * VOIDSTONE_BOTTOM_Y_RATIO) - voidstone_y;

        cv::Rect voidstone_crop(voidstone_x, voidstone_y, voidstone_width, voidstone_height);
        ss_img = (screenshot)(voidstone_crop);

        //check if need to resize template
        if(screenshot.cols != DEFAULT_RESOLUTION_X || screenshot.rows != DEFAULT_RESOLUTION_Y)
        {
            double x_ratio = (double)screenshot.cols / DEFAULT_RESOLUTION_X;
            double y_ratio = (double)screenshot.rows / DEFAULT_RESOLUTION_Y;
            cv::resize(template_og, template_og, cv::Size(), x_ratio, y_ratio);
        }
        
        // convert template to 8UC4 since bitmap screenshot is 8UC4
        cv::cvtColor(template_og, template_img, cv::COLOR_BGR2BGRA);
        
        cv::imshow("1", ss_img);
        cv::imshow("2", template_img);
        cv::waitKey(0);
        cv::destroyAllWindows();

        cv::Mat res_32f(ss_img.rows - template_img.rows + 1, ss_img.cols - template_img.cols + 1, CV_32FC1); 
        cv::matchTemplate(ss_img, template_img, res_32f, cv::TM_CCOEFF_NORMED);

        // attempt to minimize false positives
        cv::Mat res;
        res_32f.convertTo(res, CV_8U, 255.0);
    
        // yoinked from https://stackoverflow.com/a/23183388
        int sextant_odd_size = ((template_img.cols + template_img.rows) / 4) * 2 + 1; //force size to be odd
        adaptiveThreshold(res, res, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, sextant_odd_size, -128); 

        // Get coords for matches
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(res, &minVal, &maxVal, &minLoc, &maxLoc);

        if(maxVal >= 1)
        {
            cv::Point newLoc(maxLoc.x + voidstone_x, maxLoc.y + voidstone_y);
            cv::floodFill(res, maxLoc, 0); //mark drawn blob
            coords.push_back(std::move(newLoc));
        }

        return template_match(std::move(coords), std::move(template_og));
    }

    bool CheckAtlas(cv::Mat& screenshot)
    {
        cv::Mat atlas_badge = cv::imread(PATH_TO_IMGS + "atlas_badge.png", cv::IMREAD_COLOR);
        cv::Mat atlas_img, ss_atlas;

        // crop top of screen for matching
        int crop_x = screenshot.cols * BADGE_LEFT_X_RATIO;
        int crop_x_right = screenshot.cols * BADGE_RIGHT_X_RATIO;
        int crop_y = screenshot.rows * BADGE_BOTTOM_Y_RATIO;

        cv::Rect atlas_crop(crop_x, 0, crop_x_right - crop_x, crop_y);
        ss_atlas = (screenshot)(atlas_crop);

        //check if need to resize template
        if(screenshot.cols != DEFAULT_RESOLUTION_X || screenshot.rows != DEFAULT_RESOLUTION_Y)
        {
            double x_ratio = (double)screenshot.cols / DEFAULT_RESOLUTION_X;
            double y_ratio = (double)screenshot.rows / DEFAULT_RESOLUTION_Y;
            cv::resize(atlas_badge, atlas_badge, cv::Size(), x_ratio, y_ratio);
        }
        
        // convert template to 8UC4 since bitmap screenshot is 8UC4
        cv::cvtColor(atlas_badge, atlas_img, cv::COLOR_BGR2BGRA);

        cv::Mat res_32f_atlas(ss_atlas.rows - atlas_img.rows + 1, ss_atlas.cols - atlas_img.cols + 1, CV_32FC1);
        cv::matchTemplate(ss_atlas, atlas_img, res_32f_atlas, cv::TM_CCOEFF_NORMED);

        cv::Mat res_atlas;
        res_32f_atlas.convertTo(res_atlas, CV_8U, 255.0);
    
        // yoinked from https://stackoverflow.com/questions/23180630/using-opencv-matchtemplate-for-blister-pack-inspection
        int atlas_odd_size = ((atlas_img.cols + atlas_img.rows) / 4) * 2 + 1; //force size to be odd
        adaptiveThreshold(res_atlas, res_atlas, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, atlas_odd_size, -128);

        // draw rectangles
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(res_atlas, &minVal, &maxVal, &minLoc, &maxLoc);
        if (maxVal > 0)
            return true;
        else
            return false;
    }

    void drawRectOverMatches(cv::Mat& inImg, const template_match& matches, const cv::Scalar& color, int thickness)
    {
        for(auto& point : matches.m_Coords)
            cv::rectangle(inImg, point, cv::Point(point.x + matches.m_TemplateImg.cols, 
            point.y + matches.m_TemplateImg.rows), color, thickness);
    }

    // https://stackoverflow.com/a/19708947
    double getSimilarity(const cv::Mat& A, const cv::Mat& B)
    {
        if (A.rows > 0 && A.rows == B.rows && A.cols > 0 && A.cols == B.cols)
        {
            // Calculate the L2 relative error between images.
            double errorL2 = cv::norm(A, B, cv::NORM_L2);
            // Convert to a reasonable scale, since L2 error is summed across all pixels of the image.
            double similarity = errorL2 / (double)(A.rows * A.cols);
            return similarity;
        }
        else
        {
            // Images have a different size
            return 100000000.0; // Return a bad value
        }
    }
}
}
#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

namespace ipsimple {
	int Initialize(std::vector<cv::Point2f> &vecLeftRect, std::vector<cv::Point2f> &vecRightRect);
	int BlackboardProcessing(cv::Mat &matCurrImg, cv::Mat &matDstImg, bool flag);
}

namespace ipstandard {
	int Initialize(std::vector<cv::Point2f> &vecLeftRect, std::vector<cv::Point2f> &vecRightRect);
	int BlackboardProcessing(cv::Mat &matCurrImg, cv::Mat &matDstImg, bool flag);
}
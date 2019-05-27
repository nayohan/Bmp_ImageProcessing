#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int jpgToBmp(void) {
	cv::Mat image = cv::imread("image.png");
	if (image.empty()) {
		printf("error");
		return -1;
	}
	imwrite("input.bmp", image);
}
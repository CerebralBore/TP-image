#include <highgui.h>
#include <iostream>

cv::Vec3b filtreMatrice(cv::Mat matImage, cv::Mat matFiltre)
{
	int coef = 0;
	int totalR = 0;
	int totalG = 0;
	int totalB = 0;
	cv::Vec3b pix;
	for(int i=0; i<matImage.rows; i++)
	{
		for(int j=0; j<matImage.cols; j++)
		{
			coef = coef + matFiltre.at<double>(i,j);
			totalR = totalR + matImage.at<cv::Vec3b>(i,j)[2] * matFiltre.at<double>(i,j);
			totalG = totalG + matImage.at<cv::Vec3b>(i,j)[1] * matFiltre.at<double>(i,j);
			totalB = totalB + matImage.at<cv::Vec3b>(i,j)[0] * matFiltre.at<double>(i,j);
		}
	}
	pix[2] = totalR/coef;
	pix[1] = totalG/coef;
	pix[0] = totalB/coef;

	return(pix);
}


cv::Mat filtreImage(cv::Mat imageS)
{
	cv::Range row;
	cv::Range col;
	double m[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
	cv::Mat filtre = cv::Mat(3, 3, CV_64F, m);
	cv::Mat imageF = imageS.clone();

	cv::Mat matImage;
	for(int i=1; i<(imageS.rows-1); i++)
	{
		for(int j=1; j<(imageS.cols-1); j++)
		{
			row = cv::Range((i-1),(i+2));
			col = cv::Range((j-1),(j+2));
			matImage = cv::Mat(imageS, row, col);
			imageF.at<cv::Vec3b>(i,j) = filtreMatrice(matImage, filtre);
		}
	}
	return imageF;
}

int main (int argc, char* argv[])
{
	// Chemin des images
	std::string path1 = "../image/lena.jpg";
	std::string path2 = "../image/lenaModif.jpg";

	// Ouverture de l'image
	cv::Mat img1 = cv::imread (path1);

	if (img1.empty ())
	{
		std::cerr << "Couldn't open image: " << path1 << std::endl;
		cv::waitKey ();
		return EXIT_FAILURE;
	}

	cv::Mat img2 = filtreImage(img1);

	cv::imshow("lena", img1);
	cv::waitKey ();
	cv::imshow("lenaModif", img2);
	cv::imwrite(path2,img2);

	

	std::cout << "Appuyez sur une touche pour continuer" << std::endl;
	cv::waitKey ();


	return EXIT_SUCCESS;
}

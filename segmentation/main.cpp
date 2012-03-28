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


cv::Mat gris(cv::Mat imageS)
{
	cv::Mat imageF = imageS.clone();
	int red, blue, green;

	for(int i=0; i<(imageS.rows); i++)
	{
		for(int j=0; j<(imageS.cols); j++)
		{	
			//remplir d'une couleur
			//imageF.at<cv::Vec3b>(i,j) = cv::Vec3b(255,255,255);
			//channels
			 red = imageF.at<cv::Vec3b>(i,j)[2];
			 blue = imageF.at<cv::Vec3b>(i,j)[0];
			 green = imageF.at<cv::Vec3b>(i,j)[1];
			 for(int k=0; k<3; k++){
				imageF.at<cv::Vec3b>(i,j)[k] = (red + blue + green)/3 ;
			 }
		}
	}
		
return imageF;
}

cv::Mat normalize(cv::Mat imageS)
{
	cv::Mat imageF = imageS.clone();
	int min, max;

	for(int k=0; k<3; k++)
	{
		//initialisation
		min = 255;
		max = 0;

		//recherche du maximum et minimum de l'histogramme dans l'image
		for(int i=0; i<imageS.rows; i++)
		{
			for(int j=0; j<imageS.cols; j++)
			{	
				if(imageF.at<cv::Vec3b>(i,j)[k] < min){
					min = imageF.at<cv::Vec3b>(i,j)[k];
				}
				if(imageF.at<cv::Vec3b>(i,j)[k] > max){
					max = imageF.at<cv::Vec3b>(i,j)[k];
				}
			}
		}

		std::cout<<k<<" "<<min<<" "<<max<<std::endl;
	
		//etirement de l'histogramme
		for(int i=0; i<imageS.rows; i++)
		{
			for(int j=0; j<imageS.cols; j++)
			{	
				imageF.at<cv::Vec3b>(i,j)[k] = (imageF.at<cv::Vec3b>(i,j)[k] - min) * 255/(max-min); 
			 }
		}
	}
return imageF;
}

int main (int argc, char* argv[])
{
	// Initialisation
	std::string path1 = "../image/lena.jpg";
	std::string path2 = "../image/lenaModif.jpg";

	// Ouverture de l'image
	cv::Mat imgO = cv::imread (path1);
	if (imgO.empty ()){
		std::cerr << "Couldn't open image: " << path1 << std::endl;
		cv::waitKey ();
		return EXIT_FAILURE;
	}
	
	//application du filtre
	cv::Mat imgF = filtreImage(imgO);
	cv::Mat imgGris = gris(imgO);
	cv::Mat imgGrisN = normalize(imgGris);
	cv::Mat imgN = normalize(imgO);
	
	//affichage
	cvResizeWindow("win1",300,300);

	cv::imshow("lena", imgO);
	cv::waitKey ();
	cv::imshow("lenaFiltre", imgF);
	cv::waitKey ();
	cv::imshow("lenaGris", imgGris);
	cv::waitKey ();
	cv::imshow("lenaGrisNormalize", imgGrisN);
	cv::waitKey ();
	cv::imshow("lenaNormalize", imgN);

	//ecriture
	cv::imwrite(path2,imgGrisN);

	std::cout << "Appuyez sur une touche pour continuer" << std::endl;
	cv::waitKey ();
	return EXIT_SUCCESS;
}

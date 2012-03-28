#include <highgui.h>
#include <iostream>


void filtre(cv::Mat image){

	double m[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};  
	cv::Mat( 3, 3, CV_32FC2, m);	
	//image * filtre;
}

int main (int argc, char* argv[])
{
	// Remplacez la variable path par le chemin d'une image sur votre machine
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

	cv::Mat img2 = img1.clone();

	// Affichage de l'image dans une fenêtre intitulée "Hello World!"
	cv::imshow("lena", img1);
	cv::waitKey ();
	cv::imshow("lenaModif", img2);
	cv::imwrite(path2,img2);

	filtre(img1);

	// Pause
	std::cout << "Appuyez sur une touche pour continuer" << std::endl;
	cv::waitKey ();


	return EXIT_SUCCESS;
}

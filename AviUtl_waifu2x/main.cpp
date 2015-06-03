#define NOMINMAX
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>
#include "picojson.h"
#include "modelHandler.hpp"
#include "convertRoutine.hpp"
//#include "filterGL.h"
#include "filter.h"

//---------------------------------------------------------------------
//		�t�B���^�\���̒�`
//---------------------------------------------------------------------
#define	TRACK_N	4														//	�g���b�N�o�[�̐�
TCHAR	*track_name[] = { "Denoise", "Scaling", "Threads","Block" };	//	�g���b�N�o�[�̖��O
int		track_default[] = { 0, 200, 4, 512 };	//	�g���b�N�o�[�̏����l
int		track_s[] = { 0, 100, 1, 32 };	//	�g���b�N�o�[�̉����l
int		track_e[] = { 2, 400, 16, 1024 };	//	�g���b�N�o�[�̏���l

#define	CHECK_N	2														//	�`�F�b�N�{�b�N�X�̐�
TCHAR	*check_name[] = { "HELP", "DONATE" };				//	�`�F�b�N�{�b�N�X�̖��O
int		check_default[] = { -1, -1 };				//	�`�F�b�N�{�b�N�X�̏����l (�l��0��1)

FILTER_DLL filter = {
	FILTER_FLAG_EX_INFORMATION,	//	�t�B���^�̃t���O
	//	FILTER_FLAG_ALWAYS_ACTIVE		: �t�B���^����ɃA�N�e�B�u�ɂ��܂�
	//	FILTER_FLAG_CONFIG_POPUP		: �ݒ���|�b�v�A�b�v���j���[�ɂ��܂�
	//	FILTER_FLAG_CONFIG_CHECK		: �ݒ���`�F�b�N�{�b�N�X���j���[�ɂ��܂�
	//	FILTER_FLAG_CONFIG_RADIO		: �ݒ�����W�I�{�^�����j���[�ɂ��܂�
	//	FILTER_FLAG_EX_DATA				: �g���f�[�^��ۑ��o����悤�ɂ��܂��B
	//	FILTER_FLAG_PRIORITY_HIGHEST	: �t�B���^�̃v���C�I���e�B����ɍŏ�ʂɂ��܂�
	//	FILTER_FLAG_PRIORITY_LOWEST		: �t�B���^�̃v���C�I���e�B����ɍŉ��ʂɂ��܂�
	//	FILTER_FLAG_WINDOW_THICKFRAME	: �T�C�Y�ύX�\�ȃE�B���h�E�����܂�
	//	FILTER_FLAG_WINDOW_SIZE			: �ݒ�E�B���h�E�̃T�C�Y���w��o����悤�ɂ��܂�
	//	FILTER_FLAG_DISP_FILTER			: �\���t�B���^�ɂ��܂�
	//	FILTER_FLAG_EX_INFORMATION		: �t�B���^�̊g������ݒ�ł���悤�ɂ��܂�
	//	FILTER_FLAG_NO_CONFIG			: �ݒ�E�B���h�E��\�����Ȃ��悤�ɂ��܂�
	//	FILTER_FLAG_AUDIO_FILTER		: �I�[�f�B�I�t�B���^�ɂ��܂�
	//	FILTER_FLAG_RADIO_BUTTON		: �`�F�b�N�{�b�N�X�����W�I�{�^���ɂ��܂�
	//	FILTER_FLAG_WINDOW_HSCROLL		: �����X�N���[���o�[�����E�B���h�E�����܂�
	//	FILTER_FLAG_WINDOW_VSCROLL		: �����X�N���[���o�[�����E�B���h�E�����܂�
	//	FILTER_FLAG_IMPORT				: �C���|�[�g���j���[�����܂�
	//	FILTER_FLAG_EXPORT				: �G�N�X�|�[�g���j���[�����܂�
	0, 0,						//	�ݒ�E�C���h�E�̃T�C�Y (FILTER_FLAG_WINDOW_SIZE�������Ă��鎞�ɗL��)
	"AviUtl-waifu2x-GL",			//	�t�B���^�̖��O
	TRACK_N,					//	�g���b�N�o�[�̐� (0�Ȃ疼�O�����l����NULL�ł悢)
	track_name,					//	�g���b�N�o�[�̖��O�S�ւ̃|�C���^
	track_default,				//	�g���b�N�o�[�̏����l�S�ւ̃|�C���^
	track_s, track_e,			//	�g���b�N�o�[�̐��l�̉������ (NULL�Ȃ�S��0�`256)
	CHECK_N,					//	�`�F�b�N�{�b�N�X�̐� (0�Ȃ疼�O�����l����NULL�ł悢)
	check_name,					//	�`�F�b�N�{�b�N�X�̖��O�S�ւ̃|�C���^
	check_default,				//	�`�F�b�N�{�b�N�X�̏����l�S�ւ̃|�C���^
	func_proc,					//	�t�B���^�����֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
	NULL,						//	�J�n���ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
	NULL,						//	�I�����ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
	NULL,						//	�ݒ肪�ύX���ꂽ�Ƃ��ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
	func_WndProc,						//	�ݒ�E�B���h�E�ɃE�B���h�E���b�Z�[�W���������ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
	NULL, NULL,					//	�V�X�e���Ŏg���܂��̂Ŏg�p���Ȃ��ł�������
	NULL,						//  �g���f�[�^�̈�ւ̃|�C���^ (FILTER_FLAG_EX_DATA�������Ă��鎞�ɗL��)
	NULL,						//  �g���f�[�^�T�C�Y (FILTER_FLAG_EX_DATA�������Ă��鎞�ɗL��)
	"AviUtl-waifu2x-GL by MaverickTse",
	//  �t�B���^���ւ̃|�C���^ (FILTER_FLAG_EX_INFORMATION�������Ă��鎞�ɗL��)
	NULL,						//	�Z�[�u���J�n����钼�O�ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
	NULL,						//	�Z�[�u���I���������O�ɌĂ΂��֐��ւ̃|�C���^ (NULL�Ȃ�Ă΂�܂���)
};


//---------------------------------------------------------------------
//		�t�B���^�\���̂̃|�C���^��n���֐�
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport) * __stdcall GetFilterTable(void)
{
	return &filter;
}
//	���L�̂悤�ɂ����1��auf�t�@�C���ŕ����̃t�B���^�\���̂�n�����Ƃ��o���܂�
/*
FILTER_DLL *filter_list[] = {&filter,&filter2,NULL};
EXTERN_C FILTER_DLL __declspec(dllexport) ** __stdcall GetFilterTableList( void )
{
return (FILTER_DLL **)&filter_list;
}
*/


//---------------------------------------------------------------------
//		�t�B���^�����֐�
//---------------------------------------------------------------------
BOOL func_proc(FILTER *fp, FILTER_PROC_INFO *fpip)
{
	if (!fp->exfunc->is_editing(fpip->editp) || !fp->exfunc->is_filter_active(fp) || !fpip)
		return FALSE; //prevent processing if nothing valid for editing
	//***************************************//
	//       STARTS TIMIMG                  //
	//**************************************//
	int64 t1 = cv::getTickCount();
	//--------------------------------------//
	//Wrap ycp_edit into OCV Mat
	cv::Mat utlSrc(fpip->h, fpip->w, CV_16SC3, fpip->ycp_edit, fpip->max_w * 3 * sizeof(short));
	//Convert to float32 data type
	cv::Mat SrcF32;
	utlSrc.convertTo(SrcF32, CV_32FC3, (1.0 / 16.0)/255.0, 0); //YC48 is 16x oversampled.
	//Set block size
	int blockSize = fp->track[3];
	w2xc::modelUtility::getInstance().setBlockSize(cv::Size(blockSize, blockSize));
	
	//Denoise
	if (fp->track[0] > 0) //When denoise enabled, its slider value is either 1(weak) or 2(strong)
	{
		//filterGLInit(SrcF32.size().width, SrcF32.size().height); //init OpenGL
		std::string modelFileName("models/noise");
		//set denoise model file
		modelFileName = modelFileName + std::to_string(fp->track[0]) + "_model.bin";
		std::vector<std::unique_ptr<w2xc::Model> > models;
		if (!w2xc::modelUtility::generateModelFromBin(modelFileName, models))
			return FALSE;
		//set njob
		/*for (auto&& model : models) {
			model->setNumberOfJobs(cv::min(fp->track[2],cv::getNumberOfCPUs())); //may need to verify the behaviour of cv::getNumberOfCPU()
		}*/
		//Split planes
		std::vector<cv::Mat> imageSplit;
		cv::Mat imageY;
		cv::split(SrcF32, imageSplit);
		imageSplit[0].copyTo(imageY);
		//Set input/output planes
		/*std::unique_ptr<std::vector<cv::Mat> > inputPlanes = std::unique_ptr<
			std::vector<cv::Mat> >(new std::vector<cv::Mat>());
		std::unique_ptr<std::vector<cv::Mat> > outputPlanes = std::unique_ptr<
			std::vector<cv::Mat> >(new std::vector<cv::Mat>());

		inputPlanes->clear();
		inputPlanes->push_back(imageY);
		//Ready to denoise
		//std::string titlemsg("");
		//titlemsg = titlemsg + "NR: " + std::to_string(fp->track[0]) + " Th: " + std::to_string(cv::min(fp->track[2], cv::getNumberOfCPUs()));
		//SetWindowText(fp->hwnd, titlemsg.c_str());
		//Denoise
		for (int index = 0; index < models.size(); index++) {
			
			if (!models[index]->filterGL(*inputPlanes, *outputPlanes)) {
				return FALSE;
			}
			if (index != models.size() - 1) {
				inputPlanes = std::move(outputPlanes);
				outputPlanes = std::unique_ptr<std::vector<cv::Mat> >(
					new std::vector<cv::Mat>());
			}
		}*/
		w2xc::convertWithModels(imageY, imageSplit[0], models);
		//End denoise
		//outputPlanes->at(0).copyTo(imageSplit[0]); //Update Y-channel
		cv::merge(imageSplit, SrcF32); 
		//cv::cvtColor(imageYUV, image, COLOR_YUV2RGB);

		//filterGLRelease();
	}

	//*************** SCALING ***************************//
	if (fp->track[1] > 100) //only process if >100%
	{
		// calculate iteration times of 2x scaling and shrink ratio which will use at last
		int iterTimesTwiceScaling = static_cast<int>(std::ceil(
			std::log2((double)fp->track[1]/100.0)));
		double shrinkRatio = 0.0;
		if (static_cast<int>((double)fp->track[1] / 100.0)
			!= std::pow(2, iterTimesTwiceScaling)) {
			shrinkRatio = ((double)fp->track[1] / 100.0)
				/ std::pow(2.0, static_cast<double>(iterTimesTwiceScaling));
		}

		//Set model file
		std::string modelFileName("models/scale2.0x_model.bin");
		std::vector<std::unique_ptr<w2xc::Model> > models;
		if (!w2xc::modelUtility::generateModelFromBin(modelFileName, models))
			return FALSE;
		//Set njob
		/*for (auto&& model : models) {
			model->setNumberOfJobs(cv::min(fp->track[2], cv::getNumberOfCPUs())); //may need to verify the behaviour of cv::getNumberOfCPU()
		}*/
		//Ready to scale
		//std::string titlemsg("");
		//titlemsg = titlemsg + "Scale: " + std::to_string(fp->track[1]) + " Th: " + std::to_string(cv::min(fp->track[2], cv::getNumberOfCPUs()));
		//SetWindowText(fp->hwnd, titlemsg.c_str());
		//Starts scaling
		//filterGLInit(SrcF32.size().width*2, SrcF32.size().height*2);

		// 2x scaling
		for (int nIteration = 0; nIteration < iterTimesTwiceScaling;
			nIteration++) {

			//std::cout << "#" << std::to_string(nIteration + 1)
			//	<< " 2x scaling..." << std::endl;

			cv::Mat imageY;
			cv::Size imageSize = SrcF32.size();
			imageSize.width *= 2;
			imageSize.height *= 2;
			
			//We need a Nearest-Neighbour 2x image for the Y-channel
			cv::Mat Y_channel;
			cv::extractChannel(SrcF32, Y_channel, 0);
			cv::resize(Y_channel, imageY, imageSize, 0, 0, cv::INTER_NEAREST);
			// generate bicubic scaled image
			std::vector<cv::Mat> imageSplit;
			cv::Mat image2xBicubic;
			cv::resize(SrcF32, image2xBicubic, imageSize, 0, 0, cv::INTER_CUBIC);
			cv::split(image2xBicubic, imageSplit);

			/*std::unique_ptr<std::vector<cv::Mat> > inputPlanes =
				std::unique_ptr<std::vector<cv::Mat> >(
				new std::vector<cv::Mat>());
			std::unique_ptr<std::vector<cv::Mat> > outputPlanes =
				std::unique_ptr<std::vector<cv::Mat> >(
				new std::vector<cv::Mat>());

			inputPlanes->clear();
			inputPlanes->push_back(imageY);

			for (int index = 0; index < models.size(); index++) {
				//std::cout << "Iteration #" << (index + 1) << "..." << std::endl;
				if (!models[index]->filterGL(*inputPlanes, *outputPlanes)) {
					std::exit(-1);
				}
				if (index != models.size() - 1) {
					inputPlanes = std::move(outputPlanes);
					outputPlanes = std::unique_ptr<std::vector<cv::Mat> >(
						new std::vector<cv::Mat>());
				}
			}

			outputPlanes->at(0).copyTo(imageSplit[0]);*/
			if (!w2xc::convertWithModels(imageY, imageSplit[0], models)){
				MessageBox(fp->hwnd, "Something wrong with w2xc::convertWithModels", "ERROR", MB_OK | MB_ICONSTOP);
				std::exit(1);
			}
			cv::merge(imageSplit, SrcF32);
			if (shrinkRatio != 0.0){
				cv::Size lastImageSize = SrcF32.size();
				lastImageSize.width =
					static_cast<int>(static_cast<double>(lastImageSize.width
					* shrinkRatio));
				lastImageSize.height =
					static_cast<int>(static_cast<double>(lastImageSize.height
					* shrinkRatio));
				cv::resize(SrcF32, SrcF32, lastImageSize, 0, 0, cv::INTER_AREA);
			}

		} // 2x scaling : end
		//filterGLRelease();
	}
	//Send back to AviUtl, clip max size
	int fWidth, fHeight;
	fWidth = cv::min(SrcF32.size().width, fpip->max_w);
	fHeight = cv::min(SrcF32.size().height, fpip->max_h);
	cv::Rect ROI(0, 0, fWidth, fHeight);
	cv::Mat CroppedPict(SrcF32, ROI);
	fpip->h = fHeight;
	fpip->w = fWidth;
	utlSrc.release();
	cv::Mat utlDest(fpip->h, fpip->w, CV_16SC3, fpip->ycp_edit, fpip->max_w * 3 * sizeof(short));
	CroppedPict.convertTo(utlDest, CV_16SC3, 255.0*16.0, 0);
	int64 t2 = cv::getTickCount();
	int elapsed = round((double)(t2 - t1)*1000.0 / (double)cv::getTickFrequency());
	std::string titletext = "Th: " + std::to_string(cv::min(fp->track[2], cv::getNumberOfCPUs()))+ " Time: " + std::to_string(elapsed) + " ms";
	SetWindowText(fp->hwnd, titletext.c_str());
	return TRUE;
}

BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, void *editp, FILTER *fp)
{
	if (message == WM_COMMAND)
	{
		if (wparam == MID_FILTER_BUTTON)
		{
			MessageBox(fp->hwnd,
				"AviUtl port for waifu2x:OpenGL\n"
				"================================\n"
				"Denoise: (performed before scaling)\n"
				"0: Turn off denoise\n"
				"1: Weak denoise\n"
				"2: Strong denoise\n"
				"Scaling: a magnification percentage from 100% to 400%\n"
				"Thread: (Dummy slider at this moment. No effect)\n"
				"Block: Image splitting block size.\n Larger value process faster but takes more video memory.\n\n"
				"Technical Note\n"
				"=================\n"
				"*All processing are done in YUV color space\n"
				"*Only the Y-plane is scaled using machine-learned data\n"
				"*UV-planes use standard bicubic\n\n"
				"Original code bases\n"
				"=======================\n"
				"https://github.com/nagadomi/waifu2x \n"
				"https://github.com/ueshita/waifu2x-converter-glsl", "INFO", MB_OK);

		}
		if (wparam == MID_FILTER_BUTTON + 1)
		{
			MessageBox(fp->hwnd, "Please keep this dying developer alive by donating via PayPal to the email address:\n"
				"mavericktse@gmail.com", "Gochiusa Driven Development", MB_OK);
		}
	}
	return FALSE;
}

// translated from ../pythonTests/test_DynamicLoudnessGM.py
// first build and install library, then compile this file
// compile using g++ -std=c++11 test_DynamicLoudnessGM.cpp -lloudness

#include <string>
#include <loudness/Modules/AudioFilesToTrackBank.h>
#include <loudness/Models/DynamicPartialLoudnessGM.h>

int main()
{
	const loudness::TrackBank *audioBank;
	const loudness::TrackBank *IIRBank; // 0
	const loudness::TrackBank *frameBank; // 1
	const loudness::TrackBank *powerSpectrum; // 2
	const loudness::TrackBank *compressBank; // 3
	const loudness::TrackBank *roexBank; // 4
	const loudness::TrackBank *specificBank; // 5
	const loudness::TrackBank *integratedBank; // 6
	loudness::DynamicPartialLoudnessGM *model;
	int nFrames, nChannels;
	int hopSize = 1024;
	std::vector<std::string> files;
	files.push_back("../wavs/organ-g-left.wav");
	files.push_back("../wavs/organ-g-left1.wav");
	files.push_back("../wavs/piano-g-left.wav");
	files.push_back("../wavs/piano-g-left1.wav");
	loudness::AudioFilesToTrackBank audio = loudness::AudioFilesToTrackBank(files, hopSize);
	audio.initialize();
	audioBank = audio.getOutput();
	nFrames = audio.getNFrames();

	//Create the loudness model
	model = new loudness::DynamicPartialLoudnessGM("../filterCoefs/44100_IIR_23_freemid.npy");
	model->initialize(*audioBank);

	IIRBank = model->getModuleOutput(0);
	frameBank = model->getModuleOutput(1);
	powerSpectrum = model->getModuleOutput(2);
	compressBank = model->getModuleOutput(3);
	roexBank = model->getModuleOutput(4);
	specificBank = model->getModuleOutput(5);
	integratedBank = model->getModuleOutput(6);

	nChannels = specificBank->getNChannels();

	//processings
	for (int frame = 0; frame < nFrames; frame++)
	{
	    audio.process();
	    model->process(*audioBank);
	    // print audio data to file
	    //for (int smp = 0; smp < hopSize; smp++)
	   	//	std::cout << audioBank->getSample(2,0,smp) << " ";
	    //std::cout << std::endl;
	    
	    double il = 0;
	    for (int f = 0; f < nChannels; f++)
	   		il += specificBank->getSample(1,f,1);
	    std::cout << il << std::endl;
		//std::cout << powerSpectrum->getSpatialPosition(0, chn) << std::endl;
	}
}

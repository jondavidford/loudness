// translated from ../pythonTests/test_DynamicLoudnessGM.py
// first build and install library, then compile this file
// compile using g++ -std=c++11 test_DynamicLoudnessGM.cpp -lloudness

#include <string>
#include <loudness/Modules/AudioFileCutter.h>
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
	int hopSize = 512;
	std::vector<std::string> files;
	files.push_back("../wavs/guitar_r.wav");
	files.push_back("../wavs/guitar_l.wav");
	files.push_back("../wavs/bass_r.wav");
	files.push_back("../wavs/bass_l.wav");
	loudness::AudioFileCutter audio = loudness::AudioFileCutter(files, hopSize);
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

	nChannels = integratedBank->getNChannels();

	//processing
	for (int frame = 0; frame < nFrames; frame++)
	{
	    audio.process();
	    model->process(*audioBank);
	    std::cout << "frame: " << frame << std::endl;
	    //for (int chn = 0; chn < nChannels; chn++)
			//std::cout << powerSpectrum->getSpatialPosition(0, chn) << std::endl;
	}
}

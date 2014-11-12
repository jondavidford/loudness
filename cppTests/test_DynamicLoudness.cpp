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
	const loudness::TrackBank *stereoToMono; // 3 or nonexistant
	const loudness::TrackBank *sumMaskers; // 4
	const loudness::TrackBank *compressBank; // 5
	const loudness::TrackBank *roexBank; // 6
	const loudness::TrackBank *specificBank; // 7
	const loudness::TrackBank *integratedBank; // 8
	loudness::DynamicPartialLoudnessGM *model;
	int nFrames, nChannels;
	int hopSize = 1024;
	std::vector<std::string> files;
	files.push_back("../wavs/guitar_s.wav");
	files.push_back("../wavs/bass_s.wav");
	loudness::AudioFileCutter audio = loudness::AudioFileCutter(files, hopSize);
	audio.initialize();
	audioBank = audio.getOutput();
	nFrames = audio.getNFrames();

	//Create the loudness model
	model = new loudness::DynamicPartialLoudnessGM("../filterCoefs/32000_IIR_23_freemid.npy");
	model->setStereoToMono(false);
	model->initialize(*audioBank);

	IIRBank = model->getModuleOutput(0);
	frameBank = model->getModuleOutput(1);
	powerSpectrum = model->getModuleOutput(2);
	sumMaskers = model->getModuleOutput(3);
	compressBank = model->getModuleOutput(4);
	roexBank = model->getModuleOutput(5);
	specificBank = model->getModuleOutput(6);
	integratedBank = model->getModuleOutput(7);

	//processing
	for (int frame = 0; frame < nFrames; frame++)
	{
	    audio.process();
	    model->process(*audioBank);
	}

	for (int track = 0; track < integratedBank->getNTracks(); track++)
	{
		std::cout << "Instantaneous Loudness\n";
	    for (int frame = 0; frame < nFrames; frame++)
	        std::cout << integratedBank->getSample(track,0,0) << std::endl;
	}
}

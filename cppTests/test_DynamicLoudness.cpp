// translated from ../pythonTests/test_DynamicLoudnessGM.py
// compile after building the loudness library
//

#include <loudness/Modules/AudioFileCutter.h>
#include <loudness/Models/DynamicLoudnessGM.h>

int main()
{
	loudness::AudioFileCutter audio;
	const loudness::SignalBank *audioBank;
	const loudness::SignalBank *IIRBank; // 0rab
	const loudness::SignalBank *frameBank; // 1
	const loudness::SignalBank *powerSpectrum; // 2
	const loudness::SignalBank *stereoToMono; // 3
	const loudness::SignalBank *compressBank; // 4
	const loudness::SignalBank *roexBank; // 5
	const loudness::SignalBank *specificBank; // 6
	const loudness::SignalBank *loudnessBank; // 7 
	loudness::DynamicLoudnessGM *model;
	int nFrames, nChannels;
	int hopSize = 55;
	audio = loudness::AudioFileCutter("../wavs/bass_1s.wav", hopSize);
	audio.initialize();
	audioBank = audio.getOutput();
	nFrames = audio.getNFrames();

	//Create the loudness model
	model = new loudness::DynamicLoudnessGM("../filterCoefs/44100_IIR_23_freemid.npy");
	model->initialize(*audioBank);

	IIRBank = model->getModuleOutput(0);
	frameBank = model->getModuleOutput(1);
	powerSpectrum = model->getModuleOutput(2);
	compressBank = model->getModuleOutput(3);
	roexBank = model->getModuleOutput(4);
	specificBank = model->getModuleOutput(5);
	loudnessBank = model->getModuleOutput(6);

	//processing
	for (int frame = 0; frame < nFrames; frame++)
	{
	    audio.process();
	    model->process(*audioBank);

	    std::cout << loudnessBank->getSample(0,0) << std::endl;
	}
}

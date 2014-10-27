// translated from ../pythonTests/test_DynamicLoudnessGM.py

#include <loudness/Modules/AudioFileCutter.h>
#include <loudness/Models/DynamicLoudnessGM.h>

int main()
{
	loudness::AudioFileCutter audio;
	const loudness::SignalBank *audioBank;
	const loudness::SignalBank *IIRBank; // 0
	const loudness::SignalBank *frameBank; // 1
	const loudness::SignalBank *powerSpectrum; // 2
	const loudness::SignalBank *compressBank; // 3
	const loudness::SignalBank *roexBank; // 4
	const loudness::SignalBank *specificBank; // 5
	const loudness::SignalBank *loudnessBank; // 6
	loudness::DynamicLoudnessGM *model;
	int nFrames, nChannels;
	int hopSize = 32;
	audio = loudness::AudioFileCutter("../wavs/tone1kHz40dBSPL.wav", hopSize);
	audio.initialize();
	audioBank = audio.getOutput();
	nFrames = audio.getNFrames();

	//Create the loudness model
	model = new loudness::DynamicLoudnessGM("../filterCoefs/32000_IIR_23_freemid.npy");
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

	    std::cout << "Instantaneous Loudness\n";
	    for (int chn = 0; chn < loudnessBank->getNChannels(); chn++)
	    {
	        std::cout << "frame: " << frame << " chn: " << chn << "loudness: " << loudnessBank->getSample(chn,0) << std::endl;
	    }

	    std::cout << "Power Spectrum\n";
	    for (int chn = 0; chn < powerSpectrum->getNChannels(); chn++)
	    {
	    	std::cout << "center frequency: " << powerSpectrum->getCentreFreq(chn) << std::endl;
	        std::cout << "frame: " << frame << " chn: " << chn << "loudness: " << powerSpectrum->getSample(chn,0) << std::endl;
	    }
	}
}

// translated from ../pythonTests/test_DynamicLoudnessGM.py

#include <loudness/Modules/AudioFileCutter.h>
#include <loudness/Models/DynamicPartialLoudnessGM.h>

int main()
{
	loudness::AudioFileCutter audio;
	const loudness::TrackBank *audioBank;
	const loudness::TrackBank *IIRBank; // 0
	const loudness::TrackBank *frameBank; // 1
	const loudness::TrackBank *powerSpectrum; // 2
	const loudness::TrackBank *stereoToMono; // 3
	const loudness::TrackBank *sumMaskers; // 4
	const loudness::TrackBank *compressBank; // 5
	const loudness::TrackBank *roexBank; // 6
	const loudness::TrackBank *specificBank; // 7
	const loudness::TrackBank *loudnessBank; // 8
	loudness::DynamicPartialLoudnessGM *model;
	int nFrames, nChannels;
	int hopSize = 32;
	audio = loudness::AudioFileCutter("../wavs/tone1kHz40dBSPL.wav", hopSize);
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

	//processing
	for (int frame = 0; frame < nFrames; frame++)
	{
	    audio.process();
	    model->process(*audioBank);

	    std::cout << "Instantaneous Loudness\n";
	    for (int chn = 0; chn < specificBank->getNChannels(); chn++)
	    {
	        std::cout << "frame: " << frame << " chn: " << chn << "loudness: " << specificBank->getSample(0,chn,0) << std::endl;
	    }
	}
}

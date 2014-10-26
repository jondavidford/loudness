// translated from ../pythonTests/test_DynamicLoudnessGM.py
using namespace loudness;

#include "../src/Models/DynamicLoudnessGM.h"

int main()
{
	int fs = 32000;
	int hopSize = 32;
	AudioFileCutter audio = AudioFileCutter("../wavs/tone1kHz40dBSPL.wav", hopSize);
	audio.initialize();
	SignalBank audioBank = audio.getOutput();
	int nFrames = audio.getNFrames();

	//Create the loudness model
	DynamicLoudnessGM model = DynamicLoudnessGM("../filterCoefs/32000_IIR_23_freemid.npy");
	model.initialize(audioBank);
	SignalBank loudnessBank = model.getModuleOutput(model.getNModules()-1);
	nChannels = loudnessBank.getNChannels();

	//storage
	//out = np.zeros((nFrames, nChannels));

	//processing
	for (int frame = 0; frame < nFrames; frame++)
	{
	    audio.process();
	    model.process(audioBank);

	    for (int chn = 0; chn < nChannels; chn++)
	    {
	        //out[frame, chn] = loudnessBank.getSample(chn,0);
	    }
	}
}

// translated from ../pythonTests/test_DynamicLoudnessGM.py
#include "../src/Models/DynamicLoudnessGM.h"

int main()
{
	int nChannels;
	int hopSize = 32;
	loudness::AudioFileCutter audio = loudness::AudioFileCutter("../wavs/tone1kHz40dBSPL.wav", hopSize);
	audio.initialize();
	loudness::SignalBank audioBank = audio.getOutput();

	//Create the loudness model
	loudness::DynamicLoudnessGM model = loudness::DynamicLoudnessGM("../filterCoefs/32000_IIR_23_freemid.npy");
	model.initialize(audioBank);
	loudness::SignalBank loudnessBank = model.getModuleOutput(6);
	nChannels = loudnessBank.getNChannels();

	//storage
	//out = np.zeros((nFrames, nChannels));

	//processing
	for (int frame = 0; frame < 5; frame++)
	{
	    audio.process();
	    model.process(audioBank);

	    cout << "frame: " << frame << endl << endl;

	    for (int chn = 0; chn < nChannels; chn++)
	    {
	        cout << "bin: " << chn << "\t\tloudness: " << loudnessBank.getSample(chn,0);
	    }
	}
}

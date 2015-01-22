forgan = fopen('organ_l.txt');
fpiano = fopen('piano_l.txt');

hopSize = 1024;
numFrames = 216;
rms_organ = zeros(numFrames,1);
rms_piano = zeros(numFrames,1);
zheng_organ_lo = zeros(numFrames,1);
zheng_piano_lo = zeros(numFrames,1);
zheng_organ_pl = zeros(numFrames,1);
zheng_piano_pl = zeros(numFrames,1);

i=1;

while i < numFrames + 1
    organl = fscanf(forgan, '%f', hopSize);
    pianol = fscanf(fpiano, '%f', hopSize);
    
    organ = organl .* 2;
    piano = pianol .* 2;
    rms_organ(i) = rms(organ);
    rms_piano(i) = rms(piano);
    [lorgan exg] = getLoudness(organ);
    [lpiano exb] = getLoudness(piano);
    zheng_organ_lo(i) = lorgan;
    zheng_piano_lo(i) = lpiano;
    zheng_organ_pl(i) = getPartialLoudness(exg, exb);
    zheng_piano_pl(i) = getPartialLoudness(exb, exg);
    i = i + 1;
end
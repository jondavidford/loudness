fps = fopen('piano_silenced.txt');
fms = fopen('modsine.txt');

hopSize = 1024;
numFrames = 216;
rms_ps = zeros(numFrames,1);
rms_ms = zeros(numFrames,1);
zheng_ps_lo = zeros(numFrames,1);
zheng_ms_lo = zeros(numFrames,1);
zheng_ps_pl = zeros(numFrames,1);
zheng_ms_pl = zeros(numFrames,1);

i=1;

while i < numFrames + 1
    organl = fscanf(fps, '%f', hopSize);
    pianol = fscanf(fms, '%f', hopSize);
    
    organ = organl .* 2;
    piano = pianol .* 2;
    rms_ps(i) = rms(organ);
    rms_ms(i) = rms(piano);
    [lorgan exg] = getLoudness(organ);
    [lpiano exb] = getLoudness(piano);
    zheng_ps_lo(i) = lorgan;
    zheng_ms_lo(i) = lpiano;
    zheng_ps_pl(i) = getPartialLoudness(exg, exb);
    zheng_ms_pl(i) = getPartialLoudness(exb, exg);
    i = i + 1;
end
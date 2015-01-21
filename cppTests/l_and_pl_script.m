fp = fopen('piano.txt');
fpd = fopen('piano_delay.txt');

hopSize = 1024;
numFrames = 554;
rms_p = zeros(numFrames,1);
rms_pd = zeros(numFrames,1);
zheng_p_lo = zeros(numFrames,1);
zheng_pd_lo = zeros(numFrames,1);
zheng_p_pl = zeros(numFrames,1);
zheng_pd_pl = zeros(numFrames,1);

i=1;

while i < numFrames + 1
    gl = fscanf(fp, '%f', hopSize);
    bl = fscanf(fpd, '%f', hopSize);
    
    g = gl .* 2;
    b = bl .* 2;
    rms_p(i) = rms(g);
    rms_pd(i) = rms(b);
    [lg exg] = getLoudness(g);
    [lb exb] = getLoudness(b);
    zheng_p_lo(i) = lg;
    zheng_pd_lo(i) = lb;
    zheng_p_pl(i) = getPartialLoudness(exg, exb);
    zheng_pd_pl(i) = getPartialLoudness(exb, exg);
    i = i + 1;
end
%read wav
%[wave,fs]=audioread('guitar_short_16_PCM.wav');
[wave,fs]=audioread('weekend_16.wav');
t=0:1/fs:(length(wave)-1)/fs;
plot(t,wave);
% ----------------------------------------------
% Save to faile
fid=fopen('audio_data2.txt','w');
% koeficientu kiekis
fprintf(fid, '/* PCM coded audio data */ \r\n', [length(wave)]');
fprintf(fid, 'uint32_t audio_data_size = %d; \r\n', [length(wave)]');
% saugomi koeficientai + kablelis
fprintf(fid, ['const int16_t audio_data[] = { \r\n']);
for j=1:length(wave)-1
%stereo
%fprintf(fid, '%f,', [wave(j,1)]');
%fprintf(fid, '%f,', [wave(j,2)]');
%mono
%fprintf(fid, '%f,', [wave(j)]');
fprintf(fid, '%5.0f,', [wave(j)]*30000');
end
% paskutinis koeficientas be kablelio
%stereo
%fprintf(fid, '%f', [wave(length(wave),1)]');
%fprintf(fid, '%f', [wave(length(wave),2)]');
%mono
%fprintf(fid, '%f', [wave(length(wave))]');
fprintf(fid, '%.0f', [wave(length(wave))]*30000');
% pabaiga + failo uzdarymas
fprintf(fid, [' \r\n };']);
fclose(fid);
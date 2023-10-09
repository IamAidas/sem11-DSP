% FIR testas
% Filtro koeficientai saugomi faile
% FIR_filter_30ord.m - sugeneruotas filtro MATLAB kodas

% Hd = FIR_filter_30ord; % example
clear;
Hd = lowpassFinal;


Num2 = Hd.Numerator;
Den = zeros(size(Num2)); Den(1)=1;
Fs = 48000;
t = 0:1/Fs:0.003;
% Iejimo signalas 1kHz + 5kHz + 10kHz
x = sin(2*pi*1000*t)+sin(2*pi*5000*t)+sin(2*pi*10000*t);
% Filtruotas signalas
y = filter(Num2,Den,x);
subplot(2,1,1); plot(t,x);
title('Input to the Filter');
subplot(2,1,2); plot(t,y);
title('Output of the Filter'); xlabel('time (sec)');
% ----------------------------------------------
% koeficientu saugojimas i faila - STM32 DSP FIR
fid=fopen('koeficientai_STM32.txt','w');
% koeficientu kiekis
fprintf(fid, 'NUM_TAPS=%d \r\n', [length(Num2)]');
% saugomi koeficientai + kablelis
fprintf(fid, [' const float32_t firCoeffs32[NUM_TAPS] = { \r\n']);
 for j=1:length(Num2)-1
 fprintf(fid, '%f,', [Num2(j)]');
 end
% paskutinis koeficientas be kablelio
fprintf(fid, '%f', [Num2(length(Num2)-1)]');
% pabaiga + failo uzdarymas
fprintf(fid, [' \r\n };']);
fclose(fid); 



frequency_domain_plot(48000, x, y);

function frequency_domain_plot(Fs, x, y)
nfft = length(x);
k = 0:1:nfft-1; % k ? da?nio atskaitos numeris
f = k*Fs/nfft; % da?nio atskaitos numerio konvertavimas ? hercus
x_signal = abs(fft(x))/nfft; % Iejimo signalo spektro skai?iavimas.
x_signal = 20*log10(x_signal/max(x_signal)); % spektro perskai?iavimas ?
y_signal = abs(fft(y))/nfft; % Isejimo signalo spektro skai?iavimas.
y_signal = 20*log10(y_signal/max(y_signal));
figure()
subplot(2,1,1); plot(f, x_signal, 'k');
title('Input to the Filter');
xlim([0 Fs/2])
ylim([-100 0])
xlabel('f, Hz')
ylabel('Sa, dB')
subplot(2,1,2); plot(f, y_signal, 'k');
title('Output of the Filter');
xlabel('f, Hz')
xlim([0 Fs/2])
xlabel('f, Hz')
ylabel('Sa, dB')
end
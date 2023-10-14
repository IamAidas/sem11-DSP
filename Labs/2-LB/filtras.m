% FIR testas
% Filtro koeficientai saugomi faile

clear;
% HD - this is a generated MATLAB filter function
% Hd = FIR_filter_30ord; % example
 Hd = lowpassFinal;
% Hd = highPass;
%Hd = bandpass;


Num2 = Hd.Numerator;
Den = zeros(size(Num2)); Den(1)=1;
Fs = 48000;
t = 0:1/Fs:0.003;
% Input signal: 1kHz + 5kHz + 10kHz
x = sin(2*pi*1000*t) + sin(2*pi*5000*t) + sin(2*pi*10000*t);

y = filter(Num2,Den,x);
subplot(2,1,1); plot(t,x);
title('Input to the Filter');
subplot(2,1,2); plot(t,y);
title('Output of the Filter'); xlabel('time (sec)');

% ----------------------------------------------
% Save created filter coefs to a file
fid=fopen('koeficientai_STM32.txt','w');
% number of filter coefs
fprintf(fid, 'NUM_TAPS=%d \r\n', [length(Num2)]');
% save coefs + comma
fprintf(fid, [' const float32_t firCoeffs32[NUM_TAPS] = { \r\n']);
 for j=1:length(Num2)-1
 fprintf(fid, '%f,', [Num2(j)]');
 end
% Last coef after the comma
fprintf(fid, '%f', [Num2(length(Num2)-1)]');

fprintf(fid, [' \r\n };']);
fclose(fid); 


plotFrequencySpectrum(x, y, Fs);



function plotFrequencySpectrum(x, y, Fs)
    % Compute the FFT for the input signal (x)
    N = length(x);
    f = Fs * (0:(N/2)) / N;
    X = fft(x);
    P_x = abs(X / N);

    % Compute the FFT for the filtered signal (y)
    Y = fft(y);
    P_y = abs(Y / N);

    % Plot the frequency spectra
    figure;
    subplot(2, 1, 1);
    plot(f, P_x(1:N/2+1));
    title('Frequency Spectrum - Input Signal');
    xlabel('Frequency (Hz)');
    ylabel('Amplitude');
    
    % Display only positive frequencies
    axis([0 Fs/2 0 max(P_x)]);

    subplot(2, 1, 2);
    plot(f, P_y(1:N/2+1));
    title('Frequency Spectrum - Output Signal');
    xlabel('Frequency (Hz)');
    ylabel('Amplitude');
    
    % Display only positive frequencies
    axis([0 Fs/2 0 max(P_y)]);
end

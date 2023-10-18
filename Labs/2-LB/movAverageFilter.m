clear

%Moving Average filter test
%Pulse signal
pulse = zeros(1,100); pulse(30:70)=1;
n=1:1:100;
subplot(2,2,1); plot(n,pulse);
ylim([-0.5 1.5]);
title('Pulse Signal'); grid
%Noise
noisy_pulse = pulse + 0.25*randn(size(pulse));
subplot(2,2,2); plot(n,noisy_pulse);
ylim([-0.5 1.5]);
title('Noisy Pulse Signal'); grid
% 5-point MA Filter:
num=1/5*ones(1,5); den=zeros(size(num)); den(1)=1;
output = filter(num,den,noisy_pulse);
subplot(2,2,3); plot(n,output);
ylim([-0.5 1.5]);
title('Output of 3-Point Moving Avg Filter'); grid
% 10-point MA Filter:
num=1/10*ones(1,10); den=zeros(size(num)); den(1)=1;
output = filter(num,den,noisy_pulse);
subplot(2,2,4); plot(n,output);
ylim([-0.5 1.5]);
title('Output of 8-Point Moving Avg Filter'); grid



% 15-point MA Filter:
num=1/15*ones(1,10); den=zeros(size(num)); den(1)=1;
output = filter(num,den,noisy_pulse);
figure()
plot(n,output);
ylim([-0.5 1.5]);
title('Output of 15-Point Moving Avg Filter'); grid
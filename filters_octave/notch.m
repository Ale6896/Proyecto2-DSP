pkg load signal;
clear all;
close all;

Ts = 1 / 48000;

# Filtro de muesca
# Primera etapa

K = 0.9862118933068;
num1 = [1, -1.999314561864, 1];
den1 = [1, -1.956811487087, 0.9588902080813];

H1 = K*tf(num1, den1, Ts);

# Segunda etapa
num2 = [1, -1.999314561864, 1];
den2 = [1, -1.986314322974, 0.9865341498534];

H2 = K*tf(num2, den2, Ts);

H_total = H1*H2;

[B, A] = tfdata(H_total, 'v');

# Se obtiene la respues en frecuencia
w = logspace(-3, pi, 512);
H = polyval(B, exp(i.*w)) ./ polyval(A, exp(i.*w));

# Usando esta función la fase queda rara
#[h, w] = freqz(B, A);

# Impulso unitario
X = [1, zeros(1, 1023)];

# Respuesta al impulso
h = filter(B, A, X);

save 'hn_filtro_muesca.txt' h -ascii;

# Graficar la respuesta en frecuencia
figure;

subplot(2, 1, 1);
semilogx(w/(2*pi*Ts), 20*log10(abs(H)));
ylim([-100 0]);
grid on;
title("Magnitud");
xlabel("Frecuencia [rad/s]");
ylabel("Magnitud [dB]");

subplot(2, 1, 2);
semilogx(w/(2*pi*Ts), angle(H));
title("Fase");
xlabel("Frecuencia [rad/s]");
ylabel("Fase [rad]");
grid on;

figure("name", "Respuesta al impulso");
plot(h);
grid on;

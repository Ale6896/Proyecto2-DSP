pkg load signal;
clear all;
close all;

Ts = 1 / 48000;

# Filtro interesante (es un filtro pasa banda de orden 10)
# Primera etapa

K1 = 0.01936671413268;
numb1 = [1,                 0,                -1];
denb1 = [1,    -1.95681148705,   0.9588902080439];

H1 = K1*tf(numb1, denb1, Ts);

# Segunda etapa
K2 = 0.01936671413268;
numb2 = [1,                 0,                -1];
denb2 = [1,    -1.98631432301,    0.986534149892];

H2 = K2*tf(numb2, denb2, Ts);

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

save 'hn_filtro_interesante.txt' h -ascii;

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

%% Calculo de las frecuencias centrales que tendra cada
%% filtro y su posicion equivalente en un buffer 
%% de tamaño 1024


% Frecuencias centrales para ecualizacion
samples = 21;
buffer_size = 3*1024/2;
Fmin = 100;
Fmax = 24000;

F = logspace(log10(Fmin), log10(Fmax), samples);
n = ones(1,length(F));

% Mapeo de frecuencias centrales a posicion en
% buffer de 1024 muestras
[x1 y1] = deal(0,0);
[x2 y2] = deal(Fmax, buffer_size-1);

% Calculo de la pendiente para realizar el mapeo
coeff = polyfit([x1, x2], [y1, y2], 1);
m = coeff(1);
b = coeff(2);


figure(1)

% Frecuencias vistas en espacio lineal
subplot(2,1,1)
stem(F,n);

% Frecuencias vistas en espacio logaritmico
subplot(2,1,2)
stem(log10(F),n);

%% Calculo de la posicion de las frecucias en el buffer de 1024 muestras
%% La variable buffer_map contiene la posicion equivalente
%% de las frecuencias desde el rango 0 Hz hasta 24 kHz

buffer_map = round(m*F + b);

%% Calculo inverso para obtener frecuencias resultantes por redondeo
%% La variable ibuffer_map realiza el calculo inverso para obtener el
%% valor de frecuencia al cual corresponde cada posicion del buffer en
%% el rango original debido a que el redondeo modifica este valor

ibuffer_map = (buffer_map - b) / m;

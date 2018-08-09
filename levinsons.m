clear all;
[a,fs] = audioread('male.wav');
a = transpose(a);
a(fs*5:size(a,2)) = '';
%%
% rec = audiorecorder(48000,16,1);
% fs = 48000;
% input('Enter');
% record(rec,5);
% pause(6);
% a = transpose(getaudiodata(rec));
%%
n = round(.0225*fs);
l = size(a,2);
l = l - mod(l,n);
a(l+1:size(a,2)) = '';
imax = l/n;
order = input('Enter Order:');
corMat= zeros(order);
coef = zeros(imax,order+1);
epsMin  = zeros(imax,1);
eps = zeros(order,1);
e = zeros(imax,n+order);
scale = n;
for i = 1:imax
    temp= a(((i-1)*n+1):i*n);
    gap = xcorr(temp)./scale;
    gap = gap
    ap   = [1];
    for p = 0:order-1
        gamma = gap(n+p+1)/gap(n);
        r_gap = gap(2*n-1:-1:1);
        r_gap = [zeros(1,p+1), r_gap];
        r_gap(2*n:2*n+p) = '';
        gap = gap - gamma*r_gap;
        ap = [ap;0];
        ap = ap - gamma*ap(p+2:-1:1);
    end
    coef(i,:) = -transpose(ap);
    epsMin(i) = gap(n);
    e(i,:) = conv2(temp,transpose(ap));
%     cor = xcorr(a(((i-1)*n+1):i*n));
%     cor = cor/n;
%     for j = 1:order
%         corMat(j,:) = cor((n-j+1):n-j+order);
%     end
%     corMat = inv(corMat);
%     epsMin(i) = 1/corMat(1,1);
%     eps(1,1) = -epsMin(i);
%     coef(i,:) = transpose(corMat*eps)
end;
%%
e(:,n+1:n+order) = '';
for i = 1:imax
    fr = fft(e(i,:));
    [max_value, max_index] = max(fr(1:n/2));
    if max_value>0.7
        coef(i,1) = max_index;
    end
end
error = [0];
for i = 1:imax
    error = [error e(i,:)];
end
input('Enter');
soundsc(a,fs);
input('Enter');
soundsc(error,fs);
synt = zeros(1,1);
order = order+1;
short = zeros(1,n+order-1);
for i = 1:imax
    %imp = normrnd(0,sqrt(epsMin(i,1)),1,n);
    if(coef(i,1)~=-1)
        imp = zeros(1,n);
        for(kk = 1:n)
            imp(kk) = (mod(kk,round(n/coef(i,1)))==0);
        end
        imp = imp*sqrt(epsMin(i,1));
    else
        imp = normrnd(0,sqrt(epsMin(i,1)),1,n);
    end
    for j = 1:n
        short(order-1+j) = 0;
        for k = 1:order-1
            short(order-1+j) = short(order-1+j) +  short(order-1+j-k)*coef(i,k+1);
        end
        short(order-1+j) = short(order-1+j) + imp(j);
    end
    synt = [synt,short(order:n+order-1)];
end
input('Enter to play reconstructed audio');
soundsc(synt/max(synt),fs);
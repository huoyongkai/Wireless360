function [distortion]=CalDistortion_LUT_Compare(x0)%newton([0.8 0.8 0.8],0.00001,20)

global N dist bitlens imp y2fit z2fit a b c d

syms ri% 1/2 root of the 'inverse of rate'
per=eval(2^(-1.1^(d*ri^6+c*ri^4+b*ri^2+a)));%ri^4 for the sake of using newton method


y2_lot=min(y2fit):0.05:6;%max(y2fit);

for i=1:length(y2_lot)
    ztest(i)=eval(subs(per,{'ri'},{y2_lot(i)^0.5}));
    ztest_LUT(i)=interp1(y2fit,z2fit,y2_lot(i));
end

figure(1)
semilogy(y2fit,z2fit,'r-');
hold on;

semilogy(y2_lot,ztest,'*-');
hold on;
%plot(y2_lot,ztest_LUT,'-');
legend('best','math');
distortion=0;
for i=1:N
    if(x0(i)<1)
        tmpper=1;
    elseif(x0(i)^2>9.9)
        tmpper=interp1(y2fit,z2fit,9.89);
    else
        tmpper=interp1(y2fit,z2fit,x0(i)^2);
    end
    calper=eval(subs(per,{'ri'},{x0(i)}));
    fprintf('LUT vs Cal: %d - %d\n',tmpper,calper);
    fprintf('LUT vs Cal (real PER): %d - %d\n',(1-(1-tmpper)^(bitlens(i)/1000)),(1-(1-calper)^(bitlens(i)/1000)));
    fprintf('LUT vs Cal (real dist): %d - %d\n',(1-(1-tmpper)^(bitlens(i)/1000))*imp(i)*dist(i),(1-(1-calper)^(bitlens(i)/1000))*imp(i)*dist(i));
    fprintf('\n');
    distortion=distortion+(1-(1-tmpper)^(bitlens(i)/1000))*imp(i)*dist(i);
end

end
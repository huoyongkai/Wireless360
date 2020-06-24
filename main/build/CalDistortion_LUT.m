function [distortion]=CalDistortion_LUT(x0)%newton([0.8 0.8 0.8],0.00001,20)

global N dist bitlens imp y2fit z2fit

distortion=0;
for i=1:N
    if(x0(i)<1)
        tmpper=1;
    elseif(x0(i)^2>9.9)
        tmpper=interp1(y2fit,z2fit,9.89);
    else
        tmpper=interp1(y2fit,z2fit,x0(i)^2);
    end
    distortion=distortion+(1-(1-tmpper)^(bitlens(i)/1000))*imp(i)*dist(i);
end

end
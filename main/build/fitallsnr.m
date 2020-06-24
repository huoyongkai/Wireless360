[x,y,z]=textread('Table_Plr.txt','%f %f %f','headerlines',1);
fid=fopen('fitparatable.txt','w');

for snr=-5:0.5:35
    ind=1;
    for i=1:size(z)
        if(x(i)==snr)
            %x2fit(ind)=x(i);
            y2fit(ind)=y(i)/1000;
            z2fit(ind)=z(i);
            ind=ind+1;
        end
    end
    [fitresult,]=singlesnr(y2fit,z2fit);
    fprintf(fid,' %10.6f  %10.6f   %10.6f\n',fitresult.a,fitresult.b,fitresult.c);
    
end
fclose(fid);

return;

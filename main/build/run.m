
function []=run(parafile,resultfile)

global N dist bitlens imp snr a b c d maxcodedlength centerrateinv y2fit z2fit
if exist(parafile,'file')==0
    fprintf('Parameter file=%s doesnot exist!\n',parafile);
    return;
end

fileid = fopen(parafile);
data_n = textscan(fileid,'%f');
fclose(fileid);

data = data_n{1};       %读取数据完毕
N = data(1);             %第一个元素保存点的个数

dist=data(2:N+1);
bitlens=data(N+2:2*N+1);
imp=data(2*N+2:3*N+1);

ratio=imp.*dist*1000./bitlens;

snr=data(3*N+2);
maxcodedlength=data(3*N+3);
centerrateinv=data(3*N+4);

%do fiting below--------------------
[x,y,z]=textread('Table_Plr.txt','%f %f %f','headerlines',1);
ind=1;
for i=1:size(z)
    if(x(i)==snr)
        %x2fit(ind)=x(i);
        y2fit(ind)=y(i)/1000;
        z2fit(ind)=z(i);
        log2z2fit(ind)=log2(z(i));
        ind=ind+1;
    end
end
[fitresult,]=singlesnr(y2fit,log2z2fit);
a=fitresult.a;
b=fitresult.b;
c=fitresult.c;
d=fitresult.d;
%fiting finished-------------------

fid=fopen('Solutions.txt','w');
input=ones(1,N+1)*(centerrateinv^0.5);
mindist=CalDistortion_LUT(input);
rates_root=double(input);
fprintf(fid,' %d -> %d ->(LUT) %d\n',(centerrateinv^0.5),mindist,CalDistortion_LUT(input));

for org=1.00:0.257:3.2
    if(org>2)%for debug only
        org=org+0;
    end
    input=ones(1,N+1)*org;
    %input(3)=-1/500;
    [x, distortion]=newton(input,0.00001,20);
    distortion=CalDistortion_LUT(double(x));
    fprintf(fid,' %d -> %d ->(LUT) %d \n',org,distortion,CalDistortion_LUT(double(x)));
    if(distortion<mindist)
        mindist=distortion;
        rates_root=double(x);
    end
end
fclose(fid);

fid=fopen(resultfile,'w');
for i=1:length(rates_root)-1
    fprintf(fid,'%d  ',rates_root(i)^2);
end
fprintf(fid,'\n%d\n',mindist);
fclose(fid);
fprintf('all results were finished!\n');
end

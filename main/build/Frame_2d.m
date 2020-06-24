function [] = Frame_2d()
%STEREOVIDEO Summary of this function goes here
%   Detailed explanation goes here
[Y U V]=YUV_import('./virtual2D_qcif.yuv',[176 144],'4:2:0',15,0);
%size(Y)
%size(U)
%size(V)
firstAkiyoFrm=Y{3};
save('firstAkiyoFrm','firstAkiyoFrm');
return;
figure(1);
x=Cvideo1.StatMat(Y{1}-255)
bar(Cvideo1.StatMat(Y{1}-255));
first=Y{1}-255;
save('lossess.mat','first');
return;

figure(1);
bar(Cvideo1.StatMat(Y{1}-Y{2}));
diff=Y{1}-Y{2};
save('btwfrms.mat','diff');

for i=2:length(Y)
    if i==2
        x=abs(Y{i}-Y{i-1});
    else
        x=x+abs(Y{i}-Y{i-1});
    end
end
x=x/(length(Y)-1);
save('averagediff.mat','x');
return;
x=x/99;

figure(3);
bar(x);
figure(4);
%bar(abs((Cvideo1.StatMat(Y{100})-Cvideo1.StatMat(Y{99}))./Cvideo1.StatMat(
%Y{100})));
bar(Cvideo1.StatMat(Y{100})-Cvideo1.StatMat(Y{1}))
return;
figure(1);
Y{1}(1,:)
imagesc(Y{1},[0,255]);
colormap(gray);%image(a);

figure(2);
imagesc(U{1},[0,255]);
colormap(gray);%image(a);

figure(3);
imagesc(V{1},[0,255]);
colormap(gray);%image(a);

%pause;
end
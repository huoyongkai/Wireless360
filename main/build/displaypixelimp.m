
fileID = fopen('log.txt','r');

%fid=fopen('fx.txt','r'); %得到文件号
[f,count]=fscanf(fileID,'%f %f',[4096,2048]);

f=rot90(f,3);
fclose(fileID);

imagesc(f,[0,1]);
colormap(gray);%image(a);
%points=textscan(fileID,'%f %f %f','Delimiter',' ');
%size(points{2})
%plot3(points{1}(1:25000),points{2}(1:25000),points{3}(1:25000));

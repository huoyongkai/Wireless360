
fileID = fopen('log.txt','r');
points=textscan(fileID,'%f %f %f','Delimiter',' ');
size(points{2})

r=sqrt(points{1}(1)^2+points{2}(1)^2+points{3}(1)^2)
plot3(points{1}(1:end),points{2}(1:end),points{3}(1:end),'*');

hold on;

t=linspace(0,pi,25);
p=linspace(0,2*pi,25);
[theta,phi]=meshgrid(t,p);
x=sin(theta).*sin(phi)*r*0.9;
y=sin(theta).*cos(phi)*r*0.9;
z=cos(theta)*r*0.9;
surf(x,y,z);
axis equal;
[x,y,z]=textread('Table_Plr.txt','%f %f %f','headerlines',1);

plot3(x,y,z,'b*');

[a,b,c]=textread('fitparatable.txt','%f %f %f','headerlines',0);

for i=1:1:size(x)
    ind=(x(i)+5)*2+1;
    z1(i)=fitfunction(a(ind),b(ind),c(ind),y(i)/1000);
end
hold on;
plot3(x,y,z1,'r.');

return;
ind=1;
for i=1:size(z)
    if(x(i)==-5)
        x2d5(ind)=x(i);
        y2d5(ind)=y(i)/1000;
        z2d5(ind)=z(i);
        ind=ind+1;
    end
end
return;

zz=tan((z-0.5)*pi);




ind=1;
for i=1:size(z)
    if(abs(zz(i))<1000)
        xxx(ind)=x(i);
        yyy(ind)=y(i);
        zzz(ind)=zz(i);
        ind=ind+1;
    end
end


return;

for i=1:size(z)
    if(z(i)==0)
        z(i)=1.0e-308;
    end
end

logz=log10(z);

for i=1:size(logz)
    if(logz(i)==0)
        logz(i)=-1.0e-308;
    end
end

loglogz=log10(-logz);




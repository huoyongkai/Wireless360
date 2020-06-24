[x,y,z]=textread('Table_Plr.txt','%f %f %f','headerlines',1);

%zz=log10(z);
zz=zeros([size(z),1]);
for i=1:size(z)
    yy(i)=y(i)/1000;
    zz(i)=fitmy1(x(i),yy(i));
end

diffz=zz-z;
tanz=tan((z-0.5)*pi);
tanz1=tanz;
for i=1:size(tanz)
    if(tanz(i)>10)
        tanz1(i)=10;
    elseif(tanz(i)<-10)
        tanz1(i)=-10;
    end
end

ind=1;
for i=1:size(z)
    if(x(i)==30)
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





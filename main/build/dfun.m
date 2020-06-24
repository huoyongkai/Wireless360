function df=dfun()
%用来求解方程组的雅克比矩阵储存在dfun中
global N

[f,]=funoof();
index=0;
tmp=vpa(zeros(N+1,N+1));
for i=1:N
    index=index+1;
    tmp(index,:)=diff(f,['x',num2str(i)]);
end

tmp(index+1,:)=diff(f,'lamda');%final functions
df=conj(tmp');

end
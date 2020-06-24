function [distortion]=CalDistortion(x0)%newton([0.8 0.8 0.8],0.00001,20)

global N

%其中x0为迭代初值eps为精度要求N为最大迭代步数con用来记录结果是否收敛
[f,oof]=funoof();
index=0;
xNplus1=cell(1,N+1);
for i=1:N
    index=index+1;
    xNplus1{index}=['x',num2str(i)];
    %x_val{index}=x0(index);
    %f(index)=diff(lglrfun,['x',num2str(r),'x',num2str(c)]);
end
xNplus1{N+1}='lamda';

x_val=num2cell(x0);
distortion=double(vpa(subs(oof,xNplus1,x_val)));
end